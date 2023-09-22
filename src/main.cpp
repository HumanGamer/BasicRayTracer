#include <SFML/Graphics.hpp>
#include "types.h"
#include "RTLib.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

const glm::vec3 eye = glm::vec3(0.f, 0.f, 5.f);

S32 main()
{
    std::vector<Object*> objects;

    Sphere sphere;
    sphere.center = glm::vec3(0.0f, -0.7f, -0.5f);
    sphere.radius = 0.3f;
    sphere.color = glm::vec3(2.f);
    objects.push_back(&sphere);

    Triangle floor1 = Triangle();
    floor1.v1 = glm::vec3(1.f, -1.f, 1.f);
    floor1.v2 = glm::vec3(1.f, -1.f, -1.f);
    floor1.v3 = glm::vec3(-1.f, -1.f, -1.f);
    floor1.color = glm::vec3(1.f);
    objects.push_back(&floor1);

    Triangle floor2 = Triangle();
    floor2.v1 = glm::vec3(1.f, -1.f, 1.f);
    floor2.v2 = glm::vec3(-1.f, -1.f, -1.f);
    floor2.v3 = glm::vec3(-1.f, -1.f, 1.f);
    floor2.color = glm::vec3(1.f);
    objects.push_back(&floor2);

    Triangle light1 = Triangle();
    light1.v1 = glm::vec3(0.5f, 0.99f, 0.5f);
    light1.v2 = glm::vec3(-0.5f, 0.99f, -0.5f);
    light1.v3 = glm::vec3(-0.5f, 0.99f, 0.5f);
    light1.color = glm::vec3(1.f);
    objects.push_back(&light1);

    Triangle light2 = Triangle();
    light2.v1 = glm::vec3(0.5f, 0.99f, 0.5f);
    light2.v2 = glm::vec3(0.5f, 0.99f, -0.5f);
    light2.v3 = glm::vec3(-0.5f, 0.99f, -0.5f);
    light2.color = glm::vec3(1.f);
    objects.push_back(&light2);

    F64* image = new F64[WINDOW_WIDTH * WINDOW_HEIGHT * 3];
    memset(image, 0.0, sizeof(F64) * WINDOW_WIDTH * WINDOW_HEIGHT * 3);
    F64* p = image;

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray Tracer");

    sf::Image sfImage;
    sfImage.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Texture texture;
    texture.loadFromImage(sfImage);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    // Declaring variables here to prevent numerous re-declarations within while loop
    HitInfo hi, finalHi;
    Ray ray;
    F32 i = 0.f, j = 0.f, minT = 10000000.f;
    S32 r = 0, g = 0, b = 0;
    glm::vec3 color = glm::vec3(0.f);

    while(window.isOpen())
    {
        p = image;
        for (S32 y = 0; y < WINDOW_HEIGHT; y++)
        {
            for (S32 x = 0; x < WINDOW_WIDTH; x++)
            {
                minT = 10000000.f;
                finalHi.hit = false;
                hi.hit = false;
                color = glm::vec3(0.f);

                i = (2.f * F32(x) / F32(WINDOW_WIDTH)) - 1.f;
                j = (2.f * F32(WINDOW_HEIGHT - y) / F32(WINDOW_HEIGHT)) - 1.f;

                ray.origin = glm::vec3(i, j, 1.2f);
                ray.direction = glm::normalize(ray.origin - eye);

                for (S32 k = 0; k < objects.size(); k++)
                {
                    hi = objects[k]->hit(ray);
                    if (hi.hit && hi.t < minT)
                    {
                        finalHi = hi;
                        minT = hi.t;
                    }
                }

                if (finalHi.hit)
                    color = finalHi.color;

                *p += color.r;
                p++;
                *p += color.g;
                p++;
                *p += color.b;
                p++;
            }
        }

        p = image;

        for (S32 yy = 0; yy < WINDOW_HEIGHT; yy++)
        {
            for (S32 xx = 0; xx < WINDOW_WIDTH; xx++)
            {
                r = glm::clamp(pow(*p++, 1.0f / 2.2f) * 255, 0.0, 255.0);
                g = glm::clamp(pow(*p++, 1.0f / 2.2f) * 255, 0.0, 255.0);
                b = glm::clamp(pow(*p++, 1.0f / 2.2f) * 255, 0.0, 255.0);

                sfImage.setPixel(xx, yy, sf::Color(r, g, b));
            }
        }

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
