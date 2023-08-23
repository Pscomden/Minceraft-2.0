#version 330 core
in vec3 frag_coords;

out vec4 FragColor;

uniform samplerCube cube_map;

void main()
{
    FragColor = texture(cube_map, frag_coords);
}