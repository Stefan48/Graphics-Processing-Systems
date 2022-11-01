#version 430

// Input
layout(location = 0) in vec3 text_coord;

// Uniform properties
uniform bool no_color;
uniform samplerCube texture_cubemap;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    if (no_color)
    {
        out_color = vec4(0, 0, 0, 0);
        return;
    }
    vec3 color = texture(texture_cubemap, text_coord).xyz;
    out_color = vec4(color, 1);
}
