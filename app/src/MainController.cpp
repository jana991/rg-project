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
// za svetionik
static bool spotlightRotating = false;
static bool waitingForRotation = false;
static float spotlightTimer = 0.0f;
static float angle = 0.0f;
// za brod
static bool boatMoving = false;
static float eventBTimer = 0.0f;
static float boatAngle = 0.0f;
static float boatRadius = 3.0f;
static glm::vec3 boatCenter = glm::vec3(0.0f, -1.0f, -3.0f);
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


    shader->set_vec3("spotLight.position", spotlightPos);

    glm::vec3 spotlightDir;

// pozicije da li su dobre
    if (spotlightRotating) {
        angle += 0.5f * platform->dt();
        spotlightDir = glm::normalize(glm::vec3(sin(angle), -1.0f, cos(angle)));
    } else {

        spotlightDir = glm::normalize(glm::vec3(0.0f, -1.0f, 1.0f));
    }

    shader->set_vec3("spotLight.direction", spotlightDir);
    shader->set_float("spotLight.cutOff", glm::cos(glm::radians(6.0f)));
    shader->set_float("spotLight.outerCutOff", glm::cos(glm::radians(10.0f)));

    shader->set_float("spotLight.constant", 1.0f);
    shader->set_float("spotLight.linear", 0.09f);
    shader->set_float("spotLight.quadratic", 0.032f);

    shader->set_vec3("spotLight.ambient", glm::vec3(0.1f));
    shader->set_vec3("spotLight.diffuse", glm::vec3(4.0f));
    shader->set_vec3("spotLight.specular", glm::vec3(2.0f));


    shader->set_float("material.shininess", 32.0f);
    glm::vec3 dirLightDirection = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->set_vec3("dirLight.direction", dirLightDirection);
    shader->set_vec3("dirLight.ambient", glm::vec3(0.05f));
    shader->set_vec3("dirLight.diffuse", glm::vec3(0.4f));
    shader->set_vec3("dirLight.specular", glm::vec3(0.5f));
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
    // da li je g pritisnuto, i da li nije već pre toga
    if (platform->key(engine::platform::KeyId::KEY_G).state()==engine::platform::Key::State::JustPressed && !waitingForRotation && !spotlightRotating) {
        waitingForRotation = true;
        spotlightTimer = 0.0f;
        spdlog::info("Key G pressed, initialize event A");
    }
    // čeka dve sekunde pa rotira
    if (waitingForRotation) {
        spotlightTimer += dt;
        if (spotlightTimer >= 2.0f) {
            waitingForRotation = false;
            spotlightRotating = true;
            spdlog::info("event A: Spotlight rotation started");
        }
    }
    // drugi event za brod
    if (spotlightRotating && !boatMoving) {
        eventBTimer += dt;
        if (eventBTimer >= 4.0f) {
            boatMoving = true;
            spdlog::info("event B: Boat circulating started");
        }
    }

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
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    engine::resources::Model *boat = resources->model("boat");
    //shader
    engine::resources::Shader *shader = resources->shader("basic");
    shader->use();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);


    if (boatMoving) {
        boatAngle += 0.4f * platform->dt(); // brzina kretanja
    }

    float boatX = boatCenter.x + boatRadius * sin(boatAngle);
    float boatZ = boatCenter.z + boatRadius * cos(boatAngle);

    model = glm::translate(model, glm::vec3(boatX, boatCenter.y, boatZ));
    model = glm::scale(model, glm::vec3(0.07f));
    shader->set_mat4("model", model);
    boat->draw(shader);
}
void MainController::draw() {
    //clear buffers (color buffer, depth buffer)
    draw_lighthouse();
    draw_water();
    draw_boat();
    draw_skybox();
    //swap buffer
}
void MainController::end_draw() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    platform->swap_buffers();
}


}// namespace app