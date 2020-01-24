#version 400 core
out float frag_color;

in vec2 texc;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D noise_tex;

uniform vec3 samples[64];

uniform int screenwidth;
uniform int screenheight;

float kernel_size = 64.0;
uniform float radius;
float bias = .025;
uniform float power;

uniform mat4 projection;

void main()
{
    vec2 noise_scale = vec2(screenwidth / 4.0, screenheight / 4.0);
    vec3 frag_pos = texture(position_tex, texc).xyz;
    vec3 normal = normalize(texture(normal_tex, texc).rgb);
    vec3 random_vec = normalize(texture(noise_tex, texc * noise_scale).xyz);

    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    float sampz;
    for (int i = 0; i < kernel_size; i++) {
        vec3 samp = TBN * samples[i];
        samp = frag_pos + samp * radius;

        vec4 offset = vec4(samp, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * .5 + .5;

        float sample_depth = texture(position_tex, offset.xy).z;

        float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth));
        sampz = samp.z;
        occlusion += (sample_depth >= samp.z + bias ? 1.0 : 0.0) * range_check;
    }

    occlusion = 1.0 - (occlusion / kernel_size);

    frag_color = pow(occlusion,power);
}
