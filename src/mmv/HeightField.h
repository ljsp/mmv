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

#endif //MMV_HEIGHTFIELD_H
