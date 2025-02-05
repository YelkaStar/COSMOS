#ifndef BLACK_HOLE_HPP
#define BLACK_HOLE_HPP

#include <SFML/Graphics.hpp>

class BlackHole {
public:
    sf::CircleShape shape;
    float mass;
    float rotationSpeed; 

    BlackHole(float radius, float mass, sf::Vector2f position, float rotationSpeed = 0.0f);

    void update(float deltaTime);
    sf::Vector2f getPosition() const;
};

#endif 


