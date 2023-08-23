#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float cardinal_light;

out vec2 TexCoord;
out float light;

uniform mat4 MVP;
uniform ivec3 chunk_pos;

const ivec3 chunk_size = ivec3(32, 32, 32);

void main()
{
    gl_Position = MVP * vec4(aPos + chunk_pos * chunk_size, 1.0);
    TexCoord = aTexCoord;
    light = cardinal_light;
}