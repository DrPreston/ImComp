/*
***************************************************************************
*                                                                         *
*                         Platform Independent                            *
*                   Bitmap Image Reader Writer Library                    *
*                                                                         *
* Author: Arash Partow - 2002                                             *
* URL: http://www.partow.net                                              *
*                                                                         *
* Note: This library only supports 24-bits per pixel bitmap format files. *
*                                                                         *
* Copyright notice:                                                       *
* Free use of the Platform Independent Bitmap Image Reader Writer Library *
* is permitted under the guidelines and in accordance with the most       *
* current version of the Common Public License.                           *
* http://www.opensource.org/licenses/cpl1.0.php                           *
*                                                                         *
***************************************************************************
*/

#ifndef INCLUDE_BITMAP_IMAGE_HPP
#define INCLUDE_BITMAP_IMAGE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <limits>
#include <cmath>
#include <cstdlib>


struct bitmap_file_header
{
   unsigned short type;
   unsigned int   size;
   unsigned short reserved1;
   unsigned short reserved2;
   unsigned int   off_bits;

   unsigned int struct_size()
   {
      return sizeof(type)     +
            sizeof(size)      +
            sizeof(reserved1) +
            sizeof(reserved2) +
            sizeof(off_bits);
   }
};

struct bitmap_information_header
{
   unsigned int   size;
   unsigned int   width;
   unsigned int   height;
   unsigned short planes;
   unsigned short bit_count;
   unsigned int   compression;
   unsigned int   size_image;
   unsigned int   x_pels_per_meter;
   unsigned int   y_pels_per_meter;
   unsigned int   clr_used;
   unsigned int   clr_important;

   unsigned int struct_size()
   {
      return sizeof(size)             +
             sizeof(width)            +
             sizeof(height)           +
             sizeof(planes)           +
             sizeof(bit_count)        +
             sizeof(compression)      +
             sizeof(size_image)       +
             sizeof(x_pels_per_meter) +
             sizeof(y_pels_per_meter) +
             sizeof(clr_used)         +
             sizeof(clr_important);
   }
};


inline void read_bih(std::ifstream& stream,bitmap_information_header& bih);
inline void read_bfh(std::ifstream& stream, bitmap_file_header& bfh);
inline void write_bih(std::ofstream& stream, const bitmap_information_header& bih);
inline void write_bfh(std::ofstream& stream, const bitmap_file_header& bfh);

class bitmap_image
{
public:

   enum channel_mode {
                        rgb_mode = 0,
                        bgr_mode = 1
                     };

   enum color_plane {
                       blue_plane  = 0,
                       green_plane = 1,
                       red_plane   = 2
                    };


   bitmap_image()
   : file_name_(""),
     data_(0),
     bytes_per_pixel_(3),
     length_(0),
     width_(0),
     height_(0),
     row_increment_(0),
     channel_mode_(bgr_mode)
   {}

   bitmap_image(const std::string& _filename)
   : file_name_(_filename),
     data_(0),
     bytes_per_pixel_(0),
     length_(0),
     width_(0),
     height_(0),
     row_increment_(0),
     channel_mode_(bgr_mode)
   {
      load_bitmap();
   }

   bitmap_image(const unsigned int width, const unsigned int height)
   : file_name_(""),
     data_(0),
     bytes_per_pixel_(3),
     length_(0),
     width_(width),
     height_(height),
     row_increment_(0),
     channel_mode_(bgr_mode)
   {
     create_bitmap();
   }

   bitmap_image(const bitmap_image& image)
   : file_name_(image.file_name_),
     data_(0),
     bytes_per_pixel_(3),
     width_(image.width_),
     height_(image.height_),
     row_increment_(0),
     channel_mode_(bgr_mode)
   {
      create_bitmap();
      std::copy(image.data_, image.data_ + image.length_, data_);
   }

  ~bitmap_image()
   {
      delete [] data_;
   }

   bitmap_image& operator=(const bitmap_image& image)
   {
      if (this != &image)
      {
         file_name_ = image.file_name_;
         bytes_per_pixel_ = image.bytes_per_pixel_;
         width_ = image.width_;
         height_ = image.height_;
         row_increment_ = 0;
         channel_mode_ = image.channel_mode_;
         create_bitmap();
         std::copy(image.data_, image.data_ + image.length_, data_);
      }
      return *this;
   }

   inline void clear(const unsigned char v = 0x00)
   {
      std::fill(data_,data_ + length_,v);
   }

