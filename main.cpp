#include <SFML/Graphics.hpp> 
#include <omp.h>
#include "CameraController.hpp" // Управление
#include "MatterGenerator.hpp" // Генератор материи
#include "BlackHole.hpp" // Черная дыра
#include "Ship.hpp"
#include <cmath>
#include <iostream>
#include <random>
#include <limits>


void applyGravityToMatters(std::vector<Matter>& matters, const sf::Vector2f& blackHolePosition, float blackHoleMass, float gravityMultiplier, float deltaTime);
void handleCollisions(std::vector<Matter>& matters);
Matter* findNearestMatter(const Ship& ship, std::vector<Matter>& matters);

void applyGravityToMatters(std::vector<Matter>& matters, const sf::Vector2f& blackHolePosition, float blackHoleMass, float gravityMultiplier, float deltaTime) {
#pragma omp parallel for schedule(dynamic)
    for (size_t i = 0; i < matters.size(); ++i) {
        matters[i].applyGravity(blackHolePosition, blackHoleMass * gravityMultiplier, deltaTime);
    }
}

void handleCollisions(std::vector<Matter>& matters) {
    for (size_t i = 0; i < matters.size(); ++i) {
        for (size_t j = i + 1; j < matters.size(); ++j) {
            sf::Vector2f diff = matters[i].shape.getPosition() - matters[j].shape.getPosition();
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            float minDistance = matters[i].shape.getRadius() + matters[j].shape.getRadius();

            if (distance < minDistance) {
                sf::Vector2f normal = diff / distance;
                sf::Vector2f relativeVelocity = matters[i].velocity - matters[j].velocity;
                float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

                if (velocityAlongNormal > 0)
                    continue;

                float restitution = 0.8f;
                float j = -(1.0f + restitution) * velocityAlongNormal;
                j /= 1.0f / 1.0f + 1.0f / 1.0f;

                sf::Vector2f impulse = j * normal;
                matters[i].velocity += impulse;
                matters[j].velocity -= impulse;
            }
        }
    }
}

Matter* findNearestMatter(const Ship& ship, std::vector<Matter>& matters) {
    Matter* nearestMatter = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (auto& matter : matters) {
        if (matter.isColony) continue;

        sf::Vector2f diff = matter.shape.getPosition() - ship.shape.getPosition();
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (distance < minDistance) {
            minDistance = distance;
            nearestMatter = &matter;
        }
    }

    return nearestMatter;
}

