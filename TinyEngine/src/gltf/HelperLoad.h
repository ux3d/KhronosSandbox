#ifndef GLTF_HELPERLOAD_H_
#define GLTF_HELPERLOAD_H_

#include <string>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
#include <tiny_gltf.h>

#include "GLTF.h"

class HelperLoad {

private:

	tinygltf::Model model;

	bool initBuffers(GLTF& glTF);

	bool initBufferViews(GLTF& glTF);

	bool initAccessors(GLTF& glTF);

	bool initImages(GLTF& glTF, const std::string& path);

	bool initSamplers(GLTF& glTF);

	bool initTextures(GLTF& glTF);

	bool initMaterials(GLTF& glTF);

	bool initMeshes(GLTF& glTF);

	bool initNodes(GLTF& glTF);

	bool initScenes(GLTF& glTF);

public:

	HelperLoad();

	bool open(GLTF& glTF, const std::string& filename);

};

#endif /* GLTF_HELPERLOAD_H_ */
