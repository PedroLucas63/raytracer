#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Film.hpp"
#include "Math/Ray.hpp"
#include "Parser/ParserScene.hpp"
#include "Math/Vector3.hpp"
#include "Math/Point3.hpp"

namespace raytracer {
   enum CameraType {
      Perspective,
      Orthographic
   };

   struct ScreenWindow {
      float l, r, b, t;
   };

   // class Camera {
   //    public:
   //       Film film;
   //       CameraType type;
   //       Vector3 u, v, w; // Camera coordinate system basis vectors
   //       Point3 eye;
   //       ScreenWindow screenWindow;


   //       Camera(const Film& film, CameraType type):
   //          film(film), type(type) {}
   //       Camera(const ParamSets& params);
   //       ~Camera() = default;

   //       Image capture() const;
   // };

   class Camera {
      protected:
         Point3  _eye;
         Vector3 _u, _v, _w;
         double  _l, _r, _b, _t; // screen window parameters
         std::shared_ptr<Film> _film;
 
         void buildFrame(const Point3& look_from, const Point3& look_at, const Vector3& vup);
         void pixelToScreenUV(int i, int j, double& u, double& v) const;
 
      public: 
         Camera(const ParamSets& params);
         virtual ~Camera() = default;
 
         virtual Ray generate_ray(int i, int j) const = 0;
 
         Film&       film()       { return *_film; }
         const Film& film() const { return *_film; }
   };


   // ── Orthographic Camera ────────────────────────────────────────────────────
 
   class OrthographicCamera : public Camera {
      public:
         OrthographicCamera(const ParamSets& params) : Camera(params) {}
         Ray generate_ray(int i, int j) const override;
   };
 

   // ── Perspective Camera ────────────────────────────────────────────────────
   class PerspectiveCamera : public Camera {
      private:
         double _fd;
         int _fovy;
 
      public:
         PerspectiveCamera(const ParamSets& params);
         Ray generate_ray(int i, int j) const override;
   };

   class CameraFactory {
      private:
         static std::string getAndValidateCameraType(const ParamSets& params);

      public:
         static std::shared_ptr<Camera> create(const ParamSets& params);
   };
 
}

#endif // !CAMERA_HPP