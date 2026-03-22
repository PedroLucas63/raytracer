
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
#include "Api/Api.hpp"

namespace raytracer {

    extern std::unordered_map<std::string, std::function<void(const ParamSet&)>> apiFunctions;

    class ParserScene{
        public:
            void parseScene(const char* filename);
            void parseScene(const char* xmlContent, bool fromString);

        private:
            void parseDocument(tinyxml2::XMLDocument& doc);
            std::string stringToLower(std::string s);
            bool isValidElement(std::string_view);
            bool isValidAttribute(std::string_view elementName, std::string_view attrName);
            void parseAttribute(std::string_view attrName, std::string_view attrValue, raytracer::ParamSet* ps);
    };
}


#endif  // PARSER_SCENE_HPP