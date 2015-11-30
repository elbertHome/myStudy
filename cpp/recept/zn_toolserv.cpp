#include <iostream>
#include <string>
#include <memory>
#include <exception>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "zn_server.hpp"
#include "zn_config_manager.hpp"
#include "zn_exception.hpp"
#include "zn_logger.hpp"
#include "zn_misc.hpp"

static const int MODE_RECEPT = 0;
static const int MODE_RETRY = 1;
bool is_child = false;

void create_pidfile(int mode)
{
	std::string pid_file_dir = zn_config_manager::instance().get("PID_FILE_DIR");
	std::string pid_file_name = zn_config_manager::instance().get(
			(mode == MODE_RECEPT) ? "PID_FILE_NAME_RCPT" : "PID_FILE_NAME_RETRY");

	std::string pidflie = pid_file_dir + "/" + pid_file_name + ".pid";

	// PIDファイルが既にある
	if (access(pidflie.c_str(), F_OK) == 0)
	{
		ZN_THROW(zn_syserr_exception, "PID file already exists.");
	}

	int ret = -1;
	FILE *fp = NULL;
	do
	{
		char pid[16];
		size_t length;

		if ((fp = fopen(pidflie.c_str(), "w+")) == NULL)
			break;

		sprintf(pid, "%u\n", getpid());
		length = strlen(pid);
		if (length != fwrite(pid, 1, length, fp))
			break;

		if (fflush(fp) != 0)
			break;

		ret = 0;
	}
	while (0);

	if (fp != NULL)
		fclose(fp);

	if (ret != 0)
	{
		ZN_THROW(zn_syserr_exception, "PID file create error.");
	}

	zn_config_manager::instance().set("PID_FILE_NAME", pidflie);
	return;
}

void remove_pidfile()
{
	std::string pidflie = zn_config_manager::instance().get("PID_FILE_NAME");

	// PIDファイルを削除する
	if (pidflie.size() > 0)
	{
		remove(pidflie.c_str());
	}

	return;
}

int get_startup_mode(const std::string &mode)
{
	int ret;

	if (strcasecmp(mode.c_str(), "-recept") == 0)
	{
		ret = MODE_RECEPT;
	}
	else if (strcasecmp(mode.c_str(), "-retry") == 0)
	{
		ret = MODE_RETRY;
	}
	else
	{
		ZN_THROW(zn_syserr_exception, "Option parameter error.");
	}

	return ret;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: msgmvrcpt config_file (-recept | -retry)\n";
		return 1;
	}

	try
	{
		// コンフィグファイルを読み込み
		zn_config_manager::instance().load_config_file(argv[1]);

		// 起動モードを判定する
		int mode = get_startup_mode(argv[2]);

		// PIDファイル作成
		create_pidfile(mode);

		// 起動ログを出力する
		zn_logger::instance().info("msgmvrcpt started. Mode:recept");
		std::cout << "DEBUG: msgmvrcpt started. Mode:recept\n";

		if (mode == MODE_RETRY)
		{

		}
		else
		{
			zn_server serv;
			serv.run();
		}
	}
	catch(const zn_syserr_exception &e)
	{
		std::cerr << "System call failed: " << e.what() << std::endl;
	}
	catch(const zn_exception &e)
	{
		std::cerr << "Unknown exception: " << e.what() << std::endl;
	}

	if (!is_child)
	{
		// PIDファイルを削除する
		remove_pidfile();

		// プロセス終了のログを出力する
		zn_logger::instance().info("msgmvrcpt stopped. Mode:recept");
		std::cout << "DEBUG: msgmvrcpt stopped. Mode:recept\n";
	}

	return 0;
}
