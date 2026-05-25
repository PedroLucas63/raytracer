/// @file triangle.cpp
#include <chrono>
#include <iostream>
#include <iterator>

#include "rt3.hpp"
#include "triangle.hpp"

/// To help debug triangles.
std::ostream& operator<<(std::ostream& os, const Triangle& t) {
  constexpr short X{ 0 };
  constexpr short Y{ 1 };
  constexpr short Z{ 2 };

  os << "    + V0: (" << t.mesh->vertices[t.v[0]][X] << ", " << t.mesh->vertices[t.v[0]][Y] << ", "
     << t.mesh->vertices[t.v[0]][Z] << ")\n"
     << "    + V1: (" << t.mesh->vertices[t.v[1]][X] << ", " << t.mesh->vertices[t.v[1]][Y] << ", "
     << t.mesh->vertices[t.v[1]][Z] << ")\n"
     << "    + V2: (" << t.mesh->vertices[t.v[2]][X] << ", " << t.mesh->vertices[t.v[2]][Y] << ", "
     << t.mesh->vertices[t.v[2]][Z] << ")\n"

     << "    + N0: (" << t.mesh->normals[t.n[0]][X] << ", " << t.mesh->normals[t.n[0]][Y] << ", "
     << t.mesh->normals[t.n[0]][Z] << ")\n"
     << "    + N1: (" << t.mesh->normals[t.n[1]][X] << ", " << t.mesh->normals[t.n[1]][Y] << ", "
     << t.mesh->normals[t.n[1]][Z] << ")\n"
     << "    + N2: (" << t.mesh->normals[t.n[2]][X] << ", " << t.mesh->normals[t.n[2]][Y] << ", "
     << t.mesh->normals[t.n[2]][Z] << ")\n"

     << "    + UV0: (" << t.mesh->uvcoords[t.uv[0]][X] << ", " << t.mesh->uvcoords[t.uv[0]][Y]
     << ")\n"
     << "    + UV1: (" << t.mesh->uvcoords[t.uv[1]][X] << ", " << t.mesh->uvcoords[t.uv[1]][Y]
     << ")\n"
     << "    + UV2: (" << t.mesh->uvcoords[t.uv[2]][X] << ", " << t.mesh->uvcoords[t.uv[2]][Y]
     << ")\n";

  return os;
}

// This is the function called by the API to create a triangle mesh shape.
/*
 * This is the entry-point function for the client code.
 * This function decodes the `ParamSet` data and, based on that, it either
 * (1) creates a triangle mesh manually (from the XML attributes), or
 * (2) calls another function to load an OBJ file and create the triangle mesh.
 *
 * @param flip_normals This flag asks to invert the normal of all triangles.
 * This is a global flag, set by a API command.
 * @param ps The ParamSet object sent from the client code with all the
 * information related to the triangle mesh read from the scene file.
 *
 * @return The list of Shape (triangles) that we read from the scene file.
 */
