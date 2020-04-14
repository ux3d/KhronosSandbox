#version 460 core

layout (location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_pixelColor;

void main()
{
    out_pixelColor = vec4(in_color, 1.0);
}
