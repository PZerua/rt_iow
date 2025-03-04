#include "aabb.h"

const RtAABB RtAABB::empty = RtAABB(Interval::empty, Interval::empty, Interval::empty);
const RtAABB RtAABB::universe = RtAABB(Interval::universe, Interval::universe, Interval::universe);
