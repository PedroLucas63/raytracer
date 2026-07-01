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

   bool BVHAccel::intersect(const Ray& ray, const Transform& transform) const {
      float tMin, tMax;
      Ray localRay = transform.isIdentity() ? ray : transform(ray, true);
      
      if (!_bounds.intersect(localRay, tMin, tMax)) return false;

      if (_left == nullptr && _right == nullptr) {
         return _primitives  && _primitives->intersect(ray, transform);
      } else if (_left != nullptr && _left->intersect(ray, transform)) {
         return true;
      } else if (_right != nullptr && _right->intersect(ray, transform)) {
         return true;
      }

      return false;
   }

   bool BVHAccel::intersectWithSurfel(const Ray& ray, const Transform& transform, Surfel* sf) const {
      float tMin, tMax;
      Ray localRay = transform.isIdentity() ? ray : transform(ray, true);

      if (!_bounds.intersect(localRay, tMin, tMax)) return false;
      
      if (_left == nullptr && _right == nullptr) {
         return _primitives != nullptr && _primitives->intersectWithSurfel(ray, transform, sf);
      }

      if (!sf) {
         return (_left && _left->intersect(ray, transform)) || (_right && _right->intersect(ray, transform));
      }

      sf->t = std::numeric_limits<float>::infinity();
      bool hit = false;

      Surfel leftSf = Surfel();
      leftSf.t = std::numeric_limits<float>::infinity();

      Surfel rightSf = Surfel();
      rightSf.t = std::numeric_limits<float>::infinity();

      if (_left != nullptr && _left->intersectWithSurfel(ray, transform, &leftSf)) {
         hit = true;
         updateSurfel(sf, leftSf);
      } 
      
      if (_right != nullptr && _right->intersectWithSurfel(ray, transform, &rightSf)) {
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

   void BVHAccel::buildBVH(const Transform& transform) {
      if (_primitives->size() > _maxPrimitivesPerNode) {
         int elementsPerLeaf = _primitives->size() / 2;

         Axis randomAxis = getRandomAxis();
         _lastSplitAxis = randomAxis;

         std::sort(
            _primitives->begin(), 
            _primitives->end(), 
            [randomAxis](
               const auto& a, 
               const auto& b
            ) {
               return a.first->getBounds(*(a.second)).min()[randomAxis] < b.first->getBounds(*(b.second)).min()[randomAxis];
            }
         );

         _left = std::make_shared<BVHAccel>(_maxPrimitivesPerNode, _splitMethod);
         _right = std::make_shared<BVHAccel>(_maxPrimitivesPerNode, _splitMethod);
         
         auto begin = _primitives->begin();
         auto middle = begin + elementsPerLeaf;
         auto end = _primitives->end();

         _left->insert(begin, middle);
         _left->buildBVH(transform);

         _right->insert(middle, end);
         _right->buildBVH(transform);

         _primitives->clear();
      }

      updateBounds(transform);
   }

   void BVHAccel::insert(iterator const& begin, iterator const& end) {
      _primitives->insert(begin, end);
   }

   void BVHAccel::add(const instance& instance) {
      _primitives->add(instance);
   }

   void BVHAccel::merge(
      const std::shared_ptr<AggregatePrimitive>& other,
      const Transform& transform
   ) {
      auto otherBVH = std::dynamic_pointer_cast<BVHAccel>(other);
      if (otherBVH) {
         _primitives->merge(otherBVH->_primitives, transform);
      } else {
         _primitives->merge(other, transform);
      }
   }

   void BVHAccel::updateBounds(const Transform& transform) {
      if (_left && _right) {
         auto leftBounds = _left->getBounds(transform);
         auto rightBounds = _right->getBounds(transform);
         _bounds = leftBounds.merge(rightBounds);
      } else if (_left) {
         _bounds = _left->getBounds(transform);
      } else if (_right) {
         _bounds = _right->getBounds(transform);
      } else if (_primitives) {
         _bounds = _primitives->getBounds(transform);
      } else {
         _bounds = Bounds3(POINT3_ZERO, POINT3_ZERO);
      }
   }

   const Bounds3 BVHAccel::getBounds(const Transform& transform) const {
      return transform.isIdentity() ? _bounds : transform(_bounds);
   }

   Axis BVHAccel::getRandomAxis() const {
      static thread_local std::mt19937 rng{std::random_device{}()};      
      static thread_local std::uniform_int_distribution<int> dist(0, 2);

      int axis = dist(rng);

      return Axis(axis);
   }


   const BVHAccel* BVHAccel::getLeft()  const { 
      return _left.get(); 
   }

   const BVHAccel* BVHAccel::getRight() const { 
      return _right.get(); 
   }

   const PrimitiveList* BVHAccel::getPrimitives() const { 
      return _primitives.get(); 
   }

   Axis BVHAccel::getSplitAxis() const { 
      return _lastSplitAxis; 
   }
}