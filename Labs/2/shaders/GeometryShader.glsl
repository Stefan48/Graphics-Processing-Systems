#version 430

// Input and output topologies
// TODO(student): First, generate a curve (via line strip),
// then a rotation/translation surface (via triangle strip)
layout(lines) in;
// layout(line_strip, max_vertices = 256) out;
layout(triangle_strip, max_vertices = 256) out;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
// TODO(student): Declare any other uniforms here
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;

// Input
in int instance[2];


vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z * sin(u);
    float z = point.x * sin(u) + point.z * cos(u);
    return vec3(x, point.y, z);
}


vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}


// This function computes B(t) with 4 control points. For a visual example,
// see [1]. For an interactive Javascript example with the exact points in
// this code, see [2].
//
// [1] https://www.desmos.com/calculator/cahqdxeshd
// [2] https://jsfiddle.net/6yuv9htf/
vec3 bezier(float t)
{
    return  control_p0 * pow((1 - t), 3) +
            control_p1 * 3 * t * pow((1 - t), 2) +
            control_p2 * 3 * pow(t, 2) * (1 - t) +
            control_p3 * pow(t, 3);
}


// TODO(student): If you want to take things a step further, try drawing a
// Hermite spline. Hint: you can repurpose two of the control points. For a
// visual example, see [1]. For an interactive Javascript example with the
// exact points in this code, see [2].
// 
// Unlike the Javascript function, you MUST NOT call the Bezier function.
// There is another way to draw a Hermite spline, all you need is to find
// the formula.
//
// [1] https://www.desmos.com/calculator/5knm5tkr8m
// [2] https://jsfiddle.net/6yuv9htf/

void main()
{
    const int SURFACE_TYPE_ROTATION     = 0;
    const int SURFACE_TYPE_TRANSLATION  = 1;

    // You can change the value of SURFACE_TYPE to experiment
    // with different transformation types.
    const int SURFACE_TYPE = SURFACE_TYPE_ROTATION;
    int i = instance[0];

    if (i < no_of_instances)
    {
        // TODO(student): Rather than emitting vertices for the control
        // points, you must emit vertices that approximate the curve itself.

        /* gl_Position = Projection * View * vec4(control_p0, 1);   EmitVertex();
        gl_Position = Projection * View * vec4(control_p1, 1);   EmitVertex();
        gl_Position = Projection * View * vec4(control_p2, 1);   EmitVertex();
        gl_Position = Projection * View * vec4(control_p3, 1);   EmitVertex();
        EndPrimitive(); */

        float t = 0.0;
        float step_t = 1.0 / float(no_of_generated_points - 1);
        float step_translate = max_translate / float(no_of_instances);
        float step_rotate = max_rotate / float(no_of_instances);
        float trans1 = i * step_translate, trans2 = (i + 1) * step_translate;
        float rot1 = i * step_rotate, rot2 = (i + 1) * step_rotate;
        for (int j = 0; j < no_of_generated_points - 1; ++j)
        {
            vec3 pos1, pos2, pos3, pos4;
            if (SURFACE_TYPE == SURFACE_TYPE_TRANSLATION)
            {
                pos1 = translateX(bezier(t), trans1);
                pos2 = translateX(bezier(t), trans2);
                pos3 = translateX(bezier(t + step_t), trans1);
                pos4 = translateX(bezier(t + step_t), trans2);
            }
            else
            {
                pos1 = rotateY(bezier(t), rot1);
                pos2 = rotateY(bezier(t), rot2);
                pos3 = rotateY(bezier(t + step_t), rot1);
                pos4 = rotateY(bezier(t + step_t), rot2);
            }

            gl_Position = Projection * View * Model * vec4(pos1, 1);
            EmitVertex();
            gl_Position = Projection * View * Model * vec4(pos2, 1);
            EmitVertex();
            gl_Position = Projection * View * Model * vec4(pos3, 1);
            EmitVertex();
            gl_Position = Projection * View * Model * vec4(pos4, 1);
            EmitVertex();
            EndPrimitive();

            t += step_t;
        }
    }
}
