#version 400 core
layout (location = 0) out vec3 position_tex;
layout (location = 1) out vec3 normal_tex;
layout (location = 2) out vec4 diffuse_tex;

in vec3 frag_pos;
in vec2 texc;
in vec3 viewspace_normal;

uniform vec3 diffuse_color;
uniform sampler2D tex;
uniform int use_texture;

void main() {
    position_tex = frag_pos;
    normal_tex = normalize(viewspace_normal);
    vec3 diffuse_color_used = diffuse_color;
    if (use_texture != 0) {
        vec3 texColor = texture(tex, texc).rgb;
        texColor = clamp(texColor, vec3(0), vec3(1));
        diffuse_color_used = .8 * texColor + .2 * diffuse_color;
    }

    diffuse_tex.rgb = diffuse_color_used;
}
