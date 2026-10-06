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


namespace UI {
  inline std::ostream& error(std::ostream& os)   { return os << "\033[1;31m"; }
  inline std::ostream& clear(std::ostream& os)   { return os << "\033[H\033[J"; }
  inline std::ostream& reset(std::ostream& os)   { return os << "\033[0m"; }
  inline std::ostream& bold(std::ostream& os)    { return os << "\033[1m"; }
  inline std::ostream& cyan(std::ostream& os)    { return os << "\033[1;36m"; }
  inline std::ostream& green(std::ostream& os)   { return os << "\033[1;32m"; }
  inline std::ostream& yellow(std::ostream& os)  { return os << "\033[1;33m"; }
  inline std::ostream& gray(std::ostream& os)    { return os << "\033[90m"; }
}

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
concept merge_filter_type = requires(F filter, Image& image, Image& image2) { filter(image, image2);};
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
    Filter(const F& filter, Image& image2): filter([filter, image2] (Image& image) mutable {
      filter(image, image2);
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
      float multiplier = 1 - brightness_level/255.0f;
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

    static void merge(Image& image1, Image& image2){
      if(image1.height>image2.height || image1.width>image2.width){
        resize(image1,image2.width,image2.height);
      }
      
      if(image1.height<image2.height || image1.width<image2.width){
        resize(image2,image1.width,image1.height);
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
      }
      throw std::invalid_argument("Error: This filter does not accept radius parameters.");
    }

    Filter generate_filter(Filters filter_type, int x, int y, int w, int h) {
      if (filter_type == Filters::CROP) {
        return Filter(crop, x, y, w, h);
      }
      throw std::invalid_argument("Error: This filter does not accept these parameters.");
    }

    Filter generate_filter(Filters filter_type, Image& image) {
      if (filter_type == Filters::MERGE) {
        return Filter(merge, image);
      }
      throw std::invalid_argument("Error: This filter does not accept these parameters.");
    }

    static const Filterable_image create_filterable_image(std::string filename) {
      return Filterable_image(filename);
    }
};

bool filename_exists(const std::string& user_input) {
  std::filesystem::path filepath(user_input);
  return std::filesystem::exists(filepath) && !filepath.has_parent_path();
}

bool is_valid_option_choice(int choice) {
  return choice < 1 || choice > 5 ? false : true;
}

bool is_valid_filter_choice(int choice) {
  return choice < 1 || choice > 18 ? false : true;
}

void display_option_menu(bool error, std::string message = "") {
  std::cout << UI::clear;

  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "         IMAGE FILTER APP        " << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n\n";

  if (message != "" && !error) std::cout << UI::green << message << UI::reset << "\n\n";
  else if (message != "" && error) std::cout << UI::error << message << UI::reset << "\n\n";

  std::cout << UI::bold << "Please select an option:\n" << UI::reset;
  std::cout << UI::green << "1." << UI::reset << " Load new image\n";
  std::cout << UI::green << "2." << UI::reset << " Apply filter\n";
  std::cout << UI::green << "3." << UI::reset << " Save image\n";
  std::cout << UI::green << "4." << UI::reset << " Reset Image to Original State\n";
  std::cout << UI::yellow << "5." << UI::reset << " Exit program\n\n";
  
  std::cout << UI::bold << "Enter choice number: " << UI::reset;
}

void display_load_image_menu(bool error) {
  std::cout << UI::clear;

  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "         LOAD NEW IMAGE          " << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n\n";

  error && std::cout << UI::error << "No Image With That Filename Exists In Current Directory" << UI::reset << "\n\n";

  std::cout << UI::bold << "Enter the image filename only and make sure its in the current directory\n" << UI::reset;
  std::cout << "Examples: " << UI::green << "image.ppm" << UI::reset << ", " << UI::green << "data/photo.pgm" << UI::reset << "\n\n";

  std::cout << UI::bold << "Path: " << UI::reset;
}

void display_filter_choice_menu(bool error, const Filterable_image& active_image, const std::string message = "") { 
  std::cout << UI::clear;

  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "        APPLY IMAGE FILTER       " << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n";
  std::cout << "Active Image: " << UI::green << active_image.get_filename() << UI::reset << "\n\n";

  if (message != "" && !error) std::cout << UI::green << message << UI::reset << "\n\n";
  error && std::cout << UI::error << "Previous Option Was Invalid" << UI::reset << "\n\n";

  std::cout << UI::bold << "Select a filter to apply:\n" << UI::reset;
  std::cout << UI::green << "1."  << UI::reset << " Invert Colors\n";
  std::cout << UI::green << "2."  << UI::reset << " Flip Horizontally\n";
  std::cout << UI::green << "3."  << UI::reset << " Flip Vertically\n";
  std::cout << UI::green << "4."  << UI::reset << " Grayscale\n";
  std::cout << UI::green << "5."  << UI::reset << " Black & White\n";
  std::cout << UI::green << "6."  << UI::reset << " Purple Tint\n";
  std::cout << UI::green << "7."  << UI::reset << " Infrared\n";
  std::cout << UI::green << "8."  << UI::reset << " Old Television\n";
  std::cout << UI::green << "9."  << UI::reset << " Lighten Brightness\n";
  std::cout << UI::green << "10." << UI::reset << " Darken Brightness\n";
  std::cout << UI::green << "11." << UI::reset << " Rotate Image\n";
  std::cout << UI::green << "12." << UI::reset << " Sunny\n";
  std::cout << UI::green << "13." << UI::reset << " Merge 2 Images\n";
  std::cout << UI::green << "14." << UI::reset << " Resize\n";
  std::cout << UI::green << "15." << UI::reset << " Blur\n";
  std::cout << UI::green << "16." << UI::reset << " Edge Detection\n";
  std::cout << UI::green << "17." << UI::reset << " Crop Image\n";
  std::cout << UI::yellow << "18." << UI::reset << " Cancel (Back to Main Menu)\n\n";

  std::cout << UI::bold << "Enter filter choice: " << UI::reset;
}

void display_save_image_menu() {
  std::cout << UI::clear;
  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "           Save IMAGE            " << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n\n";
}

void handle_image_save(Filterable_image& active_image) {
  std::string filename;
  while (true) {
    std::cout << UI::bold << "Enter new filename to save the image to (e.g., 'output.ppm'): " << UI::reset;
    std::cin >> filename;

    std::filesystem::path p(filename);

    if (p.has_parent_path()) {
      std::cout << UI::error << "ERROR: paths are not allowed! " << UI::reset << "Type just the filename.\n\n";
      continue; 
    } 
    
    if (filename.empty()) {
      std::cout << UI::error << "ERROR: Filename cannot be empty.\n\n" << UI::reset;
      continue;
    }

    if (filename_exists(filename)) {
      std::cout << "\n" << UI::yellow << "WARNING: A file named '" << filename << "' already exists!" << UI::reset << "\n";
      std::cout << "Do you want to overwrite it? (" << UI::green << "Y" << UI::reset << "/" << UI::error << "N" << UI::reset << "): ";

      char overwrite_choice;
      std::cin >> overwrite_choice;

      if (overwrite_choice != 'y' && overwrite_choice != 'Y') {
        std::cout << "\nOperation cancelled. try a different filename.\n\n";
        continue; 
      }
    }

    break; 
  }

  Image image = active_image.get_image();

  image.saveImage(filename);
}

std::string get_image_filename () {
  std::string filename;
  std::cin >> filename;

  while (!filename_exists(filename)) {
    display_load_image_menu(true);
    std::cin >> filename;
  }

  return filename;
}

int get_selected_choice() {
  int choice = 0;
  std::cin >> choice;

  while (!is_valid_option_choice(choice)) {
    display_option_menu(true, "Previous Option Was Invalid");
    std::cin >> choice;
  }

  return choice;
}

int get_selected_filter_choice(const Filterable_image& active_image) {
  int filter_choice = 0;
  std::cin >> filter_choice;

  while (!is_valid_filter_choice(filter_choice)) {
    display_filter_choice_menu(true, active_image);
    std::cin >> filter_choice;
  }
  
  return filter_choice;
}

Filters map_choice_to_filter(int choice) {
  switch (choice) {
    case 1:  return Filters::INVERT;
    case 2:  return Filters::FLIP_HORIZONTAL;
    case 3:  return Filters::FLIP_VERTICAL;
    case 4:  return Filters::GRAY_SCALE;
    case 5:  return Filters::BLACK_AND_WHITE;
    case 6:  return Filters::PURPLE;
    case 7:  return Filters::INFRARED;
    case 8:  return Filters::TV;
    case 9:  return Filters::LIGHT;
    case 10: return Filters::DARK;
    case 11: return Filters::ROTATE;
    case 12: return Filters::SUNNY;
    case 13: return Filters::MERGE;
    case 14: return Filters::RESIZE;
    case 15: return Filters::BLUR;
    case 16: return Filters::EDGE_DETECTION;
    case 17: return Filters::CROP;
    default: throw std::invalid_argument("Out of bounds filter index");
  }
}

std::string map_choice_to_filter_name(int choice) {
  switch (choice) {
    case 1:  return "Invert";
    case 2:  return "Horizontal Flip";
    case 3:  return "Vertical Flip";
    case 4:  return "Gray Scale";
    case 5:  return "Black And White";
    case 6:  return "Purple Tint";
    case 7:  return "Infrared";
    case 8:  return "TV";
    case 9:  return "Light";
    case 10: return "Dark";
    case 11: return "Rotate";
    case 12: return "Sunny";
    case 13: return "Merge";
    case 14: return "Resize";
    case 15: return "Blur";
    case 16: return "Edge Detection";
    case 17: return "Crop";
    default: throw std::invalid_argument("Out of bounds filter index");
  }
}


void process_filter_selection(Filters choice, Filterable_image& active_image, Image_processor& processor) {
  switch (choice) {
    case Filters::INVERT:
    case Filters::FLIP_HORIZONTAL:
    case Filters::FLIP_VERTICAL:
    case Filters::GRAY_SCALE:
    case Filters::BLACK_AND_WHITE:
    case Filters::PURPLE:
    case Filters::INFRARED:
    case Filters::TV:
    case Filters::SUNNY:
    case Filters::EDGE_DETECTION: {
      processor.generate_filter(choice).apply_to_image(active_image.get_image());
      break;
    }

    case Filters::LIGHT: {
      std::cout << UI::bold << "\nEnter brightness level (0 - 255): " << UI::reset;
      int input_level = 0;
      std::cin >> input_level;
      uint8_t level = static_cast<uint8_t>(std::clamp(input_level, 0, 255));
      
      processor.generate_filter(choice, level).apply_to_image(active_image.get_image());
      break;
    }

    case Filters::DARK: {
      std::cout << UI::bold << "\nEnter darkness scale factor (0 - 255): " << UI::reset;
      int input_level = 0;
      std::cin >> input_level;
      uint8_t level = static_cast<uint8_t>(std::clamp(input_level, 0, 255));
      
      processor.generate_filter(choice, level).apply_to_image(active_image.get_image());
      break;
    }

    case Filters::ROTATE: {
      std::cout << UI::bold << "\nSelect rotation angle:\n" << UI::reset;
      std::cout << " 1. 90 Degrees\n 2. 180 Degrees\n 3. 270 Degrees\n";
      std::cout << UI::bold << "Choice: " << UI::reset;
      int deg_choice = 0;
      std::cin >> deg_choice;

      Deg degree = Deg::DEG90;
      if (deg_choice == 2) degree = Deg::DEG180;
      if (deg_choice == 3) degree = Deg::DEG270;

      processor.generate_filter(choice, degree).apply_to_image(active_image.get_image());
      break;
    }

    case Filters::MERGE: {
      std::string second_filename;
      std::filesystem::path second_path;

      while (true) {
        std::cout << UI::clear;
        std::cout << UI::cyan << UI::bold << "=== MERGE IMAGE ===" << UI::reset << "\n\n";
        std::cout << "Enter the name of the SECOND image file to merge with: ";
        std::cin >> second_filename;

        second_path = std::filesystem::path(second_filename);

        if (second_path.has_parent_path()) {
          std::cout << UI::error << "\nERROR: paths are not Allowed! " << UI::reset << "Type just the local filename (e.g., 'overlay.ppm').\n";
          std::cout << "\nPress Enter to try a different name...";
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          std::cin.get();
          continue; 
        }

        if (!std::filesystem::exists(second_path)) {
          std::cout << UI::error << "\nERROR: The file '" << second_filename << "' does not exist!" << UI::reset << "\n";
          std::cout << "Please make sure it is placed inside the current project folder.\n";
          std::cout << "\nPress Enter to try a different name...";
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          std::cin.get();
          continue;
        }

        break;
      }

      Image second_image(second_filename);
      processor.generate_filter(choice, second_image).apply_to_image(active_image.get_image());
      std::cout << UI::green << "\n[Queue] Added safe Merge Filter (Source: " << second_filename << ")." << UI::reset << "\n";
      break;
    }

    case Filters::RESIZE: {
      int new_w = 0, new_h = 0;
      
      int current_w = active_image.get_image().width;
      int current_h = active_image.get_image().height;

      while (true) {
        std::cout << UI::clear;
        std::cout << UI::cyan << UI::bold << "=== RESIZE IMAGE ===" << UI::reset << "\n";
        std::cout << "Current Dimensions: " << UI::green << current_w << " x " << current_h << UI::reset << " pixels\n\n";

        std::cout << UI::bold << "Enter New Width (Pixels): " << UI::reset;
        std::cin >> new_w;
        std::cout << UI::bold << "Enter New Height (Pixels): " << UI::reset;
        std::cin >> new_h;

        if (new_w > 0 && new_h > 0) {
          break;
        }
        std::cout << "\n" << UI::error << "ERROR: Invalid scale boundaries!" << UI::reset << "\n";
        std::cout << "• Dimensions must be positive whole numbers greater than 0.\n";
        std::cout << "• You entered: " << new_w << " x " << new_h << "\n";
        std::cout << "\nPress Enter to try specify scale dimensions again...";
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
      }

      active_image.add_filter(processor.generate_filter(choice, new_w, new_h));
      std::cout << UI::green << "\n[Queue] Added safe Resize Filter (" << new_w << "x" << new_h << ")." << UI::reset << "\n";
      break;
    }

    case Filters::BLUR: {
      std::cout << UI::bold << "\nEnter Horizontal Blur Radius (X): " << UI::reset;
      int x_rad = 0; std::cin >> x_rad;
      std::cout << UI::bold << "Enter Vertical Blur Radius (Y): " << UI::reset;
      int y_rad = 0; std::cin >> y_rad;

      processor.generate_filter(choice, x_rad, y_rad).apply_to_image(active_image.get_image());
      break;
    }

    case Filters::CROP: {
      int x = 0, y = 0, w = 0, h = 0;
      int max_w = active_image.get_image().width;
      int max_h = active_image.get_image().height;

      while (true) {
        std::cout << UI::clear;
        std::cout << UI::cyan << UI::bold << "=== CROP IMAGE ===" << UI::reset << "\n";
        std::cout << "Image Dimensions: " << UI::green << max_w << " x " << max_h << UI::reset << " pixels\n\n";

        std::cout << UI::bold << "Enter Crop Start Coordinates (X Y): " << UI::reset;
        std::cin >> x >> y;
        std::cout << UI::bold << "Enter Box Dimensions (Width Height): " << UI::reset;
        std::cin >> w >> h;

        if (x >= 0 && y >= 0 && w > 0 && h > 0 && (x + w) <= max_w && (y + h) <= max_h) {
          break; 
        }

        std::cout << "\n" << UI::error << "ERROR: Crop bounds are out of range!" << UI::reset << "\n";
        std::cout << "• Your box reached: X max = " << (x + w) << " (Limit: " << max_w << ")\n";
        std::cout << "• Your box reached: Y max = " << (y + h) << " (Limit: " << max_h << ")\n";
        std::cout << "\nPress Enter to re-specify valid crop dimensions...";
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
      }

      processor.generate_filter(choice, x, y, w, h).apply_to_image(active_image.get_image());
      break;
    }

    default:
      std::cout << UI::yellow << "\nFilter generation route missing from instance methods." << UI::reset << "\n";
      break;
  }
}

void run_application_loop() {
  Image image;
  Filterable_image active_image;
  Image_processor processor;
  bool has_unsaved_changes = false;
  int choice = 0;
  int filter_choice;
  std::string message;
  std::string filename;
  bool is_error_message = false;
  bool stop_app = false;
  while (!stop_app) {
    switch (choice) {
      case 0:
        display_option_menu(is_error_message, message);
        choice = get_selected_choice();
        message = "";
        is_error_message = false;
        continue;
      case 1:
        if (has_unsaved_changes) {
          std::cout << "\n" << UI::yellow << "WARNING: You have unsaved changes in your workspace!" << UI::reset << "\n";
          std::cout << "Loading a new file will completely discard your current filters.\n";
          std::cout << "Are you sure you want to proceed? (" << UI::green << "Y" << UI::reset << "/" << UI::error << "N" << UI::reset << "): ";
          
          char confirm; std::cin >> confirm;
          if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nOperation aborted. Returning to main menu workspace.\n";
            break;
          }
        }
        display_load_image_menu(false);
        filename = get_image_filename();
        image = Image(filename);
        active_image = Filterable_image(image, filename);
        message = "Image Loaded Successfully";
        break;
      case 2:
        if (active_image.get_filename().empty()) {
          message = "No Image Loaded To Apply A Filter To";
          is_error_message = true;
          choice = 0;
          continue;
        }
        display_filter_choice_menu(false, active_image, message);
        filter_choice = get_selected_filter_choice(active_image);
        if (filter_choice == 18) {
          choice = 0;
          message = "";
        } else {
          process_filter_selection(map_choice_to_filter(filter_choice), active_image, processor);
          message = map_choice_to_filter_name(filter_choice) + " Filter Applied Successfully";
          has_unsaved_changes = true;
        }
        continue;
      case 3:
        if (active_image.get_filename().empty()) {
          message = "No Image Loaded To Save";
          is_error_message = true;
          break;
        }
        display_save_image_menu();
        handle_image_save(active_image);
        message = "Image Saved Successfully";
        has_unsaved_changes = false;

        break;
      case 4:
        if (active_image.get_filename().empty()) {
          message = "No Image Loaded To Reset";
          is_error_message = true;
          choice = 0;
          continue;
        }
        if (has_unsaved_changes) {
          std::cout << "\n" << UI::yellow << "WARNING: You have unsaved changes in your workspace!" << UI::reset << "\n";
          std::cout << "Loading a new file will completely discard your current filters.\n";
          std::cout << "Are you sure you want to proceed? (" << UI::green << "Y" << UI::reset << "/" << UI::error << "N" << UI::reset << "): ";
          
          char confirm; std::cin >> confirm;
          if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nOperation aborted. Returning to main menu workspace.\n";
            break;
          }
        }
        active_image = Filterable_image(image, filename);
        message = "Image Reseted Successfully";
        break;
      case 5:
        if (has_unsaved_changes) {
          std::cout << "\n" << UI::yellow << "WARNING: You have unsaved changes in your workspace!" << UI::reset << "\n";
          std::cout << "Loading a new file will completely discard your current filters.\n";
          std::cout << "Are you sure you want to proceed? (" << UI::green << "Y" << UI::reset << "/" << UI::error << "N" << UI::reset << "): ";
          
          char confirm; std::cin >> confirm;
          if (confirm != 'y' && confirm != 'Y') {
            std::cout << "\nOperation aborted. Returning to main menu workspace.\n";
            break;
          }
        }
        stop_app = true;
    }
    choice = 0;
  }
}