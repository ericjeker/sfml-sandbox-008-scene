#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

#include "Component.h"

class RectangleShapeComponent final : public Component
{
public:
    explicit RectangleShapeComponent(std::unique_ptr<sf::RectangleShape> shape);

    [[nodiscard]] sf::RectangleShape* getShape() const;
    void setShape(std::unique_ptr<sf::RectangleShape> shape);

    void draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const override;

private:
    std::unique_ptr<sf::RectangleShape> shape_;
};
