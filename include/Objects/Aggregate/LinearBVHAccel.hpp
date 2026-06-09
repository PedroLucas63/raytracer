#ifndef LINEAR_BVH_ACCEL_HPP
#define LINEAR_BVH_ACCEL_HPP

#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include "Objects/Aggregate/BVHAccel.hpp"
#include "Math/Bounds3.hpp"
#include <vector>
#include <cstdint>


namespace raytracer {

   struct LinearBVHNode{
      Bounds3 bounds;
      union {
         int primitivesOffset;
         int secondChildOffset;
      };
      uint16_t nPrimitives;
      uint8_t axis;
      uint8_t pad[1];        // ensure 32 byte total size

   };


   class LinearBVHAccel : public AggregatePrimitive {
      private:
         std::vector<LinearBVHNode> _nodes;
         std::vector<std::shared_ptr<Primitive>> _orderedPrimitives;
         std::shared_ptr<BVHAccel> _buildTree;
         Bounds3 _bounds;
         int     _maxPrimitivesPerNode;
         BVHAccel::SplitMethod _splitMethod;


         int flattenBVHTree(const BVHAccel* node, int* offset);
         int countNodes(const BVHAccel* node) const;

      public:
         LinearBVHAccel(int maxPrimitveNodes = 4, BVHAccel::SplitMethod = BVHAccel::SplitMethod::MIDDLE);

         LinearBVHAccel(const ParamSet& params);
         ~LinearBVHAccel() = default;

         bool intersect(const Ray& ray) const override;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const override;

         void add(const std::shared_ptr<Primitive>& primitive) override;
         void merge(const std::shared_ptr<AggregatePrimitive>& other) override;

         void insert(iterator const& begin, iterator const& end) override;
         void buildLBVH();

         const Bounds3 getBounds() const override;
   };
}

#endif // LINEAR_BVH_ACCEL_HPP