   inline unsigned char red_channel(const unsigned int x, const unsigned int y) const
   {
      return data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)];
   }

   inline unsigned char green_channel(const unsigned int x, const unsigned int y) const
   {
      return data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)];
   }

   inline unsigned char blue_channel (const unsigned int x, const unsigned int y) const
   {
      return data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)];
   }

   inline void red_channel(const unsigned int x, const unsigned int y, const unsigned char value)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)] = value;
   }

   inline void green_channel(const unsigned int x, const unsigned int y, const unsigned char value)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)] = value;
   }

   inline void blue_channel (const unsigned int x, const unsigned int y, const unsigned char value)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)] = value;
   }

   inline unsigned char* row(unsigned int row_index) const
   {
      return data_ + (row_index * row_increment_);
   }

   inline void get_pixel(const unsigned int x, const unsigned int y,
                         unsigned char& red,
                         unsigned char& green,
                         unsigned char& blue)
   {
      blue  = data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)];
      green = data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)];
      red   = data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)];
   }

   inline void set_pixel(const unsigned int x, const unsigned int y,
                        const unsigned char red,
                        const unsigned char green,
                        const unsigned char blue)
   {
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 0)] = blue;
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 1)] = green;
      data_[(y * row_increment_) + (x * bytes_per_pixel_ + 2)] = red;
   }

   inline bool copy_from(const bitmap_image& image)
   {
      if ((image.height_ !=  height_) ||
          (image.width_  !=  width_))
      {
         return false;
      }
      std::copy(image.data_,image.data_ + image.length_,data_);
      return true;
   }

   inline bool copy_from(const bitmap_image& source_image,
                         const unsigned int& x_offset,
                         const unsigned int& y_offset)
   {
      if ((x_offset + source_image.width_) > width_)   { return false; }
      if ((y_offset + source_image.height_) > height_) { return false; }

      for (unsigned int y = 0; y < source_image.height_; ++y)
      {
         unsigned char* itr1           = row(y + y_offset) + x_offset * bytes_per_pixel_;
         const unsigned char* itr2     = source_image.row(y);
         const unsigned char* itr2_end = itr2 + source_image.width_ * bytes_per_pixel_;
         std::copy(itr2,itr2_end,itr1);
      }
      return true;
   }

   inline bool region(const unsigned int& x,
                      const unsigned int& y,
                      const unsigned int& width,
                      const unsigned int& height,
                      bitmap_image& dest_image)
   {
      if ((x + width) > width_)   { return false; }
      if ((y + height) > height_) { return false; }

      if ((dest_image.width_  < width_ ) ||
          (dest_image.height_ < height_))
      {
         dest_image.setwidth_height(width,height);
      }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr1     = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr1_end = itr1 + (width * bytes_per_pixel_);
         unsigned char* itr2     = dest_image.row(r);
         std::copy(itr1,itr1_end,itr2);
      }
      return true;
   }

   inline bool set_region(const unsigned int& x,
                          const unsigned int& y,
                          const unsigned int& width,
                          const unsigned int& height,
                          const unsigned char& value)
   {
      if ((x + width) > width_)   { return false; }
      if ((y + height) > height_) { return false; }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr     = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr_end = itr + (width * bytes_per_pixel_);
         std::fill(itr,itr_end,value);
      }
      return true;
   }

   inline bool set_region(const unsigned int& x,
                          const unsigned int& y,
                          const unsigned int& width,
                          const unsigned int& height,
                          const color_plane color,
                          const unsigned char& value)
   {
      if ((x + width) > width_)   { return false; }
      if ((y + height) > height_) { return false; }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr     = row(r + y) + x * bytes_per_pixel_ + offset(color);
         unsigned char* itr_end = itr + (width * bytes_per_pixel_);

         while (itr != itr_end)
         {
            *itr  = value;
             itr += bytes_per_pixel_;
         }
      }
      return true;
   }

   inline bool set_region(const unsigned int& x,
                          const unsigned int& y,
                          const unsigned int& width,
                          const unsigned int& height,
                          const unsigned char& red,
                          const unsigned char& green,
                          const unsigned char& blue)
   {
      if ((x + width) > width_)   { return false; }
      if ((y + height) > height_) { return false; }

      for (unsigned int r = 0; r < height; ++r)
      {
         unsigned char* itr     = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr_end = itr + (width * bytes_per_pixel_);

         while (itr != itr_end)
         {
            *(itr++) = blue;
            *(itr++) = green;
            *(itr++) = red;
         }
      }
      return true;
   }


   void reflective_image(bitmap_image& image)
   {
      image.setwidth_height(3 * width_, 3 * height_,true);
      image.copy_from(*this,width_,height_);
      vertical_flip();
      image.copy_from(*this,width_,0);
      image.copy_from(*this,width_,2 * height_);
      vertical_flip();
      horizontal_flip();
      image.copy_from(*this,0,height_);
      image.copy_from(*this,2 * width_,height_);
      horizontal_flip();
   }

   inline unsigned int width()  const { return width_;  }
   inline unsigned int height() const { return height_; }
   inline unsigned int bytes_per_pixel() const { return bytes_per_pixel_; }
   inline unsigned int pixel_count() const { return width_ *  height_; }

   inline void setwidth_height(const unsigned int width,
                               const unsigned int height,
                               const bool clear = false)
   {
      delete[] data_;
      width_  = width;
      height_ = height;
      create_bitmap();
      if (clear)
      {
         std::fill(data_,data_ + length_,0x00);
      }
   }

   void save_image(const std::string& file_name)
   {
      std::ofstream stream(file_name.c_str(),std::ios::binary);

      if (!stream)
      {
         std::cout << "bitmap_image::save_image(): Error - Could not open file "  << file_name << " for writing!" << std::endl;
		 throw "Could not write file!";
         return;
      }

      bitmap_file_header bfh;
      bitmap_information_header bih;

      bih.width            = width_;
      bih.height           = height_;
      bih.bit_count        = static_cast<unsigned short>(bytes_per_pixel_ << 3);
      bih.clr_important    =  0;
      bih.clr_used         =  0;
      bih.compression      =  0;
      bih.planes           =  1;
      bih.size             = 40;
      bih.x_pels_per_meter =  0;
      bih.y_pels_per_meter =  0;
      bih.size_image       = (((bih.width * bytes_per_pixel_) + 3) & 0xFFFC) * bih.height;

      bfh.type      = 19778;
      bfh.size      = 55 + bih.size_image;
      bfh.reserved1 = 0;
      bfh.reserved2 = 0;
      bfh.off_bits  = bih.struct_size() + bfh.struct_size();

      write_bfh(stream,bfh);
      write_bih(stream,bih);

      unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
      char padding_data[4] = {0x0,0x0,0x0,0x0};
      for (unsigned int i = 0; i < height_; ++i)
      {
         unsigned char* data_ptr = data_ + (row_increment_ * (height_ - i - 1));
         stream.write(reinterpret_cast<char*>(data_ptr),sizeof(unsigned char) * bytes_per_pixel_ * width_);
         stream.write(padding_data,padding);
      }
      stream.close();
   }

   inline void set_all_ith_bits_low(const unsigned int bitr_index)
   {
      unsigned char mask = static_cast<unsigned char>(~(1 << bitr_index));
      for (unsigned char* itr = data_; itr != data_ + length_; ++itr) { *itr &= mask; }
   }

   inline void set_all_ith_bits_high(const unsigned int bitr_index)
   {
      unsigned char mask = static_cast<unsigned char>(1 << bitr_index);
      for (unsigned char* itr = data_; itr != data_ + length_; ++itr) { *itr |= mask; }
   }

   inline void set_all_ith_channels(const unsigned int& channel, const unsigned char& value)
   {
      for (unsigned char* itr = (data_ + channel); itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         *itr = value;
      }
   }

   inline void set_channel(const color_plane color,const unsigned char& value)
   {
      for (unsigned char* itr = (data_ + offset(color)); itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         *itr = value;
      }
   }

   inline void ror_channel(const color_plane color, const unsigned int& ror)
   {
      for (unsigned char* itr = (data_ + offset(color)); itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         *itr = static_cast<unsigned char>(((*itr) >> ror) | ((*itr) << (8 - ror)));
      }
   }

   inline void set_all_channels(const unsigned char& value)
   {
      for (unsigned char* itr = data_; itr < (data_ + length_); )
      {
         *(itr++) = value;
      }
   }

   inline void set_all_channels(const unsigned char& r_value,
                                const unsigned char& g_value,
                                const unsigned char& b_value)
   {
      for (unsigned char* itr = (data_ + 0); itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         *(itr + 0) = b_value;
         *(itr + 1) = g_value;
         *(itr + 2) = r_value;
      }
   }

   inline void invert_color_planes()
   {
      for (unsigned char* itr = data_; itr < (data_ + length_); *itr = ~(*itr), ++itr);
   }

   inline void add_to_color_plane(const color_plane color,const unsigned char& value)
   {
      for (unsigned char* itr = (data_ + offset(color)); itr < (data_ + length_); (*itr) += value, itr += bytes_per_pixel_);
   }

   inline void convert_to_grayscale()
   {
      double r_scaler = 0.299;
      double g_scaler = 0.587;
      double b_scaler = 0.114;

      if (rgb_mode == channel_mode_)
      {
         double tmp = r_scaler;
         r_scaler =  b_scaler;
         b_scaler = tmp;
      }

      for (unsigned char* itr = data_; itr < (data_ + length_);)
      {
         unsigned char gray_value = static_cast<unsigned char>(
         (r_scaler * (*(itr + 2))) +
         (g_scaler * (*(itr + 1))) +
         (b_scaler * (*(itr + 0))) );
         *(itr++) = gray_value;
         *(itr++) = gray_value;
         *(itr++) = gray_value;
      }
   }

   inline const unsigned char* data()
   {
      return data_;
   }

   inline void bgr_to_rgb()
   {
      if ((bgr_mode == channel_mode_) && (3 == bytes_per_pixel_))
      {
         reverse_channels();
         channel_mode_ = rgb_mode;
      }
   }

   inline void rgb_to_bgr()
   {
      if ((rgb_mode == channel_mode_) && (3 == bytes_per_pixel_))
      {
         reverse_channels();
         channel_mode_ = bgr_mode;
      }
   }

   inline void reverse()
   {
      unsigned char* itr1 = data_;
      unsigned char* itr2 = (data_ + length_) - bytes_per_pixel_;

      while (itr1 < itr2)
      {
         for (std::size_t i = 0; i < bytes_per_pixel_; ++i)
         {
            unsigned char* citr1 = itr1 + i;
            unsigned char* citr2 = itr2 + i;
            unsigned char tmp = *citr1;
            *citr1 = *citr2;
            *citr2 = tmp;
         }
         itr1 += bytes_per_pixel_;
         itr2 -= bytes_per_pixel_;
      }
   }

   inline void horizontal_flip()
   {
      for (unsigned int y = 0; y < height_; ++y)
      {
         unsigned char* itr1 = row(y);
         unsigned char* itr2 = itr1 + row_increment_ - bytes_per_pixel_;

         while (itr1 < itr2)
         {
            for (unsigned int i = 0; i < bytes_per_pixel_; ++i)
            {
               unsigned char* p1 = (itr1 + i);
               unsigned char* p2 = (itr2 + i);
               unsigned char tmp = *p1;
               *p1 = *p2;
               *p2 = tmp;
            }
            itr1 += bytes_per_pixel_;
            itr2 -= bytes_per_pixel_;
         }
      }
   }

   inline void vertical_flip()
   {
      for (unsigned int y = 0; y < (height_ / 2); ++y)
      {
         unsigned char* itr1 = row(y);
         unsigned char* itr2 = row(height_ - y - 1);

         for (std::size_t x = 0; x < row_increment_; ++x)
         {
            unsigned char tmp = *(itr1 + x);
            *(itr1 + x) = *(itr2 + x);
            *(itr2 + x) = tmp;
         }
      }
   }

   inline void export_color_plane(const color_plane color, unsigned char* image)
   {
      for (unsigned char* itr = (data_ + offset(color)); itr < (data_ + length_); ++image, itr += bytes_per_pixel_)
      {
         (*image) = (*itr);
      }
   }

   inline void export_color_plane(const color_plane color, bitmap_image& image)
   {
      if ((width_ != image.width_) || (height_ != image.height_))
      {
         image.setwidth_height(width_,height_);
      }

      image.clear();
      unsigned char* itr1     = (data_ + offset(color));
      unsigned char* itr1_end = (data_ + length_);
      unsigned char* itr2     = (image.data_ + offset(color));

      while (itr1 < itr1_end)
      {
         (*itr2) = (*itr1);
         itr1 += bytes_per_pixel_;
         itr2 += bytes_per_pixel_;
      }
   }

   inline void export_response_image(const color_plane color, double* response_image)
   {
      for (unsigned char* itr = (data_ + offset(color)); itr < (data_ + length_); ++response_image, itr += bytes_per_pixel_)
      {
         (*response_image) = (1.0 * (*itr)) / 256.0;
      }
   }

   inline void export_gray_scale_response_image(double* response_image)
   {
      for (unsigned char* itr = data_; itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         unsigned char gray_value = static_cast<unsigned char>((0.299 * (*(itr + 2))) +
                                                               (0.587 * (*(itr + 1))) +
                                                               (0.114 * (*(itr + 0))));
         (*response_image) = (1.0 * gray_value) / 256.0;
      }
   }

   inline void export_rgb(double* red, double* green, double* blue) const
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         (*blue)  = (1.0 * (*(itr++))) / 256.0;
         (*green) = (1.0 * (*(itr++))) / 256.0;
         (*red)   = (1.0 * (*(itr++))) / 256.0;
      }
   }

   inline void export_rgb(float* red, float* green, float* blue) const
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         (*blue)  = (1.0f * (*(itr++))) / 256.0f;
         (*green) = (1.0f * (*(itr++))) / 256.0f;
         (*red)   = (1.0f * (*(itr++))) / 256.0f;
      }
   }

   inline void export_rgb(unsigned char* red, unsigned char* green, unsigned char* blue) const
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         (*blue)  = *(itr++);
         (*green) = *(itr++);
         (*red)   = *(itr++);
      }
   }

   inline void export_ycbcr(double* y, double* cb, double* cr)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++y, ++cb, ++cr)
      {
         double blue  = (1.0 * (*(itr++)));
         double green = (1.0 * (*(itr++)));
         double red   = (1.0 * (*(itr++)));
         ( *y) = clamp<double>( 16.0 + (1.0/256.0) * (  65.738 * red + 129.057 * green +  25.064 * blue),1.0,254);
         (*cb) = clamp<double>(128.0 + (1.0/256.0) * (- 37.945 * red -  74.494 * green + 112.439 * blue),1.0,254);
         (*cr) = clamp<double>(128.0 + (1.0/256.0) * ( 112.439 * red -  94.154 * green -  18.285 * blue),1.0,254);
      }
   }

   inline void export_rgb_normal(double* red, double* green, double* blue) const
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         (*blue)  = (1.0 * (*(itr++)));
         (*green) = (1.0 * (*(itr++)));
         (*red)   = (1.0 * (*(itr++)));
      }
   }

   inline void export_rgb_normal(float* red, float* green, float* blue) const
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         (*blue)  = (1.0f * (*(itr++)));
         (*green) = (1.0f * (*(itr++)));
         (*red)   = (1.0f * (*(itr++)));
      }
   }

   inline void import_rgb(double* red, double* green, double* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(256.0 * (*blue) );
         *(itr++) = static_cast<unsigned char>(256.0 * (*green));
         *(itr++) = static_cast<unsigned char>(256.0 * (*red)  );
      }
   }

   inline void import_rgb(float* red, float* green, float* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(256.0f * (*blue) );
         *(itr++) = static_cast<unsigned char>(256.0f * (*green));
         *(itr++) = static_cast<unsigned char>(256.0f * (*red)  );
      }
   }

   inline void import_rgb(unsigned char* red, unsigned char* green, unsigned char* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = (*blue );
         *(itr++) = (*green);
         *(itr++) = (*red  );
      }
   }

   inline void import_ycbcr(double* y, double* cb, double* cr)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++y, ++cb, ++cr)
      {
         double y_  =  (*y);
         double cb_ = (*cb);
         double cr_ = (*cr);
         *(itr++) = static_cast<unsigned char>(clamp((298.082 * y_ + 516.412 * cb_                 ) / 256.000 - 276.836,0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp((298.082 * y_ - 100.291 * cb_ - 208.120 * cr_ ) / 256.000 + 135.576,0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp((298.082 * y_                 + 408.583 * cr_ ) / 256.000 - 222.921,0.0,255.0));
      }
   }

   inline void import_rgb_clamped(double* red, double* green, double* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0 * (*blue) ,0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0 * (*green),0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0 * (*red)  ,0.0,255.0));
      }
   }

   inline void import_rgb_clamped(float* red, float* green, float* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0f * (*blue) ,0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0f * (*green),0.0,255.0));
         *(itr++) = static_cast<unsigned char>(clamp<double>(256.0f * (*red)  ,0.0,255.0));
      }
   }

   inline void import_rgb_normal(double* red, double* green, double* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(*blue );
         *(itr++) = static_cast<unsigned char>(*green);
         *(itr++) = static_cast<unsigned char>(*red  );
      }
   }

   inline void import_rgb_normal(float* red, float* green, float* blue)
   {
      if (bgr_mode != channel_mode_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); ++red, ++green, ++blue)
      {
         *(itr++) = static_cast<unsigned char>(*blue );
         *(itr++) = static_cast<unsigned char>(*green);
         *(itr++) = static_cast<unsigned char>(*red  );
      }
   }

   inline void subsample(bitmap_image& dest)
   {
      /*
         Half sub-sample of original image.
      */
      unsigned int w = 0;
      unsigned int h = 0;

      bool odd_width = false;
      bool odd_height = false;

      if (0 == (width_ % 2))
         w = width_ / 2;
      else
      {
         w = 1 + (width_ / 2);
         odd_width = true;
      }

      if (0 == (height_ % 2))
         h = height_ / 2;
      else
      {
         h = 1 + (height_ / 2);
         odd_height = true;
      }

      unsigned int horizontal_upper = (odd_width)  ? w - 1 : w;
      unsigned int vertical_upper   = (odd_height) ? h - 1 : h;

      dest.setwidth_height(w,h);
      dest.clear();

            unsigned char* s_itr[3];
      const unsigned char*  itr1[3];
      const unsigned char*  itr2[3];

      s_itr[0] = dest.data_ + 0;
      s_itr[1] = dest.data_ + 1;
      s_itr[2] = dest.data_ + 2;

      itr1[0] = data_ + 0;
      itr1[1] = data_ + 1;
      itr1[2] = data_ + 2;

      itr2[0] = data_ + row_increment_ + 0;
      itr2[1] = data_ + row_increment_ + 1;
      itr2[2] = data_ + row_increment_ + 2;

      unsigned int total = 0;
      for (unsigned int j = 0; j < vertical_upper; ++j)
      {
         for (unsigned int i = 0; i < horizontal_upper; ++i)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               total = 0;
               total += *(itr1[k]); itr1[k] += bytes_per_pixel_;
               total += *(itr1[k]); itr1[k] += bytes_per_pixel_;
               total += *(itr2[k]); itr2[k] += bytes_per_pixel_;
               total += *(itr2[k]); itr2[k] += bytes_per_pixel_;
               *(s_itr[k]) = static_cast<unsigned char>(total >> 2);
            }
         }
         if (odd_width)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               total = 0;
               total += *(itr1[k]); itr1[k] += bytes_per_pixel_;
               total += *(itr2[k]); itr2[k] += bytes_per_pixel_;
               *(s_itr[k]) = static_cast<unsigned char>(total >> 1);
            }
         }

         for (unsigned int k = 0; k < bytes_per_pixel_; itr1[k] += row_increment_, ++k);

         if (j != (vertical_upper - 1))
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; itr2[k] += row_increment_, ++k);
         }
      }
      if (odd_height)
      {
         for (unsigned int i = 0; i < horizontal_upper; ++i)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               total = 0;
               total += *(itr1[k]); itr1[k] += bytes_per_pixel_;
               total += *(itr2[k]); itr2[k] += bytes_per_pixel_;
               *(s_itr[k]) = static_cast<unsigned char>(total >> 1);
            }
         }
         if (odd_width)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; ++k)
            {
               (*(s_itr[k])) = *(itr1[k]);
            }
         }
      }
   }

   inline void upsample(bitmap_image& dest)
   {
      /*
         2x up-sample of original image.
      */

      dest.setwidth_height(2 * width_ ,2 * height_);
      dest.clear();

      const unsigned char* s_itr[3];
            unsigned char*  itr1[3];
            unsigned char*  itr2[3];

      s_itr[0] = data_ + 0;
      s_itr[1] = data_ + 1;
      s_itr[2] = data_ + 2;

      itr1[0] = dest.data_ + 0;
      itr1[1] = dest.data_ + 1;
      itr1[2] = dest.data_ + 2;

      itr2[0] = dest.data_ + dest.row_increment_ + 0;
      itr2[1] = dest.data_ + dest.row_increment_ + 1;
      itr2[2] = dest.data_ + dest.row_increment_ + 2;

      for (unsigned int j = 0; j < height_; ++j)
      {
         for (unsigned int i = 0; i < width_; ++i)
         {
            for (unsigned int k = 0; k < bytes_per_pixel_; s_itr[k] += bytes_per_pixel_, ++k)
            {
               *(itr1[k]) = *(s_itr[k]); itr1[k] += bytes_per_pixel_;
               *(itr1[k]) = *(s_itr[k]); itr1[k] += bytes_per_pixel_;

               *(itr2[k]) = *(s_itr[k]); itr2[k] += bytes_per_pixel_;
               *(itr2[k]) = *(s_itr[k]); itr2[k] += bytes_per_pixel_;
            }
         }
         for (unsigned int k = 0; k < bytes_per_pixel_; ++k)
         {
            itr1[k] += dest.row_increment_;
            itr2[k] += dest.row_increment_;
         }
      }
   }

   inline void alpha_blend(const double& alpha, const bitmap_image& image)
   {
      if ((image.width_ != width_) || (image.height_ != height_))
      {
         return;
      }

      if ((alpha < 0.0) || (alpha > 1.0))
      {
         return;
      }

      unsigned char* itr1     = data_;
      unsigned char* itr1_end = data_ + length_;
      unsigned char* itr2     = image.data_;

      double alpha_compliment = 1.0 - alpha;

      while (itr1 != itr1_end)
      {
         *(itr1) = static_cast<unsigned char>((alpha * (*itr2)) + (alpha_compliment * (*itr1)));
         *(itr1++);
         *(itr2++);
      }
   }

   inline double psnr(const bitmap_image& image)
   {
      if ((image.width_ != width_) || (image.height_ != height_))
      {
         return 0.0;
      }

      unsigned char* itr1 = data_;
      unsigned char* itr2 = image.data_;

      double mse = 0.0;

      while (itr1 != (data_ + length_))
      {
         double v = (static_cast<double>(*itr1) - static_cast<double>(*itr2));
         mse += v * v;
         ++itr1;
         ++itr2;
      }
      if (mse <= 0.0000001)
      {
         return 1000000.0;
      }
      else
      {
         mse /= (3.0 * width_ * height_);
         return 20.0 * std::log10(255.0 / std::sqrt(mse));
      }
   }

   inline double psnr(const unsigned int& x,
               const unsigned int& y,
               const bitmap_image& image)
   {
      if ((x + image.width()) > width_)   { return 0.0; }
      if ((y + image.height()) > height_) { return 0.0; }

      double mse = 0.0;

      for (unsigned int r = 0; r < image.height(); ++r)
      {
         unsigned char* itr1       = row(r + y) + x * bytes_per_pixel_;
         unsigned char* itr1_end   = itr1 + (image.width() * bytes_per_pixel_);
         const unsigned char* itr2 = image.row(r);
         while (itr1 != itr1_end)
         {
            double v = (static_cast<double>(*itr1) - static_cast<double>(*itr2));
            mse += v * v;
            ++itr1;
            ++itr2;
         }
      }
      if (mse <= 0.0000001)
      {
         return 1000000.0;
      }
      else
      {
         mse /= (3.0 * image.width() * image.height());
         return 20.0 * std::log10(255.0 / std::sqrt(mse));
      }
   }

   inline void histogram(const color_plane color, double hist[256])
   {
      std::fill(hist,hist + 256,0.0);
      for (unsigned char* itr = (data_ + offset(color)); itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         ++hist[(*itr)];
      }
   }

   inline void histogram_normalized(const color_plane color, double hist[256])
   {
      histogram(color,hist);
      double* h_itr = hist;
      const double* h_end = hist + 256;
      const double pixel_count = static_cast<double>(width_ * height_);
      while (h_end != h_itr) { *(h_itr++) /= pixel_count; }
   }

   inline unsigned int offset(const color_plane color)
   {
      switch(channel_mode_)
      {
         case rgb_mode : {
                            switch (color)
                            {
                               case red_plane   : return 0;
                               case green_plane : return 1;
                               case blue_plane  : return 2;
                               default          : return std::numeric_limits<unsigned int>::max();
                            }
                         }

         case bgr_mode : {
                            switch (color)
                            {
                               case red_plane   : return 2;
                               case green_plane : return 1;
                               case blue_plane  : return 0;
                               default          : return std::numeric_limits<unsigned int>::max();
                            }
                         }
         default       : return std::numeric_limits<unsigned int>::max();
      }
   }

   inline void incremental()
   {
      unsigned char current_color = 0;
      for (unsigned char* itr = data_; itr < (data_ + length_);)
      {
         (*itr++) = (current_color);
         (*itr++) = (current_color);
         (*itr++) = (current_color);
         ++current_color;
      }
   }

