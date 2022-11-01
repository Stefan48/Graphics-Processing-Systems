#version 430

// Input
layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

// Uniform properties
uniform bool no_color;
uniform vec4 color;
uniform samplerCube texture_cubemap;
uniform vec3 camera_position;

// Output
layout(location = 0) out vec4 out_color;


vec3 myReflect(vec3 V)
{
    // Compute the reflection color value
    vec3 R = reflect(V, world_normal);
    return texture(texture_cubemap, R).xyz;
}

void main()
{
    if (no_color)
    {
        out_color = vec4(color.a, color.a, color.a, 0);
        return;
    }
    vec3 V = normalize(world_position - camera_position);
    vec3 reflection_color = myReflect(V);
    // adjust last parameter (interpolation factor) to vary the effect
    vec3 mixed_color = mix(color.xyz , reflection_color, 0.5);
    out_color = vec4(mixed_color, color.a);
}
