#ifndef SHIP_HPP
#define SHIP_HPP

#include <SFML/Graphics.hpp>

class Ship {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    sf::Vector2f initialColonyPosition;

    Ship(float radius, sf::Vector2f position, sf::Vector2f velocity, sf::Vector2f initialColonyPosition)
        : velocity(velocity), initialColonyPosition(initialColonyPosition) {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(position);
    }

    void update(float deltaTime) {
        sf::Vector2f newPosition = shape.getPosition() + velocity * deltaTime;
        shape.setPosition(newPosition);
    }

    void moveTowards(const sf::Vector2f& target, float deltaTime) {
        sf::Vector2f direction = target - shape.getPosition();
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance > 0) {
            direction /= distance;
            velocity = direction * 10.0f;
        }
        update(deltaTime);
    }
};

#endif 
