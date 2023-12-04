//
// Created by lucas on 04/12/23.
//

#ifndef MMV_SCALARFIELD_H
#define MMV_SCALARFIELD_H

#include "vec.h"
#include "mesh.h"
#include "Box.h"

class ScalarField {
public:
    ScalarField();
    ~ScalarField();

    void ExportImage(const char* filename);
    void ExportMesh(const char* filename);

    virtual double Value(const Vector&) const;
    virtual Vector Gradient(const Vector&) const;
    void Slope(ScalarField* s);
    void Laplacian(ScalarField* s);
    void Drainage(ScalarField* s);
    virtual Vector Normal(const Vector&) const;
    Vector Dichotomy(Vector, Vector, double, double, double, const double& = 1.0e-4) const;
    virtual void Polygonize(int, Mesh&, const Box&, const double& = 1e-4) const;

protected:
    static const double Epsilon;       //!< Epsilon value for partial derivatives
    static int TriangleTable[256][16]; //!< Two dimensionnal array storing the straddling edges for every marching cubes configuration.
    static int edgeTable[256];         //!< Array storing straddling edges for every marching cubes configuration.
};

#endif //MMV_SCALARFIELD_H
