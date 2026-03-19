#include "tinyxml2.h"
#include "ParserScene.hpp"
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>

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

      
   bool isValidElement(std::string_view elementName) {
      auto element = elementList.find((std::string)elementName);
      return element != elementList.end();
   }


   std::string ParserScene::stringToLower(std::string s) {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
      return s;
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

            std::string attrValue= attribute->Value();
            parserAttributte(); //Todo

            // Recursive schena with element include
            // call the api
         }
         

      }
      
   }


}