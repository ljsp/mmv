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
    ScalarField(const vec2& c1, const vec2& c2, int rows, int cols, float e);
    ScalarField(const Image & img, const vec2& c1, const vec2& c2, int rows, int cols, float e);
    ~ScalarField();


    float Height(int x, int y) const;
    Image HeightImage() const;
    Vector Gradient(int x, int y) const;
    float Slope(float x, float y) const;
    Image GradientNorm(ScalarField& s);
    float Laplacian(int x, int y);
    Image LaplacianImage(ScalarField& s);
    float AccessibilityBox(float x, float y);
    float AccessibilityRay(float x, float y);
    Image AccesibilityImage(ScalarField& s);
    float AverageSlope(int x, int y);
    Image AverageSlopeImage(ScalarField& s);
    void Drainage(ScalarField& s);

    Mesh ToMesh() const;

private:
    std::vector<float> heights;
    std::vector<Vector> gradient;
    std::vector<float> slope;
    float slopeMax;
    Vector pMin, pMax;
};

#endif //MMV_SCALARFIELD_H
