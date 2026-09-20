#include "Image_Class.h"
#include <iostream>
#include <functional>

enum Filters{
  INVERT,
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


  public: 
    Filter use_filter(Filters filter) {
      switch (filter) {
        case INVERT: 
          return Filter(invert);
      }
      throw std::invalid_argument("Unknown filter type provided.");
    }
};

int main() {
  Image_processor processor;
  Image image("luffy.jpg");

  processor.use_filter(INVERT).with_image(image);
  image.saveImage("double-flip.jpg");
  return 0;
}