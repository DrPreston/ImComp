#include <string>
#include "image.h"

class ImComp;
struct threadParams
{
public:
	int start;
	int end;
	ImComp *_ImComp;
};

struct Palette
{
public:
	// These arrays are palettes  for each subpixel being measured (red, green and blue).
    // The difference in value between the two compared images subpixel will be used to pick 
    // a new value for the corresponding subpixel in the new output image.
    unsigned char paletteR[256];
	unsigned char paletteG[256];
	unsigned char paletteB[256];
};

class ImComp
{
private:
	//Difference values for each subpixel
	int rDiff;
	int gDiff;
	int bDiff;

	//My other privates
	unsigned char avg;
	unsigned char maxAvg;

	unsigned int width;
	unsigned int height;

	image *myImageA;
	image *myImageB;
	
public:
	//fields
	unsigned char sensitivity;
	image *output;
	Palette *palette;

	//methods
	ImComp(std::string, std::string, unsigned char);
	ImComp(image*, image*, unsigned char);
	ImComp(std::string, image*, unsigned char);
	ImComp(image*, std::string, unsigned char);
	image* returnOutput();
	int getWidth();
	int getHeight();
	void computePalette(unsigned char _sensitivity);
	void computePalette();
	void compareImages();
	void compareRegion(threadParams);
	~ImComp(void);

	//static methods
	static image* compareImages(image*, image*, unsigned char);
	static image* compareImages(std::string, std::string, unsigned char);
	static image* compareImages(std::string, image*, unsigned char);
	static image* compareImagesMt(image*, image*, unsigned char, int);
	static unsigned __stdcall makeThread(void*);
	static void fillPalette(Palette*, unsigned char);
};