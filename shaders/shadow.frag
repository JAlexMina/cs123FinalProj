#version 400 core
in vec4 fragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float distance = length(fragPos.xyz - lightPos);

    // map to [0;1] range
    distance = distance / far_plane;

    //write to the depth buffer by setting the special variable gl_FragDepth
    //if we do not use gl_FragDepth, then z component of gl_FragCoord would
    //be written to the depth buffer
    gl_FragDepth = distance;
}
