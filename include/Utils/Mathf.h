#ifndef MATHF_H
#define MATHF_H

#include <glm/glm.hpp>

namespace Mathf {
	bool Approximately(float a, float b);
	bool Approximately(glm::vec3 a, glm::vec3 b);
	bool Approximately(glm::vec4 a, glm::vec4 b);
	float RangeRandom(float a, float b);
}

#endif