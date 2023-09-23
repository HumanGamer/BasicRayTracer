#include <SFML/Graphics.hpp>
#include <execution>
#include "types.h"
#include "RTLib.h"

//#define RT_SKY
#define RT_ACCUMULATE

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

const S32 lightBounces = 10;
const glm::vec3 eye = glm::vec3(0.f, 0.f, 5.f);

S32 frameIndex = 1;

#define EPSILON 0.00001f

HitInfo TraceRay(const Ray& ray, std::vector<Object*> &objects)
{
    HitInfo hitInfo;
    S32 closestObject = -1;
    F32 hitDistance = std::numeric_limits<F32>::max();
    for (size_t i = 0; i < objects.size(); i++)
    {
        Object &obj = *objects[i];

        if (obj.type == 1)
        {
            // Triangle
            auto &triangle = static_cast<Triangle&>(obj);

            glm::vec3 originalNormal = glm::normalize(glm::cross(triangle.v2 - triangle.v1, triangle.v3 - triangle.v1));
            glm::vec3 n = originalNormal;

            if (glm::dot(n, ray.direction) > 0.f)
                n = -n;

            // Case 1 - Hit nothing
            if (fabs(glm::dot(n, ray.direction)) < EPSILON)
                continue;

            // Case 2 - Hit nothing
            F32 t = (glm::dot(n, triangle.v1) - glm::dot(ray.origin, n)) / glm::dot(ray.direction, n);
            if (fabs(t) < 0.0005f)
                continue;

            glm::vec3 p = ray.origin + ray.direction * t;

            glm::vec3 c1 = glm::cross(triangle.v2 - triangle.v1, p - triangle.v1);
            glm::vec3 c2 = glm::cross(triangle.v3 - triangle.v2, p - triangle.v2);
            glm::vec3 c3 = glm::cross(triangle.v1 - triangle.v3, p - triangle.v3);

            // Case 3 - Not inside the triangle
            if (glm::dot(c1, originalNormal) < 0.f || glm::dot(c2, originalNormal) < 0.f || glm::dot(c3, originalNormal) < 0.f)
                continue;

            // We have hit something inside the triangle
            if (t > 0.0f && t < hitDistance)
            {
                hitInfo.t = t;
                hitInfo.hitLocation = p;
                hitInfo.normal = n;
                hitInfo.color = triangle.color;
                hitInfo.emissive = triangle.emissive;

                hitDistance = t;
                closestObject = (S32) i;
            }
        }
        else if (obj.type == 2)
        {
            // Sphere
            const auto& sphere = static_cast<const Sphere&>(obj);

            glm::vec3 origin = ray.origin - sphere.center;

            F32 a = glm::dot(ray.direction, ray.direction);
            F32 b = 2.0f * glm::dot(origin, ray.direction);
            F32 c = glm::dot(origin, origin) - sphere.radius * sphere.radius;

            F32 discriminant = b * b - 4.0f * a * c;
            if (discriminant < 0.0f)
                continue;

            F32 closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
            if (closestT > 0.0f && closestT < hitDistance)
            {
                hitDistance = closestT;
                closestObject = (S32)i;

                hitInfo.color = sphere.color;
                hitInfo.emissive = sphere.emissive;
                hitInfo.t = hitDistance;
                hitInfo.hitLocation = origin + ray.direction * hitDistance;
                hitInfo.normal = glm::normalize(hitInfo.hitLocation);

                hitInfo.hitLocation += sphere.center;
            }
        }
    }

    if (closestObject < 0)
    {
        hitInfo.t = -1.0f;
        return hitInfo;
    }

    return hitInfo;
}

glm::vec3 renderPixel(std::vector<Object*> &objects/*, F64* &p*/, S32 x, S32 y)
{
    Ray ray;
    glm::vec3 light(0.0f);
    glm::vec3 contribution(1.0f);

    F32 i = (2.f * F32(x) / F32(WINDOW_WIDTH)) - 1.f;
    F32 j = (2.f * F32(WINDOW_HEIGHT - y) / F32(WINDOW_HEIGHT)) - 1.f;

    ray.origin = glm::vec3(i, j, 1.2f);
    ray.direction = glm::normalize(ray.origin - eye);

    for (S32 k = 0; k < lightBounces; k++)
    {
        HitInfo finalHi = TraceRay(ray, objects);

        if (finalHi.t < 0.0f)
        {
#ifdef RT_SKY
            glm::vec3 skyColor = glm::vec3(0.6f, 0.7f, 0.9f);
            light += contribution * skyColor;
#else
            light += contribution;
#endif
            break;
        }
        else
        {
                ray.origin = finalHi.hitLocation + finalHi.normal * 0.0001f;
                ray.direction = glm::normalize(finalHi.normal + glm::normalize(RandomInUnitSphere()));

                contribution *= finalHi.color;
                if (finalHi.emissive)
                    light += finalHi.color;
        }
    }

    return light;
}

S32 main()
{
    std::vector<Object *> objects;

    Sphere sphere;
    sphere.center = glm::vec3(0.0f, -0.7f, -0.5f);
    sphere.radius = 0.3f;
    sphere.color = glm::vec3(1.f);
    objects.push_back(&sphere);

    Sphere sphere2;
    sphere2.center = glm::vec3(1.0f, -0.7f, -0.5f);
    sphere2.radius = 0.3f;
    sphere2.color = glm::vec3(2.f, 0.f, 0.f);
    sphere2.emissive = true;
    //objects.push_back(&sphere2);

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

    auto *image = new glm::vec3[WINDOW_WIDTH * WINDOW_HEIGHT];
    memset(image, 0.0, sizeof(glm::vec3) * WINDOW_WIDTH * WINDOW_HEIGHT);

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Ray Tracer");

    sf::Image sfImage;
    sfImage.create(WINDOW_WIDTH, WINDOW_HEIGHT);

    sf::Texture texture;
    texture.loadFromImage(sfImage);

    sf::Sprite sprite;
    sprite.setTexture(texture);

    std::vector<S32> imageVertIter, imageHorizIter;
    imageVertIter.resize(WINDOW_HEIGHT);
    imageHorizIter.resize(WINDOW_WIDTH);
    for (S32 i = 0; i < WINDOW_HEIGHT; i++)
        imageVertIter[i] = i;
    for (S32 i = 0; i < WINDOW_WIDTH; i++)
        imageHorizIter[i] = i;

    while (window.isOpen())
    {
        if (frameIndex == 1)
        {
            memset(image, 0.0, sizeof(glm::vec3) * WINDOW_WIDTH * WINDOW_HEIGHT);
        }

        std::for_each(std::execution::par, imageVertIter.begin(), imageVertIter.end(), [&](S32 y)
        {
            std::for_each(std::execution::par, imageHorizIter.begin(), imageHorizIter.end(), [&](S32 x)
            {
                glm::vec3 color = renderPixel(objects, x, y);
                image[x + y * WINDOW_WIDTH] += color;

                glm::vec3 accumColor = image[x + y * WINDOW_WIDTH];
                accumColor /= (F32) frameIndex;

                accumColor = glm::clamp(accumColor, glm::vec3(0.f), glm::vec3(1.f));

                sfImage.setPixel(x, y, sf::Color(accumColor.r * 255.0, accumColor.g * 255.0, accumColor.b * 255.0));
            });
        });

        texture.loadFromImage(sfImage);
        sprite.setTexture(texture);

#ifdef RT_ACCUMULATE
        frameIndex++;
#else
      frameIndex = 1;
#endif

        sf::Event sfEvent;
        while (window.pollEvent(sfEvent))
        {
            if (sfEvent.type == sf::Event::Closed)
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
