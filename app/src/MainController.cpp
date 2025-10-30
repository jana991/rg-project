// Created by matfrg on 10/15/25.

#include "MainController.h"

#include "GUIController.h"
#include "spdlog/spdlog.h"

#include <engine/graphics/GraphicsController.hpp>
#include <engine/graphics/OpenGL.hpp>
#include <engine/platform/PlatformController.hpp>
#include <engine/resources/ResourcesController.hpp>

namespace app {
class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};
void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}

void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
    int fbWidth  = platform->window()->width();
    int fbHeight = platform->window()->height();
    create_msaa_and_resolve_fbos(fbWidth, fbHeight, msaa_samples);
    engine::graphics::OpenGL::enable_multisample();
}

bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}

void MainController::draw_lighthouse() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();

    engine::resources::Model *lighthouse = resources->model("lighthouse");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    auto camera = graphics->camera();
    shader->set_vec3("viewPos", camera->Position);

    glm::vec3 lighthousePos = glm::vec3(0.0f, -0.5f, -3.0f);
    glm::vec3 spotlightPos = lighthousePos + glm::vec3(0.0f, 4.5f, 0.0f);
    shader->set_vec3("spotLight.position", spotlightPos);

    glm::vec3 spotlightDirection;
    if (spotlight.spotlight_rotating) {
        spotlightDirection = glm::normalize(glm::vec3(sin(spotlight.angle), -1.0f, cos(spotlight.angle)));
    } else {
        spotlightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f));
    }
    shader->set_vec3("spotLight.direction", spotlightDirection);

    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(spotlight_cut_off_deg)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(spotlight_outer_cut_off_deg)));

    shader->set_float("spotLight.constant", spotlight_constant);
    shader->set_float("spotLight.linear", spotlight_linear);
    shader->set_float("spotLight.quadratic", spotlight_quadratic);

    glm::vec3 effective_color = spotlight.spotlight_red ? glm::vec3(1.0f, 0.0f, 0.0f) : spotlight_color;

    shader->set_vec3("spotLight.ambient", effective_color * spotlight_ambient_intensity);
    shader->set_vec3("spotLight.diffuse", effective_color * spotlight_diffuse_intensity);
    shader->set_vec3("spotLight.specular", effective_color * spotlight_specular_intensity);

    shader->set_float("material.shininess", 32.0f);
    glm::vec3 dirLightDirection = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.direction", dirLightDirection);
    shader->set_vec3("dirLight.ambient", dir_light_color * dir_light_ambient_intensity);
    shader->set_vec3("dirLight.diffuse", dir_light_color * dir_light_diffuse_intensity);
    shader->set_vec3("dirLight.specular", dir_light_color * dir_light_specular_intensity);

    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, -3.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);
    shader->set_int("material.diffuse", 0);
    shader->set_int("material.specular", 1);
    shader->set_float("material.shininess", 32.0f);
    lighthouse->draw(shader);
}
void MainController::update_camera() {
    auto gui_controller = engine::core::Controller::get<GUIController>();
    if (gui_controller->is_enabled()) return;
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
}
void MainController::update() {
    update_camera();
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();
    update_spotlight(dt);
}
void MainController::begin_draw() {
    if (ms_fbo) {
        engine::graphics::OpenGL::bind_framebuffer(ms_fbo);
    } else {
        engine::graphics::OpenGL::bind_framebuffer(0);
    }

    engine::graphics::OpenGL::set_viewport(0, 0, fb_width, fb_height);
    engine::graphics::OpenGL::clear_buffers();
}
void MainController::draw_skybox() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto skybox = resources->skybox("night_skybox");
    auto shader = resources->shader("skybox");
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}
void MainController::draw_water() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *water = resources->model("ocean");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, -3.0f));
    model = glm::scale(model, glm::vec3(0.3f));
    shader->set_mat4("model", model);
    water->draw(shader);
}

