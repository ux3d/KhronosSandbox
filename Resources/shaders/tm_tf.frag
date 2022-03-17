#version 460 core

const float SRGB_GAMMA = 2.2;
const float SRGB_INV_GAMMA = 1.0 / SRGB_GAMMA;
const float SRGB_ALPHA = 0.055;

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
// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
//

vec3 linearToSrgbFast(vec3 color)
{
    return pow(color, vec3(SRGB_INV_GAMMA));
}

vec3 srgbToLinearFast(vec3 srgbIn)
{
    return vec3(pow(srgbIn.xyz, vec3(SRGB_GAMMA)));
}

//
// see https://github.com/tobspr/GLSL-Color-Spaces/blob/master/ColorSpaces.inc.glsl
//

float linearToSrgbPerChannel(float channel)
{
    if(channel <= 0.0031308)
	{
        return 12.92 * channel;
	}
    
	return (1.0 + SRGB_ALPHA) * pow(channel, 1.0/2.4) - SRGB_ALPHA;
}

float srgbToLinearPerChannel(float channel)
{
    if (channel <= 0.04045)
	{
        return channel / 12.92;
	}
    
	return pow((channel + SRGB_ALPHA) / (1.0 + SRGB_ALPHA), 2.4);
}

vec3 linearToSrgb(vec3 color)
{
	return vec3(
        linearToSrgbPerChannel(color.r),
        linearToSrgbPerChannel(color.g),
        linearToSrgbPerChannel(color.b)
    );	
}

vec3 srgbToLinear(vec3 color)
{
	return vec3(
        srgbToLinearPerChannel(color.r),
        srgbToLinearPerChannel(color.g),
        srgbToLinearPerChannel(color.b)
    );	
}

vec3 linearToXyz(vec3 color)
{
	const mat3 RGB_2_XYZ = mat3(
		0.4124564, 0.2126729, 0.0193339,
		0.3575761, 0.7151522, 0.1191920,
		0.1804375, 0.0721750, 0.9503041
	);

	return RGB_2_XYZ * color;
}

vec3 xyzToLinear(vec3 color)
{
	const mat3 XYZ_2_RGB = mat3(
		3.2404542,-0.9692660, 0.0556434,
		-1.5371385, 1.8760108,-0.2040259,
		-0.4985314, 0.0415560, 1.0572252
	);

	return XYZ_2_RGB * color;
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
		// BT.709 => CIE-XYZ

		c = linearToXyz(c);

		// CIE-XYZ => BT.2020

		// TODO: Implement

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
