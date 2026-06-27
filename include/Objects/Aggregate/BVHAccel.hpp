#ifndef BVH_ACCEL_HPP
#define BVH_ACCEL_HPP

#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include "Math/Bounds3.hpp"
#include <vector>

namespace raytracer {

   class BVHAccel : public AggregatePrimitive {
      public:
      enum SplitMethod {
         MIDDLE
      };
      private:
      
         Bounds3 _bounds;
         std::shared_ptr<PrimitiveList> _primitives;
         std::shared_ptr<BVHAccel> _left;
         std::shared_ptr<BVHAccel> _right;
         int _maxPrimitivesPerNode;
         SplitMethod _splitMethod;
         Axis _lastSplitAxis;

         void updateSurfel(Surfel* sf, const Surfel candidate) const;
         void updateBounds(const Transform& transform);
         Axis getRandomAxis() const;

      public:
         BVHAccel(int maxPrimitivesPerNode = 4, SplitMethod splitMethod = SplitMethod::MIDDLE);
         BVHAccel(const ParamSet& params);
         ~BVHAccel() = default;

         bool intersect(const Ray& ray, const Transform& transform) const override;
         bool intersectWithSurfel(const Ray& ray, const Transform& transform, Surfel* sf) const override;

         void add(const instance& instance) override;
         void merge(
            const std::shared_ptr<AggregatePrimitive>& other,
            const Transform& transform
         ) override;

         void insert(iterator const& begin, iterator const& end) override;
         void buildBVH(const Transform& transform);

         const Bounds3 getBounds(const Transform& transform) const override;

         const BVHAccel* getLeft()  const;
         const BVHAccel* getRight() const;
         const PrimitiveList* getPrimitives() const;
         Axis getSplitAxis() const;
   };
}

#endif // BVH_ACCEL_HPP