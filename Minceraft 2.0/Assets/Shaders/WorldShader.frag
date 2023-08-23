#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float light;

uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord);
    FragColor.rgb *= light;
}