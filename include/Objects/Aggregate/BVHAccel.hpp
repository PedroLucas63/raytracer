#ifndef BVH_ACCEL_HPP
#define BVH_ACCEL_HPP

#include "Objects/Aggregate/AggregatePrimitive.hpp"
#include "Objects/Aggregate/PrimitiveList.hpp"
#include "Math/Bounds3.hpp"
#include <vector>

namespace raytracer {
   class BVHAccel : public AggregatePrimitive {
      private:
         

         Bounds3 _bounds;
         std::shared_ptr<PrimitiveList> _primitives;
         std::shared_ptr<BVHAccel> _left;
         std::shared_ptr<BVHAccel> _right;
         int _maxPrimitivesPerNode;
         enum SplitMethod {
            MIDDLE
         } _splitMethod;

         void updateSurfel(Surfel* sf, const Surfel candidate) const;
         void updateBounds();
         Axis getRandomAxis() const;

      public:
         BVHAccel(int maxPrimitivesPerNode = 4, SplitMethod splitMethod = SplitMethod::MIDDLE);
         BVHAccel(const ParamSet& params);
         ~BVHAccel() = default;

         bool intersect(const Ray& ray) const override;
         bool intersectWithSurfel(const Ray& ray, Surfel* sf) const override;

         void add(const std::shared_ptr<Primitive>& primitive) override;
         void merge(const std::shared_ptr<AggregatePrimitive>& other) override;

         void insert(iterator const& begin, iterator const& end) override;
         void buildBVH();

         const Bounds3 getBounds() const override;
   };
}

#endif // BVH_ACCEL_HPP