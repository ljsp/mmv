//
// Created by lucas on 04/12/23.
//

#ifndef MMV_GRID_H
#define MMV_GRID_H

#include "Box2.h"

class Grid : public Box2 {
private:
    int rows, cols;

public:
    Grid(const vec2& c1, const vec2& c2, int r, int c) : Box2(c1, c2), rows(r), cols(c) {}

    int Index(int i, int j) const;

    int getRows() const { return rows; }
    int getCols() const { return cols; }

    bool Inside(int i, int j) const;
    bool Inside(double u, double v) const;
};


#endif //MMV_GRID_H
