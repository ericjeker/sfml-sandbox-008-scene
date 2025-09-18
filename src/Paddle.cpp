// Copyright (c) Eric Jeker. All Rights Reserved.

#include "Paddle.h"
#include "RectangleShapeComponent.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Angle.hpp>

Paddle::Paddle() : SceneNode("paddle") {}

void Paddle::init()
{
    auto rectComponent = std::make_unique<RectangleShapeComponent>(std::make_unique<sf::RectangleShape>());

    // Create a unique pointer that cannot be shared
    const auto shape = rectComponent->getShape();
    shape->setSize({50.f, 50.f});
    shape->setFillColor(sf::Color::Red);

    addComponent(std::move(rectComponent));
}

void Paddle::update(const float dt)
{
    auto transform = this->getTransform();
    transform.setRotation(sf::degrees(transform.getRotation().asDegrees() + 10.f * dt));
    this->SceneNode::update(dt);
}
