#pragma once

#include "plane.h"
#include "bounding_box.h"
#include<glm/glm.hpp>

#define FRUSTUM_NEAR  0
#define FRUSTUM_FAR   1
#define FRUSTUM_LEFT  2
#define FRUSTUM_RIGHT 3
#define FRUSTUM_UP    4
#define FRUSTUM_DOWN  5

// Frustum class defined by six planes enclosing the frustum. The normals face inward.
class Frustum
{
public:
	Frustum() {};
	Frustum(const glm::mat4& m);

public:
	bool SphereIntersection(const glm::vec3& vecCenter, float flRadius);

	bool boxIntersection(Bounding_Box & b);

public:
	Plane p[6];
};