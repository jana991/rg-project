//
// Created by matfrg on 10/25/25.
//

#include "../include/GUIController.h"

#include <engine/core/Controller.hpp>
#include <engine/graphics/GraphicsController.hpp>
#include <engine/platform/PlatformController.hpp>
#include <imgui.h>

namespace app {


void GUIController::initialize() {
    set_enable(false);
}
void GUIController::draw() {
    auto graphics = engine::core::Controller::get<engine::graphics::GraphicsController>();
    auto camera=graphics->camera();
    graphics->begin_gui();
    ImGui::Begin("Camera info");
    ImGui::Text("Camera position: (%f, %f, %f)",camera->Position.x,camera->Position.y,camera->Position.z);
    ImGui::End();
    graphics->end_gui();
}
void GUIController::poll_events() {
    auto platform=engine::core::Controller::get<engine::platform::PlatformController>();
    if(platform->key((engine::platform::KeyId::KEY_K)).state()==engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}
} // app