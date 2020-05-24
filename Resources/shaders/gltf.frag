#version 460 core

#ifdef UNIFORMBUFFER_BINDING

layout(binding = UNIFORMBUFFER_BINDING) uniform UniformBuffer {
	vec4 baseColorFactor;

	float metallicFactor;
	float roughnessFactor;
	float normalScale;
	float occlusionStrength;

	vec3 emissiveFactor;
	uint alphaMode;

	float alphaCutoff;
	bool doubleSided;
} in_ub;

//

layout (binding = DIFFUSE_BINDING) uniform samplerCube u_diffuseTexture;

layout (binding = SPECULAR_BINDING) uniform samplerCube u_specularTexture;

layout (binding = LUT_BINDING) uniform sampler2D u_lutTexture;

#endif

//

#ifdef BASECOLOR_TEXTURE
layout (binding = BASECOLOR_BINDING) uniform sampler2D u_baseColorTexture;
#endif

#ifdef METALLICROUGHNESS_TEXTURE
layout (binding = METALLICROUGHNESS_BINDING) uniform sampler2D u_metallicRoughnessTexture;
#endif

#ifdef EMISSIVE_TEXTURE
layout (binding = EMISSIVE_BINDING) uniform sampler2D u_emissiveTexture;
#endif

#ifdef OCCLUSION_TEXTURE
layout (binding = OCCLUSION_BINDING) uniform sampler2D u_occlusionTexture;
#endif

#ifdef NORMAL_TEXTURE
layout (binding = NORMAL_BINDING) uniform sampler2D u_normalTexture;
#endif

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_view;
#ifdef NORMAL_VEC3
layout (location = 2) in vec3 in_normal;
#endif
#ifdef TANGENT_VEC4
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;
#endif
#ifdef TEXCOORD_0_VEC2
layout (location = 5) in vec2 in_texCoord0;
#endif
#ifdef TEXCOORD_1_VEC2
layout (location = 6) in vec2 in_texCoord1;
#endif
#ifdef COLOR_0_VEC4
layout (location = 7) in vec4 in_color;
#endif
#ifdef COLOR_0_VEC3
layout (location = 7) in vec4 in_color;
#endif

//

layout(location = 0) out vec4 out_pixelColor;

//

vec3 toLinear(vec3 color)
{
    return pow(color, vec3(2.2));
}

vec3 toNonLinear(vec3 color)
{
    return pow(color, vec3(1.0/2.2));
}

//

#ifdef UNIFORMBUFFER_BINDING

vec3 getLambertian(vec3 normal, vec3 diffuseColor)
{
    return diffuseColor * texture(u_diffuseTexture, normal).rgb;
}

vec3 getSpecular(vec3 normal, vec3 view, float alpha, vec3 f0)
{
    float NdotV = dot(normal, view);
    
    float mipLevels = float(textureQueryLevels(u_specularTexture) - 1);
    vec3 reflection = normalize(reflect(-view, normal));
    float lod = clamp(alpha * mipLevels, 0.0, mipLevels);
    vec3 specularSample = textureLod(u_specularTexture, reflection, lod).rgb;

    vec2 brdfSamplePoint = clamp(vec2(NdotV, alpha), vec2(0.0, 0.0), vec2(1.0, 1.0));
    vec2 brdf = texture(u_lutTexture, brdfSamplePoint).rg;

    return specularSample * (f0 * brdf.x + brdf.y);
}

#endif

//

vec4 getBaseColor()
{
#ifdef UNIFORMBUFFER_BINDING
    vec4 baseColor = in_ub.baseColorFactor;
#else
    vec4 baseColor = vec4(1.0, 1.0, 1.0, 1.0);
#endif 

#ifdef BASECOLOR_TEXTURE
    vec4 baseColorTexture = texture(u_baseColorTexture, BASECOLOR_TEXCOORD.st).rgba;

    baseColor.rgb *= toLinear(baseColorTexture.rgb);
    baseColor.a *= baseColorTexture.a;
#endif

#ifdef COLOR_0_VEC4
    baseColor *= in_color;
#endif
#ifdef COLOR_0_VEC3
    baseColor *= in_color;
#endif

    return baseColor;
}

