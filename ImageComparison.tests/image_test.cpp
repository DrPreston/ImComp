#include "CppUnitTest.h"
#include <string>
#include "imcomp.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ImComp_test
{		
	TEST_CLASS(TestImage)
	{
	public:

		image *myImage;
		image *myImage_bad;
		image *myImage_new;

		TEST_METHOD(Verify_bitmap_file_type)
		{
			Assert::AreEqual(myImage->getFileType(), 1);
		}

		TEST_METHOD(Verify_width_and_height)
		{
			Assert::AreEqual(2592, myImage->getWidth());
			Assert::AreEqual(1936, myImage->getHeight());
		}

		TEST_METHOD(Verify_file_name)
		{
			Assert::AreEqual(std::string("C:\\temp\\1.bmp"), myImage->fileName);
		}

		TEST_METHOD(Test_getPixel_with_good_input)
		{
			unsigned char myPixel[] = {0, 0, 0};
			myImage->getPixel(0, 0, myPixel);
			Assert::AreEqual(myPixel[0], (unsigned char)134);
			Assert::AreEqual(myPixel[1], (unsigned char)132);
			Assert::AreEqual(myPixel[2], (unsigned char)111);
		}

		TEST_METHOD(Test_setPixel_with_good_input)
		{
			unsigned char myPixel[] = {50, 50, 50};
			myImage->setPixel(0, 0, myPixel);
			myPixel[0] = 0;
			myPixel[1] = 0;
			myPixel[2] = 0;
			myImage->getPixel(0, 0, myPixel);
						
			Assert::AreEqual(myPixel[0], (unsigned char)50);
			Assert::AreEqual(myPixel[1], (unsigned char)50);
			Assert::AreEqual(myPixel[2], (unsigned char)50);
		}

		TEST_METHOD(Test_image_constructor_with_bad_filename)
		{
			try
			{
				myImage_bad = new image("camel poo");
			}
			catch (std::string exception)
			{
				Assert::AreEqual(std::string("Invalid file"), exception);
			}
		}

		TEST_METHOD(Test_image_constructor_with_char_array)
		{
			myImage_new = new image("C:\\temp\\Monkeys.bmp", 50, 50);
			Assert::AreEqual(std::string("C:\\temp\\Monkeys.bmp"), myImage_new->fileName);
		}

		TEST_METHOD(Test_image_constructor_with_string)
		{
			myImage_new = new image(std::string("C:\\Llamas.bmp"), 50, 50);
			Assert::AreEqual(std::string("C:\\Llamas.bmp"), myImage_new->fileName);
		}

		TEST_METHOD(Test_image_constructor_with_only_height_and_width)
		{
			myImage_new = new image(50, 50);
			Assert::AreEqual(50, myImage_new->getHeight());
			Assert::AreEqual(50, myImage_new->getWidth());
		}

		TEST_METHOD(Test_save_method_with_valid_file_name)
		{
			myImage->fileName = std::string("C:\\temp\\tests\\llamas.bmp");
			try
			{
				myImage->save();
			}
			catch (std::string exception)
			{
				Assert::Fail();
			}
			Assert::IsTrue(true);
		}

		TEST_METHOD(Test_save_method_with_invalid_file_name)
		{
			myImage->fileName = std::string("X:\\garbage");
			try
			{
				myImage->save();
			}
			catch (std::string exception)
			{
				Assert::AreEqual(std::string("Could not write file!"), exception);
			}
		}

		TestImage()
		{
			myImage = new image("C:\\temp\\1.bmp");
		}

	};
}