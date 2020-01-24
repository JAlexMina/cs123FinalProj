#version 400 core

out float frag_color;

in vec2 texc;

uniform sampler2D ssao_texture;

uniform int texwidth;
uniform int texheight;

void main()
{
    vec2 texel_size = 1.0 / vec2(texwidth, texheight);
    float result = 0.0;
    for (int x = -2; x < 2; x++) {
        for (int y = -2; y < 2; y++) {
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            vec2 to_sample = texc + offset;
            result += texture(ssao_texture, to_sample).r;
        }
    }
    frag_color = result / 16.0;
}
