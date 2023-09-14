#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float cardinal_light;

out VertexData{
    vec2 TexCoord;
    float light;
    vec3 pos;
} vertexOut;

uniform mat4 MVP;
uniform ivec3 chunk_pos;

const ivec3 chunk_size = ivec3(48, 48, 48);

void main()
{
    vertexOut.TexCoord = aTexCoord;
    vertexOut.light = cardinal_light;
    vertexOut.pos = aPos + chunk_pos * chunk_size;
    gl_Position = MVP * vec4(vertexOut.pos, 1.0);
}