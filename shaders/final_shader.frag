#version 400 core

out vec4 frag_color;

in vec2 texc;

uniform sampler2D viewpos_tex;
uniform sampler2D viewnormal_tex;
uniform sampler2D diffuse_tex;

uniform mat4 inverse_view;

uniform sampler2D ssao_tex;
uniform int use_ssao;
uniform int show_ssao_map;

// Light data
const int MAX_LIGHTS = 10;         // 0 for point, 1 for directional
uniform int lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights AND direcional lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightColors[MAX_LIGHTS];
uniform samplerCube depthCubes[MAX_LIGHTS];
uniform sampler2D depthmap;

// Shadow data
uniform int use_shadows;
uniform float far_plane;

/*
float calcPointShadow(vec3 fragPos, vec3 lightPos, int index)
{
    vec3 worldspace_pointToLight = fragPos - lightPos;
    float depthBufferVal = texture(depthCubes[index], worldspace_pointToLight).r;
    depthBufferVal = depthBufferVal * 25.0; //transform from [0,1] to [0, 25]

    float depthOfFrag = length(worldspace_pointToLight);
    float bias = 0.04;
    float shadow;
    if (depthOfFrag - bias > depthBufferVal) {
        shadow = 1.0;
    } else {
        shadow = 0.0;
    }
    return shadow;
}
*/

void main()
{
    vec2 flipped_texc = vec2(texc.x, 1 - texc.y);
    vec3 viewpos = texture(viewpos_tex, flipped_texc).xyz;
    vec3 viewnormal = normalize(texture(viewnormal_tex, flipped_texc).xyz);
    vec3 diffuse_color = texture(diffuse_tex, flipped_texc).xyz;
    vec3 specular_color = diffuse_color;
    vec3 ambient_color = vec3(.3 * diffuse_color);
    if (use_ssao != 0) {
        ambient_color *= pow(texture(ssao_tex, flipped_texc).r,5);
    }

    vec3 worldpos = vec3(inverse_view * vec4(viewpos, 1.0));
    vec3 worldnormal = normalize(mat3(inverse_view) * viewnormal);
    vec4 eye_direction = vec4(normalize(-viewpos), 0.0);

    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    float depthBufferVal;
    float depthOfFrag;

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lightTypes[i] == 0) {
            vec4 vertexToLight = vec4(normalize(lightPositions[i] - worldpos), 0.0);

            if (use_shadows == 0) {
                // compute diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, vec4(normalize(worldnormal),0.0)));
                diffuse += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);

                // Add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, vec4(normalize(worldnormal), 0.0)));
                float specIntensity = pow(max(0.0, dot(eye_direction, lightReflection)), 8.0); //put actual shininess in a texture
                specular += max (vec3(0), lightColors[i] * specular_color * specIntensity);
            } else {
                vec3 worldspace_pointToLight = worldpos - lightPositions[i];
                depthBufferVal = texture(depthCubes[i], worldspace_pointToLight).r;
                depthBufferVal = depthBufferVal * 25.0; //transform from [0,1] to [0, 25]

                depthOfFrag = length(worldspace_pointToLight);
                float bias = 0.04;
                float shadow;
                if (depthOfFrag - bias > depthBufferVal) {
                    shadow = 1.0;
                } else {
                    shadow = 0.0;
                }

                //compute diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, vec4(normalize(worldnormal),0.0)));
                diffuse += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity * (1.0 - shadow));

                // Add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, vec4(normalize(worldnormal),0.0)));
                float specIntensity = pow(max(0.0, dot(eye_direction, lightReflection)), 8.0); //put actual shininess in texture
                specular += max (vec3(0), lightColors[i] * specular_color * specIntensity * (1.0 - shadow));
            }
        } /*else if (lightTypes[i] == 1) {
            vec4 vertexToLight = vec4(normalize(-lightDirections[i]), 0.0);
            if (use_shadows == 0) {
                //add diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, vec4(normalize(worldspace_normal),0.0)));
                diffuse += max(vec3(0), lightColors[i] * diffuse_color_used * diffuseIntensity);

                //add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, vec4(normalize(worldspace_normal), 0.0)));
                vec4 eyeDirection = normalize(vec4(0,0,0,1) - vec4(worldspace_pos, 1.0));
                float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
                specular += max (vec3(0), lightColors[i] * specular_color * specIntensity);
            } else {
                float depthOfFrag = exp(lightspace_pos.z)/exp(1);
                vec2 depthmap_coords = vec2(lightspace_pos.x, lightspace_pos.y) * .5 + .5;
                float depthBufferVal = texture(depthmap, depthmap_coords).r;
                float bias = .05;
                float shadow;
                if (depthOfFrag - bias > depthBufferVal) {
                    shadow = 1.0;
                } else {
                    shadow = 0.0;
                }

                //compute diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, vec4(normalize(worldspace_normal),0.0)));
                diffuse += max(vec3(0), lightColors[i] * diffuse_color_used * diffuseIntensity * (1.0 - shadow));

                // Add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, vec4(normalize(worldspace_normal),0.0)));
                vec4 eyeDirection = normalize(vec4(0,0,0,1) - vec4(worldspace_pos, 1.0));
                float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
                specular += max (vec3(0), lightColors[i] * specular_color * specIntensity * (1.0 - shadow));
                //diffuse = vec3(depthOfFrag,depthOfFrag,depthOfFrag);
                if (depthOfFrag > 0 && depthOfFrag < 1) {
                    depthcheck = true;
                }
            }
        }
    }
    //fragColor = vec4(vec3(closest_depth), 1.0);
    if (depthcheck) {
        //fragColor = vec4(1.0, 0, 0, 1.0);
        */
    }

    frag_color = vec4(ambient_color + diffuse + specular, 1.0);
    if (show_ssao_map != 0) {
        float ssao_val = texture(ssao_tex, flipped_texc).r;
        frag_color = vec4(ssao_val, ssao_val, ssao_val,1.0);
    }
}