void MainController::draw_boat() {
    auto resources = engine::core::Controller::get<engine::resources::ResourcesController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    engine::resources::Model *Boat = resources->model("boat");
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, boat.boat_pos);
    model = glm::scale(model, glm::vec3(0.07f));
    shader->set_mat4("model", model);
    Boat->draw(shader);
}
void MainController::draw() {
    begin_draw();
    draw_lighthouse();
    draw_water();
    draw_boat();
    draw_skybox();
    end_draw();
}
void MainController::update_spotlight(float dt) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_G).state() == engine::platform::Key::State::JustPressed && !spotlight.waiting_for_rotation && !spotlight.spotlight_rotating) {
        spotlight.waiting_for_rotation = true;
        spotlight.spotlight_timer = 0.0f;
        spdlog::info("Key G pressed, initialize event A");
    }

    if (spotlight.waiting_for_rotation) {
        spotlight.spotlight_timer += dt;
        if (spotlight.spotlight_timer >= 2.0f) {
            spotlight.waiting_for_rotation = false;
            spotlight.spotlight_rotating = true;
            spdlog::info("event A: Spotlight rotation started");
        }
    }
    if (spotlight.spotlight_rotating) {
        spotlight.spotlight_timer += dt;
        spotlight.angle += 0.5f * dt;
        if (spotlight.spotlight_timer >= 6.0f && !spotlight.spotlight_red) {
            spotlight.spotlight_red = true;
            boat.ship_moving = true;
            spdlog::info("event B: Spotlight changed to red");
        }
    }
    update_boat(dt);
}
void MainController::update_boat(float dt) {
    if (boat.ship_moving) {
        boat.boat_pos.x += 0.2f * dt;
        spotlight.spotlight_timer += dt;
        if (spotlight.spotlight_timer >= 30.0f) {
            boat.ship_moving = false;
        }
    }
}

void MainController::end_draw() {
    if (ms_fbo && resolve_fbo) {
        engine::graphics::OpenGL::blit_framebuffer(ms_fbo, resolve_fbo, fb_width, fb_height);
    }

    engine::graphics::OpenGL::bind_framebuffer(0);
    if (resolve_fbo) {
        engine::graphics::OpenGL::blit_framebuffer(resolve_fbo, 0, fb_width, fb_height);
    }

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}
void MainController::create_msaa_and_resolve_fbos(int width, int height, int samples) {
    fb_width = width;
    fb_height = height;
    msaa_samples = samples;

    if (ms_fbo) {
        engine::graphics::OpenGL::delete_framebuffers(1, &ms_fbo);
        engine::graphics::OpenGL::delete_textures(1, &ms_color_tex);
        engine::graphics::OpenGL::delete_renderbuffers(1, &ms_depth_rbo);
        ms_fbo = ms_color_tex = ms_depth_rbo = 0;
    }
    if (resolve_fbo) {
        engine::graphics::OpenGL::delete_framebuffers(1, &resolve_fbo);
        engine::graphics::OpenGL::delete_textures(1, &resolve_tex);
        resolve_fbo = resolve_tex = 0;
    }

    ms_fbo = engine::graphics::OpenGL::create_framebuffer();
    engine::graphics::OpenGL::bind_framebuffer(ms_fbo);

    ms_color_tex = engine::graphics::OpenGL::create_multisample_texture(width, height, samples);

    engine::graphics::OpenGL::attach_texture_to_framebuffer(ms_fbo, ms_color_tex, /*attachment_index=*/0, /*multisample=*/true);

    ms_depth_rbo = engine::graphics::OpenGL::create_multisample_rbo(width, height, samples);
    engine::graphics::OpenGL::attach_renderbuffer_to_framebuffer(ms_fbo, ms_depth_rbo);

    if (!engine::graphics::OpenGL::is_framebuffer_complete(ms_fbo)) {
        spdlog::error("Frame buffer not complete");
    }

    resolve_fbo = engine::graphics::OpenGL::create_framebuffer();
    engine::graphics::OpenGL::bind_framebuffer(resolve_fbo);

    resolve_tex = engine::graphics::OpenGL::create_texture(width, height);

    engine::graphics::OpenGL::attach_texture_to_framebuffer(resolve_fbo, resolve_tex, /*attachment_index=*/0, /*multisample=*/false);

    if (!engine::graphics::OpenGL::is_framebuffer_complete(resolve_fbo)) {
        spdlog::error("Resolve not complete");
    }
    engine::graphics::OpenGL::bind_framebuffer(0);
    engine::graphics::OpenGL::set_viewport(0, 0, fb_width, fb_height);


    spdlog::info("MSAA create: requested samples={}", samples);
}

void MainController::on_resize(int new_w, int new_h) {
    fb_width = new_w;
    fb_height = new_h;
    create_msaa_and_resolve_fbos(fb_width, fb_height, msaa_samples);
    engine::graphics::OpenGL::set_viewport(0, 0, fb_width, fb_height);
}

} // namespace app

