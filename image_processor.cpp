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

// Append new filter types here using UPPER_CASE naming conventions. Access via Filters::ENUM_NAME.

namespace UI {
  // Basic terminal formatting commands
  inline std::ostream& error(std::ostream& os)   { return os << "\033[1;31m"; }
  inline std::ostream& clear(std::ostream& os)   { return os << "\033[H\033[J"; }
  inline std::ostream& reset(std::ostream& os)   { return os << "\033[0m"; }
  inline std::ostream& bold(std::ostream& os)    { return os << "\033[1m"; }
  inline std::ostream& cyan(std::ostream& os)    { return os << "\033[1;36m"; }
  inline std::ostream& green(std::ostream& os)   { return os << "\033[1;32m"; }
  inline std::ostream& yellow(std::ostream& os)  { return os << "\033[1;33m"; }
  inline std::ostream& gray(std::ostream& os)    { return os << "\033[90m"; }
}

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
  CROPING,
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
    // I could make use of this later
    // Filterable_Image& remove_filter() {
    //   queued_filters.pop_back();
    // }
};

class Image_processor {
  private: 
    // Put your filter here as it is, but use the keyword before it.
    static void invert(Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i++) {
        image.imageData[i] = ~image.imageData[i];
      }
    }
    

     // hazem tariq 20250176
    
  static Image croping(Image& image, int x, int y, int w, int h){
    Image cropped(w, h);
      for (int i = 0; i < w; i++){
        for (int j = 0; j < h; j++){
          for (int k = 0; k < 3; k++){
            cropped(i,j,k) = image(i+x,j+y,k);
        };
        
      };
    };
    return cropped;

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


    // Ahmed Shiref 20250033
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

    //Ahmed Shiref Farouk 20250033
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

    // Ahmed Shiref Farouk 20250033
    static Image merge(Image& image1, Image& image2){
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

      return result_img;
    }

    // Ahmed Shiref Farouk 20250033
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
        case Filters::CROPING:
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

    static const Filterable_image create_filterable_image(std::string filename) {
      return Filterable_image(filename);
    }
};


bool filename_exists(const std::string& user_input) {
  std::filesystem::path filepath(user_input);
  return std::filesystem::exists(filepath) && !filepath.has_parent_path();
}

bool is_valid_option_choice(int choice) {
  return choice < 1 || choice > 4 ? false : true;
}

bool is_valid_filter_choice(int choice) {
  return choice < 1 || choice > 5 ? false : true;
}

void display_option_menu(bool error, std::string message = "") {
  std::cout << UI::clear;

  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "         IMAGE FILTER APP        " << UI::reset << "\n";
  std::cout << UI::cyan << UI::bold << "=================================" << UI::reset << "\n\n";

  if (message != "" && !error) std::cout << UI::green << message << UI::reset << "\n\n";
  error && std::cout << UI::error << "Previous Option Was Invalid" << UI::reset << "\n\n";

  std::cout << UI::bold << "Please select an option:\n" << UI::reset;
  std::cout << UI::green << "1." << UI::reset << " Load new image\n";
  std::cout << UI::green << "2." << UI::reset << " Apply filter\n";
  std::cout << UI::green << "3." << UI::reset << " Save results to disk\n";
  std::cout << UI::yellow << "4." << UI::reset << " Exit program\n\n";
  
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
  std::cout << UI::green << "1." << UI::reset << " Convert to Grayscale\n";
  std::cout << UI::green << "2." << UI::reset << " Invert Colors (Negative)\n";
  std::cout << UI::green << "3." << UI::reset << " Apply Gaussian Blur\n";
  std::cout << UI::green << "4." << UI::reset << " Sharpen Image\n";
  std::cout << UI::yellow << "5." << UI::reset << " Cancel (Back to Main Menu)\n\n";

  std::cout << UI::bold << "Enter filter choice: " << UI::reset;
}

void display_save_result_menu() {

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
    display_option_menu(true);
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

void run_application_loop() {
  Image current_image;
  Filterable_image active_image;
  int choice = 0;
  int filter_choice;
  std::string message;
  std::string filename;
  bool stop_app = false;
  while (!stop_app) {
    switch (choice) {
      case 0:
        display_option_menu(false, message);
        choice = get_selected_choice();
        message = "";
        continue;
      case 1:
        display_load_image_menu(false);
        filename = get_image_filename();
        current_image = Image(filename);
        active_image = Filterable_image(filename);
        message = "Image Loaded Successfully";
        break;
      case 2:
        display_filter_choice_menu(false, active_image, message);
        filter_choice = get_selected_filter_choice(active_image);
        if (filter_choice == 5) {
          choice = 0;
        } else {
          message = "Filter Applied Successfully";
        }
        continue;
      case 3:
        display_save_result_menu();
        message = "Image Saved Successfully";
        break;
      case 4:
        stop_app = true;
    }
    choice = 0;
  }
}

int main() {
  run_application_loop();

  // this is the first way of applying filters:
  // - it generates a single filter at a time and then apply it to an image;
  // - for multiple filters you need to repeat the expression for each filiter
  // - it mutates the original image passed to it and it doesn't return the image
  // - example:
  // --> processor.generate_filter(Filters::ROTATE, Deg::DEG180).apply_to_image(image);
  // --> image.saveImage("rotate180deg.jpg");

  // this is the second way of applying filters:
  // - it generates a filterable_image and you can apply multiple filters all at once;
  // - the apply_filters method accepts a vector of Filter instances that can be created through the generate_filter from the image_processor
  // - it doesn't modify the origianl image and returns a new image after applying the filters that is an instance from the original Image class so you can save it
  // - instead of passing a vector, you can also just do add method chaining and then in the end use the apply_filters with empty argumenst
  // - example:
  // --> Filterable_image img = Image_processor::create_filterable_image(image);
  // --> std::vector<Filter> filters({processor.generate_filter(Filters::GRAY_SCALE), processor.generate_filter(Filters::ROTATE, Deg::DEG90)});
  // --> img.apply_filters(filters).saveImage("gray-rotate90deg.jpg");
  // or
  // --> Filterable_image img = Image_processor::create_filterable_image(image);
  // --> img.add_filter(processor.generate_filter(Filters::GRAY_SCALE)).add_filter(processor.generate_filter(Filters::ROTATE, Deg::DEG90)).apply_filters();
}