#include "RectangleShapeComponent.h"

RectangleShapeComponent::RectangleShapeComponent(std::unique_ptr<sf::RectangleShape> shape) : shape_(std::move(shape))
{
}

[[nodiscard]] sf::RectangleShape* RectangleShapeComponent::getShape() const { return shape_.get(); }
void RectangleShapeComponent::setShape(std::unique_ptr<sf::RectangleShape> shape) { shape_ = std::move(shape); }

void RectangleShapeComponent::draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const
{
    if (!shape_)
    {
        return;
    }

    sf::RenderStates states;
    states.transform = parentTransform * shape_->getTransform();
    window.draw(*shape_, states);
}
