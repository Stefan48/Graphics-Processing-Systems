#version 430

// Input and output topologies
layout(lines) in;
layout(triangle_strip, max_vertices = 102) out;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p0[3], control_p1[3], control_p2[3], control_p3[3];
uniform int no_of_instances;
uniform int no_of_generated_points;

// Input
in int instance[2];

// Output
layout(location = 0) out vec3 world_position;
layout(location = 1) out vec3 world_normal;


vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z * sin(u);
    float z = point.x * sin(u) + point.z * cos(u);
    return vec3(x, point.y, z);
}

// This function computes B(t) with 4 control points
vec3 bezier(float t, int segment)
{
    return  control_p0[segment] * pow((1 - t), 3) +
            control_p1[segment] * 3 * t * pow((1 - t), 2) +
            control_p2[segment] * 3 * pow(t, 2) * (1 - t) +
            control_p3[segment] * pow(t, 3);
}

void main()
{
    int i = instance[0];
    if (i < no_of_instances)
    {
        int no_of_segments = 3;
        for (int s = 0; s < no_of_segments; ++s)
        {
            float t = 0.0;
            float step_t = 1.0 / float(no_of_generated_points - 1);
            float step_rotate = radians(360.0) / float(no_of_instances);
            float rot1 = i * step_rotate, rot2 = (i + 1) * step_rotate;
        
            for (int j = 0; j < no_of_generated_points - 1; ++j)
            {
                vec3 pos1 = rotateY(bezier(t, s), rot1);
                vec3 pos2 = rotateY(bezier(t, s), rot2);
                vec3 pos3 = rotateY(bezier(t + step_t, s), rot1);
                vec3 pos4 = rotateY(bezier(t + step_t, s), rot2);
                world_normal = normalize(cross(pos3 - pos1, pos2 - pos1));

                vec4 world_pos = Model * vec4(pos1, 1);
                world_position = world_pos.xyz;
                gl_Position = Projection * View * world_pos;
                EmitVertex();
                world_pos = Model * vec4(pos2, 1);
                world_position = world_pos.xyz;
                gl_Position = Projection * View * world_pos;
                EmitVertex();
                world_pos = Model * vec4(pos3, 1);
                world_position = world_pos.xyz;
                gl_Position = Projection * View * world_pos;
                EmitVertex();
                world_pos = Model * vec4(pos4, 1);
                world_position = world_pos.xyz;
                gl_Position = Projection * View * world_pos;
                EmitVertex();
                EndPrimitive();

                t += step_t;
            }
        }
    }
}
