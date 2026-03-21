#include "tinyxml2.h"
#include "ParserScene.hpp"
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>
#include "RGBColor.hpp"
// #include "ConverterFunction.hpp"

namespace raytracer{
   std::unordered_map<std::string, std::vector<std::string>> elementList{
      {
      "background",
      {
         "type",
         "filename",
         "mapping",
         "color",
         "tl",
         "tr",
         "bl",
         "br",
      },
   },
   {
      "film",
      {
         "type",
         "filename",
         "img_type",
         "x_res",
         "y_res",
         "w_res",
         "h_res",
         "crop_window",
         "gamma_corrected",
      },
   },
   {
      "world_begin",
      { "" },  // no attributes
   },
   {
      "world_end",
      { "" },  // no attributes
   },
   };

   template <typename T>
   bool convert(std::string_view attrName, std::string_view attrValue, raytracer::ParamSet* ps){
      (void)attrName;
      (void)attrValue;
      (void)ps;
      return true;
   }

   // With erro, because dependes of the convertFunction
   std::unordered_map<std::string, ConverterFunction> converters{
      { "type", convert<std::string> },  
      { "name", convert<std::string> },  
      //
      { "color", convert<raytracer::RGBColor, 3> },  
      { "flip", convert<bool> },

      { "mapping", convert<std::string> },
      { "bl", convert<raytracer::RGBColor, 3> },
      { "tl", convert<raytracer::RGBColor, 3> },
      { "tr", convert<raytracer::RGBColor, 3> },
      { "br", convert<raytracer::RGBColor, 3> },
      
      { "x_res", convert<int> },
      { "y_res", convert<int> },
      { "w_res", convert<int> },
      { "h_res", convert<int> },
      { "filename", convert<std::string> },
      { "img_type", convert<std::string> },
      { "gamma_corrected", convert<bool> },
      };


   std::unordered_map<std::string, std::function<void(const raytracer::ParamSet&)>> apiFunctions{
      // { "background",  },
      // { "world_begin",  },
      // { "world_end",  },
      // { "film", },
   };

      
   bool ParserScene::isValidElement(std::string_view elementName) {
      auto element = elementList.find((std::string)elementName);
      return element != elementList.end();
   }

   bool ParserScene::isValidAttributte(std::string_view attrName) {
      auto attrList = elementList[(std::string) attrName];
      auto attr = std::find(attrList.begin(), attrList.end(), attrName);
      return attr != attrList.end();
   }


   std::string ParserScene::stringToLower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
      return s;
   }

   
   void ParserScene::parserAttributte(std::string_view attrName, std::string_view attrValue, raytracer::ParamSet* ps){
      auto it = converters.find(std::string(attrName));
      if(it == converters.end()){
         std::cerr << "No converter for attribute: " << attrName << '\n';
         return;
      }

      bool result = it->second(std::string(attrName), std::string(attrValue), ps);
      if(!result){
         std::cerr << "Fail to convert attribute: " << attrName << " value=" << attrValue << '\n';
      }
   }

   void ParserScene::parserScene(const char* filename){
      tinyxml2::XMLDocument doc;

      if(doc.LoadFile(filename) != tinyxml2::XML_SUCCESS) {
         std::cerr << "Error loading the XML file!" << '\n';
         return;
      }

      tinyxml2::XMLElement* root = doc.RootElement();

      if(root == nullptr) {
         std::cerr << "Root node of the XML tree was not found!" << '\n';
         return;
      }


      for(tinyxml2::XMLElement* childNode = root->FirstChildElement(); childNode != nullptr;  
         childNode = childNode->NextSiblingElement()){
      
         std::string element = stringToLower(childNode->Name());

         if(!isValidElement(element)){
            std::cerr << "The elemente " <<  element << "is not valid." << '\n';
            continue;
         }

         raytracer::ParamSet ps;

         for (const tinyxml2::XMLAttribute* attribute = childNode->FirstAttribute(); attribute != nullptr; 
            attribute = attribute->Next()){
            
            std::string attrName = stringToLower(attribute->Name());
            if(!isValidAttributte(attrName)){
               std::cerr << "The elemente " << element << "doesn't have an attribute " << attrName << '\n';
               continue;
            }

            auto completeAtrrbuteName = element + "." + attrName;
            parserAttributte(completeAtrrbuteName, attribute->Value(), &ps);
            // parserAttributte(attrName, attribute->Value(), &ps);

            // Recursive schena with element include

            if(attrName == "include"){
               auto filename = ps.resolve<std::string>("filename");
               if(filename.empty()){
                  std::cerr << "Include element must have a filename attribute." << '\n';
               } else {
                  parserScene(filename.c_str());
               }
            }

            // call the api
         }



         

      }
      
   }


}