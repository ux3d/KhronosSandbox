#version 460 core
#extension GL_EXT_ray_tracing : enable

hitAttributeEXT vec3 in_hitAttribute;

layout(location = 0) rayPayloadInEXT vec3 out_hitValue;

void main()
{
    out_hitValue = vec3(1.0f - in_hitAttribute.x - in_hitAttribute.y, in_hitAttribute.x, in_hitAttribute.y);
}
