#include "HelperAnimate.h"

#include "../activity/Interpolator.h"

bool HelperAnimate::update(GLTF& glTF, const AnimationChannel& channel, int32_t startIndex, int32_t stopIndex, float currentTime)
{
	const AnimationSampler& sampler = *channel.targetSampler;

	float t = 0.0f;
	if (stopIndex != -1)
	{
		t = (currentTime - sampler.inputTime[startIndex]) / (sampler.inputTime[stopIndex] - sampler.inputTime[startIndex]);
	}

	Node& node = *channel.target.targetNode;

	std::vector<float> x;
	std::vector<float> y;
	std::vector<float> xout;
	std::vector<float> yin;

	uint32_t typeCount = 1;
	if (channel.target.path == translation || channel.target.path == scale)
	{
		typeCount = 3;
	}
	else if (channel.target.path == rotation)
	{
		typeCount = 4;
	}
	else
	{
		typeCount = static_cast<uint32_t>(node.weights.size());
	}

	x.resize(typeCount);
	y.resize(typeCount);
	xout.resize(typeCount);
	yin.resize(typeCount);

	uint32_t elementCount = 1;
	uint32_t elementOffset = 0;
	if (sampler.interpolation == CUBICSPLINE)
	{
		elementCount = 3;
		elementOffset = typeCount * 3;
	}
	uint32_t offset = typeCount * elementCount;

	memcpy(x.data(), &sampler.outputValues[startIndex * offset + elementOffset], sizeof(float) * typeCount * elementCount);
	if (sampler.interpolation == CUBICSPLINE)
	{
		memcpy(xout.data(), &sampler.outputValues[startIndex * offset + elementOffset * 2], sizeof(float) * typeCount * elementCount);
	}
	if (stopIndex != -1)
	{
		memcpy(y.data(), &sampler.outputValues[stopIndex * offset + elementOffset], sizeof(float) * typeCount * elementCount);
		if (sampler.interpolation == CUBICSPLINE)
		{
			memcpy(yin.data(), &sampler.outputValues[stopIndex * offset], sizeof(float) * typeCount * elementCount);
		}
	}

	//

	if (channel.target.path == translation || channel.target.path == scale)
	{
		glm::vec3 value;

		if (sampler.interpolation == CUBICSPLINE)
		{
			value = Interpolator::cubicspline(glm::vec3(x[0], x[1], x[2]), glm::vec3(xout[0], xout[1], xout[2]), glm::vec3(yin[0], yin[1], yin[2]), glm::vec3(y[0], y[1], y[2]), t);
		}
		else if (sampler.interpolation == LINEAR)
		{
			value = Interpolator::linear(glm::vec3(x[0], x[1], x[2]), glm::vec3(y[0], y[1], y[2]), t);
		}
		else
		{
			value = Interpolator::step(glm::vec3(x[0], x[1], x[2]), glm::vec3(y[0], y[1], y[2]), t);
		}

		if (channel.target.path == translation)
		{
			node.translation = value;
		}
		else if (channel.target.path == scale)
		{
			node.scale = value;
		}
	}
	else if (channel.target.path == rotation)
	{
		glm::quat value;

		if (sampler.interpolation == CUBICSPLINE)
		{
			value = Interpolator::cubicspline(glm::quat(x[3], x[0], x[1], x[2]), glm::quat(xout[3], xout[0], xout[1], xout[2]), glm::quat(yin[3], yin[0], yin[1], yin[2]), glm::quat(y[3], y[0], y[1], y[2]), t);
		}
		else if (sampler.interpolation == LINEAR)
		{
			value = Interpolator::linear(glm::quat(x[3], x[0], x[1], x[2]), glm::quat(y[3], y[0], y[1], y[2]), t);
		}
		else
		{
			value = Interpolator::step(glm::quat(x[3], x[0], x[1], x[2]), glm::quat(y[3], y[0], y[1], y[2]), t);
		}

		node.rotation = value;
	}
	else
	{
		for (size_t i = 0; i < node.weights.size(); i++)
		{
			float value;

			if (sampler.interpolation == CUBICSPLINE)
			{
				value = Interpolator::cubicspline(x[i], xout[i], yin[i], y[i], t);
			}
			else if (sampler.interpolation == LINEAR)
			{
				value = Interpolator::linear(x[i], y[i], t);
			}
			else
			{
				value = Interpolator::step(x[i], y[i], t);
			}

			node.weights[i] = value;
		}
	}

	return true;
}

bool HelperAnimate::gatherStartStop(float& start, float& stop, const GLTF& glTF, uint32_t animationIndex)
{
	if (animationIndex > static_cast<uint32_t>(glTF.animations.size()))
	{
		return false;
	}

	const Animation& animation = glTF.animations[animationIndex];

	if (animation.samplers.size() == 0 || animation.samplers[0].inputTime.size() == 0)
	{
		return false;
	}

	start = animation.samplers[0].inputTime.front();
	stop = animation.samplers[0].inputTime.back();

	for (uint32_t i = 1; i < animation.samplers.size(); i++)
	{
		if (animation.samplers[i].inputTime.size() == 0)
		{
			return false;
		}

		start = glm::min(start, animation.samplers[i].inputTime.front());
		stop = glm::max(stop, animation.samplers[i].inputTime.back());
	}

	return true;
}

bool HelperAnimate::update(GLTF& glTF, uint32_t animationIndex, float currentTime)
{
	if (animationIndex > static_cast<uint32_t>(glTF.animations.size()))
	{
		return false;
	}

	const Animation& animation = glTF.animations[animationIndex];

	for (uint32_t i = 0; i < animation.channels.size(); i++)
	{
		const AnimationChannel& channel = animation.channels[i];
		const AnimationSampler& sampler = *channel.targetSampler;

		const std::vector<float>& inputTime = sampler.inputTime;

		if (inputTime.size() == 0)
		{
			return false;
		}

		if (currentTime < inputTime.front())
		{
			update(glTF, channel, 0, -1, currentTime);
		}
		else if (currentTime >= inputTime.back())
		{
			update(glTF, channel, static_cast<int32_t>(inputTime.size() - 1), -1, currentTime);
		}
		else
		{
			for (int32_t startIndex = 0; startIndex < static_cast<int32_t>(inputTime.size()); startIndex++)
			{
				if (currentTime >= inputTime[startIndex])
				{
					update(glTF, channel, startIndex, startIndex + 1, currentTime);

					break;
				}
			}
		}
	}

	return true;
}