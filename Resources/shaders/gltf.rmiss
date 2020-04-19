#version 460 core
#extension GL_EXT_ray_tracing : enable

layout (binding = DIFFUSE_BINDING) uniform samplerCube u_diffuseTexture;
layout (binding = SPECULAR_BINDING) uniform samplerCube u_specularTexture;
layout (binding = LUT_BINDING) uniform sampler2D u_lutTexture;

struct Payload
{
  vec3 color;
  uint depth;
  uint maxDepth;
  bool primitive;
  vec3 ray;
};

layout(location = 0) rayPayloadInEXT Payload out_hitValue;

void main()
{
	if (!out_hitValue.primitive)
	{
	    out_hitValue.color = vec3(0.0, 0.0, 0.0);
	}
	else
	{
		// TODO: Write back specular level 0 from environment map.
		out_hitValue.color = vec3(1.0, 0.0, 0.0);
	}
	out_hitValue.primitive = false;
}