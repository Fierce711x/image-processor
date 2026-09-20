#include "Image_Class.h"
#include <iostream>
#include <functional>

enum Filters{
  INVERT,
  FLIP_HORIZONTAL,
  FLIP_VERTICAL,
};

class Filter {
  private: 
    std::function<void(const Image&)> filter;

  public: 
    Filter(std::function<void(const Image&)> filter): filter(filter) {}
    const Image& with_image(const Image& image) {
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

  public: 
    Filter use_filter(Filters filter) {
      switch (filter) {
        case INVERT: 
          return Filter(invert);
        case FLIP_HORIZONTAL:
          return Filter(flip_horizontal);
        case FLIP_VERTICAL: 
          return Filter(flip_vertical);
      }
      throw std::invalid_argument("Unknown filter type provided.");
    }
};

int main() {
  Image_processor processor;
  Image image("luffy.jpg");

  processor.use_filter(FLIP_VERTICAL).with_image(image);
  image.saveImage("double-flip.jpg");
  return 0;
}