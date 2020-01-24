#version 400 core

out vec4 fragColor; //final color of fragment

in vec2 texc;
in vec3 worldspace_pos;
in vec3 worldspace_normal;
in vec4 lightspace_pos;

// Light data
const int MAX_LIGHTS = 10;         // 0 for point, 1 for directional
uniform int lightTypes[MAX_LIGHTS];
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights AND direcional lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightColors[MAX_LIGHTS];
uniform samplerCube depthCubes[MAX_LIGHTS];
uniform sampler2D depthmap;

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

// Shadow data
uniform int use_shadows;
uniform float far_plane;

// Texture data
uniform sampler2D tex;
uniform int useTexture;

//uniform samplerCube depthMap;
//uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term

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

void main(){
    vec3 diffuse_color_used = diffuse_color;
    if (useTexture != 0) {
        vec3 texColor = texture(tex, texc).rgb;
        texColor = clamp(texColor, vec3(0), vec3(1));
        diffuse_color_used = .8 * texColor + .2 * diffuse_color;
    }
    bool depthcheck = false;
    vec3 ambient = ambient_color.xyz; // Add ambient component
    vec3 diffuse = vec3(0);
    vec3 specular = vec3(0);
    float closest_depth = 0;
    //currently only works for point lights
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lightTypes[i] == 0) {
            vec4 vertexToLight = vec4(normalize(lightPositions[i] - worldspace_pos), 0.0);

            if (use_shadows == 0) {
                // compute diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, vec4(normalize(worldspace_normal),0.0)));
                diffuse += max(vec3(0), lightColors[i] * diffuse_color_used * diffuseIntensity);

                // Add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, vec4(normalize(worldspace_normal), 0.0)));
                vec4 eyeDirection = normalize(vec4(0,0,0,1) - vec4(worldspace_pos, 1.0));
                float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
                specular += max (vec3(0), lightColors[i] * specular_color * specIntensity);
            } else {
                float shadow = calcPointShadow(worldspace_pos, lightPositions[i], i);

                //compute diffuse component
                float diffuseIntensity = max(0.0, dot(vertexToLight, vec4(normalize(worldspace_normal),0.0)));
                diffuse += max(vec3(0), lightColors[i] * diffuse_color_used * diffuseIntensity * (1.0 - shadow));

                // Add specular component
                vec4 lightReflection = normalize(-reflect(vertexToLight, vec4(normalize(worldspace_normal),0.0)));
                vec4 eyeDirection = normalize(vec4(0,0,0,1) - vec4(worldspace_pos, 1.0));
                float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
                specular += max (vec3(0), lightColors[i] * specular_color * specIntensity * (1.0 - shadow));
            }
        } else if (lightTypes[i] == 1) {
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
    fragColor = vec4(ambient + diffuse + specular, 1.0);
    //fragColor = vec4(vec3(closest_depth), 1.0);
    if (depthcheck) {
        //fragColor = vec4(1.0, 0, 0, 1.0);
    }
}
