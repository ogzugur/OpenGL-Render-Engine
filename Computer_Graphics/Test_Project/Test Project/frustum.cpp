#include "frustum.h"

Frustum::Frustum(const glm::mat4 & m)
{
	p[FRUSTUM_RIGHT].n.x = m[0][3] - m[0][0];
	p[FRUSTUM_RIGHT].n.y = m[1][3] - m[1][0];
	p[FRUSTUM_RIGHT].n.z = m[2][3] - m[2][0];
	p[FRUSTUM_RIGHT].d = m[3][3] - m[3][0];

	p[FRUSTUM_LEFT].n.x = m[0][3] + m[0][0];
	p[FRUSTUM_LEFT].n.y = m[1][3] + m[1][0];
	p[FRUSTUM_LEFT].n.z = m[2][3] + m[2][0];
	p[FRUSTUM_LEFT].d = m[3][3] + m[3][0];

	p[FRUSTUM_DOWN].n.x = m[0][3] + m[0][1];
	p[FRUSTUM_DOWN].n.y = m[1][3] + m[1][1];
	p[FRUSTUM_DOWN].n.z = m[2][3] + m[2][1];
	p[FRUSTUM_DOWN].d = m[3][3] + m[3][1];

	p[FRUSTUM_UP].n.x = m[0][3] - m[0][1];
	p[FRUSTUM_UP].n.y = m[1][3] - m[1][1];
	p[FRUSTUM_UP].n.z = m[2][3] - m[2][1];
	p[FRUSTUM_UP].d = m[3][3] - m[3][1];

	p[FRUSTUM_FAR].n.x = m[0][3] - m[0][2];
	p[FRUSTUM_FAR].n.y = m[1][3] - m[1][2];
	p[FRUSTUM_FAR].n.z = m[2][3] - m[2][2];
	p[FRUSTUM_FAR].d = m[3][3] - m[3][2];

	p[FRUSTUM_NEAR].n.x = m[0][3] + m[0][2];
	p[FRUSTUM_NEAR].n.y = m[1][3] + m[1][2];
	p[FRUSTUM_NEAR].n.z = m[2][3] + m[2][2];
	p[FRUSTUM_NEAR].d = m[3][3] + m[3][2];

	// Normalize all plane normals
	for (int i = 0; i < 6; i++)
		p[i].Normalize();
}

bool Frustum::SphereIntersection(const glm::vec3 & vecCenter, float flRadius)
{
	// If none of the planes had the entity lying on its "negative" side then it must be
	// on the "positive" side for all of them. Thus the entity is inside or touching the frustum.
	return true;
}

bool Frustum::boxIntersection(Bounding_Box &b) {

	bool result = true;
	int in,out;

	// for each plane do ...
	for (int i = 0; i < 6; i++) {

		// reset counters for corners in and out
		out = 0; in = 0;
		// for each corner of the box do ...
		// get out of the cycle as soon as a box as corners
		// both inside and out of the frustum
		for (int k = 0; k < 8 && (in == 0 || out == 0); k++) {

			// is the corner outside or inside
			if (glm::dot(p[i].n, b.vertices[k]) + p[i].d <= 0)
				out++;
			else
				in++;
		}
		//if all corners are out
		if (!in)
			return (false);
		// if some corners are out and others are in
		else if (out)
			result = true;
	}
	return(result);
}