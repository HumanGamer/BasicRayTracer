#include "RTLib.h"

F32 random()
{
    return (F32)rand() / (F32)RAND_MAX;
}

glm::vec3 RandomVec3(F32 min, F32 max)
{
    return glm::vec3(
        random() * (max - min) + min,
        random() * (max - min) + min,
        random() * (max - min) + min
    );
}

glm::vec3 RandomInUnitSphere()
{
    return glm::normalize(RandomVec3(-1.0f, 1.0f));
}