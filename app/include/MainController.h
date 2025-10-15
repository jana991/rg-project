//
// Created by matfrg on 10/15/25.
//

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H
#include <engine/core/Controller.hpp>
namespace app {

class MainController: public engine::core::Controller {
    void initialize() override;

public:
    std::string_view name() const override {
        return "app::MainController";
    }
};

} // app

#endif //MAINCONTROLLER_H
