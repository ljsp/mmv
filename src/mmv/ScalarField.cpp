//
// Created by lucas on 04/12/23.
//

#include <cfloat>
#include "ScalarField.h"
#include <float.h>


ScalarField::ScalarField(const Image & img, const vec2& c1, const vec2& c2, int rows, int cols, float e)
        : Grid(c1, c2, rows, cols), slopeMax(0), pMin(0,0,FLT_MAX), pMax(0,0,FLT_MIN)
{
    heights.resize(rows * cols);
    float heightModifier =20.0f;

    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            Color c = img(x, y);
            float h = ((c.r + c.g + c.b)/(3*e)) * heightModifier;
            heights[Index(x, y)] = h;
            pMin = h < pMin.z ? Vector(x,y,h) : pMin;
            pMax = h > pMax.z ? Vector(x,y,h) : pMax;
        }
    }

    boundMin = Vector(0,0,pMin.z);
    boundMax = Vector(rows,cols,pMin.z);

    // Calculer les gradients.
    gradient.resize(rows * cols);
    for (int x = 1; x < rows; x++) {
        for (int y = 1; y < cols; y++) {
            double gx = (Height(x + 1, y) - Height(x - 1, y)) / 2.0f;
            double gy = (Height(x, y + 1) - Height(x, y - 1)) / 2.0f;
            double gz = 0.0f;
            gradient[Index(x, y)] = Vector(gx, gy, gz);
		}
	}

    // Calculer les pentes.
	slope.resize(rows * cols);
    for (int x = 1; x < rows; x++) {
        for (int y = 1; y < cols; y++) {
            Vector gradient00 = Gradient(x, y - 1);
            Vector gradient01 = Gradient(x, y + 1);
            Vector gradient10 = Gradient(x + 1, y);
            Vector gradient11 = Gradient(x - 1, y);

            // Calculer la moyenne des gradients pour le point central du quadrilat�re.
            Vector averageGradient = (gradient00 + gradient01 + gradient10 + gradient11) / 4.0;

            // Calculer la pente (magnitude du gradient moyen).
            slope[Index(x, y)] = length(averageGradient);
            if(slope[Index(x, y)] > slopeMax) slopeMax = slope[Index(x, y)];
		}
	}
}

ScalarField::ScalarField(const vec2& c1, const vec2& c2, int rows, int cols, float e)
        : Grid(c1, c2, rows, cols), slopeMax(0), pMin(0,0,FLT_MAX), pMax(0,0,FLT_MIN)
{
    heights.resize(rows * cols);
    float heightModifier = 50.0f;

    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            float sinus = (sin(2 * M_PI * x / rows) + 1) / 2;
            float cosinus = (cos(2 * M_PI * y / cols) + 1) / 2;
            float h =(sinus + cosinus) / 2 * heightModifier;
            heights[Index(x, y)] = h;
            pMin = h < pMin.z ? Vector(x,y,h) : pMin;
            pMax = h > pMax.z ? Vector(x,y,h) : pMax;
        }
    }

    boundMin = Vector(0,0,pMin.z);
    boundMax = Vector(rows,cols,pMin.z);

    // Calculer les gradients.
    gradient.resize(rows * cols);
    for (int x = 1; x < rows; x++) {
        for (int y = 1; y < cols; y++) {
            double gx = (Height(x + 1, y) - Height(x - 1, y)) / 2.0f;
            double gy = (Height(x, y + 1) - Height(x, y - 1)) / 2.0f;
            double gz = 0.0f;
            gradient[Index(x, y)] = Vector(gx, gy, gz);
        }
    }

    // Calculer les pentes.
    slope.resize(rows * cols);
    for (int x = 1; x < rows; x++) {
        for (int y = 1; y < cols; y++) {
            Vector gradient00 = Gradient(x, y - 1);
            Vector gradient01 = Gradient(x, y + 1);
            Vector gradient10 = Gradient(x + 1, y);
            Vector gradient11 = Gradient(x - 1, y);

            // Calculer la moyenne des gradients pour le point central du quadrilat�re.
            Vector averageGradient = (gradient00 + gradient01 + gradient10 + gradient11) / 4.0;

            // Calculer la pente (magnitude du gradient moyen).
            slope[Index(x, y)] = length(averageGradient);
            if(slope[Index(x, y)] > slopeMax) slopeMax = slope[Index(x, y)];
        }
    }
}

float ScalarField::Height(int x, int y) const 
{
    return Height(double(x), double(y));    

    return heights[Index(x, y)];
}

