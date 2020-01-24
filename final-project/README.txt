HOW TO USE THE PROGRAM:

Use the WASD keys to move around. Use the IJKL keys to look around.

To turn on shadows, check the "Shadows" checkbox. To turn on screen space
ambient occlusion, check the "Ambient Occlusion" checkbox. To view the texture
produced by the screen space ambient occlusion algorithm, check the "SSAO Map"
checkbox. The "Power" slider increases the effect of ambient occlusion. The
"Radius" slider increases the size of the kernel used in the ambient occlusion
algorithm.

DESIGN CHOICES:

We created a museum scene using textures, meshes, point shadows, and ambient
occlusion. The shadows are implemented using cubemaps. The scene is rendered
from the point of view of a point light and written to a cubemap. An object is
in shadow if its distance from the point light is greater than the depth in that
direction stored in the cubemap. For ambient occlusion, we calculate an
occlusion factor that determines how the ambient component of a fragment's
shading is darkened. To calculate this occlusion factor, we take samples in a
hemisphere oriented around the fragment's normal vector. The occlusion factor is
greater when there are more samples that lie inside the geometry rather than
outside.

BUGS:

The lighting is a little off.
