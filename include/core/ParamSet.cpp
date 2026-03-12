#include "ParamSet.hpp"

namespace raytracer{

   /**
    * Find a stored parameter by `key`.
    */
   std::shared_ptr<GenericType> ParamSet::find(const std::string& key) const{
      auto it = _params.find(key);;
      if(it == _params.end())
         throw std::out_of_range("ParamSet: Key not found -> " + key);
      return it->second;
   }

   bool ParamSet::has(const std::string& key) const {
      return _params.find(key) != _params.end();
   }

   void ParamSet::remove(const std::string& key) { _params.erase(key); }

   void ParamSet::clear() { _params.clear(); }

   bool ParamSet::isEmpty() const { return _params.empty(); }

   size_t ParamSet::size()  const { return _params.size(); }
}