float ScalarField::Height(double x, double y) const
{
    return heights[Index(x, y)];

    // Local coordinates
    double u=(x-double(boundMin.x))/(boundMax.x-boundMin.x);
    double v=(y-double(boundMin.y))/(boundMax.y-boundMin.y);

    // Cell location within grid
    int nu=int(u*getCols());
    int nv=int(v*getRows());

    // Local coordinates within cell
    u=u-double(nu)*(boundMax.x-boundMin.x)/double(getCols());
    v=v-double(nv)*(boundMax.y-boundMin.y)/double(getRows());

    if(!Inside(nu+1, nv+1))
    {
        return HeightGrid(nu, nv);
    }

    if (u+v<1)
    {
        return (1-u-v)*HeightGrid(nu,nv)
            +u*HeightGrid(nu+1,nv)
            +v*HeightGrid(nu,nv+1);
    }
    else
    {
        return (u+v-1)*HeightGrid(nu+1,nv+1)
            +(1-v)*HeightGrid(nu+1,nv)
            +(1-u)*HeightGrid(nu,nv+1);
    }
}

float ScalarField::HeightGrid(int x, int y) const
{
    return heights[Index(x, y)];
}

Vector ScalarField::Gradient(int x, int y) const {
    return gradient[Index(x, y)];
}

float ScalarField::Slope(float x, float y) const{
    return slope[Index(x, y)];
}

Image ScalarField::HeightImage() const {
    Image img = Image(getRows(), getCols());
    int rows = getRows();
    int cols = getCols();

#pragma omp parallel for collapse(1) shared(img)
    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            float color = Height(x, y) / 15.0f;
            img(x, y) = Color(color, color, color);
        }
    }
    return img;
}

Image ScalarField::GradientNorm(ScalarField &s) {
    Image img = Image(s.getRows(), s.getCols());
    int rows = s.getRows();
    int cols = s.getCols();

#pragma omp parallel for collapse(1) private(heights) shared(img)
    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            Vector v = s.Gradient(x, y);
            float color = length(v);
            img(x, y) = Color(color,color,color);
        }
    }
    return img;
}

float ScalarField::Laplacian(double x, double y) {
    float up, down, left, right, center;


    center = Height(x, y);

    if(Inside(x - 1, y)) {
        down = Height(x - 1, y);
    } else {
        down = center;
    }

    if(Inside(x + 1, y)) {
        up = Height(x + 1, y);
    } else {
        up = center;
    }

    if(Inside(x, y - 1)) {
        left = Height(x, y - 1);
    } else {
        left = center;
    }

    if(Inside(x, y + 1)) {
        right = Height(x, y + 1);
    } else {
        right = center;
    }

    float dx = (pMax.x - pMin.x) / (float)getCols();
    return (right + left + up + down - 4 * center) / (dx * dx);


    //float center = Height(x, y);
    //float lx = Height(x - 1, y) + Height(x + 1, y);
    //float ly = Height(x, y - 1) + Height(x, y + 1);
    //float laplacian = (lx + ly - 4 * center);
    //return laplacian;
}

Image ScalarField::LaplacianImage(ScalarField &s) {
    Image img = Image(s.getRows(), s.getCols());
    int rows = s.getRows();
    int cols = s.getCols();

#pragma omp parallel for collapse(1) private(heights) shared(img)
    for(int x = 0; x < rows; x++){
        for(int y = 0; y < cols; y++){
            float laplacian = s.Laplacian(x, y);
            float color = laplacian;
            img(x, y) = Color(color,color,color);
        }
    }
    return img;
}

Mesh ScalarField::ToMesh() const {
    Mesh mesh = Mesh(GL_TRIANGLES);

    int rows = getRows();
    int cols = getCols();

    for(int x = 0; x < rows - 1; x++) {
        for(int y = 0; y < cols - 1; y++) {
            float texX  = (float)x         / static_cast<float>(rows);
            float texY  = (float)y         / static_cast<float>(cols);
            float texX1 = (float)(x + 1)   / static_cast<float>(rows);
            float texY1 = (float)(y + 1)   / static_cast<float>(cols);

            // First Triangle
            mesh.texcoord(texX, texY);
            mesh.normal(Gradient(x, y));
            mesh.vertex(Point(x, Height(x, y), y));

            mesh.texcoord(texX, texY1);
            mesh.normal(Gradient(x, y + 1));
            mesh.vertex(Point(x, Height(x, y + 1), y + 1));

            mesh.texcoord(texX1, texY);
            mesh.normal(Gradient(x + 1, y));
            mesh.vertex(Point(x + 1, Height(x + 1, y), y));

            // Second Triangle
            mesh.texcoord(texX1, texY);
            mesh.normal(Gradient(x + 1, y));
            mesh.vertex(Point(x + 1, Height(x + 1, y), y));

            mesh.texcoord(texX, texY1);
            mesh.normal(Gradient(x, y + 1));
            mesh.vertex(Point(x, Height(x, y + 1), y + 1));

            mesh.texcoord(texX1, texY1);
            mesh.normal(Gradient(x + 1, y + 1));
            mesh.vertex(Point(x + 1, Height(x + 1, y + 1), y + 1));
        }
    }
    mesh.vertex(Point(0, 0, 0));

    std::cout << "Positions : " << mesh.positions().size() << std::endl;
    std::cout << "Normals : " << mesh.normals().size() << std::endl;
    std::cout << "Texcoords : " << mesh.texcoords().size() << std::endl;

    return mesh;
}

