#include "image.h"

image::image(int x, int y)
{
	bitmap = new bitmap_image(x, y);
	height = bitmap->height();
	width = bitmap->width();
}

image::image(char _fileName[], int x, int y)
{
	bitmap = new bitmap_image(x, y);
	height = bitmap->height();
	width = bitmap->width();
	fileName = std::string(_fileName);
}

image::image(std::string _fileName, int x, int y)
{
	bitmap = new bitmap_image(x, y);
	height = bitmap->height();
	width = bitmap->width();
	fileName = _fileName;
}

image::image(char _fileName[])
{
	fileName=std::string(_fileName);
    setExt();
    if(fileType==1)
    {
        try
		{
			bitmap = new bitmap_image(_fileName);
			height = bitmap->height();
			width = bitmap->width();
		}
		catch (char exception[])
		{
			throw std::string(exception);
		}

    }
}

image::image(std::string _fileName)
{
	fileName = _fileName;
    setExt();
    if(fileType==1)
    {
        try
		{
			bitmap = new bitmap_image(_fileName);
			height = bitmap->height();
			width = bitmap->width();
		}
		catch (char exception[])
		{
			throw std::string(exception);
		}

    }
}

void image::setExt()
{
	std::string extension = fileName.substr(fileName.find_last_of(".") + 1);
    //check if the file is a bitmap
	if(extension == "bmp")    
    {
        fileType=1;
    }
    else fileType=0;
}


int image::getFileType()
{
    return fileType;
}

int image::getHeight()
{
    return height;
}

int image::getWidth()
{
    return width;
}

void image::getPixel(int x, int y, unsigned char pixel[3])
{
	bitmap->get_pixel(x, y, pixel[0], pixel[1], pixel[2]);
}

void image::setPixel(int x, int y, unsigned char pixel[3])
{
	bitmap->set_pixel(x, y, pixel[0], pixel[1], pixel[2]);
}

void image::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	bitmap->set_pixel(x, y, r, g, b);
}

void image::save()
{
	try
	{
		bitmap->save_image(fileName);
	}
	catch (char exception[])
	{
		throw std::string(exception);
	}
}

void image::save(std::string _fileName)
{
	try
	{
		bitmap->save_image(_fileName);
	}
	catch (char exception[])
	{
		throw std::string(exception);
	}
}