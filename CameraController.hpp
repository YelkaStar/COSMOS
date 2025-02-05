#ifndef CAMERACONTROLLER_HPP
#define CAMERACONTROLLER_HPP

#include <SFML/Graphics.hpp>

class CameraController {
public:
    CameraController(sf::View& view);
    void handleInput();
    void handleEvent(const sf::Event& event);

private:
    sf::View& view;
};

#endif 
