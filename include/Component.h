#pragma once

#include <SFML/Graphics.hpp>

class Component
{
public:
    virtual ~Component() = default;

    virtual void update(float dt);

    virtual void draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const = 0;
};
