// file name: CS112_A3_Part2B_S15,16_20230211_20230100_20230246.cpp
// description: the program has 17 filters, user choose one or more of them to apply in his photo.
// Team info :
// Ahmed Shiref Farouk Mohamed    ID: 20250033      S:15,16
// Eyad Waleed Ibrahim Basit      ID: 20250101      S:15,16
// Hazem Tariq Mohamed Hussain    ID: 20250176      S:15,16
// Omar Ehab Maher Mohamed        ID: 20250425      S:15,16
// Filters (Grayscale & Flip Image & Merge Images & Sunny)            made by Ahmed Shiref Farouk Mohamed
// Filters (Black And White & Rotate Image & Edge Detection & TV)     made by Eyad Waleed Ibrahim Basit
// Filters (invert IMage & Darken And Lighten & Crop Image & Purple)  made by Hazem Tariq Mohamed Hussain
// Filters (invert IMage & Resize Image & Blur Image & Infrared)      made by Omar Ehab Maher Mohamed
#include "Image_Class.h"
#include <functional>
#include <stdexcept> 
#include <vector>
#include <algorithm>
#include <type_traits>
#include <fstream>
#include <filesystem>
#include <cmath>


// ignore this class, this is for gui.

// class my_image {
//   private:
//     std::vector<unsigned char*> images;
//     Image current_image;
//     void free_image(unsigned char* imageData) {
//       stbi_image_free(imageData);
//     }
//   public:
//     void load_image(std::string filename) {
//       if (current_image.imageData != nullptr) {
//         images.push_back(current_image.imageData);
//         current_image.imageData = nullptr;
//       }
//       current_image.loadNewImage(filename);
//     }
// };

// Append new filter types here using UPPER_CASE naming conventions. Access via Filters::ENUM_NAME.

enum class Filters{
  INVERT,
  FLIP_HORIZONTAL,
  FLIP_VERTICAL,
  GRAY_SCALE,
  BLACK_AND_WHITE,
  PURPLE,
  INFRARED,
  TV,
  LIGHT,
  DARK,
  ROTATE,
  SUNNY,
  MERGE,
  RESIZE,
  BLUR,
  EDGE_DETECTION,
  CROP,
  FRAME,
  OIL,
  SKEW,
  FANCY,
};

enum class Deg {
  DEG90,
  DEG180,
  DEG270,
};

template<typename F>
concept standard_filter_type = requires(F filter, Image& image) { filter(image);};
template<typename F>
concept brightness_filter_type = requires(F filter, Image& image, uint8_t brightness_level) { filter(image, brightness_level);};
template<typename F>
concept rotate_filter_type = requires(F filter, Image& image, Deg deg) { filter(image, deg);};
template<typename F>
concept blur_filter_type = requires(F filter, Image& image, int x_radius, int y_radius) { filter(image, x_radius, y_radius);};
template<typename F>
concept crop_filter_type = requires(F filter, Image& image, int x, int y, int w, int h) { filter(image, x, y, w, h);};
template<typename F>
concept merge_filter_type = requires(F filter, Image& image, Image& image2, int option) { filter(image, image2, option);};
template<typename F>
concept skew_filter_type = requires(F filter, Image* image, double angle_deg) { filter(image, angle_deg);};
class Filter {
  private: 
    std::function<void(Image&)> filter;

  public: 
    template<standard_filter_type F>
    Filter(const F& filter): filter([filter] (Image& image) {
      filter(image);
    }) {}

    template<brightness_filter_type F>
    Filter(const F& filter, uint8_t brightness_level): filter([filter, brightness_level](Image& image) {
      filter(image, brightness_level);
    }) {}

    template<rotate_filter_type F>
    Filter(const F& filter, Deg deg): filter([filter, deg] (Image& image) {
      filter(image, deg);
    }) {}

    template<blur_filter_type F>
    Filter(const F& filter, int x_radius, int y_radius): filter([filter, x_radius, y_radius] (Image& image) {
      filter(image, x_radius, y_radius);
    }) {}
    
    template<crop_filter_type F>
    Filter(const F& filter, int x, int y, int w, int h): filter([filter, x, y, w, h] (Image& image) {
      filter(image, x, y, w, h);
    }) {}
    
