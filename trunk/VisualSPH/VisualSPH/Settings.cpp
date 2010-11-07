#include "Settings.h"

using namespace std;
Settings::Settings()
{
	fullscreen=false;
	screendefinition[0]=800;
	screendefinition[1]=600;
	ifstream fin("settings.txt");
	fin >> path;			
	fin >>pattern;			
	fin >> firstFrame >> lastFrame >> stepFrame;
	fin >>  cx >>  cy >>  cz;
	fin >>  lx >>  ly >>  lz;
	fin.close();		
}
