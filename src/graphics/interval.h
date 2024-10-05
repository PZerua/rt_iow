#pragma once

#include "framework/math/math_utils.h"

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {} // Default interval is empty

    Interval(double min, double max) : min(min), max(max) {}

    double size() const;

    bool contains(double x) const;

    bool surrounds(double x) const;

    static const Interval empty, universe;
};
