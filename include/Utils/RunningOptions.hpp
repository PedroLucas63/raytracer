#ifndef RUNNING_OPTIONS_HPP
#define RUNNING_OPTIONS_HPP

#include <tuple>
#include <string>
#include <CLI/CLI.hpp>
#include <optional>

namespace raytracer {
   class RunningOptions {
      private:         
         using cropwindow_t = std::tuple<int, int, int, int>;

         std::string _inputSceneFile;
         std::optional<cropwindow_t> _cropwindow;
         bool _quick = false;
         std::string _output;
<<<<<<< HEAD
=======
         bool fail = true;
>>>>>>> feat/paramset

         void configureCLI(CLI::App& app);
         std::string validateOutputPath(std::string filename) const;
         std::string validateOutputExtension(std::string filename) const;
      public:
         RunningOptions() = default;
         ~RunningOptions() = default;

         bool parse(int argc, char** argv);

         std::string getInputSceneFile() const;
         std::optional<cropwindow_t> getCropwindow() const;
         bool isQuick() const;
         std::string getOutput() const;
         bool hasOutput() const;
<<<<<<< HEAD
=======
         bool isFail() const;
>>>>>>> feat/paramset
   };
}

#endif // !RUNNING_OPTIONS_HPP