void ScalarField::SetHeight(int x, int y, float newHeight)
{
    //update height
    heights[Index(x, y)] = newHeight;

    //update bounds
    pMin = newHeight < pMin.z ? Vector(x,y,newHeight) : pMin;
    pMax = newHeight > pMax.z ? Vector(x,y,newHeight) : pMax;
    boundMin = Vector(0,0,pMin.z);
    boundMax = Vector(getRows(),getCols(),pMin.z);

    //update gradiant
    double gx = (Height(x + 1, y) - Height(x - 1, y)) / 2.0f;
    double gy = (Height(x, y + 1) - Height(x, y - 1)) / 2.0f;
    double gz = 0.0f;
    gradient[Index(x, y)] = Vector(gx, gy, gz);

    //update slope
    Vector gradient00 = Gradient(x, y - 1);
    Vector gradient01 = Gradient(x, y + 1);
    Vector gradient10 = Gradient(x + 1, y);
    Vector gradient11 = Gradient(x - 1, y);

    // Calculer la moyenne des gradients pour le point central du quadrilat�re.
    Vector averageGradient = (gradient00 + gradient01 + gradient10 + gradient11) / 4.0;

    // Calculer la pente (magnitude du gradient moyen).
    slope[Index(x, y)] = length(averageGradient);
    if(slope[Index(x, y)] > slopeMax) slopeMax = slope[Index(x, y)];
}

ScalarField::~ScalarField() 
{

}

float ScalarField::AccessibilityBox(float xCurrent, float yCurrent) {
    float step = 0.1f;
    int rayon = 2;

    float zCurrent = Height(xCurrent, yCurrent);

    int rows = getRows();
    int cols = getCols();

    float total = 0;
    float superieur = 0;
    for (float x = xCurrent - rayon; x < xCurrent + rayon; x += step) {
        if (x >= 0 && x <= rows) {
            for (float y = yCurrent - rayon; y < yCurrent + rayon; y += step) {
                if (y >= 0 && y <= cols) {
                    float hauteur = Height(x, y);
                    for (float z = zCurrent - rayon; z < zCurrent + rayon; z += step) {
                        if (z > hauteur) {
                            superieur++;
                        }
                        total++;
                    }
                }
            }
        }
    }
    return superieur / total;
}

struct Ray
{
    Point o;                // origine
    Vector d;               // direction
    float tmax;             // position de l'extremite, si elle existe. le rayon est un intervalle [0 tmax]

    // le rayon est un segment, on connait origine et extremite, et tmax= 1
    Ray(const Point& origine, const Point& extremite) : o(origine), d(Vector(origine, extremite)), tmax(1) {}

    // le rayon est une demi droite, on connait origine et direction, et tmax= \inf
    Ray(const Point& origine, const Vector& direction) : o(origine), d(direction), tmax(FLT_MAX) {}

    // renvoie le point sur le rayon pour t
    Point point(const float t) const { return o + t * d; }
};

