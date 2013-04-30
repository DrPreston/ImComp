#include "bitmap_image.h"
#include <string>

class image
{
      private:
			//methods
              void setExt();

			//fields
              int fileType;  //File type codes: 0=unknown/error, 1=bmp, 2=jpg, 3=gif, 4=png
			  bitmap_image *bitmap;
			  int height;
			  int width;

      public:
		  //methods
             image(char[]);
			 image(std::string);
			 image(char[], int, int);
			 image(std::string, int, int);
			 image(int, int);
             int getWidth();
             int getHeight();
             int getFileType();
			 void getPixel(int, int, unsigned char[3]);
			 void setPixel(int, int, unsigned char[3]);
			 void setPixel(int, int, unsigned char, unsigned char, unsigned char);
			 void save();
			 void save(std::string);

		  //fields
			 std::string fileName;
             
};

