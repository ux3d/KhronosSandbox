#version 460 core

layout(binding = 0) uniform UniformBufferObject {
    mat4 transform;
} in_ub;

layout (location = 0) in vec4 in_position;
layout (location = 1) in vec2 in_texCoord;

layout (location = 0) out vec2 out_texCoord;

void main()
{
	out_texCoord = in_texCoord;

    gl_Position = in_ub.transform * in_position;
}
