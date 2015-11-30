#include <iostream>
#include <fstream>
#include <ctime>
#include <unistd.h>

void getLocalTime(char*, int);

int main(int argc, char ** argv)
{
	std::ofstream fout;
	fout.open("./out.log", std::ios_base::app);
	
	if (!fout.is_open())
	{
		return 0;
	}

	char tmp[64];
	// パラメータを出力
	for (int i = 1; i < argc; i++)
	{
		fout << "arg" << i << " = " << argv[i] << " , ";
	}
	fout << std::endl;

	getLocalTime(tmp,64);
	fout << "Start at: " << tmp << std::endl;

	// 特定時間で待ち
	sleep(10);

	getLocalTime(tmp,64);
	fout << "End at  : " << tmp << std::endl;
	fout << "================================================================" << std::endl;
	fout.close();
	return 1;
}

void getLocalTime(char * LocalTime, int length)
{
    // システム時間を取得
	time_t now_time = time(NULL);
    strftime( LocalTime, length , "%Y/%m/%d %X %A",localtime(&now_time) );
	LocalTime[length - 1] = 0;
}
