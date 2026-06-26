#include "Parser/ParserScene.hpp"
#include <algorithm>
#include <string>
#include <cctype>
#include <iostream>
#include <filesystem>
#include "Image/RGBColor.hpp"
#include <unordered_map>
#include "Math/Point3.hpp"
#include "Math/Vector3.hpp"
#include "Objects/Materials/MaterialFactory.hpp"
#include "Objects/PrimitiveFactory.hpp"
#include "Objects/Light/LightFactory.hpp"
#include "Objects/Aggregate/AggregateFactory.hpp"

namespace raytracer
{

   // ── maps ──────────────────────────────────────────────────────────────────

   std::unordered_map<std::string, std::vector<std::string>> elementList{
       {"camera", {"type", "screen_window", "fovy"}},
       {"lookat", {"look_from", "look_at", "up"}},
       {"background", {"type", "filename", "mapping", "color", "tl", "tr", "bl", "br", "t", "b", "l", "r", "spherical"}},
       {"film", {"type", "filename", "img_type", "x_res", "y_res", "w_res", "h_res", "crop_window", "gamma_corrected"}},
       {"include", {"filename"}},
       {"integrator", {"type", "near_color", "far_color", "zmin", "zmax", "depth", "mapping_interval", "n_intervals"}},
       {"world_begin", {}},
       {"world_end", {}},
       {"render_again", {}},
       {"object", {"type", "center", "origin", "radius", "norm", "material", "first_point", "second_point", "filename", "ntriangles", "indices", "vertices", "normals", "uv", "reverse_vertex_order", "compute_normals", "backface_cull", "base", "top"}},
       {"make_named_material", {"type", "name", "color", "color1", "color2", "spacing", "diffuse", "specular", "ambient", "glossiness", "mirror", "color_map"}},
       {"named_material", {"name"}},
       {"material", {"type", "color", "name", "color1", "color2", "spacing", "diffuse", "specular", "ambient", "glossiness", "mirror", "color_map"}},
       {"light_source", {"type", "I", "scale", "from", "to", "attenuation", "cutoff", "falloff", "world_radius"}},
       {"aggregator", {"type", "split_method", "max_prims_per_node"}},
       {"object_instance_begin", {"name"}},
       {"object_instance_end", {}},
       {"push_CTM", {}},
       {"pop_CTM", {}},
       {"translate", {"value"}},
       {"scale", {"value"}},
       {"rotate", {"axis", "angle"}},
      };

   std::unordered_map<std::string, ConvertFunction> converters {
      {"type", convert<std::string>},
      {"name", convert<std::string>},
      {"filename", convert<std::string>},
      {"img_type", convert<std::string>},
      {"mapping", convert<std::string>},
      {"fovy", convert<int>},

      {"color", convert<RGBColor>},
      {"bl", convert<RGBColor>},
      {"tl", convert<RGBColor>},
      {"tr", convert<RGBColor>},
      {"br", convert<RGBColor>},
      {"t", convert<RGBColor>},
      {"b", convert<RGBColor>},
      {"r", convert<RGBColor>},
      {"l", convert<RGBColor>},
      {"spherical", convert<bool>},
      {"screen_window", convert<std::vector<float>, 4>},

      {"x_res", convert<int>},
      {"y_res", convert<int>},
      {"w_res", convert<int>},
      {"h_res", convert<int>},

      {"flip", convert<bool>},
      {"gamma_corrected", convert<bool>},

      {"look_from", convert<raytracer::Point3>},
      {"look_at", convert<raytracer::Point3>},
      {"up", convert<raytracer::Vector3>},

      {"radius", convert<float>},
      {"origin", convert<raytracer::Point3>},
      {"center", convert<raytracer::Point3>},
      {"norm", convert<raytracer::Vector3>},
      {"material", convert<std::string>},
      {"first_point", convert<raytracer::Point3>},
      {"second_point", convert<raytracer::Point3>},
      {"color_map", convert<std::vector<raytracer::RGBColor>>},

      // Tringle Mesh
      {"ntriangles", convert<uint>},
      {"indices", convert<std::vector<uint>>},
      {"vertices", convert<std::vector<Point3>>},
      {"normals", convert<std::vector<Vector3>>},
      {"uv", convert<std::vector<Point2>>},
      {"reverse_vertex_order", convert<bool>},
      {"compute_normals", convert<bool>},
      {"backface_cull", convert<bool>},

      // Cylinder
      {"base", convert<raytracer::Point3>},
      {"top", convert<raytracer::Point3>},

      {"color1", convert<raytracer::RGBColor>},
      {"color2", convert<raytracer::RGBColor>},
      {"spacing", convert<float>},

      {"near_color", convert<raytracer::RGBColor>},
      {"far_color", convert<raytracer::RGBColor>},
      {"zmin", convert<float>},
      {"zmax", convert<float>},
      {"depth", convert<uint>},
      {"mapping_interval", convert<std::vector<float>>},
      {"n_intervals", convert<uint>},

      // Light attributes
      {"I", convert_color},
      {"scale", convert<raytracer::Vector3>},
      {"from", convert<raytracer::Point3>},
      {"to", convert<raytracer::Point3>},
      {"attenuation", convert<raytracer::Vector3>},
      {"diffuse", convert<raytracer::Vector3>},
      {"specular", convert<raytracer::Vector3>},
      {"ambient", convert<raytracer::Vector3>},
      {"glossiness", convert<float>},
      {"mirror", convert<raytracer::Vector3>},
      {"cutoff", convert<float>},
      {"falloff", convert<float>},
      {"world_radius", convert<float>},

      // Aggregator
      {"split_method", convert<std::string>},
      {"max_prims_per_node", convert<uint>},

      // Geometric Transformation
      {"axis", convert<Vector3>},
      {"value", convert<Vector3>},
      {"angle", convert<float>}

   };

