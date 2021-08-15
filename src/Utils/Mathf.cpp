#include "Utils/Mathf.h"

#include <random>
#include "Engine.h"

namespace Mathf {
    constexpr float ApproximatelyThreshold = 0.0005f;

	bool Approximately(float a, float b)
	{
	    return (a - ApproximatelyThreshold) < b && (a + ApproximatelyThreshold) > b;
	}
	
	bool Approximately(glm::vec3 a, glm::vec3 b)
	{
		return Mathf::Approximately(a.x, b.x) 
			&& Mathf::Approximately(a.y, b.y) 
			&& Mathf::Approximately(a.z, b.z);
	}
	
	bool Approximately(glm::vec4 a, glm::vec4 b)
	{
		return Mathf::Approximately(a.x, b.x) 
			&& Mathf::Approximately(a.y, b.y) 
			&& Mathf::Approximately(a.z, b.z) 
			&& Mathf::Approximately(a.w, b.w);
	}
	
	float RangeRandom(float a, float b)
	{
        std::uniform_real_distribution<float> distrib(a, b);
		return distrib(GameEngine->GetRandomEngine());
	}
}