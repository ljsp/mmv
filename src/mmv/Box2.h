#include "vec.h"

class Box2 {
private:
    vec2 corner1, corner2;

public:
    Box2(const vec2& c1, const vec2& c2) : corner1(c1), corner2(c2) {}

    bool Inside(const vec2& point) const;
    bool Intersect(const Box2& other) const;
};
