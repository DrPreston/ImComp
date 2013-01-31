#include "ImComp.h"
#include <Windows.h>
#include <stdio.h>
using namespace std;

int main(int argc, char *argv[])
{
		image *UnitTestRefImage;
		image *newImageA;
		image *newImageB;
		
		UnitTestRefImage = new image("C:\\temp\\UnitTestRefImage.bmp");
		newImageA = new image("C:\\temp\\1.bmp");
		newImageB = new image("C:\\temp\\2.bmp");

		SYSTEM_INFO myInfo;
		GetSystemInfo(&myInfo);

		image *output;
		output = ImComp::compareImagesMt(newImageA, newImageB, 64, myInfo.dwNumberOfProcessors);
		output->fileName = std::string("C:\\temp\\tests\\multithreaded.bmp");
		try
		{
			output->save();
		}
		catch (std::string exception)
		{
			return 0;
		}

		system("PAUSE");

		return 0;
}