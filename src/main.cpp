#include <SFML/Graphics.hpp>
#include <execution>
#include "types.h"
#include "RTLib.h"

#define RT_MultiThread

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

const S32 numSamples = 100;
const S32 maxDepth = 10;
const glm::vec3 eye = glm::vec3(0.f, 0.f, 5.f);

const F32 brightness = (2.f * 3.1415926f) * (1.f / F32(numSamples));

glm::vec3 pathTrace(Ray &ray, std::vector<Object*> &objects, S32 depth)
{
    if (depth > maxDepth)
        return glm::vec3(0.f);

    F64 minT = 10000000.f;
    HitInfo hi, finalHi;

    for (auto & object : objects)
    {
        hi = object->hit(ray);

        if (hi.hit && hi.t < minT)
        {
            minT = hi.t;
            finalHi = hi;
        }
    }

    if (!finalHi.hit)
        return glm::vec3(0.f);

    if (finalHi.emissive)
        return finalHi.color;

    // Indirect Lighting
    F32 p = 0.8f;
    F32 randomNumber = random();
    if (randomNumber > p)
        return glm::vec3(0.f);

    Ray randomRay;
    randomRay.origin = finalHi.hitLocation;
    randomRay.direction = randomDirection(finalHi.normal);

    F32 cosine = fabs(glm::dot(-ray.direction, finalHi.normal));
    glm::vec3 originalColor = finalHi.color;
    glm::vec3 newColor = pathTrace(randomRay, objects, depth + 1) * cosine;
    glm::vec3 color = newColor * originalColor;

    return color / p;

    //return glm::vec3(0.f);
}

