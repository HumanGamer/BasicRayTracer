#pragma once

#include <glm.hpp>
#include "types.h"

class HitInfo
{
public:
    F32 t;
    //bool hit;
    glm::vec3 hitLocation;
    glm::vec3 normal;
    glm::vec3 color;
    bool emissive;

    HitInfo() : t(-1.0f), hitLocation(0.f), normal(0.f), color(0.f), emissive(false) {}
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
    S32 type;

    Object(S32 type) : type(type), color(0.f), emissive(false) {}
};

class Triangle : public Object
{
public:
    glm::vec3 v1, v2, v3;

    Triangle() : Object(1), v1(0.f), v2(0.f), v3(0.f) {}
};

class Sphere : public Object
{
public:
    glm::vec3 center;
    F32 radius;

    Sphere() : Object(2), radius(0.f), center(0.f) {}
};

extern F32 random();

extern glm::vec3 RandomVec3(F32 min, F32 max);
extern glm::vec3 RandomInUnitSphere();
