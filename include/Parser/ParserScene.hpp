
#ifndef PARSER_SCENE_HPP
#define PARSER_SCENE_HPP

#include <algorithm>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

#include "Core/ParamSet.hpp"
#include "tinyxml2.h"
#include "Utils/ConvertFunction.hpp"
#include "Scene/Scene.hpp"

namespace raytracer {
    class ParserScene{
        public:
            static Scene parseScene(const char* filename);
            static Scene parseScene(const char* xmlContent, bool fromString);
        
        private:
            static Scene parseDocument(tinyxml2::XMLDocument& doc);
            static std::string stringToLower(std::string s);
            static bool isValidElement(std::string_view);
            static bool isValidAttribute(std::string_view elementName, std::string_view attrName);
            static void parseAttribute(std::string_view attrName, std::string_view attrValue, raytracer::ParamSet* ps);
    };
}


#endif  // PARSER_SCENE_HPP