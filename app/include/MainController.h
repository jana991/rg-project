//
// Created by matfrg on 10/15/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>
#include <glm/glm.hpp>
namespace app {


class MainController: public engine::core::Controller {
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
    bool spotlightRotating = false;
   bool waitingForRotation = false;
     float spotlightTimer = 0.0f;
     float angle = 0.0f;
     bool spotlightRed = false; // da pratimo da li je svetlo već postalo crveno
};
    struct Boat {
         bool shipMoving = false;
         glm::vec3 boatPos=glm::vec3(1.0f,-1.0f,-7.0f);
    };
    SpotLight spotlight;
    Boat boat;

    glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float spotlightAmbientIntensity = 0.1f;
    float spotlightDiffuseIntensity = 4.0f;
    float spotlightSpecularIntensity = 2.0f;
    float spotlightCutOffDeg = 6.0f;
    float spotlightOuterCutOffDeg = 10.0f;
    float spotlightConstant = 1.0f;
    float spotlightLinear = 0.09f;
    float spotlightQuadratic = 0.032f;


    glm::vec3 dirLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float dirLightAmbientIntensity = 0.05f;
    float dirLightDiffuseIntensity = 0.4f;
    float dirLightSpecularIntensity = 0.5f;

};
}// app

#endif //MAINCONTROLLER_H
