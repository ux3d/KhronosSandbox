#version 460 core

layout(binding = 0) uniform UniformBufferObject {
    mat4 transform;
} in_ub;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec3 in_color;

layout (location = 0) out vec3 out_color;

void main()
{
    out_color = in_color;

    gl_Position = in_ub.transform * in_position;
}
