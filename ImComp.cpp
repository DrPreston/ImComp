#include "ImComp.h"
#include <Windows.h>
#include <stdio.h>
#include <process.h>
#include <string>;

ImComp::ImComp(std::string _myImageA, std::string _myImageB, unsigned char _sensitivity)
{
	if (_sensitivity > 64) throw std::string("Value too high");

	try { myImageA = new image(_myImageA); }
	catch (std::string exception) {throw exception;}

	try { myImageB = new image(_myImageB); }
	catch (std::string exception) {throw exception;}

	if(myImageA->getHeight() != myImageB->getHeight() && myImageA->getWidth() != myImageB->getWidth())
		throw std::string("Mismatched image sizes.");

	output = new image(myImageA->getWidth(), myImageA->getHeight());

	width = output->getWidth();
	height = output->getHeight();

	sensitivity = _sensitivity;

	palette = new Palette;
	fillPalette(palette, sensitivity);

}

ImComp::ImComp(image *_myImageA, image *_myImageB, unsigned char _sensitivity)
{
	if (_sensitivity > 64) throw std::string("Value too high");

	myImageA = _myImageA;

	myImageB = _myImageB;

	if(myImageA->getHeight() != myImageB->getHeight() && myImageA->getWidth() != myImageB->getWidth())
		throw std::string("Mismatched image sizes.");

	output = new image(myImageA->getWidth(), myImageA->getHeight());
		
	width = output->getWidth();
	height = output->getHeight();

	sensitivity = _sensitivity;
	
	palette = new Palette;
	fillPalette(palette, sensitivity);
}

ImComp::ImComp(image *_myImageA, std::string _myImageB, unsigned char _sensitivity)
{
	if (_sensitivity > 64) throw std::string("Value too high");

	myImageA = _myImageA;

	try { myImageB = new image(_myImageB); }
	catch (std::string exception) {throw exception;}

	if(myImageA->getHeight() != myImageB->getHeight() && myImageA->getWidth() != myImageB->getWidth())
		throw std::string("Mismatched image sizes.");

	output = new image(myImageA->getWidth(), myImageA->getHeight());

	width = output->getWidth();
	height = output->getHeight();
	
	sensitivity = _sensitivity;
	
	palette = new Palette;
	fillPalette(palette, sensitivity);
}

ImComp::ImComp(std::string _myImageA, image *_myImageB, unsigned char _sensitivity)
{
	if (_sensitivity > 64) throw std::string("Value too high");

	try { myImageA = new image(_myImageA); }
	catch (std::string exception) {throw exception;}

	myImageB = _myImageB;

	if(myImageA->getHeight() != myImageB->getHeight() && myImageA->getWidth() != myImageB->getWidth())
		throw std::string("Mismatched image sizes.");

	output = new image(myImageA->getWidth(), myImageA->getHeight());
	
	sensitivity = _sensitivity;
	
	palette = new Palette;
	fillPalette(palette, sensitivity);
}

void ImComp::compareImages()
{
	//This loop calculates the difference between the values of each corresponding subpixel
    //and calculates the difference, then references the palette sheet to determine the values
    //for the corresponding subpixels in the output image.
	int y;
	int x;

	unsigned char pixelA[3];
	unsigned char pixelB[3];

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
            //get the current subpixels for the current pixel from the two images being compared
			myImageA->getPixel(x, y, pixelA);
			myImageB->getPixel(x, y, pixelB);

            //calculate the difference between the subpixels
			rDiff = (pixelA[0]-pixelB[0]);
			gDiff = (pixelA[1]-pixelB[1]);
			bDiff = (pixelA[2]-pixelB[2]);

            //If the difference value is negative, invert it to make it positive
			if(rDiff<0) rDiff = -rDiff;
			if(gDiff<0) gDiff = -gDiff;
			if(bDiff<0) bDiff = -bDiff;

		
			avg = (rDiff+gDiff+bDiff)/3; //find the average of the differences for each subpixel so we get an average for the whole pixel

			if(avg>maxAvg) maxAvg = avg;
			
			//set the current pixel in the new image to the values located in the palette sheet that
			//corresponds with the the average differencde calculated above.
			output->setPixel(x, y, palette->paletteR[avg], palette->paletteG[avg], palette->paletteB[avg]);
		}
		x=0;
	}
}

