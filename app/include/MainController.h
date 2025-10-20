//
// Created by matfrg on 10/15/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>
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

    void end_draw() override;



public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_H
