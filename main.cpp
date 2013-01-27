#include "ImComp.h"
#include <Windows.h>
#include <stdio.h>

//int _tmain(int argc, _TCHAR* argv[])

int main(int argc, char *argv[])
{
		ImComp *testImComp;
		image *UnitTestRefImage;
		image *newImageA;
		image *newImageB;

		unsigned char testPixel1[3];
		unsigned char testPixel2[3];

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
		getchar();

		return 0;
}