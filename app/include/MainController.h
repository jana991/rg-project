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

};
}// app

#endif //MAINCONTROLLER_H
