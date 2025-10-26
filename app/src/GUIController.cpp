//
// Created by matfrg on 10/25/25.
//

#include "../include/GUIController.h"

#include "../include/MainController.h"
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
    ImGui::ColorEdit3("Spotlight Color", (float *) &main->spotlightColor);// 0..1
    ImGui::SliderFloat("Spot Ambient", &main->spotlightAmbientIntensity, 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Spot Diffuse", &main->spotlightDiffuseIntensity, 0.0f, 10.0f, "%.3f");
    ImGui::SliderFloat("Spot Specular", &main->spotlightSpecularIntensity, 0.0f, 10.0f, "%.3f");
    ImGui::SliderFloat("Inner Cutoff (deg)", &main->spotlightCutOffDeg, 1.0f, 45.0f, "%.1f");
    ImGui::SliderFloat("Outer Cutoff (deg)", &main->spotlightOuterCutOffDeg, 1.0f, 60.0f, "%.1f");
    ImGui::SliderFloat("Spot Constant", &main->spotlightConstant, 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Spot Linear", &main->spotlightLinear, 0.0f, 1.0f, "%.4f");
    ImGui::SliderFloat("Spot Quadratic", &main->spotlightQuadratic, 0.0f, 0.1f, "%.5f");

    ImGui::Separator();
    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("DirLight Color", (float *) &main->dirLightColor);
    ImGui::SliderFloat("Dir Ambient", &main->dirLightAmbientIntensity, 0.0f, 1.0f, "%.3f");
    ImGui::SliderFloat("Dir Diffuse", &main->dirLightDiffuseIntensity, 0.0f, 2.0f, "%.3f");
    ImGui::SliderFloat("Dir Specular", &main->dirLightSpecularIntensity, 0.0f, 2.0f, "%.3f");
    ImGui::Separator();
    if (ImGui::Button("Reset Spotlight Params")) {
        main->spotlightColor = glm::vec3(1.0f);
        main->spotlightAmbientIntensity = 0.1f;
        main->spotlightDiffuseIntensity = 4.0f;
        main->spotlightSpecularIntensity = 2.0f;
        main->spotlightCutOffDeg = 6.0f;
        main->spotlightOuterCutOffDeg = 10.0f;
        main->spotlightConstant = 1.0f;
        main->spotlightLinear = 0.09f;
        main->spotlightQuadratic = 0.032f;
    }


    if (ImGui::Button("Reset DirLight")) {
        main->dirLightColor = glm::vec3(1.0f);
        main->dirLightAmbientIntensity = 0.05f;
        main->dirLightDiffuseIntensity = 0.4f;
        main->dirLightSpecularIntensity = 0.5f;
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