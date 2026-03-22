#include "ConvertFunction.hpp"

namespace raytracer {
   std::string trim(const std::string& str) {
      std::string result = str;

      result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
         return !std::isspace(ch);
      }));

      result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
         return !std::isspace(ch);
      }).base(), result.end());
      
      return result;
   }
}