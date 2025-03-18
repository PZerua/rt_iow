#pragma once

#include "interval.h"
#include "ray.h"

#include "framework/math/aabb.h"

double linear_to_gamma(double linear_component);
Interval axis_interval(const AABB& aabb, int axis);
bool ray_aabb_interval_intersection(const Ray& ray, Interval ray_t, const AABB& aabb);
