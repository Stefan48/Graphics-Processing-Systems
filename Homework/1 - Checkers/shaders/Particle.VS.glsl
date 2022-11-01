#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;


struct Particle
{
    vec4 position;
    vec4 speed;
};


layout(std430, binding = 0) buffer particles {
    Particle data[];
};


void main()
{
    vec3 pos = data[gl_VertexID].position.xyz;
    vec3 spd = data[gl_VertexID].speed.xyz;

    float dt = 0.1;

    pos = pos + spd * dt + vec3(0, -0.5, 0) * dt * dt/2.0f ;
    spd = spd + vec3(0, -0.5, 0) * dt;

    data[gl_VertexID].position.xyz = pos;
    data[gl_VertexID].speed.xyz = spd;

    gl_Position = Model * vec4(pos, 1);
}
