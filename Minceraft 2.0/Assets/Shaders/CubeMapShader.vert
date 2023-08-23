#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 frag_coords;

uniform mat4 MVP;

void main()
{
    frag_coords = aPos;
    gl_Position = MVP * vec4(aPos, 1.0);
}