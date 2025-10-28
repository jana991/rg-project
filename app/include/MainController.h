//
// Created by matfrg on 10/15/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
namespace app {


class MainController : public engine::core::Controller {
    void initialize() override;
    bool loop() override;
    void draw_lighthouse();
    void update_camera();
    void update() override;
    void begin_draw() override;
    void draw_skybox();
    void draw_water();
    void draw_boat();
    void draw() override;


    void update_spotlight(float dt);
    void update_boat(float dt);


    void end_draw() override;


public:
    std::string_view name() const override {
        return "app::MainController";
    }

    struct SpotLight {
        bool spotlight_rotating = false;
        bool waiting_for_rotation = false;
        float spotlight_timer = 0.0f;
        float angle = 0.0f;
        bool spotlight_red = false;// da pratimo da li je svetlo već postalo crveno
    };
    struct Boat {
        bool ship_moving = false;
        glm::vec3 boat_pos = glm::vec3(1.0f, -1.0f, -7.0f);
    };
    SpotLight spotlight;
    Boat boat;

    glm::vec3 spotlight_color = glm::vec3(1.0f, 1.0f, 1.0f);
    float spotlight_ambient_intensity = 0.1f;
    float spotlight_diffuse_intensity = 4.0f;
    float spotlight_specular_intensity = 2.0f;
    float spotlight_cut_off_deg = 6.0f;
    float spotlight_outer_cut_off_deg = 10.0f;
    float spotlight_constant = 1.0f;
    float spotlight_linear = 0.09f;
    float spotlight_quadratic = 0.032f;


    glm::vec3 dir_light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    float dir_light_ambient_intensity = 0.05f;
    float dir_light_diffuse_intensity = 0.4f;
    float dir_light_specular_intensity = 0.5f;

    //antialiasing
    unsigned int ms_fbo = 0;
    unsigned int ms_color_tex = 0;// GL_TEXTURE_2D_MULTISAMPLE
    unsigned int ms_depth_rbo = 0;

    unsigned int resolve_fbo = 0;
    unsigned int resolve_tex = 0;// single-sample color texture

    int msaa_samples = 4;
    int fb_width = 1280;
    int fb_height = 720;


    void create_msaa_and_resolve_fbos(int width, int height, int samples = 4);


    void on_resize(int new_w, int new_h);
};
}// namespace app

#endif//MAINCONTROLLER_H
