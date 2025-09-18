// Copyright (c) Eric Jeker. All Rights Reserved.

#pragma once
#include <string>

#include "Component.h"
#include "Scene.h"

#include <SFML/Graphics/Transformable.hpp>


class SceneNode
{

public:
    explicit SceneNode(std::string name);
    ~SceneNode() = default;

    virtual void init();

    // Name Management
    [[nodiscard]] const std::string& getName() const;
    void setName(std::string name);

    // ID and Parent Scene
    [[nodiscard]] EntityID getId() const;
    void setId(EntityID id);

    // Set Scene should be called by createNode in Scene
    [[nodiscard]] Scene* getScene() const;
    void setScene(Scene* scene);

    // Children
    // Add child should be idempotent so we never had twice the same child in the children vector
    SceneNode& addChild(std::unique_ptr<SceneNode> child);
    void removeChild(const SceneNode* child);

    // Parent
    [[nodiscard]] SceneNode& getParent() const;
    void resetParent();
    // There is a risk that we set the parent but don't put the node in the parent's children
    void setParent(SceneNode* parent);

    // Access transform for external manipulation
    sf::Transformable& getTransform();

    void addComponent(std::unique_ptr<Component> component);

    // Game Loop
    virtual void update(float dt);

    void draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const;

private:
    EntityID id_{0};
    std::string name_;

    // There is no risk of dangling pointers here because scene and parent own this node
    Scene* scene_{nullptr};
    SceneNode* parent_{nullptr};

    std::vector<std::unique_ptr<SceneNode>> children_{};

    // Each node has its own local transform
    sf::Transformable transform_;

    std::vector<std::unique_ptr<Component>> components_{};
};
