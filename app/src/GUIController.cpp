//
// Created by matfrg on 10/25/25.
//

#include "../include/GUIController.h"

#include "../include/MainController.h"
#include "spdlog/spdlog.h"

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
    auto camera = graphics->camera();
    auto main = engine::core::Controller::get<app::MainController>();

    graphics->begin_gui();

    ImGui::Begin("Light Controls");
    ImGui::Text("Spotlight settings");
    ImGui::Separator();
    ImGui::ColorEdit3("Spotlight Color", (float *) &main->spotlight_color);// 0..1
    ImGui::SliderFloat("Spot Ambient", &main->spotlight_ambient_intensity, 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Spot Diffuse", &main->spotlight_diffuse_intensity, 0.0f, 10.0f, "%.3f");
    ImGui::SliderFloat("Spot Specular", &main->spotlight_specular_intensity, 0.0f, 10.0f, "%.3f");
    ImGui::SliderFloat("Inner Cutoff (deg)", &main->spotlight_cut_off_deg, 1.0f, 45.0f, "%.1f");
    ImGui::SliderFloat("Outer Cutoff (deg)", &main->spotlight_outer_cut_off_deg, 1.0f, 60.0f, "%.1f");
    ImGui::SliderFloat("Spot Constant", &main->spotlight_constant, 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Spot Linear", &main->spotlight_linear, 0.0f, 1.0f, "%.4f");
    ImGui::SliderFloat("Spot Quadratic", &main->spotlight_quadratic, 0.0f, 0.1f, "%.5f");

    ImGui::Separator();
    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("DirLight Color", (float *) &main->dir_light_color);
    ImGui::SliderFloat("Dir Ambient", &main->dir_light_ambient_intensity, 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Dir Diffuse", &main->dir_light_diffuse_intensity, 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Dir Specular", &main->dir_light_specular_intensity, 0.0f, 2.0f, "%.3f");
    ImGui::Separator();
    if (ImGui::Button("Reset Spotlight Params")) {
        main->spotlight_color = glm::vec3(1.0f);
        main->spotlight_ambient_intensity = 0.1f;
        main->spotlight_diffuse_intensity = 4.0f;
        main->spotlight_specular_intensity = 2.0f;
        main->spotlight_cut_off_deg = 6.0f;
        main->spotlight_outer_cut_off_deg = 10.0f;
        main->spotlight_constant = 1.0f;
        main->spotlight_linear = 0.09f;
        main->spotlight_quadratic = 0.032f;
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset DirLight")) {
        main->dir_light_color = glm::vec3(1.0f);
        main->dir_light_ambient_intensity = 0.05f;
        main->dir_light_diffuse_intensity = 0.4f;
        main->dir_light_specular_intensity = 0.5f;
    }
    ImGui::End();

    graphics->end_gui();
}
void GUIController::poll_events() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key((engine::platform::KeyId::KEY_K)).state() == engine::platform::Key::State::JustPressed) {
        set_enable(!is_enabled());
    }
}
}// namespace app