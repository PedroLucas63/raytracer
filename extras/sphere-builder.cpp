#include "Math/Point3.hpp"
#include <CLI/CLI.hpp>
#include <tuple>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <numbers>

using Point3Tuple = std::tuple<double, double, double>;

struct CLIData {
   std::string output;
   int base;
   int levels;
   double radius;
   Point3Tuple point_t;
   raytracer::Point3 point;
};

struct SphereData {
   double radius;
   raytracer::Point3 center;
};

using SphereList = std::vector<SphereData>; 

void configureCLI(CLI::App& app, CLIData& data);
bool parse(CLI::App& app, int argc, char** argv);
raytracer::Point3 makePoint3(const Point3Tuple& tuple);
void prepareFile(std::ofstream& file);
void closeFile(std::ofstream& file);
SphereList generateSpheres(const CLIData& data);
SphereList generateLine(const CLIData& data);
SphereList generateTriangular(const CLIData& data);
void printSpheresInFile(SphereList& list, std::ofstream& file);

int main(int argc, char** argv) {
   CLI::App app("Procedural sphere cluster generator for RT3 raytracer scenes.", "Sphere Builder");

   CLIData data;
   configureCLI(app, data);

   if (parse(app, argc, argv)) {
      data.point = makePoint3(data.point_t);

      auto file = std::ofstream(data.output, std::ios::out);
      prepareFile(file);

      auto spheres = generateSpheres(data);
      printSpheresInFile(spheres, file);

      closeFile(file);

      return EXIT_SUCCESS;
   } else {
      return EXIT_FAILURE;
   }
}

void configureCLI(CLI::App& app, CLIData& data) {
   app.add_option(
      "output",
      data.output,
      "Output file for the generated scene."
   )
   ->type_name("<filename>")
   ->required();

   app.add_option(
      "-b,--base", 
      data.base,
      "The number of spheres in the base level of the cluster."
   )
   ->type_name("<Int>")
   ->check(CLI::Range(1, 4))
   ->required();

   app.add_option(
      "-l,--levels",
      data.levels,
      "The number of levels in the cluster."
   )
   ->type_name("<Int>")
   ->default_val(1);

   app.add_option(
      "-r,--radius",
      data.radius,
      "The radius of each sphere."
   )
   ->type_name("<Double>")
   ->default_val(1.0);

   app.add_option(
      "-t,--top",
      data.point_t,
      "The position of the top level of the cluster."
   )
   ->type_name("<Double Double Double>")
   ->default_val(std::make_tuple(0.0, 0.0, 0.0));
}

bool parse(CLI::App& app, int argc, char** argv) {
   try {
      app.parse(argc, argv);
      return true;
   } catch (const CLI::ParseError &e) {
      app.exit(e);
      return false;
   }
}

raytracer::Point3 makePoint3(const Point3Tuple& tuple) {
   return raytracer::Point3(
      std::get<0>(tuple), 
      std::get<1>(tuple), 
      std::get<2>(tuple)
   );
}

void prepareFile(std::ofstream& file) {
   file << "<RT3>\n";
   file << "\t<world_begin/>\n";
}

void closeFile(std::ofstream& file) {
   file << "\t<world_end/>\n";
   file << "</RT3>";
   file.close();
}

SphereList generateSpheres(const CLIData& data) {
   if (data.base == 1) {
      return generateLine(data);
   } else if (data.base == 2) {
      return generateTriangular(data);
   }

   return SphereList();
}

SphereList generateLine(const CLIData& data) {
   if (data.levels == 0) return SphereList();

   SphereData sphere;
   sphere.radius = data.radius;

   auto center = data.point;
   auto newY = center.getY() - 2 * data.radius * (data.levels - 1);
   center.setY(newY);

   sphere.center = center;

   auto newData = data;
   newData.levels -= 1;
   auto list = generateLine(newData);

   list.insert(list.begin(), sphere);

   return list;
}

SphereList generateTriangular(const CLIData& data) {
   if (data.levels == 0) return SphereList();

   auto h = std::numbers::sqrt3 * data.radius;

   auto center = data.point;
   auto newY = center.getY() - h * (data.levels - 1);
   center.setY(newY);

   auto newX = center.getX() - (data.levels - 1) * data.radius;
   center.setX(newX);

   SphereList spheres;
   for (int i = 0; i < data.levels; i++) {
      SphereData sphere;
      sphere.radius = data.radius;
      sphere.center = center;

      spheres.push_back(sphere);

      newX = center.getX() + 2 * data.radius;
      center.setX(newX);
   }

   auto newData = data;
   newData.levels -= 1;
   auto list = generateTriangular(newData);


   list.insert(list.begin(), spheres.begin(), spheres.end());

   return list;
}

void printSpheresInFile(SphereList& list, std::ofstream& file) {
   auto pointToString = [](raytracer::Point3& p) {
      std::ostringstream iss;
      iss << p.getX() << " " << p.getY() << " " << p.getZ();
      return iss.str();
   };

   for (auto& s : list) {
      file << "\t\t<object type=\"sphere\" radius=\"" << s.radius << "\" center=\"" << pointToString(s.center) << "\" />\n";
   }
}