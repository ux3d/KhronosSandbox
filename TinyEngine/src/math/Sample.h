#ifndef MATH_SAMPLE_H_
#define MATH_SAMPLE_H_

#include "glm_include.h"

class Sample {
public:

	static glm::vec2 concentricDisk(const glm::vec2& point);

	static glm::vec3 uniformHemisphere(const glm::vec2& point);

	static glm::vec3 cosineHemisphere(const glm::vec2& point);

	static glm::vec3 ggxHemisphere(const glm::vec2& point, float alpha);

};

#endif /* MATH_SAMPLE_H_ */
