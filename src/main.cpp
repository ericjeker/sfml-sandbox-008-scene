#include <SFML/Graphics.hpp>

// Simple EntityID type
using EntityID = std::uint64_t;
using DrawablePtr = std::unique_ptr<sf::Drawable>;

class Scene;


class Component
{
public:
    virtual ~Component() = default;

    // Optional: update per-frame
    virtual void update(float dt) {}

    // Draw this component with the given parent transform
    virtual void draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const = 0;
};

class RectangleShapeComponent final : public Component
{
public:
    explicit RectangleShapeComponent(std::unique_ptr<sf::RectangleShape> shape) : shape_(std::move(shape)) {}
    ~RectangleShapeComponent() override = default;

    [[nodiscard]] sf::RectangleShape* getShape() const { return shape_.get(); }
    void setShape(std::unique_ptr<sf::RectangleShape> shape) { shape_ = std::move(shape); }

    void draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const override
    {
        if (!shape_)
        {
            return;
        }

        sf::RenderStates states;
        states.transform = parentTransform * shape_->getTransform();
        window.draw(*shape_, states);
    }

private:
    std::unique_ptr<sf::RectangleShape> shape_;
};


class SceneNode
{
public:
    explicit SceneNode(std::string name) : name_(std::move(name)) {};
    virtual ~SceneNode() = default;

    virtual void init() {};

    // Name Management
    [[nodiscard]] const std::string& getName() const { return name_; }
    void setName(std::string name) { name_ = std::move(name); }

    // ID and Parent Scene
    [[nodiscard]] EntityID getId() const { return id_; }
    void setId(const EntityID id) { id_ = id; }

    // Set Scene should be called by createNode in Scene
    [[nodiscard]] Scene* getScene() const { return scene_; }
    void setScene(Scene* scene) { scene_ = scene; }

    // Children
    // Add child should be idempotent so we never had twice the same child in the children vector
    SceneNode& addChild(std::unique_ptr<SceneNode> child)
    {
        child->setParent(this);
        children_.emplace_back(std::move(child));
        return *children_.back();
    }
    void removeChild(const SceneNode* child)
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

    // Parent
    [[nodiscard]] SceneNode& getParent() const { return *parent_; }
    void resetParent() { parent_ = nullptr; }
    // There is a risk that we set the parent but don't put the node in the parent's children
    void setParent(SceneNode* parent) { parent_ = parent; }

    // Access transform for external manipulation
    sf::Transformable& getTransform() { return transform_; }

    void addComponent(std::unique_ptr<Component> component) { components_.emplace_back(std::move(component)); }

    // Game Loop
    virtual void update(const float dt)
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

    void draw(sf::RenderWindow& window, const sf::Transform& parentTransform) const
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

class Scene
{
public:
    Scene() = default;
    ~Scene() = default;

    // Simply return the Scene root node
    [[nodiscard]] SceneNode& getRoot() const { return *root_; }

    // Node Management, we call them node; they are entities, why calling them nodes you'll ask me?
    [[nodiscard]] SceneNode& getNode(const EntityID id) const { return *registry_.at(id); }
    [[nodiscard]] SceneNode& addNode(std::unique_ptr<SceneNode> node, SceneNode* parent = nullptr)
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

    void removeNode(const EntityID id)
    {
        const auto node = registry_.find(id);
        if (node == registry_.end())
            return;
        node->second->getParent().removeChild(node->second);
        registry_.erase(id);
    }

    [[nodiscard]] sf::Transform getTransform() const { return transform_; }

    // The game loop calls will start updating and drawing from the root node
    void update(const float dt) { root_->update(dt); }
    void draw(sf::RenderWindow& window) const { root_->draw(window, getTransform()); }

private:
    // The scene owns the root node
    std::unique_ptr<SceneNode> root_{std::make_unique<SceneNode>("Root")};

    // Map of pointers to SceneNode, this creates a risk of dangling pointers if a node is destroyed but not removed
    // from the registry
    std::unordered_map<EntityID, SceneNode*> registry_{};

    // The next ID to ensure the registry use unique identifiers
    EntityID nextId_{1};

    sf::Transform transform_{};
};

class Paddle final : public SceneNode
{
public:
    Paddle() : SceneNode("paddle") {}

    void init() override
    {
        auto rectComponent = std::make_unique<RectangleShapeComponent>(std::make_unique<sf::RectangleShape>());

        // Create a unique pointer that cannot be shared
        const auto shape = rectComponent->getShape();
        shape->setSize({50.f, 50.f});
        shape->setFillColor(sf::Color::Red);

        addComponent(std::move(rectComponent));
    }

    void update(const float dt) override
    {
        auto transform = this->getTransform();
        transform.setRotation(sf::degrees(transform.getRotation().asDegrees() + 10.f * dt));
        this->SceneNode::update(dt);
    }
};

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({640u, 480u}), "CMake SFML Project");
    window.setFramerateLimit(144);

    Scene scene;

    auto& paddleNode = scene.addNode(std::make_unique<Paddle>());
    paddleNode.getTransform().setPosition({100.f, 100.f});

    // Game Loop
    sf::Clock clock;
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }
        }

        const auto dt = clock.restart().asSeconds();
        scene.update(dt);

        window.clear();
        scene.draw(window);
        window.display();
    }
}
