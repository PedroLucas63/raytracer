#ifndef PARAMSET_HPP
#define PARAMSET_HPP

#include <map>
#include <memory>
#include <string>
#include <stdexcept>


namespace raytracer{

   class GenericType {
      public:
         virtual ~GenericType() = default;
   };
   
   template <typename T>
   class ValueType : public GenericType{
      private:
         T _value;

      public:
         explicit ValueType(const T& value) : _value(value){}

         const T& get() const {
            return _value;
         }

   };
   
   class ParamSet{
      private:
         std::map<std::string, std::shared_ptr<GenericType>> _params;
         std::shared_ptr<GenericType> find(const std::string& key) const;
         
      public:

         ParamSet() = default;
         ~ParamSet() = default;


      /**
       * Add a generic parameter identified by `key`.
       */
      template <typename T>
      void add(const std::string& key, const T& value){
         _params[key] = std::make_shared<ValueType<T>>(value);

      }

      /**
       * Resolve (get) the parameter value of type `T`.
       */
      template <typename T>
      T retrieve(const std::string& key) const{
         auto base_ptr = find(key);

         auto derived = std::dynamic_pointer_cast<ValueType<T>>(base_ptr);

         if(!derived){
            throw std::bad_cast();
         }

         return derived->get();
      }

      template <typename T>
      T retrieveOrDefault(const std::string& key, const T& defaultValue = T{}) const {
         if (!has(key)) {
            return defaultValue;
         }
         return retrieve<T>(key);
      }

      bool has(const std::string& key) const;    

      void remove(const std::string& key);
      void clear();
      bool isEmpty() const;
      size_t size()  const;
   };

}

#endif // PARAMSET_HPP
