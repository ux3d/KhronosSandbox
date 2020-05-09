#ifndef GLTF_MESH_H_
#define GLTF_MESH_H_

#include "../TinyEngine.h"

#include "Primitive.h"

class Mesh {
public:
	std::vector<Primitive> primitives;
};

#endif /* GLTF_MESH_H_ */
