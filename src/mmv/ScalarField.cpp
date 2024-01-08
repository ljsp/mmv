//
// Created by lucas on 04/12/23.
//

#include "ScalarField.h"

ScalarField::ScalarField(const Image & img, const vec2& c1, const vec2& c2, int rows, int cols, float e)
        : Grid(c1, c2, rows, cols)
{
    heights.resize(rows * cols);
    float heightModifier = 15.0f;

    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            Color c = img(x, y);
            heights[Index(x, y)] = ((c.r + c.g + c.b)/(3*e)) * heightModifier;
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

Image ScalarField::GradientNorm(ScalarField &s) {
    Image img = Image(s.getRows(), s.getCols());
    int rows = s.getRows();
    int cols = s.getCols();

    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            Vector v = s.Gradient(x, y);
            float color = length(v);
            //float color = (v.x + v.y + v .z) / 3.0f;
            img(x, y) = Color(color,color,color);
        }
    }

    return img;
}

void ScalarField::Slope(ScalarField &s) {

}

Mesh ScalarField::ToMesh() const {
    Mesh mesh = Mesh(GL_TRIANGLES);

    int rows = getRows();
    int cols = getCols();

    for(int x = 0; x < rows - 1; x++) {
        for(int y = 0; y < cols - 1; y++) {
            float texX  = x         / static_cast<float>(rows - 1);
            float texY  = y         / static_cast<float>(cols - 1);
            float texX1 = (x + 1)   / static_cast<float>(rows - 1);
            float texY1 = (y + 1)   / static_cast<float>(cols - 1);

            // First Triangle
            mesh.vertex(Point(x, Height(x, y), y));
            mesh.normal(Gradient(x, y));
            mesh.texcoord(texX, texY);

            mesh.vertex(Point(x, Height(x, y + 1), y + 1));
            mesh.normal(Gradient(x, y + 1));
            mesh.texcoord(texX, texY1);

            mesh.vertex(Point(x + 1, Height(x + 1, y), y));
            mesh.normal(Gradient(x + 1, y));
            mesh.texcoord(texX1, texY);

            // Second Triangle
            mesh.vertex(Point(x + 1, Height(x + 1, y), y));
            mesh.normal(Gradient(x + 1, y));
            mesh.texcoord(texX1, texY);

            mesh.vertex(Point(x, Height(x, y + 1), y + 1));
            mesh.normal(Gradient(x, y + 1));
            mesh.texcoord(texX, texY1);

            mesh.vertex(Point(x + 1, Height(x + 1, y + 1), y + 1));
            mesh.normal(Gradient(x + 1, y + 1));
            mesh.texcoord(texX1, texY1);
        }
    }
    mesh.vertex(Point(0, 0, 0));

    std::cout << "Positions : " << mesh.positions().size() << std::endl;
    std::cout << "Normals : " << mesh.normals().size() << std::endl;
    std::cout << "Texcoords : " << mesh.texcoords().size() << std::endl;

    return mesh;
}

ScalarField::~ScalarField() {

}
