#version 400 core
//from https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
layout (triangles) in;
// we emit 3 vertices per triangle for 6 faces of the cubemap = 18 verts
// triangle_strip is the general way of giving geometry to fragment shader
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 fragPos; //we output a fragPos to the fragment shader every time we emit a vertex

void main()
{
    for(int face = 0; face < 6; face++) {
        // gl_Layer is a built-in variable that specifies which face of the cubemap we are rendering to
        gl_Layer = face;
        for(int i = 0; i < 3; i++) {
            fragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * fragPos;
            EmitVertex(); //emit gl_Position to the currently selected face
        }
        EndPrimitive();
    }
}