image* ImComp::compareImages(image* _myImageA, image* _myImageB, unsigned char _sensitivity)
{
	if (_sensitivity > 64) throw std::string("Value too high");

	int y;
	int x;

	int rDiff = 0;
	int gDiff = 0;
	int bDiff = 0;

	int avg = 0;
	int maxAvg = 0;

	int width = _myImageA->getWidth();
	int height = _myImageA->getHeight();

	unsigned char pixelA[3];
	unsigned char pixelB[3];

	image *output = new image(width, height);

	Palette *palette = new Palette;
	fillPalette(palette, _sensitivity);

	for(y=0; y<height; y++)
	{
		for(x=0; x<width; x++)
		{
            //get the current subpixels for the current pixel from the two images being compared
			_myImageA->getPixel(x, y, pixelA);
			_myImageB->getPixel(x, y, pixelB);

            //calculate the difference between the subpixels
			rDiff = (pixelA[0]-pixelB[0]);
			gDiff = (pixelA[1]-pixelB[1]);
			bDiff = (pixelA[2]-pixelB[2]);

            //If the difference value is negative, invert it to make it positive
			if(rDiff<0) rDiff = -rDiff;
			if(gDiff<0) gDiff = -gDiff;
			if(bDiff<0) bDiff = -bDiff;

		
			avg = (rDiff+gDiff+bDiff)/3; //find the average of the differences for each subpixel so we get an average for the whole pixel

			if(avg>maxAvg) maxAvg = avg;
			
			//set the current pixel in the new image to the values located in the palette sheet that
			//corresponds with the the average differencde calculated above.
			output->setPixel(x, y, palette->paletteR[avg], palette->paletteG[avg], palette->paletteB[avg]);
		}
		x=0;
	}
	return output;
}

image* ImComp::compareImagesMt(image* _myImageA, image* _myImageB, unsigned char _sensitivity, int threads)
{
	ImComp *_ImComp = new ImComp(_myImageA, _myImageB, _sensitivity);

	int chunk = _ImComp->getHeight()/threads;

	HANDLE *_handles;
	_handles = new HANDLE[threads];
	
	for(char i=0; i<(threads); i++)
	{
		threadParams *_threadParams = new threadParams;
		_threadParams->_ImComp = _ImComp;
		if(i==(threads-1))
		{
			_threadParams->start=chunk*i;
			_threadParams->end=_ImComp->getHeight();
		}
		else
		{
			_threadParams->start=chunk*i;
			_threadParams->end=chunk*(i+1);
		}
//		std::cout << "About to start thread " << (int)i << std::endl;

		_handles[i]=(HANDLE)_beginthreadex(NULL, 0, _ImComp->makeThread, _threadParams, 0, NULL);
	}
	WaitForMultipleObjects(threads, _handles, true, 5000);
	delete[] _handles;
	return _ImComp->returnOutput();
}

