#ifndef GLTF_HELPERUPDATE_H_
#define GLTF_HELPERUPDATE_H_

#include "../math/Math.h"

#include "GLTF.h"

struct GLTF;

class HelperUpdate {
public:

	static bool update(Node& node, GLTF& glTF, const glm::mat4& parentWorldMatrix);

	static bool update(Scene& scene, GLTF& glTF, const glm::mat4& parentWorldMatrix);

	static bool update(GLTF& glTF, const glm::mat4& parentWorldMatrix);
};

#endif /* GLTF_HELPERUPDATE_H_ */
