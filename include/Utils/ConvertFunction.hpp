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
#include <cctype>
#include <stdexcept>
#include "Core/ParamSet.hpp"
#include "Image/RGBColor.hpp"
#include "Math/Point2.hpp"
#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"

namespace raytracer {
   using ConvertFunction = std::function<bool(const std::string&, const std::string&, ParamSet*)>;
   template <typename T>
   using Reader = std::function<bool(std::istream&, T&)>;

   std::string trim(const std::string& str);

   template <typename T>
   bool read_value(std::istream& stream, T& value) {
      if constexpr (std::is_integral_v<T> && sizeof(T) == 1) {
         int parsedValue;
         if (!(stream >> parsedValue))
            return false;

         if (parsedValue < static_cast<int>(std::numeric_limits<T>::min()) ||
             parsedValue > static_cast<int>(std::numeric_limits<T>::max())) {
            stream.setstate(std::ios::failbit);
            return false;
         }

         value = static_cast<T>(parsedValue);
         return true;
      } else {
         stream >> value;
         return !stream.fail();
      }
   }

   inline bool read_value(std::istream& stream, bool& value) {
      std::string token;
      if (!(stream >> token))
         return false;

      std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
         return static_cast<char>(std::tolower(c));
      });

      if (token == "true" || token == "1" || token == "on") {
         value = true;
         return true;
      }

      if (token == "false" || token == "0" || token == "off") {
         value = false;
         return true;
      }

      stream.setstate(std::ios::failbit);
      return false;
   }

   inline bool read_value(std::istream& stream, RGBColor& value) {
      double r;
      double g;
      double b;

      if (!(stream >> r))
         return false;

      if (!(stream >> g) || !(stream >> b)) {
         stream.setstate(std::ios::failbit);
         return false;
      }

      if (r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0) {
         value = RGBColor::fromNormalized(
            static_cast<float>(r), 
            static_cast<float>(g), 
            static_cast<float>(b)
         );
         return true;
      }

      if (r < 0.0 || r > 255.0 || g < 0.0 || g > 255.0 || b < 0.0 || b > 255.0) {
         stream.setstate(std::ios::failbit);
         return false;
      }

      value = RGBColor(
         static_cast<std::uint8_t>(r),
         static_cast<std::uint8_t>(g),
         static_cast<std::uint8_t>(b)
      );
      return true;
   }

   inline bool read_value(std::istream& stream, Point2& value) {
      double x;
      double y;

      if (!(stream >> x))
         return false;

      if (!(stream >> y)) {
         stream.setstate(std::ios::failbit);
         return false;
      }

      value = Point2(x, y);
      return true;
   }

   inline bool read_value(std::istream& stream, Point3& value) {
      double x;
      double y;
      double z;

      if (!(stream >> x))
         return false;

      if (!(stream >> y) || !(stream >> z)) {
         stream.setstate(std::ios::failbit);
         return false;
      }

      value = Point3(x, y, z);
      return true;
   }

   inline bool read_value(std::istream& stream, Vector3& value) {
      double x;
      double y;
      double z;

      if (!(stream >> x))
         return false;

      if (!(stream >> y) || !(stream >> z)) {
         stream.setstate(std::ios::failbit);
         return false;
      }

      value = Vector3(x, y, z);
      return true;
   }

   template <typename T>
   struct is_std_vector : std::false_type {};

   template <typename T, typename Allocator>
   struct is_std_vector<std::vector<T, Allocator>> : std::true_type {};

   template <typename T>
   inline constexpr bool is_std_vector_v = is_std_vector<T>::value;

   template <typename T>
   requires (!is_std_vector_v<T>)
   bool convert(
      const std::string& attrName, 
      const std::string& attrContent, 
      ParamSet* ps
   ) {
      if (ps == nullptr)
         throw std::invalid_argument("ParamSet pointer cannot be null");
      
      std::string trimmedContent = trim(attrContent);

      if (trimmedContent.empty())
         throw std::invalid_argument(attrName + " cannot be empty");
      std::istringstream stream(trimmedContent);

      if (stream.peek() == EOF)
         throw std::invalid_argument(attrName + " cannot be empty");

      T value;
      if (!read_value(stream, value))
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

      while (true) {
         stream >> std::ws;
         if (stream.peek() == EOF)
            break;

         Member member;
         if (!read_value(stream, member))
            throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(Member).name()));

         values.push_back(member);
      }

      ps->add(attrName, values);
      return true;
   }

   template <typename T, uint8_t N>
   requires is_std_vector_v<T>
   bool convert(
      const std::string& attrName, 
      const std::string& attrContent, 
      ParamSet* ps
   ) {
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

      while (true) {
         stream >> std::ws;
         if (stream.peek() == EOF)
            break;

         Member member;
         if (!read_value(stream, member))
            throw std::invalid_argument(attrName + ": " + trimmedContent + " contains invalid member: " + std::string(typeid(Member).name()));

         values.push_back(member);
      }

      if (values.size() != N) {
         throw std::invalid_argument(attrName + ": " + trimmedContent + " must contain exactly " + std::to_string(N) + " members");
      }

      ps->add(attrName, values);
      return true;
   }

   bool convert_color(
      const std::string& attrName, 
      const std::string& attrContent, 
      ParamSet* ps
   );
}

#endif // !CONVERT_FUNCTION_HPP