private:

   void create_bitmap()
   {
      length_ = width_ * height_ * bytes_per_pixel_;
      row_increment_ = width_ * bytes_per_pixel_;
      if (0 != data_)
      {
         delete[] data_;
      }
      data_ = new unsigned char[length_];
      valid_ = true;
   }

   void load_bitmap()
   {
      std::ifstream stream(file_name_.c_str(),std::ios::binary);
      if (!stream)
      {
         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - file " << file_name_ << " not found!" << std::endl;
		 throw "Invalid file";
         return;
      }

      bitmap_file_header bfh;
      bitmap_information_header bih;

      read_bfh(stream,bfh);
      read_bih(stream,bih);

      if (bfh.type != 19778)
      {
         stream.close();
         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - Invalid type value " << bfh.type << " expected 19778." << std::endl;
         return;
      }

      if (bih.bit_count != 24)
      {
         stream.close();
         std::cerr << "bitmap_image::load_bitmap() ERROR: bitmap_image - Invalid bit depth " << bih.bit_count << " expected 24." << std::endl;
         return;
      }

      height_ = bih.height;
      width_  = bih.width;

      bytes_per_pixel_ = bih.bit_count >> 3;

      unsigned int padding = (4 - ((3 * width_) % 4)) % 4;
      char padding_data[4] = {0,0,0,0};

      create_bitmap();

      for (unsigned int i = 0; i < height_; ++i)
      {
         unsigned char* data_ptr = row(height_ - i - 1); // read in inverted row order
         stream.read(reinterpret_cast<char*>(data_ptr),sizeof(char) * bytes_per_pixel_ * width_);
         stream.read(padding_data,padding);
      }

      valid_ = true;
   }

   inline void reverse_channels()
   {
      if (3 != bytes_per_pixel_) return;
      for (unsigned char* itr = data_; itr < (data_ + length_); itr += bytes_per_pixel_)
      {
         unsigned char tmp = *(itr + 0);
         *(itr + 0) = *(itr + 2);
         *(itr + 2) = tmp;
      }
   }

   template<typename T>
   T clamp(const T& v, const T& lower_range, const T& upper_range)
   {
      if (v < lower_range)
          return lower_range;
      else if (v >  upper_range)
         return upper_range;
      else
         return v;
   }

   bool           valid_;
   std::string    file_name_;
   unsigned char* data_;
   unsigned int   bytes_per_pixel_;
   unsigned int   length_;
   unsigned int   width_;
   unsigned int   height_;
   unsigned int   row_increment_;
   channel_mode   channel_mode_;
};

