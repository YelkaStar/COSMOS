#include "BlackHole.hpp"

BlackHole::BlackHole(float radius, float mass, sf::Vector2f position, float rotationSpeed)
    : shape(radius), mass(mass), rotationSpeed(rotationSpeed) {
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(radius, radius);
    shape.setPosition(position);
    shape.setRotation(0.0f); // Инициализируем начальный угол поворота
}

void BlackHole::update(float deltaTime) {
    float newRotation = shape.getRotation() + rotationSpeed * deltaTime;
    shape.setRotation(newRotation);
}

sf::Vector2f BlackHole::getPosition() const {
    return shape.getPosition();
}





