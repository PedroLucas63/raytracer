**COMPUTER GRAPHICS I (DIM0451) -- Ray Tracer Project**

# Project 08 - Acceleration Data Structure

## 1. Introduction

In this programming project you will add to your ray tracer (RT) an acceleration
data structure that organizes objects from a scene in a tree. This tree data
structure is called a [**Boundary Volume
Hierarchy**](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy). Instead
of the _brute-force_ approach of testing each light ray against **all** objects
in the scene, which has time complexity $O(nm)$ where $m$ is the number of
pixels and $n$ is the number of objects (triangles, spheres, etc.), the
acceleration tree will enable an efficient search by testing the ray to a few
objects stored along a tree-path of the BVH. This strategy might achieve a
$O(m\log_2(n))$ time complexity depending on how balanced the tree is.

There are, basically, two broad approaches to accelerate the ray-object
intersection test. The _space-partitioning_ approach progressively divides up
space into volumes, which in turn are placed in a tree-like data structure, and
then classifies objects into these volumes. The space division is usually done
with axis-aligned planes. Classical examples of this approach are the [K-d
tree](https://en.wikipedia.org/wiki/K-d_tree) and [BSP
tree](https://en.wikipedia.org/wiki/Binary_space_partitioning).

The _object-partitioning_ approach, on the other hand, treats each object as
indivisible elements that are placed in a bounding volume (e.g.. a box or a
sphere). Close objects are progressively bounded together in larger volumes and
a hierarchy is naturally produced. The end result is a tree data structure where
objects are placed at the leaves, and the volumes (sets of objects) are located
in the internal nodes. Usually we choose a bouding volume representation that is
easy to determine ray intersection with.

## 2. Requirements

In this project we are recommending the BVH _object-partitioning_ approach.
However, feel free to choose a different acceleration structure, if you so
desire.

The first step in this project is to make sure that every geometric primitive is
capable of generating a tight **bounding volume** (BV) surrounding it . You may
want to create a class `Bounds3f` that represent a 3D box with axis aligned
planes, defined by two 3D (float) points, `p_min` and `p_max`. Useful methods
for this class are `union` of two BVs, determining the diagonal of the BV,
determining the largest extend among all dimensions, and, more importantly,
`intersect_p(const Ray &ray, float &hit1, float &hit2)` method that returns
`true` if a ray hits the BV, and also returns the ray parameter corresponding to
the two locations the ray might have hit the BV.

The second step is to create a new class `BVHAccel` derived from the
`AggregatePrimitive` (first introduced in the UML diagram from [Project
04](../prj04)), which, in turn, derives from the `Primitive` class. This means
the BVH is actually a primitive that groups other primitives in a tree-like data
structure. So, if you have followed the RT architecture suggestions provided in
previous projects nothing changes: the scene object still holds a (shared)
pointer to a `Primitive`, which might be a `GeometricPrimitive` (e.g. a single
sphere), a `PrimList` (a vector of primitives) or the new type, `BVHAccel`. This
also means that the `BVHAccel` class must implement `intersect()` and
`intersect_p()`. These methods basically requires the traversal of the BVH tree,
looking for potential objects the might be hit by the ray and, of course,
discarding lots of other objects that have no chance of being hit by a light
ray.

In a nutshell, the bottom-up BVH algorithms receives a list (vector) of
primitive as input and generates the BVH tree as the output. The algorithm may
work like this:

1. _For each primitive from the input list generate their bounding volume and stored them into a new list of BVH nodes._
2. _Randomly choose an axis and sort all nodes on the chosen axis_
3. _Store half of the nodes into the left sub-tree, and the other half into the right sub-tree_
4. _Recursively repeat steps 2 to 4 until there is only 1 or 2 nodes left; in the former case you store the same node twice, in the left and right sub-trees, whereas for the later case, store one node on the left sub-tree and the other on the right sub-tree._

For more details on how to implement a BVH I recommend to have a look at the
Chapter 2 of [_"Ray Tracing, the next week"_](https://www.realtimerendering.com/raytracing/Ray%20Tracing_%20The%20Next%20Week.pdf)
by Peter Shirley, and/or [Section 4.3](https://pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies)
from the 3rd edition of _"Physically Based Rendering"_ (PBR) by Pharr, Jakob,
and Humphreys.

## 3. The Scene Format

This is how the BVH aggregate might be declared in the scene file. Notice that
the `scene` tag still is a collection of primitives, that might be an `object`
or an `aggregate`. In the example bellow the scene has two top level primitives,
a teapot and a plane.

The `type` attribute may be `bvh` or `list`. If you are implementing the
Pharr's version you need to provide the split method chosen. Optionally, you may
want to let the user to define how many nodes per leaf the tree should have,
which may generate shorter height trees.

```xml
<RT3>
    <!--- The Camera -->
    <lookat look_from="0 -90 30" look_at="0 0 0" up="0 1 0" />
    <camera type="perspective" fovy="25" />
    <integrator type="cel_shading"/>

    <aggregator type="bvh" split_method="middle" max_prims_per_node="4" />

    <film type="image" x_res="900" y_res="450" filename="teapot_cel_P.png" img_type="png" gamma_corrected="no" />
    <include filename="../scenes/projects_references/proj_x_cel/teapot_scene/teapot_geometry.xml" />
</RT3L>
```

At the end of the day, the scene parser should produce an `PrimList` object,
i.e. a list of primitives, for the scene. Remember that a `PrimList` is also a
single primitive. This final list might be comprised of a single primitive (say,
an aggregate) of a list of primitives.

## 4. Recommendations

Read all the suggested material before starting your implementation. The
Shirley's version is the simplest of the two BVH algorithms.

If your BVH implementation works correctly, you should notice a dramatic
reduction of the time necessary to render a scene from the previous project,
specially if the scene has a large triangle mesh.