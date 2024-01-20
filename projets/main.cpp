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
        objetScale = 0.03f;
        textureId = 0;
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

        ScalarField field = ScalarField(img, vec2(-1, -1), vec2(1, 1), img.height(), img.width(), 1);
        m_field = field.ToMesh();

        Image gradient = field.GradientNorm(field);
        Image gradientS = gradient.smooth();
        Image gradientB = gradient.Blur();
        write_image(gradient, "gradient.png");
        write_image(gradientS, "gradientS.png");
        write_image(gradientB, "gradientB.png");
        m_gradient_texture = read_texture(0, "gradientS.png");

        Image laplacian = field.LaplacianImage(field);
        write_image(laplacian, "laplacian.png");
        m_laplacian_texture = read_texture(0, "laplacian.png");

        Image accesibility = field.AccesibilityImage(field);
        write_image(accesibility, "accesibility.png");
        m_accesibility_texture = read_texture(0, "accesibility.png");

        Image averageSlope = field.AverageSlopeImage(field);
        write_image(averageSlope, "averageSlope.png");
        m_averageSlope_texture = read_texture(0, "averageSlope.png");


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
                        * Translation(Vector(objetPosition));

        Transform mvp = projection * view * model;

        program_uniform(m_program, "mvpMatrix", mvp);

        if(textureId == 0) {
            program_use_texture(m_program, "texture0", 0, m_texture);
        } else if(textureId == 1){
            program_use_texture(m_program, "texture0", 0, m_gradient_texture);
        } else if (textureId == 2) {
            program_use_texture(m_program, "texture0", 0, m_laplacian_texture);
        } else if (textureId == 3) {
			program_use_texture(m_program, "texture0", 0, m_accesibility_texture);
        } else if (textureId == 4) {
            program_use_texture(m_program, "texture0", 0, m_averageSlope_texture);
        }

        m_field.draw(m_program, true, true, true, false, false);

        imguiWindow();

        return 1;
    }

    void imguiWindow() {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplSdlGL3_NewFrame(m_window);
        ImGui::NewFrame();

        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Application average %.1f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Spacing();
        ImGui::Checkbox("Demo Window", &show_demo_window);

        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        ImGui::SeparatorText("Model Stats");
        ImGui::Text("Vertices: %d", m_field.vertex_count()); ImGui::SameLine();
        ImGui::Text("Triangles: %d", m_field.triangle_count());
        ImGui::Text("Texture: %d", textureId);

        ImGui::SeparatorText("Model Position");
        ImGui::SliderFloat("x", &objetPosition.x, -1000.0f, 1000.0f);
        ImGui::SliderFloat("y", &objetPosition.y, -1000.0f, 1000.0f);
        ImGui::SliderFloat("z", &objetPosition.z, -1000.0f, 1000.0f);

        ImGui::SeparatorText("Model Scale");
        ImGui::SliderFloat("scale", &objetScale, 0.0f, 0.05f, "%.3f");

        ImGui::SeparatorText("Textures");

        ImGui::RadioButton("Basic", &textureId, 0); ImGui::SameLine();
        ImGui::RadioButton("Gradient", &textureId, 1); ImGui::SameLine();
        ImGui::RadioButton("Laplacian", &textureId, 2); 
        ImGui::RadioButton("Accesibility", &textureId, 3); ImGui::SameLine();
        ImGui::RadioButton("Average Slope", &textureId, 4);

        if (ImGui::CollapsingHeader("Basic"))
        {
            ImVec2 texture_size(300, 300);
            ImGui::Image((void*)(intptr_t)m_texture, texture_size);
        }

        if (ImGui::CollapsingHeader("Gradient"))
        {
            ImVec2 texture_size(300, 300);
            ImGui::Image((void*)(intptr_t)m_gradient_texture, texture_size);
        }

        if (ImGui::CollapsingHeader("Laplacian"))
        {
            ImVec2 texture_size(300, 300);
            ImGui::Image((void*)(intptr_t)m_laplacian_texture, texture_size);
        }

        if (ImGui::CollapsingHeader("Accesibility"))
        {
            ImVec2 texture_size(300, 300);
            ImGui::Image((void*)(intptr_t)m_accesibility_texture, texture_size);
        }

        if (ImGui::CollapsingHeader("averageSlope"))
        {
            ImVec2 texture_size(300, 300);
            ImGui::Image((void*)(intptr_t)m_averageSlope_texture, texture_size);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
    }

protected:
    GLuint m_program;
    GLuint m_texture;
    GLuint m_gradient_texture;
    GLuint m_laplacian_texture;
    GLuint m_accesibility_texture;
    GLuint m_averageSlope_texture;
    Mesh m_field;

    // Imgui variables
    bool show_demo_window;
    Point objetPosition;
    float objetScale;
    int textureId;
    Color color;
};

int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    return 0;
}