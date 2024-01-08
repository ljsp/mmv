//
// Created by lucas on 04/12/23.
//

#ifndef MMV_SCALARFIELD_H
#define MMV_SCALARFIELD_H

#include "vec.h"
#include "mesh.h"
#include "Grid.h"
#include "image.h"


class ScalarField : public Grid {
public:
    ScalarField(const Image & img, const vec2& c1, const vec2& c2, int rows, int cols, float e);
    ~ScalarField();


    Vector Gradient(int x, int y) const;
    float Height(int x, int y) const;
    Image GradientNorm(ScalarField& s);
    void Slope(ScalarField& s);
    void Laplacian(ScalarField& s);
    void Drainage(ScalarField& s);

    void ExportImage(const char* filename);
    void ExportMesh(const char* filename);
    Mesh ToMesh() const;

private:
    std::vector<float> heights;
    std::vector<float> gradient;
};

#endif //MMV_SCALARFIELD_H
