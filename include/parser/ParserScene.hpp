
#ifndef PARSER_SCENE_HPP
#define PARSER_SCENE_HPP

#include <algorithm>
#include <string>

#include <functional>
#include "Core/ParamSet.hpp"

namespace raytracer {

    using ConverterFunction = std::function<bool(const std::string&, const std::string&, raytracer::ParamSet*)>;

    // Converter can be implemented elsewhere, here stub for linkage.
    template <typename T>
    bool convert(const std::string_view attrName, const std::string_view attrValue, raytracer::ParamSet* ps);


    class ParserScene{
        public:
            void parserScene(const char* filename);


        private:
            std::string stringToLower(std::string s);
            bool isValidElement(std::string_view);
            bool isValidAttributte(std::string_view);
            void parserAttributte(std::string_view attrName, std::string_view attrValue, raytracer::ParamSet* ps);
    };
}


#endif  // PARSER_SCENE_HPP