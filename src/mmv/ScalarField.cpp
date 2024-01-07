//
// Created by lucas on 04/12/23.
//

#include "ScalarField.h"

ScalarField::ScalarField(const Image & img, const vec2& c1, const vec2& c2, int rows, int cols, float e)
        : Grid(c1, c2, rows, cols) {
    heights.resize(rows * cols);
    for(int x=0; x<rows - 1; x++){
        for(int y=0; y<cols - 1; y++){
            Color c = img(x, y);
            heights[Index(x, cols - y)] = (c.r + c.g + c.b)/(3*e);
        }
    }

}

float ScalarField::Height(int x, int y) const {
    return heights[Index(x, y)];
}

Vector ScalarField::Gradient(int x, int y) const {
    double gx = Height(x+1, y) - Height(x-1, y);
    double gy = Height(x, y+1) - Height(x, y-1);
    double gz = 0.;

    if(gx == 0 && gy == 0){
        gz = 1.;
    }

    return Vector(gx, gy, gz);
}

ScalarField::~ScalarField() {

}

void ScalarField::Slope(ScalarField &s) {

}

Mesh ScalarField::ToMesh() const {
    Mesh mesh= Mesh(GL_TRIANGLES);

    for(int x= 0; x < getRows() - 1; x++)
    {
        for(int y= 0; y < getCols() - 1; y++)
        {
            mesh.color(White());
            mesh.vertex(Point(x, Height(x, y), y));
            mesh.vertex(Point(x, Height(x, y+1), y+1));
            mesh.vertex(Point(x+1, Height(x+1, y), y));

            mesh.color(White());
            mesh.vertex(Point(x+1, Height(x+1, y), y));
            mesh.vertex(Point(x, Height(x, y+1), y+1));
            mesh.vertex(Point(x+1, Height(x+1, y+1), y+1));
        }
    }

    return mesh;
}
