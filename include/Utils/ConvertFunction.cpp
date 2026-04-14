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

   bool convert_color(
      const std::string& attrName, 
      const std::string& attrContent, 
      ParamSet* ps
   ) {
      if (ps == nullptr)
         throw std::invalid_argument("ParamSet pointer cannot be null");
   
      std::string trimmedContent = trim(attrContent);

      if (trimmedContent.empty())
         throw std::invalid_argument(attrName + " cannot be empty");

      std::vector<double> members;
      std::istringstream stream(trimmedContent);

      if (stream.peek() == EOF)
         throw std::invalid_argument(attrName + " cannot be empty");
      
      while (true) {
         double member;
         if (!(stream >> member)) {
            if (stream.eof()) break;
            throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(double).name()));
         }

         members.push_back(member);
      }

      if (members.size() != 3) {
         throw std::invalid_argument(attrName + ": " + trimmedContent + " must contain exactly " + std::to_string(3) + " members");
      }

      if (std::all_of(
         members.begin(), 
         members.end(), 
         [](double m) { 
            return m >= 0.0 && m <= 1.0; 
         })
      ) {
         auto color = RGBColor::fromNormalized(members[0], members[1], members[2]);
         ps->add(attrName, color);
         return true; 
      }

      auto int_members = std::vector<std::uint8_t>(members.size());
      for (size_t i = 0; i < members.size(); ++i) {
         if (members[i] < 0.0 || members[i] > 255.0) {
            throw std::invalid_argument(attrName + ": " + trimmedContent + " contains out-of-range member: " + std::to_string(members[i]));
         }
         int_members[i] = static_cast<std::uint8_t>(members[i]);
      }
      
      RGBColor color(int_members.begin(), int_members.end());

      ps->add(attrName, color);
      return true;
   }
}