    template<merge_filter_type F>
    Filter(const F& filter, Image& image2, int option): filter([filter, image2, option] (Image& image) mutable {
      filter(image, image2, option);
    }) {}

    template<skew_filter_type F>
    Filter(const F& filter, double angle_deg): filter([filter, angle_deg] (Image& image) {
      filter(image, angle_deg);
    }) {}

    void apply_to_image(Image& image) const {
      filter(image);
    }
};

class Filterable_image {
  private:
    Image image;
    std::string filename = "";
    std::vector<Filter> queued_filters;

  public:
    Filterable_image() = default;
    Filterable_image(std::string filename): image(filename), filename(filename) {}
    Filterable_image(Image& image, std::string filename): image(image), filename(filename) {}

    Image& apply_filters(std::vector<Filter> &filters) {
      for (const auto& f : filters) {
        f.apply_to_image(image);
      }
      return image;
    }

    Image& apply_filters() {
      for (const auto& f : queued_filters) {
        f.apply_to_image(image);
      }
      queued_filters.clear();
      return image;
    }

    Filterable_image& add_filter(Filter filter) {
      queued_filters.push_back(filter);
      return *this;
    }

    std::string get_filename() const {
      return filename;
    }

    Image& get_image() {
      return image;
    }
};

class Image_processor {
  private: 
    static void invert(Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i++) {
        image.imageData[i] = ~image.imageData[i];
      }
    }
    
  static void crop(Image& image, int x, int y, int w, int h){
    Image cropped_image(w, h);
      for (int i = 0; i < w; i++){
        for (int j = 0; j < h; j++){
          for (int k = 0; k < 3; k++){
            cropped_image(i,j,k) = image(i+x,j+y,k);
        };
        
      };
    };
    image = cropped_image;
  }

    static void light(Image& image, uint8_t brightness_level) {
      float multiplier = brightness_level/255.0f + 1;
      for(int i=0;i<(image.height*image.channels*image.width);i+=image.channels){
        int r=  image.imageData[i]*multiplier;
        int g = image.imageData[i+1]*multiplier;
        int b = image.imageData[i+2]*multiplier;

        image.imageData[i]= static_cast<uint8_t>(std::clamp(r,0,255));
        image.imageData[i+1] = static_cast<uint8_t>(std::clamp(g,0,255));
        image.imageData[i+2] = static_cast<uint8_t>(std::clamp(b,0,255));
      }
    }

    static void dark(Image& image, uint8_t brightness_level) {
      float multiplier = 1.0f / (brightness_level/255.0f + 1.0f);
      for(int i=0;i<(image.height*image.channels*image.width);i+=image.channels){
        int r =  image.imageData[i]*multiplier;
        int g =  image.imageData[i+1]*multiplier;
        int b =  image.imageData[i+2]*multiplier;

        image.imageData[i]= static_cast<uint8_t>(std::clamp(r,0,255));
        image.imageData[i+1] = static_cast<uint8_t>(std::clamp(g,0,255));
        image.imageData[i+2] = static_cast<uint8_t>(std::clamp(b,0,255));
      }
    }

    static void flip_horizontal(Image& image) {
      int channels = image.channels;
      int row_length = image.width * channels;

      for (int i = 0; i < image.height; i++) {
        int l = 0, r = (image.width - 1) * image.channels;
        int start_index = row_length * i;
        while (l < r) {

          for (int c = 0; c < channels; c++) {
            int left_pixle_channel_index = start_index + l + c;
            int right_pixle_channel_index = start_index + r + c;

            int temp = image.imageData[left_pixle_channel_index];
            image.imageData[left_pixle_channel_index] = image.imageData[right_pixle_channel_index];
            image.imageData[right_pixle_channel_index] = temp;
          }

          l += channels;
          r -= channels;
        }
      }
    }

    static void flip_vertical(Image& image) {
      int channels = image.channels;
      int row_length = image.width * channels;

      for (int i = 0; i < row_length; i++) {
        int l = i, r = i + (image.height - 1) * row_length;
        while (l < r) {
            int temp = image.imageData[l];
            image.imageData[l] = image.imageData[r];
            image.imageData[r] = temp;
          l += row_length;
          r -= row_length;
        }
      }
    }

    static int get_brightness(Image& image, int pixle_index) {
      int r = image.imageData[pixle_index];
      int g = image.imageData[pixle_index+1];
      int b = image.imageData[pixle_index+2];
      return r*0.2126 + g*0.7152 + b*0.0722;
    }

    static void gray_scale(Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i+= image.channels) {
        int gray_value = get_brightness(image, i);
        image.imageData[i] = gray_value;
        image.imageData[i+1] = gray_value;
        image.imageData[i+2] = gray_value;
      }
    } 

    static void black_and_white(Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i+= image.channels) {
        int gray_value = get_brightness(image, i);
        int value = gray_value <= 127 ? 0 : 255;
        image.imageData[i] = value;
        image.imageData[i+1] = value;
        image.imageData[i+2] = value;
      }
    }

    static void purple(Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i += image.channels) { 
        // image.imageData[i] = std::min(255, image.imageData[i] + 90); 
        image.imageData[i+1] = std::max(0, image.imageData[i+1] - 60); 
        // image.imageData[i+2] = std::min(255, image.imageData[i+2] + 90); 
      }
    }

    static void infrared(Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i+= image.channels) {
        unsigned char& G = image.imageData[i+1];
        unsigned char& B = image.imageData[i+2];
        
        int brightness = get_brightness(image, i);
        int targetValue = 255 - brightness; 
        image.imageData[i] = 255;

        if (targetValue < 128) {
            float t = targetValue / 127.0f;
            G = t * 100;
            B = t * 110;
        } else {
            float t = (targetValue - 128) / 127.0f;
            G = 100 + t * (255 - 100);
            B = 110 + t * (255 - 110);
        }
      }
    }

    static void television(Image& image) {
      int row_length = image.channels * image.width;
      for (int i = 0; i < row_length * image.height; i+= image.channels) {
        unsigned char& R = image.imageData[i];
        unsigned char& G = image.imageData[i+1];
        unsigned char& B = image.imageData[i+2];
        int row = i / row_length;

        if (row % 2 == 0) {
          R = std::min(255, R + 20);
          G = std::min(255, G + 20);
          B = std::min(255, B + 20);
        } else {
          R = std::max(0, R - 20);
          G = std::max(0, G - 20);
          B = std::max(0, B - 20);
        }
        
      }
    }

    static void rotate(Image& image, Deg deg) {
      if (deg == Deg::DEG90) {
        Image rotated_image(image.height, image.width);
        for (int i = 0; i < image.width * image.height; i++) {
          int current_row = i / image.width;
          int pixle_in_row = i % image.width;
          int rotated_pixle_index = (rotated_image.width * (pixle_in_row + 1) - 1 - current_row) * image.channels;
          rotated_image.imageData[rotated_pixle_index] = image.imageData[i*3];
          rotated_image.imageData[rotated_pixle_index + 1] = image.imageData[i*3+1];
          rotated_image.imageData[rotated_pixle_index + 2] = image.imageData[i*3+2];
        }
        image = rotated_image;
      } else if (deg == Deg::DEG270) {
        Image rotated_image(image.height, image.width);
        for (int i = 0; i < image.width * image.height; i++) {
          int current_row = i / image.width;
          int pixle_in_row = i % image.width;
          int rotated_pixle_index = (rotated_image.width * (rotated_image.height - (pixle_in_row + 1)) + current_row) * image.channels;
          rotated_image.imageData[rotated_pixle_index] = image.imageData[i*3];
          rotated_image.imageData[rotated_pixle_index + 1] = image.imageData[i*3+1];
          rotated_image.imageData[rotated_pixle_index + 2] = image.imageData[i*3+2];
        }
        image = rotated_image;
      } else if (deg == Deg::DEG180) {
        Image rotated_image(image.width, image.height);
        for (int i = 0; i < image.width * image.height; i++) {
          int rotated_pixle_index = (rotated_image.width * rotated_image.height - (1 + i)) * image.channels;
          rotated_image.imageData[rotated_pixle_index] = image.imageData[i*3];
          rotated_image.imageData[rotated_pixle_index + 1] = image.imageData[i*3+1];
          rotated_image.imageData[rotated_pixle_index + 2] = image.imageData[i*3+2];
        }
        image = rotated_image;
      } else {
        throw std::invalid_argument("Unknown rotation degree provided.");
      }
    }

    static void resize(Image& image,int new_width,int new_height){
      
      Image result(new_width,new_height);

      float scaleX= static_cast<float>(image.width)/new_width;
      float scaleY= static_cast<float>(image.height)/new_height;

      for(int y=0;y<result.height;y++){
          int copiedY = std::min(static_cast<int>(scaleY*y),image.height-1);
        for(int x=0;x<result.width;x++){
          int copiedX = std::min(static_cast<int>(scaleX*x),image.width-1);
          int srcIdx = (copiedY*image.width+copiedX)*image.channels;
          int outIdx = (y*result.width+x)*result.channels;
          for(int c=0;c<result.channels;c++){
            result.imageData[outIdx+c] = image.imageData[srcIdx+c];
          }
        }
      }

      image = result;
    }

    static void merge(Image& image1, Image& image2, int option){
      if (option == 1){
        resize(image2,image1.width,image1.height);
      } else if(option == 2){
        resize(image1,image2.width,image2.height);
      } else if (option == 3) {
        int w = std::min(image1.width, image2.width);
        int h = std::min(image1.height, image2.height);
        resize(image1, w, h);
        resize(image2, w, h);
      }

      Image result_img(image1.width,image1.height);

      for(int y=0;y<result_img.height;y++) {
        for(int x=0;x<result_img.width;x++) {

          int idx1 = (y*image1.width+x)*image1.channels;
          int idx2 = (y*image2.width+x)*image2.channels;
          int outidx = (y*result_img.width+x)*result_img.channels;

          int blendedR = (image1.imageData[idx1]+image2.imageData[idx2])/2;
          int blendedG = (image1.imageData[idx1+1]+image2.imageData[idx2+1])/2;
          int blendedB = (image1.imageData[idx1+2]+image2.imageData[idx2+2])/2;

          result_img.imageData[outidx] = blendedR;
          result_img.imageData[outidx+1] = blendedG;
          result_img.imageData[outidx+2] = blendedB;

        }
      }

      image1 = result_img;
    }

    static void sunny_effect(Image& image){
      for(int i=0;i<(image.channels*image.height*image.width);i+=image.channels){
        float r = image.imageData[i];
        float g = image.imageData[i+1];
        float b = image.imageData[i+2];

        image.imageData[i] = (unsigned char)(std::clamp(r*1.15f,0.0f,255.0f));
        image.imageData[i+1] = (unsigned char)(std::clamp(g*1.05f,0.0f,255.0f));
        image.imageData[i+2] = (unsigned char)(std::clamp(b*0.85f,0.0f,255.0f));
      }
    }

    static void blur_vertical(Image& image, int radius) {
      Image output_image(image.width, image.height);
      int window_size = radius * 2 + 1;
      for (int i = 0; i < image.width * image.height; i++) {
        int current_row = i / image.width;
        int pixle_in_row = i % image.width;
        int current_pixle_index = (current_row * image.width + pixle_in_row) * image.channels;
        if (current_row < radius || current_row >= image.height - radius) {
          for (int channel = 0; channel < image.channels; channel++) {
            output_image.imageData[current_pixle_index + channel] = image.imageData[current_pixle_index + channel];
          }
        } else {
          for (int channel = 0; channel < image.channels; channel++) {
            int channel_blur_value = 0;
              for (int ky = -radius; ky <= radius; ky++) {
                int neighbor_index = ((current_row + ky) * image.width + pixle_in_row) * image.channels;
                channel_blur_value += image.imageData[neighbor_index + channel];
              }
            output_image.imageData[current_pixle_index + channel] = channel_blur_value / window_size;
          }
        }
      }
      image = output_image;
    }
    static void blur_horizontal(Image& image, int radius) {
      Image output_image(image.width, image.height);
      int window_size = radius * 2 + 1;
      for (int i = 0; i < image.width * image.height; i++) {
        int current_row = i / image.width;
        int pixle_in_row = i % image.width;
        int current_pixle_index = (current_row * image.width + pixle_in_row) * image.channels;
        if (pixle_in_row < radius || pixle_in_row >= image.width - radius) {
          for (int channel = 0; channel < image.channels; channel++) {
            output_image.imageData[current_pixle_index + channel] = image.imageData[current_pixle_index + channel];
          }
        } else {
          for (int channel = 0; channel < image.channels; channel++) {
            int channel_blur_value = 0;
            for (int kx = -radius; kx <= radius; kx++) {
              int neighbor_index = (current_row * image.width + pixle_in_row + kx) * image.channels;
              channel_blur_value += image.imageData[neighbor_index + channel];
            }
            output_image.imageData[current_pixle_index + channel] = channel_blur_value / window_size;
          }
        }
      }
      image = output_image;
    }

    static void blur(Image& image, int x_radius, int y_radius) {
      blur_horizontal(image, x_radius);
      blur_vertical(image, y_radius);
    }

    static void soft_blur(Image& image) {
      Image output_image(image.width, image.height);
      int kernel[3][3] = {
        {1, 2, 1},
        {2, 4, 2},
        {1, 2, 1},
      };

      for (int i = 0; i < image.width * image.height; i++) {
        int current_row = i / image.width;
        int pixle_in_row = i % image.width;
        int current_pixle_index = (current_row * image.width + pixle_in_row) * image.channels;
        if (current_row < 1 || current_row >= image.height - 1 || pixle_in_row < 1 || pixle_in_row >= image.width - 1) {
          for (int channel = 0; channel < image.channels; channel++) {
            output_image.imageData[current_pixle_index + channel] = image.imageData[current_pixle_index + channel];
          }
        } else {
          for (int channel = 0; channel < image.channels; channel++) {
            int channel_blur_value = 0;
            for (int ky = -1; ky <= 1; ky++) {
              for (int kx = -1; kx <= 1; kx++) {
                int neighbor_index = ((current_row + ky) * image.width + pixle_in_row + kx) * image.channels;
                int kernel_value = kernel[ky + 1][kx + 1];
                int channle_value = image.imageData[neighbor_index + channel];
                channel_blur_value += kernel_value * channle_value;
              }
            }
            output_image.imageData[current_pixle_index + channel] = channel_blur_value / 16;
          }
        }
      }
      image = output_image;
    }

    static void edge_detection(Image& image) {
      blur(image, 2, 2);
      // soft_blur(image);
      gray_scale(image);
      // black_and_white(image);
      Image output_image(image.width, image.height);
      int Gy[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1},
      };

      int Gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1},
      };

      for (int i = 0; i < image.width * image.height; i++) {
        int current_row = i / image.width;
        int pixle_in_row = i % image.width;
        int current_pixle_index = (current_row * image.width + pixle_in_row) * image.channels;
        if (current_row < 1 || current_row >= image.height - 1 || pixle_in_row < 1 || pixle_in_row >= image.width - 1) {
          for (int channel = 0; channel < image.channels; channel++) {
            output_image.imageData[current_pixle_index + channel] = image.imageData[current_pixle_index + channel];
          }
        } else {
          int accumelated_gx = 0;
          int accumelated_gy = 0;
          for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
              int neighbor_index = ((current_row + ky) * image.width + pixle_in_row + kx) * image.channels;
              int gx_value = Gx[ky + 1][kx + 1];
              int gy_value = Gy[ky + 1][kx + 1];
              int gray_value = get_brightness(image, neighbor_index);
              accumelated_gx += gx_value * gray_value;
              accumelated_gy += gy_value * gray_value;
            }
          }
          int total_edge_strength = std::abs(accumelated_gx) + std::abs(accumelated_gy);
          for (int channel = 0; channel < image.channels; channel++) {
            output_image.imageData[current_pixle_index + channel] = std::clamp(255 - total_edge_strength, 0, 255);
          }
        }
      }
      image = output_image;
    }

    static void frame(Image& image){
      int w = image.width + 2 * 20;  //because border is on both sides :)
      int h = image.height + 2 * 20;
      Image framed(w ,h);

      for (int x = 0; x < w; x++){
        for (int y = 0; y < h; y++){
          framed(x, y, 0) = 0;
          framed(x, y, 1) = 0;
          framed(x, y, 2) = 255;
        }
      }
      
      for (int i = 0; i < image.width; i++){
        for (int j = 0; j < image.height; j++){
          for (int k = 0; k < 3; k++){
            framed(i + 20, j + 20, k) = image(i,j,k);
          }
        }
      }

      image = framed;
    }

    static void oil(Image& image){
      Image result(image.width,image.height);
      const int groups = 20;
      int radius = 3; 

      for (int x = 0; x < image.width; x++){
        for (int y = 0; y < image.height; y++){
          std::vector<int> bucket_count(groups, 0); 
          std::vector<int> bucket_r(groups , 0);      
          std::vector<int> bucket_g(groups , 0);
          std::vector<int> bucket_b(groups , 0);

          for (int dx = -radius; dx <= radius; dx++){
            for (int dy = -radius; dy <= radius; dy++){
              int nx = x + dx;
              int ny = y + dy;
              if (nx < 0 || ny < 0 || nx >= image.width || ny >= image.height) continue; 
      
              int r = image(nx, ny, 0); 
              int g = image(nx, ny, 1);
              int b = image(nx, ny, 2);

              int intensity = (r + g + b) / 3;              
              int bucket_index = intensity * groups / 256;  

              bucket_count[bucket_index]++;
              bucket_r[bucket_index] += r;  
              bucket_g[bucket_index] += g;
              bucket_b[bucket_index] += b;
            }
          }

          int winner = 0;
          for (int b = 1; b < groups; b++){
            if (bucket_count[b] > bucket_count[winner]){
              winner = b;
            }
          }
          result(x, y, 0) = bucket_r[winner] / bucket_count[winner];
          result(x, y, 1) = bucket_g[winner] / bucket_count[winner];
          result(x, y, 2) = bucket_b[winner] / bucket_count[winner];  
        }
      }
      image = result;
    }

    static void skew_vertical(Image& image, double angle_deg) {
      double rad = angle_deg * (M_PI / 180.0);
      int shift = static_cast<int>(image.width * std::tan(std::abs(rad)));
      int new_height = image.height + shift;

      Image output_image(image.width, new_height);

      for (int x = 0; x < output_image.width; ++x) {
        for (int y = 0; y < output_image.height; ++y) {
          for (int c = 0; c < output_image.channels; ++c) {
            output_image(x, y, c) = 255;
          }
        }
      }

      for (int x = 0; x < image.width; ++x) {
        int y_shift = (angle_deg >= 0) ? static_cast<int>(x * std::tan(rad)) : static_cast<int>((image.width - 1 - x) * std::tan(std::abs(rad)));

        for (int y = 0; y < image.height; ++y) {
          int target_y = y + y_shift;
          if (target_y >= 0 && target_y < new_height) {
            for (int c = 0; c < image.channels; ++c) {
              output_image(x, target_y, c) = image(x, y, c);
            }
          }
        }
      }
      image = output_image;
    }

    static void skew_horizontal(Image& image, double angle_deg) {
      double rad = angle_deg * (M_PI / 180.0);
      double tan_angle = std::tan(rad);
      double abs_tan = std::abs(tan_angle);
      
      int shift = static_cast<int>(image.height * abs_tan);
      int new_width = image.width + shift;

      Image output_image(new_width, image.height);

      // LOOP OVER THE DESTINATION IMAGE
      for (int y = 0; y < output_image.height; ++y) {
        // Calculate the horizontal shift for this row based on the Y position
        // This elevates/shifts the bottom or top depending on the angle sign
        int x_shift = (angle_deg >= 0) ? static_cast<int>((image.height - 1 - y) * tan_angle) : static_cast<int>(y * abs_tan);

        for (int x = 0; x < output_image.width; ++x) {
          // Map BACKWARD horizontally to find the source pixel
          int src_x = x - x_shift;

          if (src_x >= 0 && src_x < image.width) {
            for (int c = 0; c < output_image.channels; ++c) {
              output_image(x, y, c) = image(src_x, y, c);
            }
          } else {
            // Background padding
            for (int c = 0; c < output_image.channels; ++c) {
              output_image(x, y, c) = 255; 
            }
          }
        }
      }
      image = output_image;
    }

    static void fancy(Image& image){
      int gap = 5;
      int line = 3;
      int w = image.width + 2 * 20;  
      int h = image.height + 2 * 20;
      Image framed(w ,h);

      for (int x = 0; x < w; x++){
        for (int y = 0; y < h; y++){
          framed(x, y, 0) = 0;
          framed(x, y, 1) = 0;
          framed(x, y, 2) = 255;
        }
      }

      for (int i = 0; i < image.width; i++){
        for (int j = 0; j < image.height; j++){
          int d = std::min(std::min(i, j), std::min(image.width - 1 - i, image.height - 1 - j));
          bool on_line = d >= gap && d < gap + line;
          for (int k = 0; k < 3; k++){
            framed(i + 20, j + 20, k) = on_line ? 255 : image(i, j, k);
          }
        }
      }
      image = framed;
    }

  public: 
    Filter generate_filter(Filters filter_type) {
      switch (filter_type) {
        case Filters::INVERT: 
          return Filter(invert);
        case Filters::FLIP_HORIZONTAL:
          return Filter(flip_horizontal);
        case Filters::FLIP_VERTICAL: 
          return Filter(flip_vertical);
        case Filters::GRAY_SCALE:
          return Filter(gray_scale);
        case Filters::BLACK_AND_WHITE: 
          return Filter(black_and_white);
        case Filters::PURPLE: 
          return Filter(purple);
        case Filters::INFRARED:
          return Filter(infrared);
        case Filters::TV:
          return Filter(television);
        case Filters::SUNNY:
          return Filter(sunny_effect);
        case Filters::EDGE_DETECTION:
          return Filter(edge_detection);
        case Filters::OIL:
          return Filter(oil);
        case Filters::FRAME:
          return Filter(frame);
        case Filters::FANCY:
          return Filter(fancy);
        case Filters::LIGHT:
        case Filters::DARK:
        case Filters::ROTATE:
        case Filters::CROP:
          throw std::invalid_argument("Error: This filter requires configuration arguments.");
        default:
          throw std::invalid_argument("Unknown filter type provided.");
      }
    }

    Filter generate_filter(Filters filter_type, uint8_t brightness_level) {
      switch (filter_type) {
        case Filters::LIGHT:
          return Filter(light, brightness_level);
        case Filters::DARK:
          return Filter(dark, brightness_level);
        default: 
          throw std::invalid_argument("Error: This filter does not accept a brightness parameter.");
      }
    }

    Filter generate_filter(Filters filter_type, Deg rotation_degree) {
      if (filter_type == Filters::ROTATE) {
        return Filter(rotate, rotation_degree);
      }
      throw std::invalid_argument("Error: This filter does not accept a rotation parameter.");
    }

    Filter generate_filter(Filters filter_type, int x_radius, int y_radius) {
      if (filter_type == Filters::BLUR) {
        return Filter(blur, x_radius, y_radius);
      } else if (filter_type == Filters::RESIZE) {
        return Filter(resize, x_radius, y_radius);
      } else throw std::invalid_argument("Error: This filter does not accept radius parameters.");
    }

    Filter generate_filter(Filters filter_type, int x, int y, int w, int h) {
      if (filter_type == Filters::CROP) {
        return Filter(crop, x, y, w, h);
      }
      throw std::invalid_argument("Error: This filter does not accept these parameters.");
    }

    Filter generate_filter(Filters filter_type, Image& image, int option) {
      if (filter_type == Filters::MERGE) {
        return Filter(merge, image, option);
      }
      throw std::invalid_argument("Error: This filter does not accept these parameters.");
    }

    Filter generate_filter(Filters filter_type, double angle) {
      if (filter_type == Filters::SKEW) {
        return Filter(skew_horizontal, angle);
      } else {
        throw std::invalid_argument("Error: This filter does not accept an angle parameter.");
      }
    }

    static const Filterable_image create_filterable_image(std::string filename) {
      return Filterable_image(filename);
    }
};