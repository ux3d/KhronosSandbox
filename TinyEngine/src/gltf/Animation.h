#ifndef GLTF_ANIMATION_H_
#define GLTF_ANIMATION_H_

#include <cstdint>
#include <vector>

enum InterpolationAlgorithm {
	LINEAR,
	STEP,
	CUBICSPLINE
};

enum NodePath{
	translation,
	rotation,
	scale,
	weights
};

struct AnimationChannelTarget {
	int32_t node = -1;
	NodePath path = translation;

	// Generic helper

	Node* targetNode = nullptr;
};

struct AnimationSampler {
	int32_t input = -1;
	int32_t output = -1;
	InterpolationAlgorithm interpolation = LINEAR;

	// Generic helper

	std::vector<float> inputTime;

	std::vector<float> outputValues;
};

struct AnimationChannel {
	int32_t sampler = -1;
	AnimationChannelTarget target;

	// Generic helper

	AnimationSampler* targetSampler = nullptr;
};

struct Animation {
	std::vector<AnimationChannel> channels;

	std::vector<AnimationSampler> samplers;
};

#endif /* GLTF_ANIMATION_H_ */