float getMetallic()
{
#ifdef UNIFORMBUFFER_BINDING
    float metallic = in_ub.metallicFactor;
#else
    float metallic = 1.0;
#endif 

#ifdef METALLICROUGHNESS_TEXTURE
    metallic *= texture(u_metallicRoughnessTexture, METALLICROUGHNESS_TEXCOORD.st).b;
#endif

    return metallic;
}

float getRoughness()
{
#ifdef UNIFORMBUFFER_BINDING
    float roughness = in_ub.roughnessFactor;
#else
    float roughness = 1.0;
#endif 

#ifdef METALLICROUGHNESS_TEXTURE
    roughness *= texture(u_metallicRoughnessTexture, METALLICROUGHNESS_TEXCOORD.st).g;
#endif

    return roughness;
}

vec3 getEmissive()
{
#ifdef UNIFORMBUFFER_BINDING
    vec3 emissive = in_ub.emissiveFactor;
#else
    vec3 emissive = vec3(0.0, 0.0, 0.0);
#endif 

#ifdef EMISSIVE_TEXTURE
    emissive *= toLinear(texture(u_emissiveTexture, EMISSIVE_TEXCOORD.st).rgb);
#endif

    return emissive;
}

float getOcclusion()
{
    float occlusion = 1.0;

#ifdef OCCLUSION_TEXTURE
    occlusion = texture(u_occlusionTexture, OCCLUSION_TEXCOORD.st).r;
#endif

    return occlusion;
}

#ifdef NORMAL_VEC3
vec3 getNormal()
{
    vec3 normal = normalize(in_normal);

#ifdef NORMAL_TEXTURE
    vec3 tangent;
    vec3 bitangent;

#ifdef TANGENT_VEC4
    tangent = normalize(in_tangent);
    bitangent = normalize(in_bitangent);
#else
    vec3 pos_dx = dFdx(in_position);
    vec3 pos_dy = dFdy(in_position);
    vec3 tex_dx = dFdx(vec3(NORMAL_TEXCOORD.st, 0.0));
    vec3 tex_dy = dFdy(vec3(NORMAL_TEXCOORD.st, 0.0));
    tangent = normalize((tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t));
    bitangent = cross(normal, tangent);
#endif

    mat3 tbn = mat3(tangent, bitangent, normal);

    vec3 n = texture(u_normalTexture, NORMAL_TEXCOORD.st).rgb;
    n = normalize((2.0 * n - 1.0) * vec3(in_ub.normalScale, in_ub.normalScale, 1.0));

    normal = tbn * n;
#endif

    if (in_ub.doubleSided && !gl_FrontFacing)
    {
        normal = -normal;
    }

    return normal;
}
#endif

void main()
{
    vec4 baseColor = getBaseColor();
    float alphaChannel = baseColor.a;

#ifdef NORMAL_VEC3
    vec3 view = normalize(in_view);

    vec3 normal = getNormal();

    vec3 f0 = vec3(0.04);
    float metallic = getMetallic();
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0) * (1.0 - metallic);
    f0 = mix(f0, baseColor.rgb, metallic);

    float roughness = getRoughness();

    // BRDF
    vec3 color = getEmissive() + getLambertian(normal, diffuseColor) + getSpecular(normal, view, roughness, f0);

    // Ambient occlusion
    color = mix(color, color * getOcclusion(), in_ub.occlusionStrength);
#else
	vec3 color = baseColor.rgb;
#endif

#ifdef UNIFORMBUFFER_BINDING
    if (in_ub.alphaMode == 0)
    {
        alphaChannel = 1.0;
    }
    else if (in_ub.alphaMode == 1)
    {
        if(alphaChannel < in_ub.alphaCutoff)
        {
            discard;
        }
        alphaChannel = 1.0;
    }
#endif

    out_pixelColor = vec4(toNonLinear(color), alphaChannel);
}
