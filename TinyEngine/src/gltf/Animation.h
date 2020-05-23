#ifndef GLTF_ANIMATION_H_
#define GLTF_ANIMATION_H_

#include <vector>

struct AnimationChannel {
	// TODO: Implement.
};

struct AnimationSampler {
	// TODO: Implement.
};

struct Animation {
	std::vector<AnimationChannel> channels;

	std::vector<AnimationSampler> samplers;
};

#endif /* GLTF_ANIMATION_H_ */
