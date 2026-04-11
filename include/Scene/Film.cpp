#include "Film.hpp"
#include "Utils/ImageUtils.hpp"
#include <filesystem>
#include <string>

namespace {
   void ensureOutputDirectoryExists(const std::filesystem::path& outputPath);
   std::filesystem::path makeUniqueFilename(const std::filesystem::path& outputPath);
}

namespace raytracer {
   /** Constructors */
   Film::Film(const ParamSets& params): _image(1, 1, 3) { // Default to a 1x1 image until properly initialized
      auto it = params.find("film");
      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'film' parameters.");

      const ParamSet& filmParams = it->second;
      std::string type = filmParams.retrieveOrDefault<std::string>("type", "image");

      if (type == "image") {
         _type = FilmType::TypeImage;

          // Check for resolution parameters
         if (filmParams.has("filename")) {
            _filename = filmParams.retrieve<std::string>("filename");
         }

         if (filmParams.has("x_res") && filmParams.has("y_res")) {
            int x_res = filmParams.retrieve<int>("x_res");
            int y_res = filmParams.retrieve<int>("y_res");
            _image = Image(x_res, y_res, 3);
         } else if (filmParams.has("w_res") && filmParams.has("h_res")) {
            int w_res = filmParams.retrieve<int>("w_res");
            int h_res = filmParams.retrieve<int>("h_res");
            _image = Image(w_res, h_res, 3);
         } else {
            throw std::invalid_argument("Film parameters must include either (x_res and y_res) or (w_res and h_res) for image type.");
         }
      } else {
         throw std::invalid_argument("Unsupported film type: " + type);
      }
   }

   Film Film::load(std::string filename) {
      Image image = ImageUtils::loadImage(filename);
      return Film(image, filename);
   }

   /** Getters and Setters */
   uint16_t Film::getWidth() const {
      return _image.getWidth();
   }
   uint16_t Film::getHeight() const {
      return _image.getHeight();
   }
   uint8_t Film::getChannels() const {
      return _image.getChannels();
   }
   RGBColor Film::getPixel(uint16_t row, uint16_t col) const {
      return _image.getPixel(row, col);
   }
   std::string Film::getFilename() const {
      return _filename;
   }
   Image Film::getImage() const {
      return _image;
   }
   FilmType Film::getType() const {
      return _type;
   }
   void Film::setImage(const Image& image) {
      _image = image;
   }
   void Film::setFilename(const std::string& filename) {
      _filename = filename;
   }
   void Film::setPixel(const RGBColor& pixel, uint16_t row, uint16_t col) {
      _image.setPixel(pixel, row, col);
   }

   /** Access Operator */
   RGBColor Film::operator()(uint16_t row, uint16_t col) {
      return _image(row, col);
   }
   RGBColor Film::operator[](Point2 position) {
      return _image[position];
   }

   /** Conversion functions */
   Film Film::toGrayScale() const {
      Image grayImage = _image.toGrayScale();
      return Film(grayImage, _filename);
   }

   /** Save the film to a file */
   void Film::save() const {
      if (_filename.empty()) {
         throw std::runtime_error("Filename is not set. Cannot save the film.");
      }

      std::filesystem::path outputPath(_filename);
      ensureOutputDirectoryExists(outputPath);
      outputPath = makeUniqueFilename(outputPath);
      ImageUtils::saveImage(_image, outputPath.string());
   }
   
}

namespace {
   void ensureOutputDirectoryExists(const std::filesystem::path& outputPath) {
      const auto outputDir = outputPath.parent_path();
      if (!outputDir.empty() && !std::filesystem::exists(outputDir)) {
         std::filesystem::create_directories(outputDir);
      }
   }

   std::filesystem::path makeUniqueFilename(const std::filesystem::path& outputPath) {
      if (!std::filesystem::exists(outputPath)) {
         return outputPath;
      }

      const auto extension = outputPath.extension().string();
      const auto originalStem = outputPath.stem().string();
      std::filesystem::path candidate;
      int suffix = 1;

      do {
         candidate = outputPath.parent_path() / (originalStem + "-" + std::to_string(suffix) + extension);
         suffix++;
      } while (std::filesystem::exists(candidate));

      return candidate;
   }
}