float ScalarField::AccessibilityRay(float xCurrent, float yCurrent) {
    float radius = 10;
    float epsilon = 0.1;
    float tmax = radius;
    float zCurrent = Height(xCurrent, yCurrent);
    float step = epsilon;

    int numRays = 0;
    int numHits = 0;

    float lambda = slopeMax; //hmax

    for (float theta = 0.0f; theta <= M_PI; theta += step) {
        for (float phi = 0.0f; phi <= M_PI; phi += step) {
            // D�finir la direction du rayon dans la demi-sph�re.
            float sin_theta = sin(theta);
            float cos_theta = cos(theta);

            Vector l= Vector(
                    std::cos(phi) * sin_theta,
                    cos_theta,
                    sin_theta * std::sin(phi)
                );

            // Cr�er le rayon.
            Ray ray(Point(xCurrent, yCurrent, zCurrent), l);

            // Initialiser les variables pour suivre l'avancement du rayon.
            float t = 0.0f;
            bool hitTerrain = false;

            // Parcourir le rayon jusqu'� tmax.
            while (t < tmax) 
            {
                // Obtenir la position actuelle du rayon.
                Point currentPosition = ray.point(t);

                float h = Height(currentPosition.x, currentPosition.y);

                // V�rifier si le rayon touche le terrain.
                if (currentPosition.z < h || h <= (pMin.z - 5.f)) 
                {
                    hitTerrain = true;
                    break;
                }


                // Avancer le rayon de epsilon.
                t += std::max(abs(h - currentPosition.z) / lambda, epsilon);
            }

            // Incr�menter le nombre total de rayons.
            numRays++;

            // Si le rayon a touch� le terrain, ne rien faire.
            // Sinon, incr�mente le nombre de hits.
            if (!hitTerrain) {
                numHits++;
            }
        }
    }

    // Calculer l'accessibilit� comme le ratio des rayons n'ayant pas touch� le terrain.
    float accessibility = static_cast<float>(numHits) / static_cast<float>(numRays);

    return accessibility;
}

Image ScalarField::AccesibilityImage(ScalarField& s) {
    Image img = Image(s.getRows(), s.getCols());
    int rows = s.getRows();
    int cols = s.getCols();

#pragma omp parallel for collapse(1) private(heights) shared(img)
    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            float acces = s.AccessibilityRay(x, y);  
            img(x, y) = Color(acces, acces, acces);
        }
    }

    return img;
}

void ScalarField::Drainage(ScalarField &s) {

}

void ScalarField::ApplyThermicalErosion()
{
    const float angle = M_PI / 4;
    const float k = 0.0005;


    float tanAngle = tan(angle);
    //float spaceStep = (pMax.x - pMin.x) / float(getRows());
  
    for (int x = 0; x < getRows(); x++)
    {
        for (int y = 0; y < getCols(); y++)
        {
            float h = Height(x, y);
    
            float maxSlope = 0;
            int km, lm;
    
            for (int x2 = -1; x2 <= 1; x2++)
            {
                for (int y2 = -1; y2 <= 1; y2++)
                {
                    if(x2 == 0 && y2 == 0) 
                        continue;
                    if(x + x2 < 0 || x + x2 >= getRows() || y + y2 < 0 || y + y2 >= getCols()) 
                        continue;

                    float h2 = Height(x2, y2);
    
                    float slope = (h - h2);
                    //float slope = (h - h2) / spaceStep;

                    if(slope > tanAngle && slope > maxSlope)
                    {
                        maxSlope = slope;
                        km = x2;
                        lm = y2;
                    }
                }
            }
    
            if(maxSlope <= 0.0001) 
                continue;
        
            //Apply errosion
            float delta = maxSlope - tanAngle;
            SetHeight(x,        y,          Height(x,      y     ) - (k * delta ));//- Laplacian(x, y) * k));
            SetHeight(x + km,   y + lm,     Height(x + km, y + lm) +  k * delta);
        }
    }
}

float ScalarField::AverageSlope(int x, int y) {
    // pente des 8 direction autour du point
    float voisin1 = Slope(x - 1, y - 1);
    float voisin2 = Slope(x, y - 1);
    float voisin3 = Slope(x + 1, y - 1);
    float voisin4 = Slope(x - 1, y);
    float voisin5 = Slope(x + 1, y);
    float voisin6 = Slope(x - 1, y + 1);
    float voisin7 = Slope(x, y + 1);
    float voisin8 = Slope(x + 1, y + 1);

    // moyenne des pentes
    float moyenne = (voisin1 + voisin2 + voisin3 + voisin4 + voisin5 + voisin6 + voisin7 + voisin8) / 8.0f;

	return moyenne;
}

Image ScalarField::AverageSlopeImage(ScalarField& s) {
    Image img = Image(s.getRows(), s.getCols());
    int rows = s.getRows();
    int cols = s.getCols();

#pragma omp parallel for collapse(1) private(heights) shared(img)
    for (int x = 1; x < rows; x++) {
        for (int y = 1; y < cols; y++) {
            float averageSlope = s.AverageSlope(x, y);
            img(x, y) = Color(averageSlope, averageSlope, averageSlope);
        }
    }

    return img;
}
