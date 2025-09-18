// Copyright (c) Eric Jeker. All Rights Reserved.

#pragma once
#include <memory>
#include <unordered_map>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Transform.hpp>

// Simple EntityID type
using EntityID = std::uint64_t;

// Forward declaration to avoid including SceneNode.h here
class SceneNode;

class Scene
{
public:
    Scene();
    ~Scene() = default;

    // Simply return the Scene root node
    [[nodiscard]] SceneNode& getRoot() const;

    // Node Management, we call them node; they are entities, why calling them nodes you'll ask me?
    [[nodiscard]] SceneNode& getNode(EntityID id) const;
    [[nodiscard]] SceneNode& addNode(std::unique_ptr<SceneNode> node, SceneNode* parent = nullptr);

    void removeNode(EntityID id);

    [[nodiscard]] sf::Transform getTransform() const;

    // The game loop calls will start updating and drawing from the root node
    void update(float dt);
    void draw(sf::RenderWindow& window) const;

private:
    // The scene owns the root node
    std::unique_ptr<SceneNode> root_{};

    // Map of pointers to SceneNode, this creates a risk of dangling pointers if a node is destroyed but not removed
    // from the registry
    std::unordered_map<EntityID, SceneNode*> registry_{};

    // The next ID to ensure the registry use unique identifiers
    EntityID nextId_{1};

    sf::Transform transform_{};
};
