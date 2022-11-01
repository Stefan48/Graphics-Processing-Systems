#version 430

// Input and output topologies
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

// Uniform properties
uniform bool no_color;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;

// Output
layout(location = 0) out vec2 texture_coord;

// Local variables
vec3 vpos = gl_in[0].gl_Position.xyz;
vec3 forward = normalize(eye_position - vpos);
vec3 right = normalize(cross(forward, vec3(0, 1, 0)));
vec3 up = normalize(cross(forward, right));


void EmitPoint(vec2 offset)
{
    vec3 pos = right * offset.x + up * offset.y + vpos;
    gl_Position = Projection * View * vec4(pos, 1.0);
    EmitVertex();
}


void main()
{
    if (no_color || vpos.y < 0)
    {
        return;
    }

    float ds = 0.05;
    texture_coord = vec2(0, 0);
    EmitPoint(vec2(-ds, -ds));

    texture_coord = vec2(0, 1);
    EmitPoint(vec2(-ds, ds));

    texture_coord = vec2(1, 0);
    EmitPoint(vec2(ds, -ds));

    texture_coord = vec2(1, 1);
    EmitPoint(vec2(ds, ds));
}
