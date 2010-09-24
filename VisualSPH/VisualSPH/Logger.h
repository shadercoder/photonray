#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

using namespace std;

class Logger
{
	ofstream fileErrors, fileWarnings, fileInfo;
public:

	Logger();

	~Logger();

	//----------------FUNCTIONS-------------------//
	void errors(string& );

	void warnings(string& );

	void info(string& );
};