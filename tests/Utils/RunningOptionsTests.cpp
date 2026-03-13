#include <chrono>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "Utils/RunningOptions.hpp"

namespace {
   class TemporaryDirectory {
      public:
         TemporaryDirectory() {
            const auto uniqueId = std::chrono::steady_clock::now().time_since_epoch().count();
            _path = std::filesystem::temp_directory_path() /
               ("raytracer_running_options_tests_" + std::to_string(uniqueId));
            std::filesystem::create_directories(_path);
         }

         ~TemporaryDirectory() {
            std::error_code errorCode;
            std::filesystem::remove_all(_path, errorCode);
         }

         const std::filesystem::path& path() const {
            return _path;
         }

      private:
         std::filesystem::path _path;
   };

   class ArgvBuilder {
      public:
         explicit ArgvBuilder(std::initializer_list<std::string> args)
            : _storage(args) {
            _argv.reserve(_storage.size());
            for (auto& arg : _storage) {
               _argv.push_back(arg.data());
            }
         }

         int argc() const {
            return static_cast<int>(_argv.size());
         }

         char** argv() {
            return _argv.data();
         }

      private:
         std::vector<std::string> _storage;
         std::vector<char*> _argv;
   };

   std::filesystem::path createSceneFile(const std::filesystem::path& directory, const std::string& name = "scene.xml") {
      const auto path = directory / name;
      std::ofstream output(path);
      output << "<scene/>";
      return path;
   }
}

TEST_CASE("RunningOptions parse succeeds with required input scene") {
   TemporaryDirectory temporaryDirectory;
   const auto scenePath = createSceneFile(temporaryDirectory.path());

   raytracer::RunningOptions options;
   ArgvBuilder args {"raytracer", scenePath.string()};

   REQUIRE(options.parse(args.argc(), args.argv()));
   REQUIRE(options.getInputSceneFile() == scenePath.string());
   REQUIRE_FALSE(options.getCropwindow().has_value());
   REQUIRE_FALSE(options.isQuick());
   REQUIRE(options.getOutput() == "output.png");
}

TEST_CASE("RunningOptions parse accepts cropwindow quick and output options") {
   TemporaryDirectory temporaryDirectory;
   const auto scenePath = createSceneFile(temporaryDirectory.path());

   const auto outputPath = temporaryDirectory.path() / "images" / "render.png";

   raytracer::RunningOptions options;
   ArgvBuilder args {
      "raytracer",
      scenePath.string(),
      "--cropwindow", "1", "2", "3", "4",
      "--quick",
      "--output", outputPath.string()
   };

   REQUIRE(options.parse(args.argc(), args.argv()));

   const auto cropwindow = options.getCropwindow();
   REQUIRE(cropwindow.has_value());

   const auto& [x0, x1, y0, y1] = *cropwindow;
   REQUIRE(x0 == 1);
   REQUIRE(x1 == 2);
   REQUIRE(y0 == 3);
   REQUIRE(y1 == 4);

   REQUIRE(options.isQuick());
   REQUIRE(options.getOutput() == outputPath.string());
   REQUIRE(std::filesystem::exists(outputPath.parent_path()));
}

TEST_CASE("RunningOptions parse fails without required input scene") {
   raytracer::RunningOptions options;
   ArgvBuilder args {"raytracer"};

   REQUIRE_FALSE(options.parse(args.argc(), args.argv()));
}

TEST_CASE("RunningOptions parse fails when input scene does not exist") {
   TemporaryDirectory temporaryDirectory;
   const auto missingScene = temporaryDirectory.path() / "missing.xml";

   raytracer::RunningOptions options;
   ArgvBuilder args {"raytracer", missingScene.string()};

   REQUIRE_FALSE(options.parse(args.argc(), args.argv()));
}

TEST_CASE("RunningOptions parse fails for out-of-range cropwindow values") {
   TemporaryDirectory temporaryDirectory;
   const auto scenePath = createSceneFile(temporaryDirectory.path());

   raytracer::RunningOptions options;
   ArgvBuilder args {
      "raytracer",
      scenePath.string(),
      "--cropwindow", "0", "1", "2", "10001"
   };

   REQUIRE_FALSE(options.parse(args.argc(), args.argv()));
}
