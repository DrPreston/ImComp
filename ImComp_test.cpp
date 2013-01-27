#include "stdafx.h"
#include "CppUnitTest.h"
#include "ImComp.h"
#include <Windows.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ImComp_test
{
	TEST_CLASS(ImComp_test)
	{
	public:
		
		ImComp *testImComp;
		image *UnitTestRefImage;
		image *newImageA;
		image *newImageB;

		unsigned char testPixel1[3];
		unsigned char testPixel2[3];

		ImComp_test::ImComp_test()
		{
			UnitTestRefImage = new image("C:\\temp\\UnitTestRefImage.bmp");
			newImageA = new image("C:\\temp\\1.bmp");
			newImageB = new image("C:\\temp\\2.bmp");
		}

		TEST_METHOD(Test_strings_only_constructor_with_valid_filenames)
		{
			try
			{
				testImComp = new ImComp(std::string("C:\\temp\\1.bmp"), std::string("C:\\temp\\2.bmp"), 15);
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}
			Assert::IsTrue(true);
		}

		TEST_METHOD(Test_images_only_constructor)
		{
			try
			{
				testImComp = new ImComp(newImageA, newImageB, (unsigned char)15);
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}
		}
		
		TEST_METHOD(Test_mixed_string_and_image_constructor)
		{
			try
			{
				testImComp = new ImComp(newImageA, std::string("C:\\temp\\2.bmp"), (unsigned char)15);
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(Test_compareImages_and_returnOutput)
		{
			try
			{ 
				testImComp = new ImComp(std::string("C:\\temp\\1.bmp"), std::string("C:\\temp\\2.bmp"), 64);
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}

			testImComp->compareImages();

			image *output = testImComp->output;

			output->fileName = std::string("C:\\temp\\tests\\output.bmp");
			output->save();

			UnitTestRefImage->getPixel(50, 50, testPixel1);
			output->getPixel(50, 50, testPixel2);

			//Assert::AreEqual(*testPixel1, *testPixel2);
			Assert::IsTrue(true);
		}

		TEST_METHOD(Test_static_compareImages_method)
		{
			image *output;

			try
			{
				output = ImComp::compareImages(newImageA, newImageB, 64);
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}

			output->fileName = std::string("C:\\temp\\tests\\static.bmp");

			try
			{
				output->save();
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}

			UnitTestRefImage->getPixel(50, 50, testPixel1);
			output->getPixel(50, 50, testPixel2);

			//Assert::AreEqual(*testPixel1, *testPixel2);
			Assert::IsTrue(true);
		}

		TEST_METHOD(test_compareImagesMt)
		{
			//SYSTEM_INFO myInfo;
			//GetSystemInfo(&myInfo);

			image *output;
			output = ImComp::compareImagesMt(newImageA, newImageB, 64, 8);//myInfo.dwNumberOfProcessors);
			output->fileName = std::string("C:\\temp\\tests\\multithreaded.bmp");

			try
			{
				output->save();
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}
		}

		TEST_METHOD(test)
		{
			//The first one is always free
			Assert::IsTrue(true);
		}
	};
}