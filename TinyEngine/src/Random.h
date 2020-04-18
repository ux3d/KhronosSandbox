#ifndef RANDOM_H_
#define RANDOM_H_

#include "Math.h"

class Random {

private:

	static float radicalInverse(uint32_t bits);

public:

	static glm::vec2 hammersley(uint32_t i, uint32_t N);

	static glm::vec2 mersenne();

};

#endif /* RANDOM_H_ */
