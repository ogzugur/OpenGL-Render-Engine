#pragma once

#include <glm/glm.hpp>

// Plane class defined by a normal vector which should aways be unit length and a "distance" to the origin.
// Also can be thought of as ax + by + cz + d = 0, the equation of a plane in 3D space, where a b and c are the x, y, z of the normal n.
class Plane
{
public:
	void Normalize()
	{
		// It helps a ton if our planes are normalized, meaning n is unit length.
		// To normalize the plane, we do this operation:
		// s(ax + by + cz + d) = s(0)
		// We calculate s by using 1/|n|, and it gets us the number we must scale n by to make it unit length.
		// Notice how d needs to be scaled also.

		float flScale = 1 / glm::length(n);
		n.x *= flScale;
		n.y *= flScale;
		n.z *= flScale;
		d *= flScale;
	}
public:
	glm::vec3 n; // The normal
	float d;  // The "distance" to the origin.
};
