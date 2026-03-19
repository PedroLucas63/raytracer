#include "RunningOptions.hpp"
#include <filesystem>
#include <algorithm>
#include <vector>

namespace raytracer {
   std::string RunningOptions::validateOutputPath(std::string filename) const {
      const std::filesystem::path path{filename};
      const std::filesystem::path parent = path.parent_path();

      if (!std::filesystem::exists(parent)) {
         try {
            std::filesystem::create_directories(parent);
         } catch (const std::filesystem::filesystem_error& e) {
            throw CLI::ValidationError(filename, "Failed to create output directory");
         }
      }
      return std::string{};
   }

   std::string RunningOptions::validateOutputExtension(std::string filename) const {
      const std::filesystem::path path{filename};
      std::string extension = path.extension().string();
      std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

      std::vector<std::string> supportedExtensions = {".png", ".jpg", ".jpeg", ".bmp", ".tiff", ".ppm", ".tga"};

      if (std::find(supportedExtensions.begin(), supportedExtensions.end(), extension) != supportedExtensions.end()) {
         return std::string{};
      }

      throw CLI::ValidationError(filename, "Unsupported output file extension: " + extension);
   }

   void RunningOptions::configureCLI(CLI::App& app) {
      app.add_option("input_scene_file", _inputSceneFile, "Input scene file.")
         ->type_name("<input_scene_file>")
         ->required()
         ->check(CLI::ExistingFile);
      app.add_option("-c,--cropwindow", _cropwindow, "Specify an image crop window.")
         ->type_name("<x0, x1, y0, y1>")
         ->check(CLI::Range(0, 10000));
      app.add_flag("-q,--quick", _quick, "Reduces quality parameters to render image quickly.");
      app.add_option("-o,--output", _output, "Write the rendered image to <filename>.")
         ->type_name("<filename>")
         ->check([this](const std::string& output_path) {
            validateOutputPath(output_path);
            validateOutputExtension(output_path);
            return std::string{};
         });
   }

   bool RunningOptions::parse(int argc, char** argv) {
      CLI::App app("A simple raytracer implemented in C++");
      configureCLI(app);

      try {
         app.parse(argc, argv);
         fail = false;
      } catch (const CLI::ParseError &e) {
         app.exit(e);
         fail = true;
      }

      return !fail;
   }

   std::string RunningOptions::getInputSceneFile() const {
      return _inputSceneFile;
   }

   std::optional<RunningOptions::cropwindow_t> RunningOptions::getCropwindow() const {
      return _cropwindow;
   }

   bool RunningOptions::isQuick() const {
      return _quick;
   }

   std::string RunningOptions::getOutput() const {
      return _output;
   }

   bool RunningOptions::hasOutput() const {
      return !_output.empty();
   }

   bool RunningOptions::isFail() const {
      return fail;
   }
}
