#include <SFML/Graphics.hpp>
#include "types.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

S32 main()
{
    F64* image = new F64[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    memset(image, 0.0, sizeof(F64) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray Tracer");

    sf::Image sfImage;
    sfImage.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Texture texture;
    texture.loadFromImage(sfImage);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    while(window.isOpen())
    {


        texture.loadFromImage(sfImage);
        sprite.setTexture(texture);

        sf::Event sfEvent;
        while(window.pollEvent(sfEvent))
        {
            if(sfEvent.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
