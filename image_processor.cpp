#include "Image_Class.h"
#include <functional>
#include <stdexcept> 
#include <vector>
#include<algorithm>

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
};

enum class Deg {
  DEG90,
  DEG180,
  DEG270,
};

class Filter {
  private: 
    std::function<void(Image&)> filter;

  public: 
    template<typename Func, typename... Args>
    Filter(const Func& filter, Args... args): filter([filter, args...](Image& image) {
      filter(image, args...);
    }) {}

    void apply_to_image(Image& image) const {
      filter(image);
    }
};

class Filterable_image {
  private :
    Image image;
    std::vector<Filter> queued_filters;

  public :
    Filterable_image(const Image& image): image(image) {}

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

    Filterable_image& add_filter(Filter& filter) {
      queued_filters.push_back(filter);
      return *this;
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
    //Ahmed Shiref 20250033

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
    //Ahmed Shiref 20250033

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

    static int get_brightness(Image& image, int pixleIndex) {
      int r = image.imageData[pixleIndex];
      int g = image.imageData[pixleIndex+1];
      int b = image.imageData[pixleIndex+2];
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
        Image new_image(image.height, image.width);
        int row_length = new_image.width * new_image.channels;
        for (int i = 0; i < image.width * image.height; i++) {
          int current_row = i / image.width;
          int pixle_in_row = i % image.width;
          new_image.imageData[(new_image.width * (pixle_in_row + 1) - 1 - current_row) * image.channels] = image.imageData[i*3];
          new_image.imageData[(new_image.width * (pixle_in_row + 1) - 1 - current_row) * image.channels + 1] = image.imageData[i*3+1];
          new_image.imageData[(new_image.width * (pixle_in_row + 1) - 1 - current_row) * image.channels + 2] = image.imageData[i*3+2];
        }
        image = new_image;
      } else if (deg == Deg::DEG270) {
        Image new_image(image.height, image.width);
        int row_length = new_image.width * new_image.channels;
        for (int i = 0; i < image.width * image.height; i++) {
          int current_row = i / image.width;
          int pixle_in_row = i % image.width;
          new_image.imageData[(new_image.width * (new_image.height - (pixle_in_row + 1)) + current_row) * image.channels] = image.imageData[i*3];
          new_image.imageData[(new_image.width * (new_image.height - (pixle_in_row + 1)) + current_row) * image.channels + 1] = image.imageData[i*3+1];
          new_image.imageData[(new_image.width * (new_image.height - (pixle_in_row + 1)) + current_row) * image.channels + 2] = image.imageData[i*3+2];
        }
        image = new_image;
      } else if (deg == Deg::DEG180) {
        Image new_image(image.width, image.height);
        int row_length = new_image.width * new_image.channels;
        for (int i = 0; i < image.width * image.height; i++) {
          new_image.imageData[(new_image.width * new_image.height - (1 + i)) * image.channels] = image.imageData[i*3];
          new_image.imageData[(new_image.width * new_image.height - (1 + i)) * image.channels + 1] = image.imageData[i*3+1];
          new_image.imageData[(new_image.width * new_image.height - (1 + i)) * image.channels + 2] = image.imageData[i*3+2];
        }
        image = new_image;
      } else {
        throw std::invalid_argument("Unknown rotation degree provided.");
      }
    }

    


     static void sunnyEffect(const Image& image){
      for(int i=0;i<(image.channels*image.height*image.width);i+=image.channels){
        float r = image.imageData[i];
        float g = image.imageData[i+1];
        float b = image.imageData[i+2];

        image.imageData[i] = (unsigned char)(std::clamp(r*1.15f,0.0f,255.0f));
        image.imageData[i+1] = (unsigned char)(std::clamp(g*1.05f,0.0f,255.0f));
        image.imageData[i+2] = (unsigned char)(std::clamp(b*0.85f,0.0f,255.0f));
      }
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
          return Filter(sunnyEffect);
        case Filters::LIGHT:
        case Filters::DARK:
        case Filters::ROTATE:
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
          return Filter(Image_processor::rotate, rotation_degree);
      }
      throw std::invalid_argument("Error: This filter does not accept a rotation parameter.");
    }

    static const Filterable_image create_filterable_image(const Image &image) {
      return Filterable_image(image);
    }
};

int main() {
  Image image("House.bmp");
  Image_processor processor;

  processor.generate_filter(Filters::SUNNY).apply_to_image(image);

  image.saveImage("sun.bmp");

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
  // --> img.add_filter(processor.generate_filter(Filters::GRAY_SCALE)).add_filter(processor.generate_filter(Filters::ROTATE, Deg::DEG90)).apply_filter();
}