#include "CameraController.hpp"

CameraController::CameraController(sf::View& view) : view(view) {}

void CameraController::handleInput() {
    float moveSpeed = 500.0f; // Скорость перемещения камеры
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        view.move(-moveSpeed, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        view.move(moveSpeed, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        view.move(0, -moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        view.move(0, moveSpeed);
    }
}

void CameraController::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            view.zoom(0.9f);
        }
        else {
            view.zoom(1.1f);
        }
    }
}
