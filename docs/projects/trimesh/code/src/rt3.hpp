/// @file rt3.hpp

#ifndef RT3_HPP
#define RT3_HPP

#include <iostream>
using std::cerr;
using std::cin;
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <array>
using std::array;

#include <vector>
using std::vector;

#include <algorithm>
using std::copy;

#include <memory>
using std::shared_ptr;

#include <map>
using std::map;

// Alias to make the code similar to production code.
using real_t = float;
using Point3f = array<real_t, 3>;
using Normal3f = array<real_t, 3>;
using Point2f = array<real_t, 2>;

/* Just to compile. TODO: reaplace this in your code accordingly. */
class Bounds3f {};
class Ray {};
class Surfel {};

using ParamSet = std::map<string, string>;

class Shape {
public:
  Shape() = default;
  // Return the triangle's bounding box.
  // virtual Bounds3f object_bound() const { retur};
  virtual bool intersect(const Ray& ray, float* thit, Surfel* isect) const = 0;
  [[nodiscard]] virtual bool intersect_p(const Ray& ray) const = 0;
};

#endif