inline bool big_endian()
{
   unsigned int v = 0x01;
   return (1 != reinterpret_cast<char*>(&v)[0]);
}

inline unsigned short flip(const unsigned short& v)
{
   return ((v >> 8) | (v << 8));
}

inline unsigned int flip(const unsigned int& v)
{
   return (((v & 0xFF000000) >> 0x18) |
           ((v & 0x000000FF) << 0x18) |
           ((v & 0x00FF0000) >> 0x08) |
           ((v & 0x0000FF00) << 0x08));
}

template<typename T>
inline void read_from_stream(std::ifstream& stream,T& t)
{
   stream.read(reinterpret_cast<char*>(&t),sizeof(T));
}

template<typename T>
inline void write_to_stream(std::ofstream& stream,const T& t)
{
   stream.write(reinterpret_cast<const char*>(&t),sizeof(T));
}

inline void read_bfh(std::ifstream& stream, bitmap_file_header& bfh)
{
   read_from_stream(stream,bfh.type);
   read_from_stream(stream,bfh.size);
   read_from_stream(stream,bfh.reserved1);
   read_from_stream(stream,bfh.reserved2);
   read_from_stream(stream,bfh.off_bits);

   if (big_endian())
   {
      flip(bfh.type);
      flip(bfh.size);
      flip(bfh.reserved1);
      flip(bfh.reserved2);
      flip(bfh.off_bits);
   }
}

