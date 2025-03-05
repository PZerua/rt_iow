#include "interval.h"

const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);

double Interval::size() const {
    return max - min;
}

bool Interval::contains(double x) const {
    return min <= x && x <= max;
}

bool Interval::surrounds(double x) const {
    return min < x && x < max;
}

double Interval::clamp(double x) const {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
