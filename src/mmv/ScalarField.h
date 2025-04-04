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
    ScalarField() : Grid(vec2(0,0),vec2(0,0),0,0) {}
    ScalarField(const vec2& c1, const vec2& c2, int rows, int cols, float e);
    ScalarField(const Image & img, const vec2& c1, const vec2& c2, int rows, int cols, float e);
    ~ScalarField();


    float Height(int x, int y) const;
    Image HeightImage() const;
    float Height(double x, double y) const;
    float HeightGrid(int x, int y) const;
    Vector Gradient(int x, int y) const;
    float Slope(float x, float y) const;
    Image GradientNorm(ScalarField& s);
    float Laplacian(double x, double y);
    Image LaplacianImage(ScalarField& s);
    float AccessibilityBox(float x, float y);
    float AccessibilityRay(float x, float y);
    Image AccesibilityImage(ScalarField& s);
    float AverageSlope(int x, int y);
    Image AverageSlopeImage(ScalarField& s);
    void Drainage(ScalarField& s);

    void ApplyThermicalErosion();

    Mesh ToMesh() const;

private:
    void SetHeight(int x, int y, float newHeight);

    std::vector<float> heights;
    std::vector<Vector> gradient;
    std::vector<float> slope;
    float slopeMax;
    Vector pMin, pMax;
    Vector boundMin, boundMax;
};

#endif //MMV_SCALARFIELD_H