inline void write_bfh(std::ofstream& stream, const bitmap_file_header& bfh)
{
   if (big_endian())
   {
      flip(bfh.type);
      flip(bfh.size);
      flip(bfh.reserved1);
      flip(bfh.reserved2);
      flip(bfh.off_bits);
   }

   write_to_stream(stream,bfh.type);
   write_to_stream(stream,bfh.size);
   write_to_stream(stream,bfh.reserved1);
   write_to_stream(stream,bfh.reserved2);
   write_to_stream(stream,bfh.off_bits);
}

inline void read_bih(std::ifstream& stream,bitmap_information_header& bih)
{
   read_from_stream(stream,bih.size);
   read_from_stream(stream,bih.width);
   read_from_stream(stream,bih.height);
   read_from_stream(stream,bih.planes);
   read_from_stream(stream,bih.bit_count);
   read_from_stream(stream,bih.compression);
   read_from_stream(stream,bih.size_image);
   read_from_stream(stream,bih.x_pels_per_meter);
   read_from_stream(stream,bih.y_pels_per_meter);
   read_from_stream(stream,bih.clr_used);
   read_from_stream(stream,bih.clr_important);
   if (big_endian())
   {
      flip(bih.size);
      flip(bih.width);
      flip(bih.height);
      flip(bih.planes);
      flip(bih.bit_count);
      flip(bih.compression);
      flip(bih.size_image);
      flip(bih.x_pels_per_meter);
      flip(bih.y_pels_per_meter);
      flip(bih.clr_used);
      flip(bih.clr_important);
   }
}

