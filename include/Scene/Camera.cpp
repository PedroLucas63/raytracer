#include "Camera.hpp"
#include <cmath>
#include <stdexcept>

namespace raytracer {
   // Camera::Camera(const ParamSets& params): film(1, 1, 3) { // Default to a 1x1 image until properly initialized
   //    auto it = params.find("camera");
   //    if (it == params.end())
   //       throw std::invalid_argument("ParamSets does not contain 'camera' parameters.");

   //    const ParamSet& cameraParams = it->second;
   //    const std::string typeParam = cameraParams.retrieveOrDefault<std::string>("type", "orthographic");

   //    if (typeParam == "orthographic")
   //       type = CameraType::Orthographic;
   //    else if (typeParam == "perspective")
   //       type = CameraType::Perspective;
   //    else
   //       throw std::invalid_argument("Unsupported camera type: " + type);

   //    film = Film(params);
   // }
   
   // Image Camera::capture() const {
   //    return film.getImage();
   // }



   void Camera::buildFrame(const Point3& look_from,
                           const Point3& look_at,
                           const Vector3& vup) {
      // Convenção left-hand do enunciado:
      //   w = normalize(look_at - look_from)   — gaze direction
      //   u = normalize(vup x w)               — right
      //   v = normalize(w x u)                 — up real
      _eye = look_from;
      _w   = (look_at - look_from).normalize();
      _u   = vup.cross(_w).normalize();
      _v   = _w.cross(_u).normalize();
   }

   Camera::Camera(std::shared_ptr<Film> film,
                  const Point3& look_from, const Point3& look_at, const Vector3& vup,
                  double l, double r, double b, double t)
      : _l(l), _r(r), _b(b), _t(t), _film(std::move(film))
   {
      buildFrame(look_from, look_at, vup);
   }

   void Camera::pixelToScreenUV(int i, int j, double& u, double& v) const {
      // Fórmula: u = l + (r-l)*(i+0.5)/nx
      //          v = b + (t-b)*(j+0.5)/ny
      int nx = static_cast<int>(_film->getWidth());
      int ny = static_cast<int>(_film->getHeight());
      u = _l + (_r - _l) * (i + 0.5) / nx;
      v = _b + (_t - _b) * (j + 0.5) / ny;
   }


   Ray OrthographicCamera::generate_ray(int i, int j) const {
      double u, v;
      pixelToScreenUV(i, j, u, v);
 
      // origin = e + u*u_hat + v*v_hat
      // direction = w (All rays are parallel in orthographic projection)
      Point3  origin    = _eye + _u * u + _v * v;
      Vector3 direction = _w;
      return Ray(origin, direction);
   }
 
   // ── PerspectiveCamera ─────────────────────────────────────────────────────
 
   Ray PerspectiveCamera::generate_ray(int i, int j) const {
      double u, v;
      pixelToScreenUV(i, j, u, v);
 
      // origin = e
      // direction = fd*w + u*u_hat + v*v_hat
      Point3  origin    = _eye;
      Vector3 direction = _w * _fd + _u * u + _v * v;
      return Ray(origin, direction);
   }
}