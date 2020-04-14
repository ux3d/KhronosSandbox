#version 460 core
#extension GL_EXT_ray_tracing : enable

layout (binding = DIFFUSE_BINDING) uniform samplerCube u_diffuseTexture;
layout (binding = SPECULAR_BINDING) uniform samplerCube u_specularTexture;
layout (binding = LUT_BINDING) uniform sampler2D u_lutTexture;

layout(location = 0) rayPayloadInEXT vec3 out_hitValue;

void main()
{
    out_hitValue = vec3(0.0, 0.0, 0.0);
}