   // ── helpers ──────────────────────────────────────────────────────────────────

   bool ParserScene::isValidElement(std::string_view elementName)
   {
      auto element = elementList.find((std::string)elementName);
      return element != elementList.end();
   }

   bool ParserScene::isValidAttribute(std::string_view elementName, std::string_view attrName)
   {
      auto it = elementList.find(std::string(elementName));
      if (it == elementList.end())
         return false;
      const auto &attrs = it->second;
      return std::find(attrs.begin(), attrs.end(), attrName) != attrs.end();
   }

   std::string ParserScene::stringToLower(std::string s)
   {
      std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                     { return std::tolower(c); });
      return s;
   }

   void ParserScene::parseAttribute(std::string_view attrName, std::string_view attrValue, ParamSet *ps)
   {
      auto it = converters.find(std::string(attrName));
      if (it == converters.end())
      {
         std::cerr << "[Parser] No converter for attribute: " << attrName << '\n';
         return;
      }
      if (!it->second(std::string(attrName), std::string(attrValue), ps))
         std::cerr << "[Parser] Failed to convert: " << attrName << " = " << attrValue << '\n';
   }

   // ── parsing functions ──────────────────────────────────────────────────────────────────

   /**
    * @param filename The path to the XML file to parse
    * @brief Load document from file and delegate to parseDocument
    */
   void ParserScene::parseScene(const char *filename, Scene &scene,
                                OnElementCallback onElement)
   {
      tinyxml2::XMLDocument doc;
      if (doc.LoadFile(filename) != tinyxml2::XML_SUCCESS)
      {
         std::cerr << "[Parser] Failed to load XML: " << filename << '\n';
         return;
      }
      parseDocument(doc, scene, onElement);
   }

   /**
    * @param xmlContent The XML content as a string
    * @param fromString A boolean flag to differentiate this overload (not used in logic)
    * @brief Load document from string and delegate to parseDocument (useful for testing)
    */
   void ParserScene::parseScene(const char *xmlContent, bool fromString, Scene &scene,
                                OnElementCallback onElement)
   {
      tinyxml2::XMLDocument doc;
      if (doc.Parse(xmlContent) != tinyxml2::XML_SUCCESS)
      {
         std::cerr << "[Parser] Failed to parse XML string.\n";
         return;
      }
      parseDocument(doc, scene, onElement);
   }

   void ParserScene::parseDocument(tinyxml2::XMLDocument &doc, Scene &scene,
                                   OnElementCallback onElement)
   {
      tinyxml2::XMLElement *root = doc.RootElement();
      if (!root)
      {
         throw std::runtime_error("XML document has no root element.");
      }

      scene = Scene();

      for (auto *node = root->FirstChildElement(); node; node = node->NextSiblingElement())
      {
         const std::string element = node->Name();

         if (!isValidElement(element))
         {
            std::cerr << "[Parser] Invalid element: <" << element << ">\n";
            continue;
         }

         ParamSet ps;

         for (auto *attr = node->FirstAttribute(); attr; attr = attr->Next())
         {
            const std::string attrName = attr->Name();

            if (!isValidAttribute(element, attrName))
            {
               std::cerr << "[Parser] <" << element << "> has no attribute '" << attrName << "'\n";
               continue;
            }

            auto it = converters.find(attrName);
            if (it == converters.end())
            {
               std::cerr << "[Parser] No converter for attribute: " << attrName << '\n';
               continue;
            }

            if (!it->second(attrName, attr->Value(), &ps))
               std::cerr << "[Parser] Failed to convert: " << attrName << " = " << attr->Value() << '\n';
         }

         if (element == "include")
         {
            const auto incFile = ps.retrieve<std::string>("filename");
            if (incFile.empty())
            {
               std::cerr << "[Parser] <include> is missing 'filename'.\n";
               continue;
            }
            if (!std::filesystem::exists(incFile))
            {
               std::cerr << "[Parser] Included file not found: " << incFile << '\n';
               continue;
            }

            auto includedScene = Scene();
            parseScene(incFile.c_str(), includedScene, nullptr);
            scene.include(includedScene);

            // Notify for the include itself.
            if (onElement)
               onElement(scene, element, ps);

            if (includedScene.getParams().count("world_end") > 0)
            {
               ParamSet empty;
               if (onElement)
                  onElement(scene, "world_end", empty);
            }
      } else if (element == "aggregator") {
            try
            {
               auto aggregator = AggregateFactory::create(ps);
               if (aggregator) {
                  scene.addAggregate(aggregator);
               }
            }
            catch (const std::exception &e)
            {
               std::cerr << "[Parser] Failed to create aggregator: " << e.what() << '\n';
            }
            
            if (onElement)
               onElement(scene, element, ps);
      } else if (element == "material")
         {
            try
            {
               auto material = MaterialFactory::create(ps);
               scene.addMaterial(material);
            }
            catch (const std::exception &e)
            {
               std::cerr << "[Parser] Failed to create material: " << e.what() << '\n';
            }
            if (onElement)
               onElement(scene, element, ps);
         }
         else if (element == "make_named_material")
         {
            try
            {
               auto material = MaterialFactory::create(ps);
               scene.addNamedMaterial(material);
            }
            catch (const std::exception &e)
            {
               std::cerr << "[Parser] Failed to create named material: " << e.what() << '\n';
            }
            if (onElement)
               onElement(scene, element, ps);
         }
         else if (element == "named_material")
         {
            if (!ps.has("name"))
            {
               std::cerr << "[Parser] <named_material> is missing 'name'.\n";
               continue;
            }
            try
            {
               std::string name = ps.retrieve<std::string>("name");
               scene.activateNamedMaterial(name);
            }
            catch (const std::exception &e)
            {
               std::cerr << "[Parser] Failed to activate named material: " << e.what() << '\n';
            }
            if (onElement)
               onElement(scene, element, ps);
         }
         else if (element == "object")
         {
            if (!scene.hasAggregate()) {
               scene.addAggregate(std::make_shared<PrimitiveList>());
            }

            try
            {
               auto primitives = PrimitiveFactory::create(ps, scene);
               scene.addPrimitives(primitives);
            }
            catch (const std::exception &e)
            {
               std::cerr << "[Parser] Failed to create object: " << e.what() << '\n';
            }
            if (onElement)
               onElement(scene, element, ps);
         }
         else if (element == "light_source")
         {
            try
            {
               auto light = LightFactory::create(ps);
               scene.addLight(light);
            }
            catch (const std::exception &e)
            {
               std::cerr << "[Parser] Failed to create light source: " << e.what() << '\n';
            }

            if (onElement)
               onElement(scene, element, ps);
         }
         else if(element == "push_CTM"
               || element == "pop_CTM"
               || element == "translate"
               || element == "rotate"
               || element == "scale") {

            if (onElement){
               onElement(scene, element, ps);
            }
         }
         else
         {
            scene.setParam(element, ps);
            if (onElement)
               onElement(scene, element, ps);
         }
      }
   }
};