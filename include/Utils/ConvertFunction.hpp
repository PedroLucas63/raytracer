#ifndef CONVERT_FUNCTION_HPP
#define CONVERT_FUNCTION_HPP

#include <functional>
#include <string>
#include <type_traits>
#include <istream>
#include <vector>
#include "Core/ParamSet.hpp"

using ConvertFunction = std::function<bool(const std::string&, const std::string&, raytracer::ParamSet*)>;

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

template <typename T>
bool convert(const std::string& attrName, const std::string& attrContent, raytracer::ParamSet* ps) {
   assert(ps);
   
   std::string trimmedContent = trim(attrContent);

   T value;
   if (trimmedContent.empty())
      throw std::invalid_argument("attrContent cannot be empty");
   std::istringstream stream(trimmedContent);

   if (stream.peek() == EOF)
      throw std::invalid_argument("attrContent cannot be empty");
   
   if (std::is_same<bool, T>::value)
      stream >> std::boolalpha >> value;
   else
      stream >> value;
   
   if (stream.fail())
      throw std::invalid_argument("attrContent is not a valid " + std::string(typeid(T).name()));
   else if (stream.peek() != EOF)
      throw std::invalid_argument("attrContent contains extra characters"); 

   ps->add(attrName, value);
   return true;
}

template <typename T, typename K, uint8_t N>
bool convert(const std::string& attrName, const std::string& attrContent, raytracer::ParamSet* ps) {
   assert(ps);
   std::string trimmedContent = trim(attrContent);

   if (trimmedContent.empty())
      throw std::invalid_argument("attrContent cannot be empty");

   std::vector<K> members;
   std::istringstream stream(trimmedContent);

   if (stream.peek() == EOF)
      throw std::invalid_argument("attrContent cannot be empty");
   
   while (stream.peek() != EOF) {
      K member;
      if (std::is_same<bool, T>::value)
         stream >> std::boolalpha >> member;
      else
         stream >> member;

      if (stream.fail())         
         throw std::invalid_argument("attrContent contains invalid member: " + std::string(typeid(K).name()));

      members.push_back(member);
   }

   if (members.size() != N)
      throw std::invalid_argument("attrContent must contain exactly " + std::to_string(N) + " members");

   T value(members.begin(), members.end());

   ps->add(attrName, value);
   return true;
}

#endif // !CONVERT_FUNCTION_HPP