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
#include <cstdint>
#include <stdexcept>
#include "Core/ParamSet.hpp"
#include "Image/RGBColor.hpp"

namespace raytracer {
   using ConvertFunction = std::function<bool(const std::string&, const std::string&, ParamSet*)>;

   std::string trim(const std::string& str);

   template <typename T>
   struct is_std_vector : std::false_type {};

   template <typename T, typename Allocator>
   struct is_std_vector<std::vector<T, Allocator>> : std::true_type {};

   template <typename T>
   inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

   template <typename T>
   requires (!is_std_vector_v<T>)
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

   template <typename T>
   requires is_std_vector_v<T>
   bool convert(const std::string& attrName, const std::string& attrContent, ParamSet* ps) {
      if (ps == nullptr)
         throw std::invalid_argument("ParamSet pointer cannot be null");

      std::string trimmedContent = trim(attrContent);

      if (trimmedContent.empty())
         throw std::invalid_argument(attrName + " cannot be empty");

      std::istringstream stream(trimmedContent);

      if (stream.peek() == EOF)
         throw std::invalid_argument(attrName + " cannot be empty");

      using Member = typename T::value_type;
      T values;

      if constexpr (std::is_same_v<Member, RGBColor>) {
         std::vector<double> members;

         while (true) {
            double member;
            if (!(stream >> member)) {
               if (stream.eof()) break;
               throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(double).name()));
            }

            members.push_back(member);
         }

         if (members.empty()) {
            throw std::invalid_argument(attrName + " cannot be empty");
         }

         if (members.size() % 3 != 0) {
            throw std::invalid_argument(attrName + ": " + trimmedContent + " must contain groups of 3 members for RGBColor values");
         }

         for (size_t i = 0; i < members.size(); i += 3) {
            const double r = members[i];
            const double g = members[i + 1];
            const double b = members[i + 2];

            if (r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0) {
               values.push_back(RGBColor::fromNormalized(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b)));
               continue;
            }

            if (r < 0.0 || r > 255.0 || g < 0.0 || g > 255.0 || b < 0.0 || b > 255.0) {
               throw std::invalid_argument(attrName + ": " + trimmedContent + " contains out-of-range RGBColor component");
            }

            values.emplace_back(
               static_cast<std::uint8_t>(r),
               static_cast<std::uint8_t>(g),
               static_cast<std::uint8_t>(b)
            );
         }
      } else {
         while (true) {
            if constexpr (std::is_same_v<Member, bool>) {
               bool member;
               if (!(stream >> std::boolalpha >> member)) {
                  if (stream.eof()) break;
                  throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(Member).name()));
               }

               values.push_back(member);
            } else if constexpr (std::is_integral_v<Member> && sizeof(Member) == 1) {
               int parsedMember;
               if (!(stream >> parsedMember)) {
                  if (stream.eof()) break;
                  throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(Member).name()));
               }

               if (parsedMember < static_cast<int>(std::numeric_limits<Member>::min()) ||
                   parsedMember > static_cast<int>(std::numeric_limits<Member>::max())) {
                  throw std::invalid_argument(attrName + ": " + trimmedContent + " contains out-of-range member: " + std::to_string(parsedMember));
               }

               values.push_back(static_cast<Member>(parsedMember));
            } else {
               Member member;
               if (!(stream >> member)) {
                  if (stream.eof()) break;
                  throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(Member).name()));
               }

               values.push_back(member);
            }
         }
      }

      ps->add(attrName, values);
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

   bool convert_color(
      const std::string& attrName, 
      const std::string& attrContent, 
      ParamSet* ps
   );
}

#endif // !CONVERT_FUNCTION_HPP