void ImComp::compareRegion(threadParams _threadParams)
{
	//This loop calculates the difference between the values of each corresponding subpixel
    //and calculates the difference, then references the palette sheet to determine the values
    //for the corresponding subpixels in the output image.
	int y;
	int x;
	int start = _threadParams.start;
	int end = _threadParams.end;
	int _width = _threadParams._ImComp->getWidth();
	int _avg = 0;
	int _maxAvg = 0;
	int _rDiff;
	int _gDiff;
	int _bDiff;

	

	unsigned char pixelA[3];
	unsigned char pixelB[3];

	for(y=start; y<end; y++)
	{
		for(x=0; x<_width; x++)
		{
            //get the current subpixels for the current pixel from the two images being compared
			myImageA->getPixel(x, y, pixelA);
			myImageB->getPixel(x, y, pixelB);

            //calculate the difference between the subpixels
			_rDiff = (pixelA[0]-pixelB[0]);
			_gDiff = (pixelA[1]-pixelB[1]);
			_bDiff = (pixelA[2]-pixelB[2]);

            //If the difference value is negative, invert it to make it positive
			if(_rDiff<0) _rDiff = -_rDiff;
			if(_gDiff<0) _gDiff = -_gDiff;
			if(_bDiff<0) _bDiff = -_bDiff;

		
			_avg = (_rDiff+_gDiff+_bDiff)/3; //find the average of the differences for each subpixel so we get an average for the whole pixel

			if(_avg>_maxAvg) _maxAvg=_avg;
			
			//set the current pixel in the new image to the values located in the palette sheet that
			//corresponds with the the average differencde calculated above.
			output->setPixel(x, y, palette->paletteR[_avg],
								   palette->paletteG[_avg],
								   palette->paletteB[_avg]);
		}
	}
	maxAvg=_maxAvg;
//	_endthread();
}

unsigned __stdcall ImComp::makeThread(void *_threadParams)
{
	threadParams *mythreadParams = (threadParams*)_threadParams;
//	std::cout << "Starting thread " << mythreadParams->start << std::endl;
	mythreadParams->_ImComp->compareRegion(*mythreadParams);
//	std::cout << "Ending thread " << mythreadParams->end << std::endl;
	delete mythreadParams;
	return 0;
}

image* ImComp::returnOutput()
{
	return output;
}

int ImComp::getWidth()
{
	return width;
}

int ImComp::getHeight()
{
	return height;
}

ImComp::~ImComp(void)
{
}

void ImComp::fillPalette(Palette* _palette, unsigned char _sensitivity)
{
	if (_sensitivity > 64) throw std::string("Value too high");

	int i;
	for(i=0; i<_sensitivity; i++) //Blue indicates relatively minimal change. Filling out the blue palette sheet.
	{
		_palette->paletteR[i] = 0;  //Red is being filled with zeros here
		_palette->paletteG[i] = 0;  //Blue is being filled with zeroes here
		if(i*255/_sensitivity>255) _palette->paletteB[i]=255;
		else _palette->paletteB[i]=i*255/_sensitivity;
	}

	for(i=_sensitivity; i<_sensitivity*2; i++) //Filling out the green sheet.
	{
		if(((i-_sensitivity)*255/_sensitivity)>255) 
		{
				_palette->paletteG[i]=255;
				_palette->paletteB[i]=0;
		}
		
		else  //Smothly scale up green and down blue.
		{
				_palette->paletteG[i]=(i-_sensitivity)*255/_sensitivity;
				_palette->paletteB[i] = 255-(i-_sensitivity)*255/_sensitivity;
		}
		
		_palette->paletteR[i] = 0;  //Red is still getting zeroes
	}

	for(i=i; i<_sensitivity*3; i++) //Red indicates severe change. Filling out the red sheet.
	{
		if((i-_sensitivity*3)*255/+_sensitivity>255)
		{
			_palette->paletteR[i]=255;
			_palette->paletteG[i]=0;
		}
		else  //Smoothly scale up red, keep green to make yellow
		{

			_palette->paletteR[i] = (i-_sensitivity*3)*255/_sensitivity;
			_palette->paletteG[i] = 255;
		}

		_palette->paletteB[i] = 0;
	}

	for(i; i<_sensitivity*4; i++)   //Scale down green for a smooth fade in to red
	{
		_palette->paletteR[i] = 255;
		_palette->paletteG[i] = 255-(i-_sensitivity*4)*255/_sensitivity;
		_palette->paletteB[i] = 0;
	}

	for(i=i; i<256; i++)   //Fill the rest of the sheet with nothing but red
	{
		_palette->paletteR[i] = 255;
		_palette->paletteG[i] = 0;
		_palette->paletteB[i] = 0;
	}
}

