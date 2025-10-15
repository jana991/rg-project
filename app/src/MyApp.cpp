//
// Created by matfrg on 10/15/25.
//

#include "../include/MyApp.h"

#include "../../engine/libs/spdlog/include/spdlog/spdlog.h"
#include "MainController.h"

namespace app {

void MyApp::app_setup() {
    spdlog::info("App setup completed");
    auto maincontroller=register_controller<app::MainController>();
}
} // app