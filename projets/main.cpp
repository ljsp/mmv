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
        objetPosition = Point(-100, 0, -100);
        objectRotation = 0;
        objetScale = 0.03f;
        color= White();

#ifdef __linux__
        Image img = read_image("../data/terrain/terrain_texture.png");
        m_texture = read_texture(0, "../data/terrain/terrain_texture.png");
        m_program = read_program("../data/shaders/textures.glsl");
#else
        Image img = read_image("data/terrain/terrain_texture.png");
        m_texture = read_texture(0, "data/terrain/terrain_texture.png");
        m_program = read_program("data/shaders/textures.glsl");
#endif
        program_print_errors(m_program);

        ScalarField field= ScalarField(img, vec2(-1, -1), vec2(1, 1), img.height(), img.width(), 1);
        m_field = field.ToMesh();

        glClearColor(0.2f, 0.2f, 0.2f, 1.f);  // couleur par defaut de la fenetre
        glClearDepth(1.f);                    // profondeur par defaut
        glDepthFunc(GL_LESS);                 // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);              // activer le ztest

        return 0;
    }

    int quit() {
        m_field.release();
        release_program(m_program);
        ImGui_ImplSdlGL3_Shutdown();
        ImGui::DestroyContext();
        return 0;
    }

    int render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(m_program);

        Transform view= camera().view();
        Transform projection= camera().projection();
        Transform model= Identity()
                        * Scale(objetScale)
                        * Translation(Vector(objetPosition))
                        * RotationY(objectRotation);

        Transform mvp = projection * view * model;

        program_uniform(m_program, "mvpMatrix", mvp);
        program_use_texture(m_program, "texture0", 0, m_texture);

        m_field.draw(m_program, true, true, true, false, false);

        imguiWindow();

        return 1;
    }

    void imguiWindow() {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplSdlGL3_NewFrame(m_window);
        ImGui::NewFrame();

        ImGui::Begin("Settings", nullptr);
        ImGui::Text("Application average %.1f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Spacing();
        ImGui::Checkbox("Demo Window", &show_demo_window);

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::SeparatorText("Model Position");
        ImGui::SliderFloat("x", &objetPosition.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("y", &objetPosition.y, -1000.0f, 1000.0f);
        ImGui::SliderFloat("z", &objetPosition.z, -1000.0f, 1000.0f);
        ImGui::SliderFloat("rotation", &objectRotation, -180.0f, 180.0f, "%.3f");

        ImGui::SeparatorText("Model Scale");
        ImGui::SliderFloat("scale", &objetScale, 0.0f, 0.05f, "%.3f");

        ImGui::End();
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    }

protected:
    GLuint m_program;
    GLuint m_texture;
    Mesh m_field;

    // Imgui variables
    bool show_demo_window;
    Point objetPosition;
    float objectRotation;
    float objetScale;
    Color color;
};

int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    return 0;
}