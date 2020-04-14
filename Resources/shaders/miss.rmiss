#version 460 core
#extension GL_EXT_ray_tracing : enable

layout(location = 0) rayPayloadInEXT vec3 out_hitValue;

void main()
{
    out_hitValue = vec3(0.0, 0.0, 0.0);
}