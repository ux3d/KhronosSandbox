#version 460 core

layout(push_constant) uniform UniformPushConstant {
    mat4 projection;
    mat4 view;
    mat4 world;
} in_upc;

layout (location = POSITION_LOC) in vec3 in_position;
#ifdef NORMAL_VEC3
layout (location = NORMAL_LOC) in vec3 in_normal;
#endif
#ifdef TANGENT_VEC4
layout (location = TANGENT_LOC) in vec4 in_tangent;
#endif
#ifdef TEXCOORD_0_VEC2
layout (location = TEXCOORD_0_LOC) in vec2 in_texCoord0;
#endif
#ifdef TEXCOORD_1_VEC2
layout (location = TEXCOORD_1_LOC) in vec2 in_texCoord1;
#endif
#ifdef COLOR_0_VEC4
layout (location = COLOR_0_LOC) in vec4 in_color;
#endif
#ifdef COLOR_0_VEC3
layout (location = COLOR_0_LOC) in vec3 in_color;
#endif

layout (location = 0) out vec3 out_position;
layout (location = 1) out vec3 out_view;
#ifdef NORMAL_VEC3
layout (location = 2) out vec3 out_normal;
#endif
#ifdef TANGENT_VEC4
layout (location = 3) out vec3 out_tangent;
layout (location = 4) out vec3 out_bitangent;
#endif
#ifdef TEXCOORD_0_VEC2
layout (location = 5) out vec2 out_texCoord0;
#endif
#ifdef TEXCOORD_1_VEC2
layout (location = 6) out vec2 out_texCoord1;
#endif
#ifdef COLOR_0_VEC4
layout (location = 7) out vec4 out_color;
#endif
#ifdef COLOR_0_VEC3
layout (location = 7) out vec4 out_color;
#endif

#ifdef HAS_TARGET_POSITION
layout (binding = TARGET_POSITION_BINDING) buffer Position { vec3 i[]; } u_targetPosition;
#endif
#ifdef HAS_TARGET_NORMAL
layout (binding = TARGET_NORMAL_BINDING) buffer Normal { vec3 i[]; } u_targetNormal;
#endif
#ifdef HAS_TARGET_TANGENT
layout (binding = TARGET_TANGENT_BINDING) buffer Tangent { vec3 i[]; } u_targetTangent;
#endif

layout (location = 8) flat out float out_determinant;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(in_upc.world)));

#ifdef NORMAL_VEC3
    vec3 normal = in_normal;
    out_normal = normalMatrix * normal;
#endif

#ifdef TANGENT_VEC4
    vec3 tangent = in_tangent.xyz;
    vec3 bitangent = cross(normal, tangent) * in_tangent.w;
    out_tangent = normalMatrix * tangent;
    out_bitangent = normalMatrix * bitangent;
#endif

#ifdef TEXCOORD_0_VEC2
    out_texCoord0 = in_texCoord0;
#endif
#ifdef TEXCOORD_1_VEC2
    out_texCoord1 = in_texCoord1;
#endif

#ifdef COLOR_0_VEC4
    out_color = in_color;
#endif
#ifdef COLOR_0_VEC3
    out_color = vec4(in_color, 1.0);
#endif

    vec4 position = in_upc.world * vec4(in_position, 1.0);
    out_position = position.xyz / position.w;

    out_determinant = determinant(in_upc.world);
    
    out_view = inverse(mat3(in_upc.view)) * vec3(0.0, 0.0, 1.0);

    gl_Position = in_upc.projection * in_upc.view * position;
}
