#ifndef CONVERT_FUNCTION_HPP
#define CONVERT_FUNCTION_HPP

#include <functional>
#include <string>
#include <type_traits>
#include <istream>
#include <vector>
#include <sstream>  
#include <algorithm>
#include <limits>
#include "Core/ParamSet.hpp"

namespace raytracer {
   using ConvertFunction = std::function<bool(const std::string&, const std::string&, ParamSet*)>;

   std::string trim(const std::string& str);

   template <typename T>
   bool convert(const std::string& attrName, const std::string& attrContent, ParamSet* ps) {
      if (ps == nullptr)
         throw std::invalid_argument("ParamSet pointer cannot be null");
      
      std::string trimmedContent = trim(attrContent);

      if (trimmedContent.empty())
         throw std::invalid_argument(attrName + " cannot be empty");
      std::istringstream stream(trimmedContent);

      if (stream.peek() == EOF)
         throw std::invalid_argument(attrName + " cannot be empty");

      T value;
      if constexpr (std::is_same_v<bool, T>) {
         stream >> std::boolalpha >> value;
      } else if constexpr (std::is_integral_v<T> && sizeof(T) == 1) {
         int parsedValue;
         stream >> parsedValue;

         if (stream.fail())
            throw std::invalid_argument(attrName + ": " + trimmedContent + " is not a valid " + std::string(typeid(T).name()));

         if (parsedValue < static_cast<int>(std::numeric_limits<T>::min()) ||
             parsedValue > static_cast<int>(std::numeric_limits<T>::max())) {
            throw std::invalid_argument(attrName + ": " + trimmedContent + " is out of range for " + std::string(typeid(T).name()));
         }

         value = static_cast<T>(parsedValue);
      } else {
         stream >> value;
      }

      if (stream.fail())
         throw std::invalid_argument(attrName + ": " + trimmedContent + " is not a valid " + std::string(typeid(T).name()));
      else if (stream.peek() != EOF)
         throw std::invalid_argument(attrName + ": " + trimmedContent + " contains extra characters");

      ps->add(attrName, value);
      return true;
   }

   template <typename T, typename K, uint8_t N>
   bool convert(const std::string& attrName, const std::string& attrContent, ParamSet* ps) {
      if (ps == nullptr)
         throw std::invalid_argument("ParamSet pointer cannot be null");
   
      std::string trimmedContent = trim(attrContent);

      if (trimmedContent.empty())
         throw std::invalid_argument(attrName + " cannot be empty");

      std::vector<K> members;
      std::istringstream stream(trimmedContent);

      if (stream.peek() == EOF)
         throw std::invalid_argument(attrName + " cannot be empty");
      
      while (true) {
         if constexpr (std::is_same_v<K, bool>) {
            bool member;
            if (!(stream >> std::boolalpha >> member)) {
               if (stream.eof()) break;
               throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(K).name()));
            }

            members.push_back(member);
         } else if constexpr (std::is_integral_v<K> && sizeof(K) == 1) {
            int parsedMember;
            if (!(stream >> parsedMember)) {
               if (stream.eof()) break;
               throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(K).name()));
            }

            if (parsedMember < static_cast<int>(std::numeric_limits<K>::min()) ||
                parsedMember > static_cast<int>(std::numeric_limits<K>::max())) {
               throw std::invalid_argument(attrName + ": " + trimmedContent + " contains out-of-range member: " + std::to_string(parsedMember));
            }

            members.push_back(static_cast<K>(parsedMember));
         } else {
            K member;
            if (!(stream >> member)) {
               if (stream.eof()) break;
               throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(K).name()));
            }

            members.push_back(member);
         }
      }

      if (members.size() != N) {
         throw std::invalid_argument(attrName + ": " + trimmedContent + " must contain exactly " + std::to_string(N) + " members");
      }

      T value(members.begin(), members.end());

      ps->add(attrName, value);
      return true;
   }
}

#endif // !CONVERT_FUNCTION_HPP