//
// Created by lucas on 04/12/23.
//

#ifndef MMV_HEIGHTFIELD_H
#define MMV_HEIGHTFIELD_H

#include "ScalarField.h"

class HeightField : public ScalarField {
public:
    HeightField();
    ~HeightField();

    double Height(const double&,const double&);
    double HeightGrid(int,int);

protected:
    Vector a, b; // Bounding box
    int nx, ny; // Discretization
    double z[]; // Array of heights
};

class Sphere : public ScalarField{

public:
    Sphere(const Vector& c, double r) : center(c), radius(r) {}
    double Value(const Vector& p) const override;

private:
    Vector center;
    double radius;
};


#endif //MMV_HEIGHTFIELD_H
