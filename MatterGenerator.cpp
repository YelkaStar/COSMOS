#include "MatterGenerator.hpp"
#include <cmath>

MatterGenerator::MatterGenerator(const sf::Vector2f& blackHolePosition, float blackHoleMass)
    : blackHolePosition(blackHolePosition), blackHoleMass(blackHoleMass) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

std::vector<Matter> MatterGenerator::generateMatter(int count) {
    std::vector<Matter> matters;
    matters.reserve(count);
    for (int i = 0; i < count; ++i) {
        float radius = getRandomRadius();
        float mass = getMassForRadius(radius);
        sf::Vector2f position = getRandomPosition();
        sf::Vector2f velocity = getOrbitalVelocity(position);
        matters.emplace_back(radius, mass, position, velocity);
    }
    return matters;
}

float MatterGenerator::getRandomRadius() {
    int category = std::rand() % 3;
    switch (category) {
    case 0: return 20.0f;  
    case 1: return 40.0f;  
    case 2: return 60.0f;  
    default: return 3.0f;
    }
}

float MatterGenerator::getMassForRadius(float radius) {
    if (radius == 3.0f) return 1.0f;   
    if (radius == 5.0f) return 2.0f;   
    if (radius == 7.0f) return 3.0f;  
    return 2.0f;
}

sf::Vector2f MatterGenerator::getRandomPosition() {
    float minDistance = 100.0f; // Минимальное расстояние от чёрной дыры
    float maxDistance = 17500.0f; // Максимальное расстояние от чёрной дыры
    float distance = minDistance + static_cast<float>(std::rand()) / RAND_MAX * (maxDistance - minDistance);

    // Генерируем угол от 0 до 2π
    float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.0f * 3.14159265f;

    // Вычисляем координаты
    float x = blackHolePosition.x + distance * std::cos(angle);
    float y = blackHolePosition.y + distance * std::sin(angle);

    return sf::Vector2f(x, y);
}

sf::Vector2f MatterGenerator::getOrbitalVelocity(const sf::Vector2f& position) {
    sf::Vector2f direction = position - blackHolePosition;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    // Нормализуем направление
    direction /= distance;

    // Вычисляем орбитальную скорость v = sqrt(G * M / R)
    float G = 1.0f; // Гравитационная постоянная
    float speed = std::sqrt(G * blackHoleMass / distance);

    // Увеличиваем скорость с помощью множителя
    float speedMultiplier = 3.0f; 
    speed *= speedMultiplier;

    sf::Vector2f tangent(-direction.y, direction.x); // Перпендикулярный вектор

    return tangent * speed;
}

Matter::Matter(float radius, float mass, sf::Vector2f position, sf::Vector2f velocity)
    : shape(radius), velocity(velocity), mass(mass), isColonized(false) {
    shape.setOrigin(radius, radius);
    shape.setPosition(position);
    shape.setFillColor(sf::Color::White); // Цвет по умолчанию для неколонизированных звёзд
}

void Matter::applyGravity(const sf::Vector2f& blackHolePosition, float blackHoleMass, float deltaTime) {
    sf::Vector2f direction = blackHolePosition - shape.getPosition();
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    direction /= distance; // Нормализуем направление

    // Вычисляем силу притяжения
    float G = 1.0f; // Гравитационная постоянная
    float force = (G * mass * blackHoleMass) / (distance * distance);

    // Вычисляем ускорение
    sf::Vector2f acceleration = direction * (force / mass);

    // Обновляем скорость
    velocity += acceleration * deltaTime;
}

bool Matter::checkCollision(const Matter& other) const {
    float dx = shape.getPosition().x - other.shape.getPosition().x;
    float dy = shape.getPosition().y - other.shape.getPosition().y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < (shape.getRadius() + other.shape.getRadius());
}

void Matter::resolveCollision(Matter& other) {

    // Нормаль между объектами
    sf::Vector2f normal = other.shape.getPosition() - shape.getPosition();
    float distance = std::sqrt(normal.x * normal.x + normal.y * normal.y);
    normal /= distance;

    // Относительная скорость
    sf::Vector2f relativeVelocity = other.velocity - velocity;

    // Скорость вдоль нормали
    float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

    if (velocityAlongNormal > 0)
        return;

    float restitution = 1.0f; // 1.0f для полностью упругого столкновения

    float impulseScalar = -(1 + restitution) * velocityAlongNormal;
    impulseScalar /= (1 / mass + 1 / other.mass);

    sf::Vector2f impulse = impulseScalar * normal;

    velocity -= (impulse / mass);
    other.velocity += (impulse / other.mass);
}
