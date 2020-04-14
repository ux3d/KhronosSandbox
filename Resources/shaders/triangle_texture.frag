#version 460 core

layout (binding = 1) uniform sampler2D u_colorTexture;

layout (location = 0) in vec2 in_texCoord;

layout(location = 0) out vec4 out_pixelColor;

void main()
{
    out_pixelColor = vec4(texture(u_colorTexture, in_texCoord.st).rgb, 1.0);
}
