#include "Parser/ParserScene.hpp"
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>
#include <filesystem>
#include "Image/RGBColor.hpp"
#include <unordered_map>
#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include "Objects/Materials/MaterialFactory.hpp"
#include "Objects/PrimitiveFactory.hpp"
#include "Objects/Light/LightFactory.hpp"

namespace raytracer{

   // ── maps ──────────────────────────────────────────────────────────────────

   std::unordered_map<std::string, std::vector<std::string>> elementList {
      { "camera",     { "type", "screen_window", "fovy"} },
      { "lookat",    { "look_from", "look_at", "up" } },
      { "background", { "type", "filename", "mapping", "color", "tl", "tr", "bl", "br", "t", "b", "l", "r", "spherical" } },
      { "film",       { "type", "filename", "img_type", "x_res", "y_res",
                        "w_res", "h_res", "crop_window", "gamma_corrected" } },
      { "include",    { "filename" } },
      { "integrator", { "type", "near_color", "far_color", "zmin", "zmax" } },
      { "world_begin",{} },
      { "world_end",  {} },
      { "render_again", {} },
      { "object",     { "type", "center", "origin", "radius", "norm", "material" } },
      { "make_named_material", { "type", "name", "color", "color1", "color2", "spacing" } },
      { "named_material",      { "name" } },
      { "material",   { "type", "color", "name", "color1", "color2", "spacing" } },
      { "light_source", { "type", "I", "scale", "from", "to" } }
   };

   std::unordered_map<std::string, ConvertFunction> converters {
      { "type",            convert<std::string> },
      { "name",            convert<std::string> },
      { "filename",        convert<std::string> },
      { "img_type",        convert<std::string> },
      { "mapping",         convert<std::string> },
      { "fovy",            convert<int> },


      { "color",           convert_color },
      { "bl",              convert_color },
      { "tl",              convert_color },
      { "tr",              convert_color },
      { "br",              convert_color },
      { "t",               convert_color },
      { "b",               convert_color },
      { "r",               convert_color },
      { "l",               convert_color },
      { "spherical",       convert<bool> },
      { "screen_window",   convert<std::vector<float>, float, 4> },

      { "x_res",           convert<int> },
      { "y_res",           convert<int> },
      { "w_res",           convert<int> },
      { "h_res",           convert<int> },

      { "flip",            convert<bool> },
      { "gamma_corrected", convert<bool> },

      { "look_from",       convert<raytracer::Point3, double, 3> },
      { "look_at",         convert<raytracer::Point3, double, 3> },
      { "up",              convert<raytracer::Vector3, double, 3> },

      { "radius",          convert<float> },
      { "origin",          convert<raytracer::Point3, double, 3> },
      { "center",          convert<raytracer::Point3, double, 3> },
      { "norm",            convert<raytracer::Vector3, double, 3> },
      { "material",        convert<std::string> },

      { "color1",           convert<raytracer::RGBColor, std::uint8_t, 3> },
      { "color2",           convert<raytracer::RGBColor, std::uint8_t, 3> },
      { "spacing",          convert<float> },

      { "near_color",       convert<raytracer::RGBColor, std::uint8_t, 3> },
      { "far_color",        convert<raytracer::RGBColor, std::uint8_t, 3> },
      { "zmin",             convert<float> },
      { "zmax",             convert<float> },

      // Light attributes
      { "I",                convert_color },
      { "scale",            convert<raytracer::Vector3, double, 3> },
      { "from",             convert<raytracer::Point3, double, 3> },
      { "to",               convert<raytracer::Point3, double, 3> },
   };

   // ── helpers ──────────────────────────────────────────────────────────────────

      
   bool ParserScene::isValidElement(std::string_view elementName) {
      auto element = elementList.find((std::string)elementName);
      return element != elementList.end();
   }

   bool ParserScene::isValidAttribute(std::string_view elementName, std::string_view attrName) {
      auto it = elementList.find(std::string(elementName));
      if (it == elementList.end()) return false;
      const auto& attrs = it->second;
      return std::find(attrs.begin(), attrs.end(), attrName) != attrs.end();
   }

