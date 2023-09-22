#include "RTLib.h"

HitInfo Triangle::hit(Ray ray) const
{
    HitInfo hi;

    glm::vec3 originalNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
    glm::vec3 n = originalNormal;

    if (glm::dot(n, ray.direction) > 0.f)
        n = -n;

    // Case 1 - Hit nothing
    if (fabs(glm::dot(n, ray.direction)) < EPSILON)
        return hi;

    // Case 2 - Hit nothing
    F32 t = (glm::dot(n, v1) - glm::dot(ray.origin, n)) / glm::dot(ray.direction, n);
    if (fabs(t) < 0.0005f)
        return hi;

    glm::vec3 p = ray.origin + ray.direction * t;

    glm::vec3 c1 = glm::cross(v2 - v1, p - v1);
    glm::vec3 c2 = glm::cross(v3 - v2, p - v2);
    glm::vec3 c3 = glm::cross(v1 - v3, p - v3);

    // Case 3 - Not inside the triangle
    if (glm::dot(c1, originalNormal) < 0.f || glm::dot(c2, originalNormal) < 0.f || glm::dot(c3, originalNormal) < 0.f)
        return hi;

    // We have hit something inside the triangle
    hi.hit = true;
    hi.t = t;
    hi.hitLocation = p;
    hi.normal = n;
    hi.color = color;
    hi.emissive = emissive;

    return hi;
}

HitInfo Sphere::hit(Ray ray) const
{
    HitInfo hi;
    F32 t;

    glm::vec3 temp = ray.origin - center;
    F32 a = glm::dot(ray.direction, ray.direction);
    F32 b = 2.f * glm::dot(temp, ray.direction);
    F32 c = glm::dot(temp, temp) - radius * radius;

    F32 discriminant = b * b - 4.f * a * c;

    if (discriminant > 0.f)
    {
        t = (-b - sqrt(discriminant)) / (2.f * a);
        if (t > EPSILON)
        {
            hi.hit = true;
            hi.t = t;
            hi.normal = glm::normalize(ray.origin + t * ray.direction) - center;
            hi.hitLocation = ray.origin + t * ray.direction;
            hi.color = color;
            hi.emissive = emissive;

            return hi;
        }

        t = (-b + sqrt(discriminant)) / (2.f * a);
        if (t > EPSILON)
        {
            hi.hit = true;
            hi.t = t;
            hi.normal = glm::normalize(ray.origin + t * ray.direction) - center;
            hi.hitLocation = ray.origin + t * ray.direction;
            hi.color = color;
            hi.emissive = emissive;

            return hi;
        }
    }

    return hi;
}

F32 random()
{
    return (F32)rand() / (F32)RAND_MAX;
}

// Get random vector in hemisphere.
glm::vec3 randomVector()
{
    glm::vec3 d;

    do
    {
        d = 2.f * glm::vec3(random(), random(), random()) - glm::vec3(1.f);
    } while (glm::dot(d, d) > 1.f);

    return glm::normalize(d);
}

glm::vec3 randomDirection(glm::vec3 normal)
{
    return glm::normalize(randomVector() + normal);
}