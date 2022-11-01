#version 430

// Input
layout(location = 0) in vec2 texture_coord;

// Uniform properties
uniform sampler2D texture_1;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // If the particle texture had alpha values, I could have GL_DEPTH_TEST enabled and uncomment the following lines
    // if (texture(texture_1, texture_coord).a < 0.75)
    // {
    //    discard;
    // }
    vec3 color = texture(texture_1, texture_coord).xyz;
    out_color = vec4(color, 1);
}
