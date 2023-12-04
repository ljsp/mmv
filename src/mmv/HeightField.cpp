//
// Created by lucas on 04/12/23.
//

#include "HeightField.h"

double HeightField::Height(const double& x, const double& y)
{
    // Local coordinates
    double u=(x-a.x)/(b.x-a.x);
    double v=(y-a.y)/(b.y-a.y);

    // Cell location within grid
    int nu = int(u * nx);
    int nv = int(v * ny);

    // Local coordinates within cell
    u = u - nu * (b.x - a.x) / nx;
    v = v - nv * (b.y - a.y) / ny;

    /*
    if ( u + v < 1) {
        return (1 - u - v) * HeightGrid(i, j)
                + u * HeightGrid(i + 1, j)
                + v * HeightGrid(i, j + 1);
    }
    else
    {
        return (u + v - 1) * HeightGrid(i + 1 , j + 1)
                + (1 - v ) * HeightGrid(i + 1, j)
                + ( 1 - u) * HeightGrid(i, j + 1);
    }
    */
}

double Sphere::Value(const Vector &p) const {
    return length2(p - center) - radius;
}
