#ifndef MATTER_GENERATOR_HPP
#define MATTER_GENERATOR_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "Ship.hpp"  

class Matter {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float mass;
    bool isColonized = false;
    bool isTargeted = false;
    bool isColony = false;
    bool hasShip = false;

    Matter(float radius, float mass, sf::Vector2f position, sf::Vector2f velocity);

    void update(float deltaTime) {
        sf::Vector2f newPosition = shape.getPosition() + velocity * deltaTime;
        shape.setPosition(newPosition);
    }

    void applyGravity(const sf::Vector2f& blackHolePosition, float blackHoleMass, float deltaTime);
    bool checkCollision(const Matter& other) const;
    void resolveCollision(Matter& other);

    Ship createShip() {
        hasShip = true;
        return Ship(5.0f, shape.getPosition(), sf::Vector2f(0.0f, 0.0f), shape.getPosition());
    }
};

class MatterGenerator {
public:
    MatterGenerator(const sf::Vector2f& blackHolePosition, float blackHoleMass);
    std::vector<Matter> generateMatter(int count);

private:
    float getRandomRadius();
    float getMassForRadius(float radius);
    sf::Vector2f getRandomPosition();
    sf::Vector2f getOrbitalVelocity(const sf::Vector2f& position);

    sf::Vector2f blackHolePosition;
    float blackHoleMass;
};

#endif 