   std::string ParserScene::stringToLower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
      return s;
   }

   void ParserScene::parseAttribute(std::string_view attrName, std::string_view attrValue, ParamSet* ps) {
      auto it = converters.find(std::string(attrName));
      if (it == converters.end()) {
         std::cerr << "[Parser] No converter for attribute: " << attrName << '\n';
         return;
      }
      if (!it->second(std::string(attrName), std::string(attrValue), ps))
         std::cerr << "[Parser] Failed to convert: " << attrName << " = " << attrValue << '\n';
   }

   // ── parsing functions ──────────────────────────────────────────────────────────────────
   
   /**
    * @param filename The path to the XML file to parse
    * @brief Load document from file and delegate to parseDocument
    */
   void ParserScene::parseScene(const char* filename, Scene& scene,
                                 OnElementCallback onElement) {
      tinyxml2::XMLDocument doc;
      if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) {
         std::cerr << "[Parser] Failed to load XML: " << filename << '\n';
         return;
      }
      parseDocument(doc, scene, onElement);
   }

   /**
    * @param xmlContent The XML content as a string
    * @param fromString A boolean flag to differentiate this overload (not used in logic)
    * @brief Load document from string and delegate to parseDocument (useful for testing)
    */
   void ParserScene::parseScene(const char* xmlContent, bool fromString, Scene& scene,
                                 OnElementCallback onElement) {
      tinyxml2::XMLDocument doc;
      if (doc.Parse(xmlContent) != tinyxml2::XML_SUCCESS) {
         std::cerr << "[Parser] Failed to parse XML string.\n";
         return;
      }
      parseDocument(doc, scene, onElement);
   }

   void ParserScene::parseDocument(tinyxml2::XMLDocument& doc, Scene& scene,
                                    OnElementCallback onElement) {
      tinyxml2::XMLElement* root = doc.RootElement();
      if (!root) {
         throw std::runtime_error("XML document has no root element.");
      }

      scene = Scene();

      for (auto* node = root->FirstChildElement(); node; node = node->NextSiblingElement()) {
         const std::string element = stringToLower(node->Name());

         if (!isValidElement(element)) {
            std::cerr << "[Parser] Invalid element: <" << element << ">\n";
            continue;
         }

         ParamSet ps;

         for (auto* attr = node->FirstAttribute(); attr; attr = attr->Next()) {
            const std::string attrName = stringToLower(attr->Name());

            if (!isValidAttribute(element, attrName)) {
               std::cerr << "[Parser] <" << element << "> has no attribute '" << attrName << "'\n";
               continue;
            }

            auto it = converters.find(attrName);
            if (it == converters.end()) {
               std::cerr << "[Parser] No converter for attribute: " << attrName << '\n';
               continue;
            }

            if (!it->second(attrName, attr->Value(), &ps))
               std::cerr << "[Parser] Failed to convert: " << attrName << " = " << attr->Value() << '\n';
         }

         if (element == "include") {
            const auto incFile = ps.retrieve<std::string>("filename");
            if (incFile.empty()) {
               std::cerr << "[Parser] <include> is missing 'filename'.\n";
               continue;
            }
            if (!std::filesystem::exists(incFile)) {
               std::cerr << "[Parser] Included file not found: " << incFile << '\n';
               continue;
            }

            auto includedScene = Scene();
            parseScene(incFile.c_str(), includedScene, nullptr);
            scene.include(includedScene);

            // Notify for the include itself.
            if (onElement) onElement(scene, element, ps);

            if (includedScene.getParams().count("world_end") > 0) {
               ParamSet empty;
               if (onElement) onElement(scene, "world_end", empty);
            }

         } else if (element == "material") {
            try {
               auto material = MaterialFactory::create(ps);
               scene.addMaterial(material);
            } catch (const std::exception& e) {
               std::cerr << "[Parser] Failed to create material: " << e.what() << '\n';
            }
            if (onElement) onElement(scene, element, ps);

         } else if (element == "make_named_material") {
            try {
               auto material = MaterialFactory::create(ps);
               scene.addNamedMaterial(material);
            } catch (const std::exception& e) {
               std::cerr << "[Parser] Failed to create named material: " << e.what() << '\n';
            }
            if (onElement) onElement(scene, element, ps);

         } else if (element == "named_material") {
            if (!ps.has("name")) {
               std::cerr << "[Parser] <named_material> is missing 'name'.\n";
               continue;
            }
            try {
               std::string name = ps.retrieve<std::string>("name");
               scene.activateNamedMaterial(name);
            } catch (const std::exception& e) {
               std::cerr << "[Parser] Failed to activate named material: " << e.what() << '\n';
            }
            if (onElement) onElement(scene, element, ps);

         } else if (element == "object") {
            try {
               auto object = PrimitiveFactory::create(ps, scene);
               scene.addPrimitive(object);
            } catch (const std::exception& e) {
               std::cerr << "[Parser] Failed to create object: " << e.what() << '\n';
            }
            if (onElement) onElement(scene, element, ps);

         } else if (element == "light_source") {
            try {
               auto light = LightFactory::create(ps);
               scene.addLight(light);
            } catch (const std::exception& e) {
               std::cerr << "[Parser] Failed to create light source: " << e.what() << '\n';
            }

            if (onElement) onElement(scene, element, ps);
            
         } else {
            scene.setParam(element, ps);
            if (onElement) onElement(scene, element, ps);
         }
      }
   }
};