vector<shared_ptr<Shape>> create_triangle_mesh_shape(bool flip_normals, const ParamSet& ps) {
  bool bkfc_on{ true };                // Controls whether the backface cull should be done or not.
  bool reverse_vertex_order{ false };  // If this is true, we store vertices in
                                       // reverse order inside the mesh.
  bool compute_normals{ false };       // Indicate whether we need to calculate the
                                       // triangle's normals manually.

  // This is a collection of data structures passed between function calls.
  // This structure may be filled in with information from a OBJ file
  // or from the XML attributes extracted from the scene file itself.
  shared_ptr<TriangleMesh> mesh = std::make_shared<TriangleMesh>();  // Default Ctro.

  // Retrieve filename.
  string filename = ps.at("filename");  // Retrieving data associated with 'filename' attrib.
  // Retrieve backface ON/OFF
  string bkf_on_str = ps.at("backface_cull");
  if (bkf_on_str == "off" or bkf_on_str == "false") {
    bkfc_on = false;
  }
  // Retrieve Reverse vertex order ON/OFF
  string rvo_str = ps.at("reverse_vertex_order");
  if (rvo_str == "on" or rvo_str == "true") {
    reverse_vertex_order = true;
  }
  // Retrieve compute normals flag
  string cn_str = ps.at("compute_normals");
  if (cn_str == "on" or cn_str == "true") {
    compute_normals = true;
  }

  // Here we have only two option: read mesh data from OBJ file or from the
  // ParamSet.
  if (filename != "") {  // Read mesh data from an OBJ file provided in the scene file.
    // Call our auxiliary function that interfaces with tinyobjloader API.
    if (load_mesh_data(filename, reverse_vertex_order, compute_normals, flip_normals, mesh)) {
      std::cout << ">>> Mesh data successfuly loaded!\n";
    } else {
      std::cout << ">>> Mesh data loading failed!\n";
    }
  } else {  // This means we must read the mesh data from the scene file, not
            // from the OBJ file.

    // TODO: retrieve the attributes of the triangle mesh present in the XML
    // scene file.
    //
    // Here you must retrieve from the ParamSet all the data that might appear
    // in a trianglemesh object. Usually these are the vertices, normals, and
    // indices.
    //
    // You should read all the data into the `tri_mesh_data` object so that the
    // function call below works, regardless if the data came from the OBJ file
    // or was read directly from the scene file.
  }

  // At this point, the tri_mesh_data object has already been filled in with
  // data coming either from a OBJ file or from the scene file.
  return create_triangle_mesh(mesh,
                              bkfc_on);  // Note the use of move semantics here.
}

/// This function calls the basic tinyobjloader loading function and stores all
/// the data into the tinyobjloader's internal data structures.
bool load_mesh_data(const std::string& filename,
                    bool rvo,
                    bool cn,
                    bool fn,
                    shared_ptr<TriangleMesh> md) {
  // Default load parameters
  const char* basepath = nullptr;
  bool triangulate = true;

  std::cout << "Loading " << filename << '\n';

  // This is the tinyobj internal data structures.
  tinyobj::attrib_t attrib;              // Hold all the data, such as vertex, normals, UV coords
  std::vector<tinyobj::shape_t> shapes;  // Represents a collection of faces, grouped in a unit.
  std::vector<tinyobj::material_t> materials;  // Materials, NOT USED HERE.

  // Warnings and Error values.
  std::string warn;
  std::string err;

  // Timing loading.
  //================================================================================
  auto start = std::chrono::steady_clock::now();
  //================================================================================
  bool ret = tinyobj::LoadObj(
    &attrib, &shapes, &materials, &warn, &err, filename.c_str(), basepath, triangulate);
  //================================================================================
  std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
  // Store the time difference between start and end
  std::chrono::duration<double> diff = end - start;

  // Milliseconds (10^-3)
  std::cout << "\t\t>>> " << std::chrono::duration<double, std::milli>(diff).count() << " ms"
            << '\n';

  // Nanoseconds (10^-9)
  std::cout << "\t\t>>> " << std::chrono::duration<double, std::nano>(diff).count() << " ns"
            << '\n';

  // Seconds
  auto diff_sec = std::chrono::duration_cast<std::chrono::seconds>(diff);
  std::cout << "\t\t>>> " << diff_sec.count() << " s" << '\n';
  //================================================================================

  if (!warn.empty()) {
    cout << "WARN: " << warn << '\n';
  }

  if (!err.empty()) {
    cerr << "ERR: " << err << '\n';
  }

  if (!ret) {
    cout << "Failed to load/parse .obj.\n";
    return false;
  }

  // Let us now "convert" or "migrate" the data from tinyobjloader data
  // structure into our mesh data format.
  extract_obj_data(attrib,
                   shapes,  // TinyObjeLoader data structures (IN)
                   rvo,
                   cn,
                   fn,   // Mesh modifiers (IN)
                   md);  // Reference to the mesh data to fill in. (OUT)

  return true;
}

