#include "DXUT.h"
#include "Logger.h"

Logger::Logger()
{
	//------------OPEN FILE-------------//
	fileErrors.open("Errors.txt", ios::app);								
	fileWarnings.open("Warnings.txt", ios::app);							
	fileInfo.open("Info.txt", ios::app);
}

Logger::~Logger()
{
	//-------------CLOSE IT----------//
	fileErrors.close();													
	fileWarnings.close();
	fileInfo.close();
}

//----------------FUNCTIONS-------------------//
void Logger::errors(string& a)
{
	char dateStr[9];
	_strdate( dateStr);					//standart operation, that check current time.
	fileErrors << '[';
	for (int i = 0; i < 9; ++i)
	{
		fileErrors << dateStr[i];
	}

	fileErrors<<' ';

	_strtime( dateStr);
	
	for (int i = 0; i < 9; ++i)
	{
		fileErrors<<dateStr[i];
	}

	fileErrors << ']' << ' ' << a << endl;
}

void Logger::warnings(string& a)
{
	char dateStr[9];
	_strdate( dateStr);				  //standart operation, that check current time.

	fileWarnings<<'[';
	for (int i = 0; i < 9; ++i)
	{
		fileWarnings<<dateStr[i];
	}
	
	_strtime( dateStr);
	
	for (int i = 0; i < 9; ++i)
	{
		fileWarnings<<dateStr[i];
	}

	fileWarnings<<']'<<' '<<a<<endl;
}

void Logger::info(string& a)
{
	char dateStr[9];
	_strdate( dateStr);					//standart operation, that check current time.
	fileInfo<<'[';
	for (int i = 0; i < 9; ++i)
	{
		fileInfo<<dateStr[i];
	}

	_strtime( dateStr);
	
	for (int i = 0; i < 9; ++i)
	{
		fileInfo<<dateStr[i];
	}

	fileInfo<<']'<<' '<<a<<endl;
}
