#include "RunningOptions.hpp"
#include <filesystem>

namespace raytracer {
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
         ->check([](const std::string& output_path) {
               const std::filesystem::path path{output_path};
               const std::filesystem::path parent = path.parent_path();

               if (!std::filesystem::exists(parent)) {
                  std::filesystem::create_directories(parent);
               }
               return std::string{};
         });
      
   }

   bool RunningOptions::parse(int argc, char** argv) {
      CLI::App app("A simple raytracer implemented in C++");
      configureCLI(app);

      try {
         app.parse(argc, argv);
         return true;
      } catch (const CLI::ParseError &e) {
         app.exit(e);
         return false;
      }
   }

   const std::string& RunningOptions::getInputSceneFile() const {
      return _inputSceneFile;
   }

   const std::optional<RunningOptions::cropwindow_t>& RunningOptions::getCropwindow() const {
      return _cropwindow;
   }

   bool RunningOptions::isQuick() const {
      return _quick;
   }

   const std::string& RunningOptions::getOutput() const {
      return _output;
   }
}
