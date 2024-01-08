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

    // Tester si un rayon intersecte le terrain
    // Utilisé pour l'accessibilité
    // bool Intersect(const Ray& ray, double& t, double&, Vector&) const;


    // Shere tracing
    // constante de lipchitz = sqrt( 1 + plus grande valeur des pentes)

protected:
    Vector a, b; // Bounding box
};

#endif //MMV_HEIGHTFIELD_H