void render(std::vector<Object*> &objects, F64* &p, S32 x, S32 y)
{
    HitInfo hi, finalHi;
    Ray ray;
    F32 i = 0.f, j = 0.f, minT = 10000000.f;
    glm::vec3 color = glm::vec3(0.f);
    minT = 10000000.f;
    finalHi.hit = false;
    hi.hit = false;
    color = glm::vec3(0.f);

    i = (2.f * F32(x) / F32(WINDOW_WIDTH)) - 1.f;
    j = (2.f * F32(WINDOW_HEIGHT - y) / F32(WINDOW_HEIGHT)) - 1.f;

    ray.origin = glm::vec3(i, j, 1.2f);
    ray.direction = glm::normalize(ray.origin - eye);

    for (auto object : objects)
    {
        hi = object->hit(ray);
        if (hi.hit && hi.t < minT)
        {
            finalHi = hi;
            minT = hi.t;
        }
    }

    if (finalHi.hit)
    {
        //color = finalHi.color;
        if (finalHi.emissive)
            color = finalHi.color;
        else
        {
            Ray randomRay;
            randomRay.origin = finalHi.hitLocation;
            randomRay.direction = randomDirection(finalHi.normal);

            glm::vec3 originalColor = finalHi.color;
            glm::vec3 newColor = pathTrace(randomRay, objects, 0);

            color = newColor * originalColor;

            color *= brightness;
        }
    }

    U32 index = (x + y * WINDOW_WIDTH) * 3;

    p[index + 0] += color.r;
    p[index + 1] += color.g;
    p[index + 2] += color.b;

//    *p += color.r;
//    p++;
//    *p += color.g;
//    p++;
//    *p += color.b;
//    p++;
}

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
    light1.emissive = true;
    objects.push_back(&light1);

    Triangle light2 = Triangle();
    light2.v1 = glm::vec3(0.5f, 0.99f, 0.5f);
    light2.v2 = glm::vec3(0.5f, 0.99f, -0.5f);
    light2.v3 = glm::vec3(-0.5f, 0.99f, -0.5f);
    light2.color = glm::vec3(1.f);
    light2.emissive = true;
    objects.push_back(&light2);

    Triangle ceiling1 = Triangle();
    ceiling1.v1 = glm::vec3(1.f, 1.f, 1.f);
    ceiling1.v2 = glm::vec3(-1.f, 1.f, -1.f);
    ceiling1.v3 = glm::vec3(-1.f, 1.f, 1.f);
    ceiling1.color = glm::vec3(1.f);
    objects.push_back(&ceiling1);

    Triangle ceiling2 = Triangle();
    ceiling2.v1 = glm::vec3(1.f, 1.f, 1.f);
    ceiling2.v2 = glm::vec3(1.f, 1.f, -1.f);
    ceiling2.v3 = glm::vec3(-1.f, 1.f, -1.f);
    ceiling2.color = glm::vec3(1.f);
    objects.push_back(&ceiling2);

    Triangle left1 = Triangle();
    left1.v1 = glm::vec3(-1.f, -1.f, -1.f);
    left1.v2 = glm::vec3(-1.f, 1.f, 1.f);
    left1.v3 = glm::vec3(-1.f, -1.f, 1.f);
    left1.color = glm::vec3(1.f, 0.f, 0.f);
    objects.push_back(&left1);

    Triangle left2 = Triangle();
    left2.v1 = glm::vec3(-1.f, -1.f, -1.f);
    left2.v2 = glm::vec3(-1.f, 1.f, -1.f);
    left2.v3 = glm::vec3(-1.f, 1.f, 1.f);
    left2.color = glm::vec3(1.f, 0.f, 0.f);
    objects.push_back(&left2);

    Triangle right1 = Triangle();
    right1.v1 = glm::vec3(1.f, 1.f, 1.f);
    right1.v2 = glm::vec3(1.f, -1.f, -1.f);
    right1.v3 = glm::vec3(1.f, -1.f, 1.f);
    right1.color = glm::vec3(0.f, 1.f, 0.f);
    objects.push_back(&right1);

    Triangle right2 = Triangle();
    right2.v1 = glm::vec3(1.f, -1.f, -1.f);
    right2.v2 = glm::vec3(1.f, 1.f, 1.f);
    right2.v3 = glm::vec3(1.f, 1.f, -1.f);
    right2.color = glm::vec3(0.f, 1.f, 0.f);
    objects.push_back(&right2);

    Triangle back1 = Triangle();
    back1.v1 = glm::vec3(1.f, -1.f, -1.f);
    back1.v2 = glm::vec3(-1.f, 1.f, -1.f);
    back1.v3 = glm::vec3(-1.f, -1.f, -1.f);
    back1.color = glm::vec3(1.f);
    objects.push_back(&back1);

    Triangle back2 = Triangle();
    back2.v1 = glm::vec3(1.f, -1.f, -1.f);
    back2.v2 = glm::vec3(1.f, 1.f, -1.f);
    back2.v3 = glm::vec3(-1.f, 1.f, -1.f);
    back2.color = glm::vec3(1.f);
    objects.push_back(&back2);

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
    S32 s = 0;
    //S32 r = 0, g = 0, b = 0;

    std::vector<S32> imageVertIter, imageHorizIter;
    imageVertIter.resize(WINDOW_HEIGHT);
    imageHorizIter.resize(WINDOW_WIDTH);
    for (S32 i = 0; i < WINDOW_HEIGHT; i++)
        imageVertIter[i] = i;
    for (S32 i = 0; i < WINDOW_WIDTH; i++)
        imageHorizIter[i] = i;

    while(window.isOpen())
    {
        if (s < numSamples)
        {
            p = image;

#ifdef RT_MultiThread
            std::for_each(std::execution::par, imageVertIter.begin(), imageVertIter.end(),
              [&](S32 y)
              {
                  std::for_each(std::execution::par, imageHorizIter.begin(), imageHorizIter.end(),
                    [&](S32 x)
                    {
                        render(objects, p, x, y);
                    });
              });

            std::for_each(std::execution::par, imageVertIter.begin(), imageVertIter.end(),
              [&](S32 yy)
              {
                  std::for_each(std::execution::par, imageHorizIter.begin(), imageHorizIter.end(),
                    [&](S32 xx)
                    {
                        U32 index = (xx + yy * WINDOW_WIDTH) * 3;

                        U32 r = glm::clamp(pow(p[index + 0], 1.0f / 2.2f) * 255, 0.0, 255.0);
                        U32 g = glm::clamp(pow(p[index + 1], 1.0f / 2.2f) * 255, 0.0, 255.0);
                        U32 b = glm::clamp(pow(p[index + 2], 1.0f / 2.2f) * 255, 0.0, 255.0);

                        sfImage.setPixel(xx, yy, sf::Color(r, g, b));
                    });
              });
#else
            for (S32 y = 0; y < WINDOW_HEIGHT; y++)
            {
                for (S32 x = 0; x < WINDOW_WIDTH; x++)
                {
                    render(rtStruct, objects, p, x, y);
                }
            }

            for (S32 yy = 0; yy < WINDOW_HEIGHT; yy++)
            {
                for (S32 xx = 0; xx < WINDOW_WIDTH; xx++)
                {

                    U32 index = (xx + yy * WINDOW_WIDTH) * 3;

                    U32 r = glm::clamp(pow(p[index + 0], 1.0f / 2.2f) * 255, 0.0, 255.0);
                    U32 g = glm::clamp(pow(p[index + 1], 1.0f / 2.2f) * 255, 0.0, 255.0);
                    U32 b = glm::clamp(pow(p[index + 2], 1.0f / 2.2f) * 255, 0.0, 255.0);

                    sfImage.setPixel(xx, yy, sf::Color(r, g, b));
                }
            }
#endif

            texture.loadFromImage(sfImage);
            sprite.setTexture(texture);

            s++;

            printf("Sample %d/%d\n", s, numSamples);
        } else
        {
            static bool printed = false;
            if (!printed)
            {
                printf("Done!\n");
                printed = true;
            }
        }

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
