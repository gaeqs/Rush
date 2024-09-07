//
// Created by gaelr on 25/01/2024.
//

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <rush/geometry/aabb_base.h>
#include <rush/geometry/sphere_base.h>
#include <rush/geometry/plane_base.h>
#include <rush/geometry/ray_base.h>
#include <rush/geometry/edge_base.h>
#include <rush/geometry/triangle_base.h>
#include <rush/geometry/intersections.h>
#include <rush/geometry/raycast.h>

#include <rush/geometry/tree_base.h>
#include <rush/geometry/dynamic_tree_base.h>
#include <rush/geometry/static_tree_base.h>

namespace rush {
    using Ray1f = Ray<1, float>;
    using Ray2f = Ray<2, float>;
    using Ray3f = Ray<3, float>;
    using Ray4f = Ray<4, float>;

    using Ray1d = Ray<1, double>;
    using Ray2d = Ray<2, double>;
    using Ray3d = Ray<3, double>;
    using Ray4d = Ray<4, double>;
}


#endif //GEOMETRY_H
