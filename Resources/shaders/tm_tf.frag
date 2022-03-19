#version 460 core

const float SRGB_GAMMA_FAST = 2.2;
const float SRGB_INV_GAMMA_FAST = 1.0 / SRGB_GAMMA_FAST;

layout(binding = 0) uniform UniformBufferObject {
	int tonemap;
	int transferFunction;
	float monitorMaximumNits;
    bool imageSrgbNonLinear;
	bool debug;
} in_ub;

layout (binding = 1) uniform sampler2D u_colorTexture;

layout (location = 0) in vec2 in_texCoord;

layout(location = 0) out vec4 out_pixelColor;

//
// Helper functions
//

float luminance(vec3 color)
{
	return 0.2126*color.r + 0.7152*color.g + 0.0722*color.b;
}

//
// Color space conversions
// 

// see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html

vec3 rec709ToSrgbNonLinearFast(vec3 color)
{
    return pow(color, vec3(SRGB_INV_GAMMA_FAST));
}

vec3 srgbNonLinearToRec709Fast(vec3 color)
{
    return vec3(pow(color.xyz, vec3(SRGB_GAMMA_FAST)));
}

// see https://github.com/tobspr/GLSL-Color-Spaces/blob/master/ColorSpaces.inc.glsl

float rec709ToSrgbNonLinearPerChannel(float channel)
{
    if(channel <= 0.0031308)
	{
        return 12.92 * channel;
	}
    
	return 1.055 * pow(channel, 1.0/2.4) - 0.055;
}

float srgbNonLinearToRec709PerChannel(float channel)
{
    if (channel <= 0.04045)
	{
        return channel / 12.92;
	}
    
	return pow((channel + 0.055) / 1.055, 2.4);
}

vec3 rec709ToSrgbNonLinear(vec3 color)
{
	return vec3(
        rec709ToSrgbNonLinearPerChannel(color.r),
        rec709ToSrgbNonLinearPerChannel(color.g),
        rec709ToSrgbNonLinearPerChannel(color.b)
    );	
}

vec3 srgbNonLinearToRec709(vec3 color)
{
	return vec3(
        srgbNonLinearToRec709PerChannel(color.r),
        srgbNonLinearToRec709PerChannel(color.g),
        srgbNonLinearToRec709PerChannel(color.b)
    );	
}

float rec709ToRec709NonLinearPerChannel(float channel)
{
    if(channel <= 0.018)
	{
        return 4.5 * channel;
	}
    
	return 1.099 * pow(channel, 1.0/2.2) - 0.099;
}

float rec709NonLinearToRec709PerChannel(float channel)
{
    if (channel <= 0.081)
	{
        return channel / 4.5;
	}
    
	return pow((channel + 0.099) / 1.099, 2.2);
}

vec3 rec709ToRec709NonLinear(vec3 color)
{
	return vec3(
        rec709ToRec709NonLinearPerChannel(color.r),
        rec709ToRec709NonLinearPerChannel(color.g),
        rec709ToRec709NonLinearPerChannel(color.b)
    );	
}

vec3 rec709NonLinearToRec709(vec3 color)
{
	return vec3(
        rec709NonLinearToRec709PerChannel(color.r),
        rec709NonLinearToRec709PerChannel(color.g),
        rec709NonLinearToRec709PerChannel(color.b)
    );	
}

vec3 rec709ToXyz(vec3 color)
{
	const mat3 m = mat3(
		0.4124564, 0.2126729, 0.0193339,
		0.3575761, 0.7151522, 0.1191920,
		0.1804375, 0.0721750, 0.9503041
	);

	return m * color;
}

vec3 xyzToRec709(vec3 color)
{
	const mat3 m = mat3(
		 3.2404542,-0.9692660, 0.0556434,
		-1.5371385, 1.8760108,-0.2040259,
		-0.4985314, 0.0415560, 1.0572252
	);

	return m * color;
}

// see https://www.shadertoy.com/view/WltSRB

// Rec709 => XYZ => D65_2_D60 => AP1
vec3 rec709ToAcesAp1(vec3 color)
{
	const mat3 m = mat3
	(
		0.613097, 0.070194, 0.020616,
		0.339523, 0.916354, 0.109570,
		0.047379, 0.013452, 0.869815
	);

	return m * color;
}

// Inverse from above
vec3 acesAp1ToRec709(vec3 color)
{
	const mat3 m = mat3
	(
		 1.704859, -0.130078, -0.023964,
		-0.621715,  1.140734, -0.128975,
		-0.083299, -0.010560,  1.153013

	);

	return m * color;
}

// see https://www.itu.int/rec/R-REC-BT.2087-0-201510-I
// see https://www.itu.int/pub/R-REP-BT.2407

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

// see https://www.khronos.org/registry/DataFormat/specs/1.3/dataformat.1.3.html#TRANSFER_PQ

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
// Tonemappings
//

// see https://64.github.io/tonemapping/

vec3 tonemapReinhard(vec3 color)
{
	return color / (vec3(1.0) + color);
}

vec3 tonemapReinhardJodie(vec3 color)
{
    float L = luminance(color);
    vec3 tonemappedColor = tonemapReinhard(color);
    return mix(color / (1.0 + L), tonemappedColor, tonemappedColor);
}

// see https://www.shadertoy.com/view/WltSRB

vec3 tonemapAcesHill(vec3 color)
{
	// RRT and ODT fit

    vec3 a = (color            + 0.0245786) * color;
    vec3 b = (color * 0.983729 + 0.4329510) * color + 0.238081;
    
    return a / b;
}

//
// Main program
//

void main()
{
	vec3 c = texture(u_colorTexture, in_texCoord.st).rgb;
	if (in_ub.imageSrgbNonLinear)
	{
		c = srgbNonLinearToRec709Fast(c);
	}

	//
	// Tone mapping
	//

	if (in_ub.tonemap == 0)
	{
		// None

		// Do nothing
	}
	else if (in_ub.tonemap == 1)
	{
		// Reinhard

		c = tonemapReinhard(c);
	}
	else if (in_ub.tonemap == 2)
	{
		// Reinhard Jodie

		c = tonemapReinhardJodie(c);	
	}
	else if (in_ub.tonemap == 3)
	{
		// BT.709 => AP1

	    c = rec709ToAcesAp1(c);

		// ACES (Hill)
		c = tonemapAcesHill(c);

		// AP1 => BT.709

    	c = acesAp1ToRec709(c);
	}

	//
	// Color Primary
	//

	if (in_ub.transferFunction == 2)
	{
		// BT.709 => CIE-XYZ

		c = rec709ToXyz(c);

		// CIE-XYZ => BT.2020

		c = xyzToRec2020(c);
	}
	
	//
	// Transfer function
	//

	if (in_ub.transferFunction == 0)
	{
		// Rec709

		// No conversion required
	}
	else if (in_ub.transferFunction == 1)
	{
		// sRGB

		c = rec709ToSrgbNonLinear(c);
	}
	else if (in_ub.transferFunction == 2)
	{
		// PQ

		// Adjust to monitor maximum possible nits

		c = in_ub.monitorMaximumNits * c;

		// PQ EOTF-1

		c = rec2020ToPq(c);
	}

	//
	// In debug mode, write out any clamped pixel in red
	//

	if (in_ub.debug)
	{
		if (c.r > 1.0 || c.g > 1.0 || c.b > 1.0)
		{
			c = vec3(1.0, 0.0, 0.0);
		}
	}

	//

    out_pixelColor = vec4(c, 1.0);
}
