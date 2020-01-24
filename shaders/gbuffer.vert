#version 400 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 5) in vec2 texcoords;

out vec3 frag_pos;
out vec2 texc;
out vec3 viewspace_normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewspace_pos = view * model * vec4(position, 1.0);
    frag_pos = viewspace_pos.xyz;
    texc = texcoords;
    viewspace_normal = transpose(inverse(mat3(view * model))) * normal;

    gl_Position = projection * viewspace_pos;
}
