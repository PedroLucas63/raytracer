#include "Camera.hpp"
#include <cmath>
#include <stdexcept>
#include <vector>

namespace raytracer {
   Camera::Camera(const ParamSets& params) {
      // Camera parameters
      auto it = params.find("camera");

      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'camera' parameters.");
      
      const ParamSet& cameraParams = it->second;
      
      auto screen_window = cameraParams.retrieveOrDefault(std::string("screen_window"), std::vector<float>{-1.555, 1.555, -1.0, 1.0});

      _l = screen_window[0];
      _r = screen_window[1];
      _b = screen_window[2];
      _t = screen_window[3];

      // Film parameters
      _film = std::make_shared<Film>(params);

      // Lookat parameters
      auto it_lookat = params.find("lookat");

      if (it_lookat == params.end())
         throw std::invalid_argument("ParamSets does not contain 'lookat' parameters.");
      
      const ParamSet& lookatParams = it_lookat->second;

      if (!lookatParams.has("look_from") || !lookatParams.has("look_at") || !lookatParams.has("up"))
         throw std::invalid_argument("Lookat parameters must include 'look_from', 'look_at', and 'up'.");

      Point3 look_from = lookatParams.retrieve<Point3>("look_from");
      Point3 look_at = lookatParams.retrieve<Point3>("look_at");
      Vector3 vup = lookatParams.retrieve<Vector3>("up");

      buildFrame(look_from, look_at, vup);
   }
   void Camera::buildFrame(const Point3& look_from,
                           const Point3& look_at,
                           const Vector3& vup) {
      _eye = look_from;
      _w   = (look_at - look_from).normalize();
      _u   = vup.cross(_w).normalize();
      _v   = _u.cross(_w).normalize();
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

   PerspectiveCamera::PerspectiveCamera(const ParamSets& params) : Camera(params) {
      // Camera parameters
      auto it = params.find("camera");

      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'camera' parameters.");
      
      const ParamSet& cameraParams = it->second;

      if (!cameraParams.has("fovy"))
         throw std::invalid_argument("Camera parameters must include 'fovy' for perspective camera.");

      _fovy = cameraParams.retrieve<int>("fovy");
      _fd = 1.0 / std::tan((_fovy * M_PI / 180.0) / 2.0);
   }

   std::string CameraFactory::getAndValidateCameraType(const ParamSets& params) {
      auto it = params.find("camera");
      if (it == params.end())
         throw std::invalid_argument("ParamSets does not contain 'camera' parameters.");
      
      const ParamSet& cameraParams = it->second;
         
      if (!cameraParams.has("type"))
         throw std::invalid_argument("Camera parameters must include 'type'.");

      std::string type = cameraParams.retrieve<std::string>("type");
      if (type != "perspective" && type != "orthographic")
         throw std::invalid_argument("Unsupported camera type: " + type);

      return type;
   }

   std::unique_ptr<Camera> CameraFactory::build(const ParamSets& params) {
      std::string type = getAndValidateCameraType(params);

      if (type == "perspective") {
         return std::make_unique<PerspectiveCamera>(params);
      } else if (type == "orthographic") {
         return std::make_unique<OrthographicCamera>(params);
      }

      throw std::invalid_argument("Unsupported camera type: " + type);
   }
}