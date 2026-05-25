cmake_minimum_required(VERSION 3.10)
project (LoadMeshTest VERSION 1.0.0 LANGUAGES CXX )

#=== SETTING VARIABLES ===#
# Compiling flags
set( GCC_COMPILE_FLAGS "-Wall" )


#=== Main App ===

include_directories( "src" "ext" )

add_executable( load_mesh_demo "src/main.cpp"
                               "src/triangle.cpp"
                               "ext/tiny_obj_loader.cc" )

target_compile_features( load_mesh_demo PUBLIC cxx_std_17 )
