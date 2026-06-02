#include "Objects/Aggregate/BVHAccel.hpp"
#include <random>

namespace raytracer {
   BVHAccel::BVHAccel(int maxPrimitivesPerNode, SplitMethod splitMethod): 
      AggregatePrimitive(),
      _bounds(Point3(0, 0, 0), Point3(0, 0, 0)), 
      _primitives(std::make_shared<PrimitiveList>()), 
      _left(nullptr), 
      _right(nullptr),
      _maxPrimitivesPerNode(maxPrimitivesPerNode),
      _splitMethod(splitMethod) { }   

   BVHAccel::BVHAccel(const ParamSet& params): 
      AggregatePrimitive(),
      _bounds(Point3(0, 0, 0), Point3(0, 0, 0)), 
      _primitives(std::make_shared<PrimitiveList>()), 
      _left(nullptr), 
      _right(nullptr)
      {
         _maxPrimitivesPerNode = params.retrieveOrDefault<uint>("max_prims_per_node", 4);
         std::string splitMethodStr = params.retrieveOrDefault<std::string>("split_method", "middle");

         if (splitMethodStr == "middle") {
            _splitMethod = SplitMethod::MIDDLE;
         } else {
            throw std::invalid_argument("Invalid split method: " + splitMethodStr);
         }
      }

   bool BVHAccel::intersect(const Ray& ray) const {
      float tMin, tMax;
      if (!_bounds.intersect(ray, tMin, tMax)) return false;

      if (_left == nullptr && _right == nullptr) {
         return _primitives  && _primitives->intersect(ray);
      } else if (_left != nullptr && _left->intersect(ray)) {
         return true;
      } else if (_right != nullptr && _right->intersect(ray)) {
         return true;
      }

      return false;
   }

   bool BVHAccel::intersectWithSurfel(const Ray& ray, Surfel* sf) const {
      float tMin, tMax;
      if (!_bounds.intersect(ray, tMin, tMax)) return false;
      
      if (_left == nullptr && _right == nullptr) {
         return _primitives != nullptr && _primitives->intersectWithSurfel(ray, sf);
      }

      if (!sf) {
         return (_left && _left->intersect(ray)) || (_right && _right->intersect(ray));
      }

      sf->t = std::numeric_limits<float>::infinity();
      bool hit = false;

      Surfel leftSf = Surfel();
      leftSf.t = std::numeric_limits<float>::infinity();

      Surfel rightSf = Surfel();
      rightSf.t = std::numeric_limits<float>::infinity();

      if (_left != nullptr && _left->intersectWithSurfel(ray, &leftSf)) {
         hit = true;
         updateSurfel(sf, leftSf);
      } 
      
      if (_right != nullptr && _right->intersectWithSurfel(ray, &rightSf)) {
         hit = true;
         updateSurfel(sf, rightSf);
      }

      return hit;
   }

   void BVHAccel::updateSurfel(Surfel* sf, const Surfel candidate) const {
      if (candidate.t < sf->t) {
         *sf = candidate;
      }
   }

   void BVHAccel::buildBVH() {
      if (_primitives->size() > _maxPrimitivesPerNode) {
         int elementsPerLeaf = _primitives->size() / 2;

         Axis randomAxis = getRandomAxis();

         std::sort(
            _primitives->begin(), 
            _primitives->end(), 
            [randomAxis](
               const std::shared_ptr<Primitive>& a, 
               const std::shared_ptr<Primitive>& b
            ) {
               return a->getBounds().min()[randomAxis] < b->getBounds().min()[randomAxis];
            }
         );

         _left = std::make_shared<BVHAccel>(_maxPrimitivesPerNode, _splitMethod);
         _right = std::make_shared<BVHAccel>(_maxPrimitivesPerNode, _splitMethod);
         
         auto begin = _primitives->begin();
         auto middle = begin + elementsPerLeaf;
         auto end = _primitives->end();

         _left->insert(begin, middle);
         _left->buildBVH();

         _right->insert(middle, end);
         _right->buildBVH();

         _primitives->clear();
      }

      updateBounds();
   }

   void BVHAccel::insert(iterator const& begin, iterator const& end) {
      _primitives->insert(begin, end);
   }

   void BVHAccel::add(const std::shared_ptr<Primitive>& primitive) {
      _primitives->add(primitive);
   }

   void BVHAccel::merge(const std::shared_ptr<AggregatePrimitive>& other) {
      auto otherBVH = std::dynamic_pointer_cast<BVHAccel>(other);
      if (otherBVH) {
         _primitives->merge(otherBVH->_primitives);
      } else {
         _primitives->merge(other);
      }
   }

   void BVHAccel::updateBounds() {
      if (_left && _right) {
         auto leftBounds = _left->getBounds();
         auto rightBounds = _right->getBounds();
         _bounds = leftBounds.merge(rightBounds);
      } else if (_left) {
         _bounds = _left->getBounds();
      } else if (_right) {
         _bounds = _right->getBounds();
      } else if (_primitives) {
         _bounds = _primitives->getBounds();
      } else {
         _bounds = Bounds3(POINT3_ZERO, POINT3_ZERO);
      }
   }

   const Bounds3 BVHAccel::getBounds() const {
      return _bounds;
   }

   Axis BVHAccel::getRandomAxis() const {
      static thread_local std::mt19937 rng{std::random_device{}()};      
      static thread_local std::uniform_int_distribution<int> dist(0, 2);

      int axis = dist(rng);

      return Axis(axis);
   }
}