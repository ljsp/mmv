//
// Created by lucas on 04/12/23.
//

#include "Box2.h"

bool Box2::Inside(const vec2 &point) const {
    return (point.x >= corner1.x && point.x <= corner2.x) &&
           (point.y >= corner1.y && point.y <= corner2.y);
}

bool Box2::Intersect(const Box2 &other) const {
    return (corner1.x <= other.corner2.x && corner2.x >= other.corner1.x) &&
           (corner1.y <= other.corner2.y && corner2.y >= other.corner1.y);
}
