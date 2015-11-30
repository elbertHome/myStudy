class zn_network_req_generator
{
	public:
		static std::string get_auth_request(const std::string &username, const std::string &password)
		{
			std::string ret = "<?xml version='1.0' encoding='UTF-8'?>"
				"<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">"
				  "<soap:Header>"
					"<context xmlns=\"urn:zimbra\">"
					  "<nosession/>"
					  "<userAgent name=\"zmprov\" version=\"7.1.4_GA_2555\"/>"
					"</context>"
				  "</soap:Header>"
					"<soap:Body>"
					  "<AuthRequest xmlns=\"urn:zimbraAdmin\">"
					  "<name>$username</name>"
					  "<password>$password</password>"
				  "</AuthRequest>"
				  "</soap:Body>"
				"</soap:Envelope>";

			size_t found = ret.find("$username");
			ret.replace(found, 9, username);
			found = ret.find("$password");
			ret.replace(found, 9, password);
			return ret;
		}

		static std::string get_modify_account_request(const std::string &authtoken, const std::string &zimbra_id, const std::string &user_password, const std::string &password, const std::string &include_spam)
		{
			std::string ret = "<?xml version='1.0' encoding='UTF-8'?>"
				"<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">"
				  "<soap:Header>"
					"<context xmlns=\"urn:zimbra\">"
					  "<authToken>$authToken</authToken>"
					  "<nosession/>"
					  "<userAgent name=\"zmprov\" version=\"7.1.3_GA_3346\"/>"
					"</context>"
				  "</soap:Header>"
				  "<soap:Body>"
					"<ModifyAccountRequest xmlns=\"urn:zimbraAdmin\">"
					  "<id>$zimbra_id</id>"
					  "<a n=\"userPassword\">$user_password</a>"
					  "<a n=\"ezwebUserPassword\">$password</a>"
					  "<a n=\"zimbraPrefIncludeSpamInSearch\">$include_spam</a>"
					"</ModifyAccountRequest>"
				  "</soap:Body>"
				"</soap:Envelope>";

			size_t found = ret.find("$authToken");
			ret.replace(found, 10, authtoken);
			found = ret.find("$zimbra_id");
			ret.replace(found, 10, zimbra_id);
			found = ret.find("$user_password");
			ret.replace(found, 14, user_password);
			found = ret.find("$password");
			ret.replace(found, 9, password);
			found = ret.find("$include_spam");
			ret.replace(found, 13, include_spam);
			return ret;
		}

		static std::string get_set_password_request(const std::string &authtoken, const std::string &zimbra_id, const std::string &password)
		{
			std::string ret = "<?xml version='1.0' encoding='UTF-8'?>"
				"<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">"
				  "<soap:Header>"
					"<context xmlns=\"urn:zimbra\">"
					  "<authToken>$authtoken</authToken>"
					  "<nosession/>"
					  "<userAgent name=\"zmprov\" version=\"7.1.3_GA_3346\"/>"
					"</context>"
				  "</soap:Header>"
				  "<soap:Body>"
					"<SetPasswordRequest  xmlns=\"urn:zimbraAdmin\">"
					  "<id>$zimbra_id</id>"
					  "<newPassword>$password</newPassword>"
					"</SetPasswordRequest>"
				  "</soap:Body>"
				"</soap:Envelope>";

			size_t found = ret.find("$authtoken");
			ret.replace(found, 10, authtoken);
			found = ret.find("$zimbra_id");
			ret.replace(found, 10, zimbra_id);
			found = ret.find("$password");
			ret.replace(found, 9, password);
			return ret;
		}

		static std::string get_delegate_auth_request(const std::string &authtoken, const std::string &account)
		{
			std::string ret = "<?xml version='1.0' encoding='UTF-8'?>"
				"<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">"
				  "<soap:Header>"
					"<context xmlns=\"urn:zimbra\">"
					  "<authToken>$authToken</authToken>"
					  "<nosession/>"
					  "<userAgent name=\"zclient\" version=\"7.1.4_GA_2555\"/>"
					"</context>"
				  "</soap:Header>"
				  "<soap:Body>"
					"<DelegateAuthRequest duration=\"86400\" xmlns=\"urn:zimbraAdmin\">"
					  "<account by=\"id\">$account</account>"
					"</DelegateAuthRequest>"
				  "</soap:Body>"
				"</soap:Envelope>";

			size_t found = ret.find("$authToken");
			ret.replace(found, 10, authtoken);
			found = ret.find("$account");
			ret.replace(found, 8, account);
			return ret;
		}

		static std::string get_folder_request(const std::string &authtoken, const std::string &folder)
		{
			std::string ret = "<?xml version='1.0' encoding='UTF-8'?>"
				"<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">"
				  "<soap:Header>"
					"<context xmlns=\"urn:zimbra\">"
					  "<authToken>$authToken</authToken>"
					  "<nosession/>"
					  "<userAgent name=\"zclient\" version=\"7.1.4_GA_2555\"/>"
					"</context>"
				  "</soap:Header>"
				  "<soap:Body>"
					"<GetFolderRequest xmlns=\"urn:zimbraMail\">"
					  "<folder path=\"$folder\"/>"
					"</GetFolderRequest>"
				  "</soap:Body>"
				"</soap:Envelope>";

			size_t found = ret.find("$authToken");
			ret.replace(found, 10, authtoken);
			found = ret.find("$folder");
			ret.replace(found, 7, folder);
			return ret;
		}

		static std::string get_info_request(const std::string &authtoken)
		{
			std::string ret = "<?xml version='1.0' encoding='UTF-8'?>"
				"<soap:Envelope xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">"
				  "<soap:Header>"
					"<context xmlns=\"urn:zimbra\">"
					  "<authToken>$authToken</authToken>"
					  "<session/>"
					  "<userAgent name=\"zclient\" version=\"7.1.4_GA_2555\"/>"
					"</context>"
				  "</soap:Header>"
				  "<soap:Body>"
					"<GetInfoRequest sections=\"mbox,prefs,attrs,props,idents,sigs,dsrcs,children\" xmlns=\"urn:zimbraAccount\"/>"
				  "</soap:Body>"
				"</soap:Envelope>";

			size_t found = ret.find("$authToken");
			ret.replace(found, 10, authtoken);

			return ret;
		}

		static std::string get_mailbox_dump_cookie(const std::string &authtoken)
		{
			return "ZM_AUTH_TOKEN=" + authtoken;
		}

		/*
		static std::string get_mailbox_dump_url(const std::string &prefix, const std::string &host, const std::string &port, const std::string &suffix)
		{
			std::string ret = prefix;
			ret += host;
			ret += ":";
			ret += port;
			ret += suffix;
			return ret;
		}

		static std::string get_soap_url(const std::string &prefix, const std::string &host, const std::string &port, const std::string &suffix)
		{
			std::string ret = prefix;
			ret += host;
			ret += ":";
			ret += port;
			ret += suffix;
			return ret;
		}
		*/
		static std::string get_url(const std::string &prefix, const std::string &host, const std::string &port, const std::string &suffix)
		{
			std::string ret = prefix;
			ret += host;
			ret += ":";
			ret += port;
			ret += suffix;
			return ret;
		}
};
