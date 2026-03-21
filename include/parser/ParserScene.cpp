#include "tinyxml2.h"
#include "ParserScene.hpp"
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>
#include <filesystem>
#include "Utils/ConvertFunction.hpp"
#include "Image/RGBColor.hpp"

namespace raytracer{

   // ── maps ──────────────────────────────────────────────────────────────────

   std::unordered_map<std::string, std::vector<std::string>> elementList {
      { "background", { "type", "filename", "mapping", "color", "tl", "tr", "bl", "br" } },
      { "film",       { "type", "filename", "img_type", "x_res", "y_res",
                        "w_res", "h_res", "crop_window", "gamma_corrected" } },
      { "include",    { "filename" } },
      { "world_begin",{} },
      { "world_end",  {} },
   };

   std::unordered_map<std::string, ConvertFunction> converters {
      { "type",            convert<std::string> },
      { "name",            convert<std::string> },
      { "filename",        convert<std::string> },
      { "img_type",        convert<std::string> },
      { "mapping",         convert<std::string> },

      { "color",           convert<RGBColor, std::uint8_t, 3> },
      { "bl",              convert<RGBColor, std::uint8_t, 3> },
      { "tl",              convert<RGBColor, std::uint8_t, 3> },
      { "tr",              convert<RGBColor, std::uint8_t, 3> },
      { "br",              convert<RGBColor, std::uint8_t, 3> },

      { "x_res",           convert<int> },
      { "y_res",           convert<int> },
      { "w_res",           convert<int> },
      { "h_res",           convert<int> },

      { "flip",            convert<bool> },
      { "gamma_corrected", convert<bool> },
   };


   std::unordered_map<std::string, std::function<void(const raytracer::ParamSet&)>> apiFunctions{
      { "film", raytracer::Api::film },
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

   // ── main parsing functions ──────────────────────────────────────────────────────────────────
   
   void ParserScene::parseScene(const char* filename) {
      tinyxml2::XMLDocument doc;

      if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) {
         std::cerr << "[Parser] Failed to load XML: " << filename << '\n';
         return;
      }

      tinyxml2::XMLElement* root = doc.RootElement();
      if (!root) {
         std::cerr << "[Parser] XML has no root element.\n";
         return;
      }

      for (auto* node = root->FirstChildElement(); node; node = node->NextSiblingElement()) {
         const std::string element = stringToLower(node->Name());

         if (!isValidElement(element)) {
            std::cerr << "[Parser] Unknown element: <" << element << ">\n";
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
            parseScene(incFile.c_str());
            continue;
         }

         auto it = apiFunctions.find(element);
         if (it == apiFunctions.end()) {
            std::cerr << "[Parser] No API function for element: <" << element << ">\n";
            continue;
         }

         std::cout << "[Parser] Calling API for <" << element << ">.\n";
         it->second(ps);
      }
   }


   

};