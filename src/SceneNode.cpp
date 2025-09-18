// Copyright (c) Eric Jeker. All Rights Reserved.

#include "SceneNode.h"


SceneNode::SceneNode(std::string name) : name_(std::move(name)) {};

void SceneNode::init() {}

[[nodiscard]] const std::string& SceneNode::getName() const { return name_; }

void SceneNode::setName(std::string name) { name_ = std::move(name); }

[[nodiscard]] EntityID SceneNode::getId() const { return id_; }

void SceneNode::setId(const EntityID id) { id_ = id; }

[[nodiscard]] Scene* SceneNode::getScene() const { return scene_; }

void SceneNode::setScene(Scene* scene) { scene_ = scene; }

SceneNode& SceneNode::addChild(std::unique_ptr<SceneNode> child)
{
    child->setParent(this);
    children_.emplace_back(std::move(child));
    return *children_.back();
}

void SceneNode::removeChild(const SceneNode* child)
{
    for (auto it = children_.begin(); it != children_.end(); ++it)
    {
        if (it->get() == child)
        {
            (*it)->resetParent();
            children_.erase(it);
            break;
        }
    }
}

[[nodiscard]] SceneNode& SceneNode::getParent() const { return *parent_; }

void SceneNode::resetParent() { parent_ = nullptr; }

void SceneNode::setParent(SceneNode* parent) { parent_ = parent; }

sf::Transformable& SceneNode::getTransform() { return transform_; }

void SceneNode::addComponent(std::unique_ptr<Component> component) { components_.emplace_back(std::move(component)); }

void SceneNode::update(const float dt)
{
    for (const auto& component : components_)
    {
        component->update(dt);
    }

    for (const auto& child : children_)
    {
        child->update(dt);
    }
}

void SceneNode::draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const
{
    const sf::Transform combinedTransform = parentTransform * transform_.getTransform();

    for (const auto& component : components_)
    {
        component->draw(window, combinedTransform);
    }

    for (const auto& child : children_)
    {
        child->draw(window, combinedTransform);
    }
}
