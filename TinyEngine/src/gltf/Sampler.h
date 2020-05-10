#ifndef GLTF_SAMPLER_H_
#define GLTF_SAMPLER_H_

#include "../TinyEngine.h"

class Sampler {
public:
    VkFilter                magFilter = VK_FILTER_LINEAR;
    VkFilter                minFilter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode     mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    VkSamplerAddressMode    addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode    addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    float                   minLod = 0.0f;
    float                   maxLod = 1.0f;
};

#endif /* GLTF_SAMPLER_H_ */
