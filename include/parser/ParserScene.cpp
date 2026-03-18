#include "tinyxml2.h"
#include "ParserScene.hpp"


namespase raytracer{

   Scene ParserScene::parse(const std::string& filename){
      tinyxml2::XMLDocument doc;
      if(doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS){
         throw std::runtime_error("Failed to load XML file: " + filename);
      }

      Scene scene;
   }
}