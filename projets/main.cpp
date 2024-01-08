//
// Created by lucas on 08/09/23.
//


#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "draw.h"
#include "app_camera.h"

#include "program.h"
#include "uniforms.h"

#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

#include "HeightField.h"
#include "ScalarField.h"

Mesh make_grid( const int n= 10 )
{
    Mesh grid= Mesh(GL_LINES);

    // grille
    grid.color(White());
    for(int x= 0; x < n; x++)
    {
        float px= float(x) - float(n)/2 + .5f;
        grid.vertex(Point(px, 0, - float(n)/2 + .5f));
        grid.vertex(Point(px, 0, float(n)/2 - .5f));
    }

    for(int z= 0; z < n; z++)
    {
        float pz= float(z) - float(n)/2 + .5f;
        grid.vertex(Point(- float(n)/2 + .5f, 0, pz));
        grid.vertex(Point(float(n)/2 - .5f, 0, pz));
    }

    // axes XYZ
    grid.color(Red());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(1, .1, 0));

    grid.color(Green());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, 1, 0));

    grid.color(Blue());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, .1, 1));

    glLineWidth(2);

    return grid;
}

class TP : public AppCamera
{
public:
    TP( ) : AppCamera(1920, 1080) {}

    int init( )
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSdlGL3_Init(m_window, "#version 330");

        show_demo_window = false;
        lightPosition= Point(0, 10, 10);
        objetPosition= Point(0, 9, 0);
        objectRotation = 0;
        objetScale = 0.1f;
        toonLevel= 4;
        color= White();

#ifdef __linux__
        Image img = read_image("../data/terrain/terrain.png");
#else
        Image img = read_image("data/terrain/terrain.png");
#endif
        ScalarField field= ScalarField(img, vec2(-1, -1), vec2(1, 1), img.height() , img.width() , 1);
        m_field = field.ToMesh();

        m_repere= make_grid(10);

#ifdef __linux__
        m_cube = read_mesh("../data/cube.obj");
#else
		m_cube = read_mesh("data/cube.obj");
#endif
        if(m_cube.materials().count() == 0) return -1;
        if(!m_cube.vertex_count()) return -1;
        
#ifdef __linux__
        m_objet = read_mesh("../data/bigguy.obj");
#else
		m_objet = read_mesh("data/bigguy.obj");
#endif
        if(m_objet.materials().count() == 0) return -1;
        if(!m_objet.vertex_count()) return -1;
        
#ifdef __linux__
        m_texture = read_texture(0, "../data/monde.jpg");
#else
		m_texture = read_texture(0, "data/monde.jpg");
#endif 

        m_groups = m_objet.groups();

#ifdef __linux__
        m_program = read_program("../data/shaders/texturesAndToon.glsl");
#else
        m_program = read_program("data/shaders/texturesAndToon.glsl");
#endif 
        program_print_errors(m_program);

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);  // couleur par defaut de la fenetre
        glClearDepth(1.f);                                   // profondeur par defaut
        glDepthFunc(GL_LESS);                                 // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                              // activer le ztest

        return 0;
    }

    int quit( )
    {
        m_repere.release();
        m_objet.release();
        m_cube.release();
        m_sphere.release();
        release_program(m_program);
        ImGui_ImplSdlGL3_Shutdown();
        ImGui::DestroyContext();
        return 0;
    }

    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //draw(m_repere, Identity(), camera());
        draw(m_field, Identity() * Scale(0.03) * Translation(-100, 0, -100), camera());

        glUseProgram(m_program);
        program_use_texture(m_program, "texture0", 0, m_texture);
        setUniforms();

        //m_sphere.draw(m_program, true, false, false, false, false);

        handleKeys();

        imguiWindow();

        return 1;
    }

    void setUniforms()
    {
        Transform view= camera().view();
        Transform projection= camera().projection();
        Transform model= Identity() * Scale(objetScale) *
                         Translation(Vector(objetPosition)) * RotationY(objectRotation);
        Transform mvp= projection * view * model;

        location= glGetUniformLocation(m_program, "mvpMatrix");
        glUniformMatrix4fv(location, 1, GL_TRUE, mvp.data());

        location= glGetUniformLocation(m_program, "lightPosition");
        glUniform3f(location, lightPosition.x, lightPosition.y, lightPosition.z);

        location= glGetUniformLocation(m_program, "diffuse");
        glUniform4f(location, color.r, color.g, color.b, color.a);

        location= glGetUniformLocation(m_program, "toonLevel");
        glUniform1i(location, toonLevel);
    }

    void handleKeys( )
    {
        if(key_state(SDLK_LEFT) || key_state(SDLK_q))
            lightPosition = lightPosition - Vector(0.1f, 0, 0);
        if(key_state(SDLK_RIGHT) || key_state(SDLK_d))
            lightPosition = lightPosition + Vector(0.1f, 0, 0);
        if(key_state(SDLK_DOWN) || key_state(SDLK_z))
            lightPosition = lightPosition + Vector(0, 0, 0.1f);
        if(key_state(SDLK_UP) || key_state(SDLK_s))
            lightPosition = lightPosition - Vector(0, 0, 0.1f);
        if(key_state(SDLK_SPACE))
            lightPosition = lightPosition + Vector(0, 0.1f, 0);
        if(key_state(SDLK_LSHIFT))
            lightPosition = lightPosition - Vector(0, 0.1f, 0);
    }

    void imguiWindow( )
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplSdlGL3_NewFrame(m_window);
        ImGui::NewFrame();

        ImGui::Begin("Settings", nullptr);
        ImGui::Text("Application average %.1f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Spacing();
        ImGui::Checkbox("Demo Window", &show_demo_window);

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SeparatorText("Light Position");
        ImGui::SliderFloat("light x", &lightPosition.x, -10.0f, 10.0f);
        ImGui::SliderFloat("light y", &lightPosition.y, -10.0f, 10.0f);
        ImGui::SliderFloat("light z", &lightPosition.z, -10.0f, 10.0f);

        ImGui::SeparatorText("Toon Subdivision");
        ImGui::SliderInt("Toon Level", &toonLevel, 1, 30, "%d", ImGuiSliderFlags_Logarithmic);

        ImGui::SeparatorText("Model Color");
        ImGui::ColorEdit3("color", (float*)&color);

        ImGui::SeparatorText("Model Position");
        ImGui::SliderFloat("x", &objetPosition.x, -10.0f, 10.0f);
        ImGui::SliderFloat("y", &objetPosition.y, -10.0f, 10.0f);
        ImGui::SliderFloat("z", &objetPosition.z, -10.0f, 10.0f);
        ImGui::SliderFloat("rotation", &objectRotation, -180.0f, 180.0f, "%.3f");

        ImGui::SeparatorText("Model Scale");
        ImGui::SliderFloat("scale", &objetScale, 0.0f, 5.0f, "%.3f");

        ImGui::End();
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());

    }

protected:
    GLuint m_program;
    GLuint m_texture;
    Mesh m_objet;
    Mesh m_repere;
    Mesh m_cube;
    Mesh m_sphere;
    Mesh m_field;
    std::vector<TriangleGroup> m_groups;
    int location;

    // Imgui variables
    bool show_demo_window;
    Point lightPosition;
    Point objetPosition;
    float objectRotation;
    float objetScale;
    int toonLevel;
    Color color;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    return 0;
}