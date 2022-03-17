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
	const mat3 m = mat3(
		0.4124564, 0.2126729, 0.0193339,
		0.3575761, 0.7151522, 0.1191920,
		0.1804375, 0.0721750, 0.9503041
	);

	return m * color;
}

vec3 xyzToLinear(vec3 color)
{
	const mat3 m = mat3(
		3.2404542,-0.9692660, 0.0556434,
		-1.5371385, 1.8760108,-0.2040259,
		-0.4985314, 0.0415560, 1.0572252
	);

	return m * color;
}

//
// see https://www.itu.int/rec/R-REC-BT.2087-0-201510-I
// see https://www.itu.int/pub/R-REP-BT.2407
//

vec3 rec2020ToXyz(vec3 color)
{
	const mat3 m = mat3(
		 0.6370, 0.2627, 0.0,
		 0.1446, 0.6780, 0.0281,
		 0.1689, 0.0593, 1.0610
	);

	return m * color;
}

vec3 xyzToRec2020(vec3 color)
{
	const mat3 m = mat3(
		 1.7167, -0.6667,  0.0176,
		-0.3557,  1.6165, -0.0428,
		-0.2534,  0.0158,  0.9421
	);

	return m * color;
}

//
// see https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.html#TRANSFER_PQ
//

float rec2020ToPqPerChannel(float channel)
{
	const float m1 = 2610.0 / 16384.0;
	const float m2 = 2523.0 / 4096.0 * 128.0;
	const float c1 = 3424.0 / 4096.0;
	const float c2 = 2413.0 / 4096.0 * 32.0;
	const float c3 = 2392.0 / 4096.0 * 32.0;

	float Y = channel / 10000.0;
	float Ym1 = pow(Y, m1);

	return pow((c1 + c2 * Ym1)/(1.0 + c3 * Ym1), m2);
}

vec3 rec2020ToPq(vec3 color)
{
	return vec3(
        rec2020ToPqPerChannel(color.r),
        rec2020ToPqPerChannel(color.g),
        rec2020ToPqPerChannel(color.b)
    );	
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
		// Linear

		// No conversion required
	}
	else if (in_ub.transferFunction == 1)
	{
		// sRGB

		c = linearToSrgbFast(c);
	}
	else if (in_ub.transferFunction == 2)
	{
		// PQ

		// BT.709 => CIE-XYZ

		c = linearToXyz(c);

		// CIE-XYZ => BT.2020

		c = xyzToRec2020(c);

		// PQ EOTF -1

		c = rec2020ToPq(c);
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
