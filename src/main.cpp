#include <SFML/Graphics.hpp>

#include "Paddle.h"
#include "Scene.h"


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
