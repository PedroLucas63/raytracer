#include "Objects/Aggregate/LinearBVHAccel.hpp"

namespace raytracer {
   LinearBVHAccel::LinearBVHAccel(int maxPrimitivesPerNode, BVHAccel::SplitMethod splitMethod): 
      AggregatePrimitive(),
      _bounds(Point3(0, 0, 0), Point3(0, 0, 0)), 
      _maxPrimitivesPerNode(maxPrimitivesPerNode),
      _splitMethod(splitMethod) { }   

   LinearBVHAccel::LinearBVHAccel(const ParamSet& params): 
      AggregatePrimitive(),
      _bounds(Point3(0, 0, 0), Point3(0, 0, 0))
      {
         _maxPrimitivesPerNode = params.retrieveOrDefault<uint>("max_prims_per_node", 4);
         std::string splitMethodStr = params.retrieveOrDefault<std::string>("split_method", "middle");

         if (splitMethodStr == "middle") {
            _splitMethod = BVHAccel::SplitMethod::MIDDLE;
         } else {
            throw std::invalid_argument("Invalid split method: " + splitMethodStr);
         }

         _buildTree = std::make_shared<BVHAccel>(_maxPrimitivesPerNode, _splitMethod);
      }


   // ========================================


   void LinearBVHAccel::buildLBVH() {
      _buildTree->buildBVH();
      int totalNodes = countNodes(_buildTree.get());
      _nodes.resize(totalNodes);

      int offset = 0;
      flattenBVHTree(_buildTree.get(), &offset);

      _bounds = _nodes[0].bounds;
      _buildTree.reset();

   }

   int LinearBVHAccel::countNodes(const BVHAccel* node) const{
      if(!node) return 0;
      return + 1 + countNodes(node->getLeft()) + countNodes(node->getRight());
   }


   int LinearBVHAccel::flattenBVHTree(const BVHAccel* node, int* offset){
      LinearBVHNode* linearNode = &_nodes[*offset];
      linearNode->bounds = node->getBounds();

      int myOffset = (*offset)++;

      const BVHAccel* left = node->getLeft();
      const BVHAccel* right = node->getRight();

      if(left == nullptr && right == nullptr){
         linearNode->primitivesOffset = static_cast<int>(_orderedPrimitives.size());
 
         PrimitiveList* prims = const_cast<PrimitiveList*>(node->getPrimitives());
         uint16_t count = 0;
         if (prims) {
            for (auto it = prims->begin(); it != prims->end(); ++it) {
               _orderedPrimitives.push_back(*it);
               ++count;
            }
         }
         linearNode->nPrimitives = count;
         linearNode->axis        = 0;
      }else{
         linearNode->nPrimitives = 0;
         linearNode->axis        = static_cast<uint8_t>(node->getSplitAxis());
         if (left)  flattenBVHTree(left, offset);
         linearNode->secondChildOffset = right ? flattenBVHTree(right, offset) : -1;
      }

      return myOffset;
   }

   bool LinearBVHAccel::intersect(const Ray& ray) const {
      if(_nodes.empty()) return false;
      
      int toVisit[64];
      int toVisitOffset = 0;
      int currentIdx    = 0;

      while(true){
         const LinearBVHNode& node = _nodes[currentIdx];
         float tMin, tMax;

         if(node.bounds.intersect(ray, tMin, tMax)){
            if(node.nPrimitives > 0){
               for (int i = 0; i < node.nPrimitives; i++){
                  if(_orderedPrimitives[node.primitivesOffset + i]->intersect(ray)) return true;
               }
            } else{
               toVisit[toVisitOffset++] = node.secondChildOffset;
               currentIdx = currentIdx + 1;
               continue;
            }
         }
         if (toVisitOffset == 0) break;
         currentIdx = toVisit[--toVisitOffset];
      }

      return false;
   }

   bool LinearBVHAccel::intersectWithSurfel(const Ray& ray, Surfel* sf) const {
      if (_nodes.empty()) return false;
 
      if (sf) sf->t = std::numeric_limits<float>::infinity();
   
      bool hit          = false;
      int  toVisit[64];
      int  toVisitOffset = 0;
      int  currentIdx    = 0;

      while(true){
         const LinearBVHNode& node = _nodes[currentIdx];
         float tMin, tMax;

         if(node.bounds.intersect(ray, tMin, tMax)){
            if(node.nPrimitives > 0){
               for (int i = 0; i < node.nPrimitives; i++){
                  const auto& prim = _orderedPrimitives[node.primitivesOffset + i];

                  if(!sf){
                     if (prim->intersect(ray)) return true;                    
                  } else{
                     Surfel candidate;
                     candidate.t = std::numeric_limits<float>::infinity();
                     if (prim->intersectWithSurfel(ray, &candidate)) {
                        hit = true;
                        if (candidate.t < sf->t) *sf = candidate;
                     }
                  }
               }
            } else{
               toVisit[toVisitOffset++] = node.secondChildOffset;
               currentIdx = currentIdx + 1;
               continue;
            }
         }
         if (toVisitOffset == 0) break;
         currentIdx = toVisit[--toVisitOffset];
      }

      return hit;
   }
   
   void LinearBVHAccel::add(const std::shared_ptr<Primitive>& primitive) {
      _buildTree->add(primitive);
   }

   void LinearBVHAccel::insert(iterator const& begin, iterator const& end) {
      _buildTree->insert(begin, end);
   }


   void LinearBVHAccel::merge(const std::shared_ptr<AggregatePrimitive>& other) {
      _buildTree->merge(other);
   }

   const Bounds3 LinearBVHAccel::getBounds() const {
      return _bounds;
   }


}