/**
 * This is the function that converts from the tinyobjloader's internal storage
 * to our mesh data organization.
 */
void extract_obj_data(const tinyobj::attrib_t& attrib,
                      const std::vector<tinyobj::shape_t>& shapes,
                      bool reverse_order,
                      bool cn,
                      bool fn,
                      /* OUT */ shared_ptr<TriangleMesh> md) {
  std::cout << "-- SUMMARY of the OBJ file --\n";
  std::cout << "# of vertices  : " << (attrib.vertices.size() / 3) << '\n';
  std::cout << "# of normals   : " << (attrib.normals.size() / 3) << '\n';
  std::cout << "# of texcoords : " << (attrib.texcoords.size() / 2) << '\n';
  std::cout << "# of shapes    : " << shapes.size() << '\n';
  std::cout << "-----------------------------\n";

  // Retrieve the complete list of vertices.
  auto n_vertices{ attrib.vertices.size() / 3 };
  for (auto idx_v{ 0U }; idx_v < n_vertices; idx_v++) {
    cout << "   v[" << static_cast<long>(idx_v) << "] = ( "
         << static_cast<double>(attrib.vertices[(3 * idx_v) + 0]) << ", "
         << static_cast<double>(attrib.vertices[(3 * idx_v) + 1]) << ", "
         << static_cast<double>(attrib.vertices[(3 * idx_v) + 2]) << " )\n";

    // Store the vertex in the mesh data structure.
    md->vertices.push_back(Point3f{ attrib.vertices[(3 * idx_v) + 0],
                                    attrib.vertices[(3 * idx_v) + 1],
                                    attrib.vertices[(3 * idx_v) + 2] });
  }

  // Read the normals
  // TODO: flip normals if requested.
  real_t flip = (fn) ? -1 : 1;
  auto n_normals{ attrib.normals.size() / 3 };

  // Do we need to compute the normals? Yes only if the user requeste or there
  // are no normals in the file. if ( cn == true or n_normals == 0)
  // {
  //    // TODO: COmpute normals here.
  //    // compute_normals();
  // }
  // else {// Read normals from file. This corresponds to the entire 'for'
  // below.

  // Traverse the normals read from the OBJ file.
  for (auto idx_n{ 0U }; idx_n < n_normals; idx_n++) {
    cout << "   n[" << static_cast<long>(idx_n) << "] = ( "
         << static_cast<double>(attrib.normals[(3 * idx_n) + 0]) << ", "
         << static_cast<double>(attrib.normals[(3 * idx_n) + 1]) << ", "
         << static_cast<double>(attrib.normals[(3 * idx_n) + 2]) << " )\n";

    // Store the normal.
    md->normals.push_back(Normal3f{ attrib.normals[(3 * idx_n) + 0] * flip,
                                    attrib.normals[(3 * idx_n) + 1] * flip,
                                    attrib.normals[(3 * idx_n) + 2] * flip });
  }

  // Read the complete list of texture coordinates.
  auto n_texcoords{ attrib.texcoords.size() / 2 };
  for (auto idx_tc{ 0U }; idx_tc < n_texcoords; idx_tc++) {
    cout << "   t[" << static_cast<long>(idx_tc) << "] = ( "
         << static_cast<double>(attrib.texcoords[(2 * idx_tc) + 0]) << ", "
         << static_cast<double>(attrib.texcoords[(2 * idx_tc) + 1]) << " )\n";

    // Store the texture coords.
    md->uvcoords.push_back(
      Point2f{ attrib.texcoords[(2 * idx_tc) + 0], attrib.texcoords[(2 * idx_tc) + 1] });
  }

  // Read mesh connectivity and store it as lists of indices to the real data.
  auto n_shapes{ shapes.size() };
  md->n_triangles = 0;  // We must reset this here.
  // In case the OBJ file has the triangles organized in several shapes or
  // groups, we ignore this and store all triangles as a single mesh dataset.
  // This is why we need to reset the triangle count here.
  for (auto idx_s{ 0U }; idx_s < n_shapes; idx_s++) {
    cout << "The shape[" << idx_s << "].name = " << shapes[idx_s].name << '\n';
    cout << "Size of shape[" << idx_s
         << "].mesh.indices: " << static_cast<unsigned long>(shapes[idx_s].mesh.indices.size())
         << '\n';
    cout << "shape[" << idx_s << "].num_faces: "
         << static_cast<unsigned long>(shapes[idx_s].mesh.num_face_vertices.size()) << '\n';

    // For each face print out the indices lists.
    size_t index_offset = 0;
    // # of triangles for this "shape" (group).
    // NOTE that we are accumulate the number of triangles coming from the
    // shapes present in the OBJ file.
    md->n_triangles += shapes[idx_s].mesh.num_face_vertices.size();
    for (auto idx_f{ 0 }; idx_f < md->n_triangles; idx_f++) {
      // Number of vertices per face (always 3, in our case)
      size_t fnum = shapes[idx_s].mesh.num_face_vertices[idx_f];

      cout << "  face[" << idx_f << "].fnum = " << static_cast<unsigned long>(fnum) << '\n';

      // TODO: Invert order of vertices if flag is on. (DONE!)
      if (reverse_order) {
        std::cout << "Reverse order\n";
        // For each vertex in the face print the corresponding indices
        for (int v = fnum - 1; v >= 0; v--) {
          tinyobj::index_t idx = shapes[idx_s].mesh.indices[index_offset + v];
          cout << "    face[" << idx_f << "].v[" << v << "].indices = " << idx.vertex_index << "/"
               << idx.normal_index << "/" << idx.texcoord_index << '\n';
          // Add the indices to the global list of indices we need to pass on to
          // the mesh object.
          md->vertex_indices.push_back(idx.vertex_index);
          md->normal_indices.push_back(idx.normal_index);
          md->uvcoord_indices.push_back(idx.texcoord_index);
        }
      } else {  // Keep the original vertex order
        // For each vertex in the face get the corresponding indices
        for (size_t v = 0; v < fnum; v++) {
          tinyobj::index_t idx = shapes[idx_s].mesh.indices[index_offset + v];
          cout << "    face[" << idx_f << "].v[" << v << "].indices = " << idx.vertex_index << "/"
               << idx.normal_index << "/" << idx.texcoord_index << '\n';
          // Add the indices to the global list of indices we need to pass on to
          // the mesh object. This goes to the mesh data structure.
          md->vertex_indices.push_back(idx.vertex_index);
          md->normal_indices.push_back(idx.normal_index);
          md->uvcoord_indices.push_back(idx.texcoord_index);
        }
      }

      // Advance over to the next triangle.
      index_offset += fnum;
    }
  }

  cout << "This is the list of indices: \n";

  cout << "   + Vertices [ ";
  std::copy(md->vertex_indices.begin(),
            md->vertex_indices.end(),
            std::ostream_iterator<int>(std::cout, " "));
  cout << "]\n";

  cout << "   + Normals [ ";
  std::copy(md->normal_indices.begin(),
            md->normal_indices.end(),
            std::ostream_iterator<int>(std::cout, " "));
  cout << "]\n";

  cout << "   + UV coords [ ";
  std::copy(md->uvcoord_indices.begin(),
            md->uvcoord_indices.end(),
            std::ostream_iterator<int>(std::cout, " "));
  cout << "]\n";
}

/// This function creates the internal data structure, required by the RT3.
vector<shared_ptr<Shape>> create_triangle_mesh(shared_ptr<TriangleMesh> mesh, bool backface_cull) {
  // List of shapes (triangles) we need to return to the client.
  vector<shared_ptr<Shape>> tris;
  // Create the triangles, which are just references to the mesh database.
  tris.reserve(mesh->n_triangles);
  for (int i = 0; i < mesh->n_triangles; ++i) {
    tris.emplace_back(std::make_shared<Triangle>(mesh, i, backface_cull));
  }
  return tris;
}
