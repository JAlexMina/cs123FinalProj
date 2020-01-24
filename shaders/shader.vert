#version 400 core

layout(location = 0) in vec3 position; // Position of the vertex
layout(location = 1) in vec3 normal;   // Normal of the vertex
layout(location = 5) in vec2 texCoord; // UV texture coordinates

out vec2 texc;
out vec3 worldspace_pos;
out vec3 worldspace_normal;
out vec4 lightspace_pos;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 light_transform;

void main()
{
    gl_Position = p * v * m * vec4(position, 1.0);

    texc = texCoord;
    worldspace_pos = vec3(m * vec4(position, 1.0));
    worldspace_normal = normalize(mat3(transpose(inverse(m))) * normal);
    lightspace_pos = light_transform * m * vec4(position, 1.0);
}
