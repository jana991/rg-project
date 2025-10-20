//
// Created by matfrg on 10/15/25.
//

#include "MainController.h"

#include "../../engine/libs/spdlog/include/spdlog/spdlog.h"

#include <engine/platform/PlatformController.hpp>

namespace app {
void MainController::initialize() {
    spdlog::info('Controller initialized');
}
bool MainController::loop() {
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down()) {
        return false;
    }
    return true;
}


}// namespace app