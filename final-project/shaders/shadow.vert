#version 400 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    //all the vertex shader needs to do is convert vertices to world space
    gl_Position = model * vec4(aPos, 1.0);
}
