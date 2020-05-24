#ifndef GLTF_MESH_H_
#define GLTF_MESH_H_

#include <vector>

#include "Primitive.h"

struct Mesh {
	std::vector<Primitive> primitives;

	std::vector<float> weights;
};

#endif /* GLTF_MESH_H_ */