inline void write_bih(std::ofstream& stream, const bitmap_information_header& bih)
{
   if (big_endian())
   {
      flip(bih.size);
      flip(bih.width);
      flip(bih.height);
      flip(bih.planes);
      flip(bih.bit_count);
      flip(bih.compression);
      flip(bih.size_image);
      flip(bih.x_pels_per_meter);
      flip(bih.y_pels_per_meter);
      flip(bih.clr_used);
      flip(bih.clr_important);
   }
   write_to_stream(stream,bih.size);
   write_to_stream(stream,bih.width);
   write_to_stream(stream,bih.height);
   write_to_stream(stream,bih.planes);
   write_to_stream(stream,bih.bit_count);
   write_to_stream(stream,bih.compression);
   write_to_stream(stream,bih.size_image);
   write_to_stream(stream,bih.x_pels_per_meter);
   write_to_stream(stream,bih.y_pels_per_meter);
   write_to_stream(stream,bih.clr_used);
   write_to_stream(stream,bih.clr_important);
}

struct rgb_store
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
};

inline void rgb_to_ycbcr(const unsigned int& length, double* red, double* green, double* blue,
                                                     double* y,   double* cb,    double* cr)
{
   unsigned int i = 0;
   while (i < length)
   {
      ( *y) =   16.0 + (  65.481 * (*red) +  128.553 * (*green) +  24.966 * (*blue));
      (*cb) =  128.0 + ( -37.797 * (*red) +  -74.203 * (*green) + 112.000 * (*blue));
      (*cr) =  128.0 + ( 112.000 * (*red) +  -93.786 * (*green) -  18.214 * (*blue));
      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}

inline void ycbcr_to_rgb(const unsigned int& length, double* y,   double* cb,    double* cr,
                                                     double* red, double* green, double* blue)
{
   unsigned int i = 0;
   while (i < length)
   {
      double y_  =  (*y) -  16.0;
      double cb_ = (*cb) - 128.0;
      double cr_ = (*cr) - 128.0;

        (*red) = 0.000456621 * y_                    + 0.00625893 * cr_;
      (*green) = 0.000456621 * y_ - 0.00153632 * cb_ - 0.00318811 * cr_;
       (*blue) = 0.000456621 * y_                    + 0.00791071 * cb_;

      ++i;
      ++red; ++green; ++blue;
      ++y;   ++cb;    ++cr;
   }
}

inline void subsample(const unsigned int& width,
                      const unsigned int& height,
                      const double* source,
                      unsigned int& w,
                      unsigned int& h,
                      double** dest)
{
   /*  Single channel.  */

   w = 0;
   h = 0;

   bool odd_width = false;
   bool odd_height = false;

   if (0 == (width % 2))
      w = width / 2;
   else
   {
      w = 1 + (width / 2);
      odd_width = true;
   }

   if (0 == (height % 2))
      h = height / 2;
   else
   {
      h = 1 + (height / 2);
      odd_height = true;
   }
   unsigned int horizontal_upper = (odd_width)  ? w - 1 : w;
   unsigned int vertical_upper   = (odd_height) ? h - 1 : h;

   *dest = new double[w * h];

   double* s_itr = *dest;
   const double* itr1 = source;
   const double* itr2 = source + width;

   for (unsigned int j = 0; j < vertical_upper; ++j)
   {
      for (unsigned int i = 0; i < horizontal_upper; ++i, ++s_itr)
      {
          (*s_itr)  = *(itr1++);
          (*s_itr) += *(itr1++);
          (*s_itr) += *(itr2++);
          (*s_itr) += *(itr2++);
          (*s_itr) /=  4.0;
      }
      if (odd_width)
      {
         (*(s_itr++)) = ( (*itr1++) + (*itr2++) ) / 2.0;
      }

      itr1 += width;
      if (j != (vertical_upper -1))
      {
         itr2 += width;
      }
   }
   if (odd_height)
   {
      for (unsigned int i = 0; i < horizontal_upper; ++i, ++s_itr)
      {
         (*s_itr) += (*(itr1++));
         (*s_itr) += (*(itr1++));
         (*s_itr) /= 2.0;
      }
      if (odd_width)
      {
         (*(s_itr++)) = (*itr1);
      }
   }
}

inline void upsample(const unsigned int& width,
                     const unsigned int& height,
                     const double* source,
                     unsigned int& w,
                     unsigned int& h,
                     double** dest)
{
   /* Single channel. */

   w = 2 * width;
   h = 2 * height;

   *dest = new double[w * h];

   const double* s_itr = source;
         double* itr1  = *dest;
         double* itr2  = *dest + w;

   for (unsigned int j = 0; j < height; ++j)
   {
      for (unsigned int i = 0; i < width; ++i, ++s_itr)
      {
          *(itr1++) = (*s_itr);
          *(itr1++) = (*s_itr);
          *(itr2++) = (*s_itr);
          *(itr2++) = (*s_itr);
      }
      itr1 += w;
      itr2 += w;
   }
}



void checkered_pattern(const unsigned int x_width,
                       const unsigned int y_width,
                       const unsigned char value,
                       const bitmap_image::color_plane color,
                             bitmap_image& image);

void plasma(bitmap_image& image,
            const double& x,     const double& y,
            const double& width, const double& height,
            const double& c1,    const double& c2,
            const double& c3,    const double& c4,
            const double& roughness = 3.0,
            const rgb_store colormap[] = 0);

double psnr_region(const unsigned int& x,     const unsigned int& y,
                   const unsigned int& width, const unsigned int& height,
                   const bitmap_image& image1, const bitmap_image& image2);

void hierarchical_psnr_r(const double& x,     const double& y,
                         const double& width, const double& height,
                         const bitmap_image& image1, bitmap_image& image2, const double& threshold,
                         const rgb_store colormap[]);

void hierarchical_psnr(bitmap_image& image1,bitmap_image& image2, const double threshold, const rgb_store colormap[]);



class image_drawer
{
public:

   image_drawer(bitmap_image& image)
   : image_(image),
     pen_width_(1),
     pen_color_red_(0),
     pen_color_green_(0),
     pen_color_blue_(0){};

   void rectangle(int x1, int y1, int x2, int y2)
   {
      line_segment(x1,y1,x2,y1);
      line_segment(x2,y1,x2,y2);
      line_segment(x2,y2,x1,y2);
      line_segment(x1,y2,x1,y1);
   }

   void triangle(int x1, int y1, int x2, int y2,int x3, int y3)
   {
      line_segment(x1,y1,x2,y2);
      line_segment(x2,y2,x3,y3);
      line_segment(x3,y3,x1,y1);
   }

   void quadix(int x1, int y1, int x2, int y2,int x3, int y3, int x4, int y4)
   {
      line_segment(x1,y1,x2,y2);
      line_segment(x2,y2,x3,y3);
      line_segment(x3,y3,x4,y4);
      line_segment(x4,y4,x1,y1);
   }

   void line_segment(int x1, int y1, int x2, int y2)
   {
      int steep = 0;
      int sx    = ((x2 - x1) > 0) ? 1 : -1;
      int sy    = ((y2 - y1) > 0) ? 1 : -1;
      int dx    = abs(x2 - x1);
      int dy    = abs(y2 - y1);

      if (dy > dx)
      {
         steep = x1;  x1 = y1;  y1 = steep;  /* swap x1 and y1 */
         steep = dx;  dx = dy;  dy = steep;  /* swap dx and dy */
         steep = sx;  sx = sy;  sy = steep;  /* swap sx and sy */
         steep = 1;
      }

      int e = 2 * dy - dx;

      for (int i = 0; i < dx; ++i)
      {
         if (steep)
            plot_pen_pixel(y1,x1);
         else
            plot_pen_pixel(x1,y1);

         while (e >= 0)
         {
            y1 += sy;
            e -= (dx << 1);
         }
         x1 += sx;
         e  += (dy << 1);
      }
      plot_pen_pixel(x2,y2);
   }

   void horiztonal_line_segment(int x1, int x2, int y)
   {
      if (x1 > x2)
      {
         std::swap(x1,x2);
      }

      for (int i = 0; i < (x2 - x1); ++i)
      {
         plot_pen_pixel(x1 +  i,y);
      }
   }

   void vertical_line_segment(int y1, int y2, int x)
   {
      if (y1 > y2)
      {
         std::swap(y1,y2);
      }

      for (int i = 0; i < (y2 - y1); ++i)
      {
         plot_pen_pixel(x, y1 +  i);
      }
   }

   void ellipse(int centerx, int centery, int a, int b)
   {
      int t1 = a * a;
      int t2 = t1 << 1;
      int t3 = t2 << 1;
      int t4 = b * b;
      int t5 = t4 << 1;
      int t6 = t5 << 1;
      int t7 = a * t5;
      int t8 = t7 << 1;
      int t9 = 0;

      int d1 = t2 - t7 + (t4 >> 1);
      int d2 = (t1 >> 1) - t8 + t5;
      int x  = a;
      int y  = 0;

      int negative_tx = centerx - x;
      int positive_tx = centerx + x;
      int negative_ty = centery - y;
      int positive_ty = centery + y;

      while (d2 < 0)
      {
         plot_pen_pixel(positive_tx,positive_ty);
         plot_pen_pixel(positive_tx,negative_ty);
         plot_pen_pixel(negative_tx,positive_ty);
         plot_pen_pixel(negative_tx,negative_ty);

         ++y;

         t9 = t9 + t3;

         if (d1 < 0)
         {
            d1 = d1 + t9 + t2;
            d2 = d2 + t9;
         }
         else
         {
            x--;
            t8 = t8 - t6;
            d1 = d1 + (t9 + t2 - t8);
            d2 = d2 + (t9 + t5 - t8);
            negative_tx = centerx - x;
            positive_tx = centerx + x;
         }

         negative_ty = centery - y;
         positive_ty = centery + y;
      }

      do
      {
         plot_pen_pixel(positive_tx,positive_ty);
         plot_pen_pixel(positive_tx,negative_ty);
         plot_pen_pixel(negative_tx,positive_ty);
         plot_pen_pixel(negative_tx,negative_ty);

         x--;
         t8 = t8 - t6;

         if (d2 < 0)
         {
            ++y;
            t9 = t9 + t3;
            d2 = d2 + (t9 + t5 - t8);
            negative_ty = centery - y;
            positive_ty = centery + y;
         }
         else
            d2 = d2 + (t5 - t8);

         negative_tx = centerx - x;
         positive_tx = centerx + x;
      }
      while (x >= 0);
   }

   void circle(int centerx, int centery, int radius)
   {
      int x = 0;
      int d = (1 - radius) << 1;

      while (radius >= 0)
      {
         plot_pen_pixel(centerx + x,centery + radius);
         plot_pen_pixel(centerx + x,centery - radius);
         plot_pen_pixel(centerx - x,centery + radius);
         plot_pen_pixel(centerx - x,centery - radius);

         if ((d + radius) > 0)
            d -= ((--radius) << 1) - 1;
         if (x > d)
            d += ((++x) << 1) + 1;
      }
   }

   void plot_pen_pixel(int x, int y)
   {
      switch (pen_width_)
      {
         case 1  : plot_pixel(x,y);
                   break;

         case 2  : {
                      plot_pixel(x    , y    );
                      plot_pixel(x + 1, y    );
                      plot_pixel(x + 1, y + 1);
                      plot_pixel(x    , y + 1);
                   }
                   break;

         case  3 : {
                      plot_pixel(x    , y - 1);
                      plot_pixel(x - 1, y - 1);
                      plot_pixel(x + 1, y - 1);

                      plot_pixel(x    , y    );
                      plot_pixel(x - 1, y    );
                      plot_pixel(x + 1, y    );

                      plot_pixel(x    , y + 1);
                      plot_pixel(x - 1, y + 1);
                      plot_pixel(x + 1, y + 1);
                   }
                   break;

         default : plot_pixel(x,y);
                   break;
      }
   }

   void plot_pixel(int x, int y)
   {
      image_.set_pixel(x,y,pen_color_red_,pen_color_green_,pen_color_blue_);
   }

   void pen_width(const unsigned int& width)
   {
      if ((width > 0) && (width < 4))
      {
         pen_width_ = width;
      }
   }

   void pen_color(const unsigned char& red,
                  const unsigned char& green,
                  const unsigned char& blue)
   {
      pen_color_red_   = red;
      pen_color_green_ = green;
      pen_color_blue_  = blue;
   }

private:

   image_drawer(const image_drawer& id);
   image_drawer& operator =(const image_drawer& id);

   bitmap_image& image_;
   unsigned int  pen_width_;
   unsigned char pen_color_red_;
   unsigned char pen_color_green_;
   unsigned char pen_color_blue_;
};

extern const rgb_store autumn_colormap[1000];

extern const rgb_store copper_colormap[1000];

extern const rgb_store gray_colormap[1000];

extern const rgb_store hot_colormap[1000];

extern const rgb_store hsv_colormap[1000];

extern const rgb_store jet_colormap[1000];

extern const rgb_store prism_colormap[1000];

extern const rgb_store vga_colormap[1000];

extern const rgb_store yarg_colormap[1000];

#endif
