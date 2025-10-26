//
// Created by matfrg on 10/15/25.
//

#include "MainController.h"

#include "../../engine/libs/assimp/code/AssetLib/3MF/3MFXmlTags.h"
#include "../../engine/libs/assimp/code/AssetLib/glTF2/glTF2Exporter.h"
#include "../../engine/libs/spdlog/include/spdlog/spdlog.h"
#include "GUIController.h"

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
    auto gui_controller=engine::core::Controller::get<GUIController>();
    if(!gui_controller->is_enabled()) {
        auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
        camera->rotate_camera(position.dx, position.dy);
    }
}



void MainController::initialize() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
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
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();

    engine::resources::Model *lighthouse = resources->model("lighthouse");

    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();

    auto camera = graphics->camera();
    shader->set_vec3("viewPos", camera->Position);


    glm::vec3 lighthousePos = glm::vec3(0.0f, -0.5f, -3.0f);
    glm::vec3 spotlightPos = lighthousePos + glm::vec3(0.0f, 4.5f, 0.0f); // na vrhu svetionika

    // Setup spotlight
    shader->set_vec3("spotLight.position", spotlightPos);

    glm::vec3 spotlightDirection;
    if (spotlight.spotlightRotating) {
        spotlightDirection = glm::normalize(glm::vec3(sin(spotlight.angle), -1.0f, cos(spotlight.angle)));
    } else {
        spotlightDirection = glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f));
    }
    shader->set_vec3("spotLight.direction", spotlightDirection);

    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(spotlightCutOffDeg)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(spotlightOuterCutOffDeg)));

    shader->set_float("spotLight.constant", spotlightConstant);
    shader->set_float("spotLight.linear", spotlightLinear);
    shader->set_float("spotLight.quadratic", spotlightQuadratic);

    // ambient/diffuse/specular: po defaultu koristimo spotlightColor * intensity
    glm::vec3 effectiveColor = spotlight.spotlightRed ? glm::vec3(1.0f, 0.0f, 0.0f) : spotlightColor;

    shader->set_vec3("spotLight.ambient", effectiveColor * spotlightAmbientIntensity);
    shader->set_vec3("spotLight.diffuse", effectiveColor * spotlightDiffuseIntensity);
    shader->set_vec3("spotLight.specular", effectiveColor * spotlightSpecularIntensity);


    shader->set_float("material.shininess", 32.0f);
    glm::vec3 dirLightDirection = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.direction", dirLightDirection);
    shader->set_vec3("dirLight.ambient", dirLightColor * dirLightAmbientIntensity);
    shader->set_vec3("dirLight.diffuse", dirLightColor * dirLightDiffuseIntensity);
    shader->set_vec3("dirLight.specular", dirLightColor * dirLightSpecularIntensity);

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
    auto gui_controller=engine::core::Controller::get<GUIController>();
    if(gui_controller->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera = graphics->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KeyId::KEY_W).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_A).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_S).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
    }
    if (platform->key(engine::platform::KeyId::KEY_D).is_down()) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
    }
}
void MainController::update() {

    update_camera();

    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    float dt = platform->dt();
    update_spotlight(dt);
    // da li je g pritisnuto, i da li nije već pre toga





}
void MainController::begin_draw() {
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
    //shader
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
    model = glm::translate(model,boat.boatPos);
    model = glm::scale(model, glm::vec3(0.07f));
    shader->set_mat4("model", model);

    Boat->draw(shader);
}
void MainController::draw() {
    //clear buffers (color buffer, depth buffer)
    draw_lighthouse();
    draw_water();
    draw_boat();
    draw_skybox();
    //swap buffer
}
void MainController::update_spotlight(float dt) {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_G).state()==engine::platform::Key::State::JustPressed && !spotlight.waitingForRotation && !spotlight.spotlightRotating) {
        spotlight.waitingForRotation = true;
        spotlight.spotlightTimer = 0.0f;
        spdlog::info("Key G pressed, initialize event A");
    }

    // čeka dve sekunde pa rotira
    if (spotlight.waitingForRotation) {
        spotlight.spotlightTimer += dt;
        if (spotlight.spotlightTimer >= 2.0f) {
            spotlight.waitingForRotation = false;
            spotlight.spotlightRotating = true;
            spdlog::info("event A: Spotlight rotation started");
        }
    }
    if (spotlight.spotlightRotating) {
        spotlight.spotlightTimer += dt;


        spotlight.angle += 0.5f * dt; // rad/s

        // Event B: 4 sekunde nakon početka rotacije svetlo postaje crveno
        if (spotlight.spotlightTimer >= 6.0f && !spotlight.spotlightRed) {
            spotlight.spotlightRed = true;
            boat.shipMoving=true;
            spdlog::info("event B: Spotlight changed to red");
        }
    }
    update_boat(dt);
}
void MainController::update_boat(float dt) {
    if (boat.shipMoving) {
        boat.boatPos.x += 0.2f * dt;  // polako napred
        spotlight.spotlightTimer += dt;
        if(spotlight.spotlightTimer>=30.0f) {
            boat.shipMoving=false;
        }
    }
}

void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}


}// namespace app