int main() {
    omp_set_dynamic(0);
    omp_set_num_threads(omp_get_max_threads());
    omp_set_nested(1);

    sf::RenderWindow window(sf::VideoMode(1800, 1600), "Black Hole Simulation");
    if (!window.isOpen()) {
        std::cerr << "Error: Could not open window." << std::endl;
        return -1;
    }

    sf::Vector2f blackHolePosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    BlackHole blackHole(50.0f, 500.0f, blackHolePosition);

    MatterGenerator generator(blackHolePosition, blackHole.mass);
    std::vector<Matter> matters = generator.generateMatter(7560); // Материя или же звезды "(Точки)"
    std::vector<Ship> ships;

    float gravityMultiplier = 70.5f;
    bool blackHoleActive = true;
    bool blackHolePaused = false;
    bool collisionHandlingActive = false;
    bool colonizationActive = false; 
    float timeScale = 1.0f;
    float additionalDistance = 50.0f;
    float deletionRadius = blackHole.shape.getRadius() + additionalDistance;

    sf::View view(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    CameraController cameraController(view);

    sf::Clock clock;

    // начальная колония и корабль
    if (!matters.empty()) {
        matters[0].isColony = true;
        matters[0].shape.setFillColor(sf::Color::Green);
        ships.push_back(matters[0].createShip());
    }

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds() * timeScale;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            cameraController.handleEvent(event);

            if (event.type == sf::Event::KeyPressed) {
                switch (event.key.code) {
                case sf::Keyboard::Num0:
                    blackHoleActive = !blackHoleActive;
                    break;
                case sf::Keyboard::Num1:
                    gravityMultiplier = 0.5f; // Маса дырки = Большоя скорость засасывания
                    break;
                case sf::Keyboard::Num2:
                    gravityMultiplier = 100.0f; // Маса дырки = Большоя скорость засасывания
                    break;
                case sf::Keyboard::Num3: 
                    gravityMultiplier = 20000.0f; // Маса дырки = Большоя скорость засасывания
                    break;
                case sf::Keyboard::Num4:
                    gravityMultiplier = 400000.0f; // Маса дырки = Большоя скорость засасывания
                    break;
                case sf::Keyboard::Num5:
                    gravityMultiplier = 8000000.0f; // Маса дырки = Большоя скорость засасывания
                    break;
                case sf::Keyboard::Num6:
                    gravityMultiplier = 16.0f; // Маса дырки = Большоя скорость засасывания 
                    break;
                case sf::Keyboard::Num7: // НЕ ОПТИМИЗИРОВАНО "(Реал Физики Столкновений)"
                    collisionHandlingActive = !collisionHandlingActive;
                    break;
                case sf::Keyboard::Num8: { // Флаг для активации колонизацыи
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, matters.size() - 1);
                    int randomIndex = dis(gen);
                    matters[randomIndex].isColony = true;
                    matters[randomIndex].shape.setFillColor(sf::Color::Green);
                    colonizationActive = true; //  колонизацыя 
                    break;
                }
                case sf::Keyboard::P:
                    blackHolePaused = !blackHolePaused;
                    break;
                case sf::Keyboard::Add:
                    timeScale *= 2.0f;
                    break;
                case sf::Keyboard::Subtract:
                    timeScale /= 2.0f;
                    break;
                default:
                    break;
                }
            }
        }

        cameraController.handleInput();
        window.setView(view);

        blackHole.update(deltaTime);

        if (blackHoleActive && !blackHolePaused) {
            applyGravityToMatters(matters, blackHole.getPosition(), blackHole.mass, gravityMultiplier, deltaTime);
        }

        if (collisionHandlingActive) {
            handleCollisions(matters);
        }

        for (auto& matter : matters) {
            matter.update(deltaTime);
        }

        // Обработка кораблей
        if (colonizationActive) {
            for (size_t i = 0; i < ships.size();) {
                Matter* nearestMatter = findNearestMatter(ships[i], matters);
                if (nearestMatter) {
                    ships[i].moveTowards(nearestMatter->shape.getPosition(), deltaTime);

                    sf::Vector2f diff = nearestMatter->shape.getPosition() - ships[i].shape.getPosition();
                    float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);

                    if (distance < ships[i].shape.getRadius() + nearestMatter->shape.getRadius()) {
                        // Сохраняем исходную колонию
                        sf::Vector2f returnPosition = ships[i].initialColonyPosition;

                        // Колонизируем новую звезду
                        nearestMatter->isColony = true;
                        nearestMatter->shape.setFillColor(sf::Color::Green);

                        // Создаем новый корабль
                        if (!nearestMatter->hasShip) {
                            ships.push_back(nearestMatter->createShip());
                            nearestMatter->hasShip = true;
                        }

                        // Возвращаем корабль на родину 
                        ships[i].shape.setPosition(returnPosition);
                        ships[i].velocity = sf::Vector2f(0.0f, 0.0f);

                        i++; 
                    }
                    else {
                        i++; 
                    }
                }
                else {
                    i++; 
                }
            }
        }

        // Кил зона дырка
        matters.erase(std::remove_if(matters.begin(), matters.end(),
            [&](const Matter& matter) {
                sf::Vector2f direction = blackHole.getPosition() - matter.shape.getPosition();
                float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
                return distance < deletionRadius;
            }), matters.end());

        // Отрисовка
        window.clear(sf::Color::Black);

        sf::FloatRect visibleArea(view.getCenter() - view.getSize() / 2.0f, view.getSize());

        if (blackHoleActive) {
            window.draw(blackHole.shape);
        }

        for (auto& matter : matters) {
            if (visibleArea.intersects(matter.shape.getGlobalBounds())) {
                window.draw(matter.shape);
            }
        }

        for (auto& ship : ships) {
            if (visibleArea.intersects(ship.shape.getGlobalBounds())) {
                window.draw(ship.shape);
            }
        }

        window.display();
    }

    return 0;
}
