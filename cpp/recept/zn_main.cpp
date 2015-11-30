#include <iostream>
#include <string>
#include <memory>

#include "zn_http.hpp"
#include "zn_mailbox.hpp"
#include "zn_cimap.hpp"
#include "zn_xml.hpp"
#include "zn_ldap.hpp"
#include "zn_zimbra_soap.hpp"
#include "zn_mailbox_dump.hpp"
#include "zn_migration_queue.hpp"
#include "zn_network_req_generator.hpp"
#include "zn_hostlist.hpp"

#include "zn_config_manager.hpp"

#include "zn_exception.hpp"
#include "zn_logger.hpp"

int main(int argc, char *argv[])
{
	zn_logger::instance().info() << "md_msgmove start." << zn_buffered_log::endl;

	// オプション値取得
	if (argc != 3)
	{
		zn_logger::instance().error() <<"no startup parameter." << zn_buffered_log::endl;
		return 1;
	}
	else
	{
		// コンフィグパラメータ取得
		zn_config_manager::instance().load_config_file(argv[1]);

		// save login id from argv
		zn_config_manager::instance().set("LOGIN_ID", argv[2]);
	}

	// 移行要求取得
	// zimbra_ddc/zimbra can also login.
	zn_migration_queue migration_table_queue(
			zn_config_manager::instance().get("DB_HOST_NAME"),
			zn_config_manager::instance().get_long("DB_PORT_NO"),
			zn_config_manager::instance().get("DB_USER"),
			zn_xml::get_zimbra_mysql_passwd(readfile(zn_config_manager::instance().get("DB_PASSWORD_XML"))),
			zn_config_manager::instance().get("DB_NAME"),
			zn_config_manager::instance().get("DB_TABLE_NAME"));
	zn_migration_row migration_table_row;
	try
	{
		migration_table_queue.init();
		migration_table_row = migration_table_queue.get_migration_information(
				zn_config_manager::instance().get("LOGIN_ID")
				);
	}
	catch (mysqlpp::Exception &e)
	{
		zn_logger::instance().error() << "Query exception: " << e.what() << zn_buffered_log::endl;
		return 1;
	}
	catch (zn_migration_queue_exception &e)
	{
		zn_logger::instance().error() << e.what() << zn_buffered_log::endl;
		return 1;
	}

	// 移行情報取得
	zn_zimbra_ldap::values ldap_values;
	if (migration_table_row.proc_status == zn_migration_queue::NOT_PROCESS)
	{
		// LDAP 検索でデータ取得
		bool nsim_confirm_flag = false;
		zn_zimbra_ldap::attributes attr;
		try
		{
			attr.push_back("zimbraId");
			attr.push_back("ezwebUserPassword");
			attr.push_back("zimbraMailHost");

			ldap_values = zn_zimbra_ldap::get_zimbra_ldap_values(
					zn_config_manager::instance().get("LOGIN_ID"),
					attr);
		}
		catch (const zn_ldap_no_account_exception &e)
		{
			nsim_confirm_flag = true;
		}
		catch (const zn_retry_exceed_exception &e)
		{
			zn_logger::instance().error() << "ldap retry over." << e.what() << zn_buffered_log::endl;
			return 1;
		}

		// nsim
		if (nsim_confirm_flag)
		{
			try
			{
				nsim_hostlist hostlist;
				hostlist.load(zn_config_manager::instance().get("NSIM_HOST_FILEPATH"));

				zn_nsim_session nsim_session;
				nsim_session.init(
						hostlist.get_ip(to_string(migration_table_row.node_id)),
						zn_config_manager::instance().get("NSIM_PORT_NO")
						);
				nsim_session.nsim_login(
						migration_table_row.login_id,
						zn_config_manager::instance().get("NSIM_LOGON_PASSWORD")
						);
			}
			catch (const zn_cimap_no_exception &e)
			{
				//migration_table_queue.delete_data(migration_table_row.login_id);
				zn_logger::instance().error() << "zlogin no" << e.what() << zn_buffered_log::endl;
				return 1;
			}
			catch (const zn_cimap_bad_exception &e)
			{
				//migration_table_queue.delete_data(migration_table_row.login_id);
				zn_logger::instance().error() << "zlogin bad" << e.what() << zn_buffered_log::endl;
				return 1;
			}
			catch (const zn_retry_exceed_exception &e)
			{
				// 移行ステータス更新（移行中断）
				migration_table_queue.update_proc_status(migration_table_row.login_id, zn_migration_queue::MIGRATION_BREAKOUT);
				zn_logger::instance().error() << "nsim retry over." << e.what() << zn_buffered_log::endl;
				return 1;
			}
		}

		// update db
		migration_table_queue.update_zimbra_id(
				zn_config_manager::instance().get("LOGIN_ID"),
				ldap_values[0]);
	}
	else if (migration_table_row.proc_status == zn_migration_queue::EXPORTING)
	{
		//
		return 1;
	}
	else
	{
		return 1;
	}

	migration_table_queue.update_proc_status(
			zn_config_manager::instance().get("LOGIN_ID"),
			zn_migration_queue::EXPORTING);

	// init arguments
	//std::string zimbraid = "6cfe9ce5-abc9-4f13-9afc-598c53c22b81";
	//std::string zimbraid = "03f35af4-8c63-4b65-83d6-22689fed0340"; // test001
	//std::string zimbraid = "531d6c17-dd4e-464a-9d36-8f67602f0cd8"; // case332
	std::string zimbra_id = ldap_values[0];
	//std::string user_password = ldap_values[1];
	std::string user_password = "password";
	//std::string zimbra_mail_host = ldap_values[2];
	std::string zimbra_mail_host = migration_table_row.eas_host;
	try
	{
		mbs_hostlist hostlist;
		hostlist.load(zn_config_manager::instance().get("MBS_HOST_FILEPATH"));

		zimbra_mail_host = hostlist.get_ip(zimbra_mail_host);
	}
	catch (...)
	{
	}

	// http init
	zn_mailbox_p mailbox;
	zn_folder_p folder;
	try
	{
		zn_http::init();

		// auth token
		zn_zimbra_soap soap(zimbra_mail_host, zimbra_id);

		std::string new_password = soap.change_password(user_password, true);

		// get soap folder
		//zn_folders folders = zn_xml::get_folders(
		//		zimbra_soap.post(zn_network_req_generator::get_folder_request(auth_token, "/")));
		folder = zn_xml::get_zimbra_folder(soap.get_info());

		// download
		std::string toolserv_output_dir = zn_config_manager::instance().get("TOOLSERV_OUTPUT_DIR");
		toolserv_output_dir += "/";
		zn_file_downloader rest_downloader(toolserv_output_dir);
		std::string cookies = zn_network_req_generator::get_mailbox_dump_cookie(soap.get_delegate_token());

		std::string rest_url = zn_network_req_generator::get_url(
				zn_config_manager::instance().get("MBS_REST_URL_PREFIX"),
				zimbra_mail_host,
				zn_config_manager::instance().get("MBS_REST_PORT_NO"),
				zn_config_manager::instance().get("MBS_REST_URL_SUFFIX"));
		std::string mailarchive_filename =
			zn_config_manager::instance().get("MAILARCHIVE_PREFIX") + "_" +
			zn_config_manager::instance().get("LOGIN_ID") + "_" +
			get_current_datetime() +
			".tgz";
		rest_downloader.download(rest_url, cookies, mailarchive_filename);

		zn_http::cleanup();

		// extract and get mailbox
		std::string mailarchive_unzip_dir = zn_config_manager::instance().get("MAILARCHIVE_UNZIP_DIR");
		zn_zimbra_mailarchive_dumper mailarchive_dumper(mailarchive_unzip_dir);
		mailbox = mailarchive_dumper.dump(toolserv_output_dir + "/" + mailarchive_filename);
	}
	catch (const zn_http_transfer_exception &e)
	{
		if (e.errcode() == 28)
		{
			// timeout
		}
		zn_logger::instance().error() << e.what() << zn_buffered_log::endl;
	}
	catch (const zn_xml_exception &e)
	{
		zn_logger::instance().error() << e.what() << zn_buffered_log::endl;
	}
	catch(const zn_exception &e)
	{
		zn_logger::instance().error() << e.what() << zn_buffered_log::endl;
	}

	// 移行ステータスを「解析中」に変更する。
	migration_table_queue.update_proc_status(
			zn_config_manager::instance().get("LOGIN_ID"),
			zn_migration_queue::ANALYZING);

	// folder filters 1
	zn_folder_unnecessary_folder_remover folder_unnecessary_folder_remover;
	folder->add_filter(dynamic_cast<zn_folder_filter *>(&folder_unnecessary_folder_remover));
	folder->perform_filter();

	std::string folder_list_filename =
		zn_config_manager::instance().get("TOOLSERV_OUTPUT_DIR") + "/" +
		zn_config_manager::instance().get("FOLDERLIST_PREFIX") + "_" +
		zn_config_manager::instance().get("LOGIN_ID") + "_" +
		get_current_datetime();
	writefile(folder_list_filename, folder->serialize());

	// mailbox filters 1
	zn_mailbox_limiter mailbox_limiter(zn_config_manager::instance().get_long("NSIM_MAIL_STORE_MAX"));
	mailbox->add_filter(dynamic_cast<zn_mailbox_filter *>(&mailbox_limiter));

	zn_mailbox_sorter mailbox_sorter;
	mailbox->add_filter(dynamic_cast<zn_mailbox_filter *>(&mailbox_sorter));

	mailbox->perform_filter();

	// folder filters 2
	zn_folder_name_changer folder_name_changer;
	folder->add_filter(dynamic_cast<zn_folder_filter *>(&folder_name_changer));

	zn_folder_name_encoder folder_name_encoder;
	folder->add_filter(dynamic_cast<zn_folder_filter *>(&folder_name_encoder));

	folder->perform_filter();

	// mailbox filters 2
	zn_mailbox_backup_folder_replacement mailbox_backup_folder_replacement(folder_name_encoder.get_replacement_folderid());
	mailbox->add_filter(dynamic_cast<zn_mailbox_filter *>(&mailbox_backup_folder_replacement));

	zn_mailbox_mail_counter mailbox_mail_counter;
	mailbox->add_filter(dynamic_cast<zn_mailbox_filter *>(&mailbox_mail_counter));

	mailbox->perform_filter();

	// folder filters 3
	zn_folder_priority_sorter folder_priority_sorter(mailbox_mail_counter.get_mail_counter());
	folder->add_filter(dynamic_cast<zn_folder_filter *>(&folder_priority_sorter));
	
	folder->perform_filter();

	//zn_config_manager::instance().get_long("NSIM_FOLDER_MAX");

	std::string transdata_filename =
		zn_config_manager::instance().get("TOOLSERV_OUTPUT_DIR") + "/" +
		zn_config_manager::instance().get("TRANSDATA_PREFIX") + "_" +
		zn_config_manager::instance().get("LOGIN_ID") + "_" +
		get_current_datetime();
	writefile(transdata_filename, folder->serialize() + "\n" + mailbox->serialize());

	// dump folder
	std::string folder_dump = folder->serialize();
	std::cout << folder_dump << std::endl;

	// dump mailbox
	std::string mailbox_dump = mailbox->serialize();
	std::cout << mailbox_dump << std::endl;

	for (zn_mailbox::iterator it = mailbox->begin(); it != mailbox->end(); ++it)
	{
		if (!it->avaliable)
		{
			continue;
		}

		std::cout << "=======================================\n";
		std::cout << it->mail.get_content();
		std:: cout << std::endl;
		std::cout << "=======================================\n";
	}

	// 移行ステータスを「移行中」に変更する。
	migration_table_queue.update_proc_status(
			zn_config_manager::instance().get("LOGIN_ID"),
			zn_migration_queue::MIGRATING);

	// NSIMの移行IMAP機能と接続し、拡張IMAPコマンドを用いて、データ移行する。
	nsim_hostlist hostlist;
	hostlist.load(zn_config_manager::instance().get("NSIM_HOST_FILEPATH"));

	zn_cimap_session imap_session;
	imap_session.init(
			hostlist.get_ip(to_string(migration_table_row.node_id)),
			zn_config_manager::instance().get("NSIM_PORT_NO"));

	// welcome
	zn_cimap_interaction_p welcome_cmd = imap_session.get_cmd<zn_cimap_cmd_welcome>();
	welcome_cmd->perform();

	// login
	zn_cimap_interaction_p login_cmd = imap_session.get_cmd<zn_cimap_cmd_login>();
	login_cmd->set_option(CIMAP_USERNAME, zn_config_manager::instance().get("LOGIN_ID")).
		set_option(CIMAP_PASSWORD, user_password).
		perform();

	// create
	zn_cimap_interaction_p create_cmd = imap_session.get_cmd<zn_cimap_cmd_create>();
	for (zn_folder::iterator it = folder->begin(); it != folder->end(); ++it)
	{
		create_cmd->clear_options();
		create_cmd->set_option(CIMAP_MAILBOXNAME, it->name).perform();
	}

	// append
	zn_cimap_interaction_p append_cmd = imap_session.get_cmd<zn_cimap_cmd_append>();

	for (zn_mailbox::iterator it = mailbox->begin(); it != mailbox->end(); ++it)
	{
		if (!it->avaliable)
		{
			continue;
		}

		append_cmd->clear_options();
		append_cmd->set_option(CIMAP_MAILBOXNAME, folder->get_info(it->meta.folder_id).utf7name).
			set_option(CIMAP_FLAGS, it->meta.is_read?"(\\Seen)":"").
			set_option(CIMAP_DATETIME, "").
			set_option(CIMAP_LITERAL, it->mail.get_content()).
			perform();
	}

	// logout
	zn_cimap_interaction_p logout_cmd = imap_session.get_cmd<zn_cimap_cmd_logout>();
	logout_cmd->perform();

	// アカウント属性再更新
	try
	{
		zn_zimbra_soap soap(zimbra_mail_host, zimbra_id);
		std::string new_password = soap.change_password(user_password, false);
	}
	catch (...)
	{
	}

	// 移行アカウントの情報を削除する。
	migration_table_queue.delete_data(zn_config_manager::instance().get("LOGIN_ID"));

	zn_logger::instance().info() << "copytool end." << zn_buffered_log::endl;
	return 0;
}
