#ifndef GLTF_HELPERUPDATE_H_
#define GLTF_HELPERUPDATE_H_

#include "GLTF.h"

class HelperUpdate {
public:

	static bool update(Primitive& primitive, GLTF& glTF, const glm::mat4& parentWorldMatrix);

	static bool update(Mesh& mesh, GLTF& glTF, const glm::mat4& parentWorldMatrix);

	static bool update(Node& node, GLTF& glTF, const glm::mat4& parentWorldMatrix);

	static bool update(Scene& scene, GLTF& glTF, const glm::mat4& parentWorldMatrix);

	static bool update(GLTF& glTF, const glm::mat4& parentWorldMatrix);
};

#endif /* GLTF_HELPERUPDATE_H_ */
