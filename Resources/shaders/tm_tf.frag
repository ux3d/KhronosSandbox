#version 460 core

const float GAMMA = 2.2;
const float INV_GAMMA = 1.0 / GAMMA;

layout(binding = 0) uniform UniformBufferObject {
    bool srgbIn;
	int transferFunction;
	//
	bool debug;
} in_ub;

layout (binding = 1) uniform sampler2D u_colorTexture;

layout (location = 0) in vec2 in_texCoord;

layout(location = 0) out vec4 out_pixelColor;

//

// linear to sRGB approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 linearToSrgbFast(vec3 color)
{
    return pow(color, vec3(INV_GAMMA));
}

// sRGB to linear approximation
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
vec3 srgbToLinearFast(vec3 srgbIn)
{
    return vec3(pow(srgbIn.xyz, vec3(GAMMA)));
}

//

void main()
{
	vec3 c = texture(u_colorTexture, in_texCoord.st).rgb;
	if (in_ub.srgbIn)
	{
		c = srgbToLinearFast(c);
	}

	// Tone mapping

	// TODO: Implement several ones
	
	// Transfer functions

	if (in_ub.transferFunction == 0)
	{
		// No conversion required
	}
	else if (in_ub.transferFunction == 1)
	{
		c = linearToSrgbFast(c);
	}
	else if (in_ub.transferFunction == 2)
	{
		// TODO: Implement PQ transfer function
	}

	//

	if (in_ub.debug)
	{
		if (c.r > 1.0 || c.g > 1.0 || c.g > 1.0)
		{
			c = vec3(1.0, 0.0, 0.0);
		}
	}

	//

    out_pixelColor = vec4(c, 1.0);
}
