#version 400 core
layout (location = 0) in vec3 position;

uniform mat4 light_mat;
uniform mat4 model;

void main()
{
    gl_Position = light_mat * model * vec4(position, 1.0);
}
