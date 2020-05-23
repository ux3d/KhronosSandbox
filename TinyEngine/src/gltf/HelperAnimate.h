#ifndef GLTF_HELPERANIMATE_H_
#define GLTF_HELPERANIMATE_H_

#include <cstdint>

#include "../math/Math.h"

#include "GLTF.h"

struct GLTF;

class HelperAnimate {

private:

	static bool update(GLTF& glTF, const AnimationChannel& channel, int32_t startIndex, int32_t stopIndex, float currentTime);

public:

	static bool gatherStop(float& stop, const GLTF& glTF, uint32_t animationIndex);

	static bool update(GLTF& glTF, uint32_t animationIndex, float currentTime);

};

#endif /* GLTF_HELPERANIMATE_H_ */
