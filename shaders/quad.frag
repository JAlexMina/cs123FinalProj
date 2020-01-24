#version 400 core

in vec2 texc;

out vec4 color;

uniform sampler2D depthmap;

void main()
{
    vec4 tex_color = texture(depthmap, texc);
    color = vec4(tex_color.r, tex_color.r, tex_color.r, 1.0);
    //color = vec4(tex_color.rgb, 1.0);
}
