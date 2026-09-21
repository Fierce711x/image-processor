#include "Image_Class.h"
#include <functional>
#include <stdexcept> 
#include <vector>
enum class Filters{
  INVERT,
  FLIP_HORIZONTAL,
  FLIP_VERTICAL,
  GRAY_SCALE,
  BLACK_AND_WHITE,
};

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
//         current_image.imageData == nullptr;
//       }
//       current_image.loadNewImage(filename);
//     }
// };

class Filter {
  private: 
    std::function<void(const Image&)> filter;

  public: 
    Filter(std::function<void(const Image&)> filter): filter(filter) {}
    const Image& on_image(const Image& image) {
      filter(image);
      return image;
    }
};

class Image_processor {
  private: 
    static void invert(const Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i++) {
        image.imageData[i] = ~image.imageData[i];
      }
    }

    static void flip_horizontal(const Image& image) {
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

    static void flip_vertical(const Image& image) {
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

    static int get_gray_value(const Image& image, int pixleIndex) {
      int r = image.imageData[pixleIndex];
      int g = image.imageData[pixleIndex+1];
      int b = image.imageData[pixleIndex+2];
      return r*0.2126 + g*0.7152 + b*0.0722;
    }

    static void gray_scale(const Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i+= image.channels) {
        int gray_value = get_gray_value(image, i);
        image.imageData[i] = gray_value;
        image.imageData[i+1] = gray_value;
        image.imageData[i+2] = gray_value;
      }
    } 

    static void black_and_white(const Image& image) {
      for (int i = 0; i < image.channels * image.height * image.width; i+= image.channels) {
        int gray_value = get_gray_value(image, i);
        int value = gray_value <= 127 ? 0 : 255;
        image.imageData[i] = value;
        image.imageData[i+1] = value;
        image.imageData[i+2] = value;
      }
    }

  public: 
    Filter use_filter(Filters filter) {
      switch (filter) {
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
      }
      throw std::invalid_argument("Unknown filter type provided.");
    }
};

int main() {
  Image image("luffy.jpg");
  Image_processor processor;
  processor.use_filter(Filters::BLACK_AND_WHITE).on_image(image);
  image.saveImage("invert.jpg");
}