#pragma once

#include <glm.hpp>
#include "types.h"

//using namespace glm;

#define EPSILON 0.00001f

class HitInfo
{
public:
    F32 t;
    bool hit;
    glm::vec3 hitLocation;
    glm::vec3 normal;
    glm::vec3 color;
    bool emissive;

    HitInfo() : t(0.f), hit(false), hitLocation(0.f), normal(0.f), color(0.f), emissive(false) {}
};

class Ray
{
public:
    glm::vec3 origin;
    glm::vec3 direction;

    Ray() : origin(0.f), direction(0.f) {}
};

class Object
{
public:
    glm::vec3 color;
    bool emissive;

    Object() : color(0.f), emissive(false) {}

    virtual HitInfo hit(Ray &ray) const = 0;
};

class Triangle : public Object
{
public:
    glm::vec3 v1, v2, v3;

    Triangle() : v1(0.f), v2(0.f), v3(0.f) {}

    HitInfo hit(Ray &ray) const override;
};

class Sphere : public Object
{
public:
    glm::vec3 center;
    F32 radius;

    Sphere() : radius(0.f), center(0.f) {}

    HitInfo hit(Ray &ray) const override;
};

extern F32 random();
extern glm::vec3 randomVector(); // Get random vector in hemisphere.
extern glm::vec3 randomDirection(glm::vec3 normal);
