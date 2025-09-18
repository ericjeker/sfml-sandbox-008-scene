// Copyright (c) Eric Jeker. All Rights Reserved.

#include "Scene.h"
#include "SceneNode.h"

Scene::Scene()
{
    root_ = std::make_unique<SceneNode>("Root");
}

// Simply return the Scene root node
[[nodiscard]] SceneNode& Scene::getRoot() const { return *root_; }

// Node Management, we call them node; they are entities, why calling them nodes you'll ask me?
[[nodiscard]] SceneNode& Scene::getNode(const EntityID id) const { return *registry_.at(id); }
[[nodiscard]] SceneNode& Scene::addNode(std::unique_ptr<SceneNode> node, SceneNode* parent)
{
    node->setId(nextId_++);
    node->setScene(this);
    node->init();

    // If no parent is specified, we assume it's a child of the root node
    if (!parent)
        parent = root_.get();

    const auto raw = node.get();

    // Move ownership to the parent node
    parent->addChild(std::move(node));

    registry_[raw->getId()] = raw;
    return *raw;
}

void Scene::removeNode(const EntityID id)
{
    const auto node = registry_.find(id);
    if (node == registry_.end())
        return;
    node->second->getParent().removeChild(node->second);
    registry_.erase(id);
}

[[nodiscard]] sf::Transform Scene::getTransform() const { return transform_; }

// The game loop calls will start updating and drawing from the root node
void Scene::update(const float dt) { root_->update(dt); }
void Scene::draw(sf::RenderWindow& window) const { root_->draw(window, getTransform()); }
