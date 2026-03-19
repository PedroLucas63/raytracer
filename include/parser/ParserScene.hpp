
#ifndef PARSER_SCENE_HPP
#define PARSER_SCENE_HPP

#include <algorithm>
#include <string>

#include <functional>
#include "Core/ParamSet.hpp"

namespace raytracer {


    class ParserScene{
        public:
            void parserScene(const char* filename);


        private:
            std::string stringToLower(std::string s);
            bool isValidElement(std::string_view);
            bool isValidAttributte(std::string_view);
            void parserAttributte();
    };


}


#endif  // PARSER_SCENE_HPP