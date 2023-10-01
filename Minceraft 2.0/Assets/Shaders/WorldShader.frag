#version 330 core
out vec4 FragColor;

in VertexData{
    vec2 TexCoord;
    float light;
    vec3 pos;
} vertexIn;

uniform sampler2D ourTexture;
uniform float fogStart;
uniform float fogEnd;
uniform vec3 cameraPos;

// https://vicrucann.github.io/tutorials/osg-shader-fog/

float getFogFactor(float d)
{
    if (d>=fogEnd) return 1;
    if (d<=fogStart) return 0;

    return 1 - (fogEnd - d) / (fogEnd - fogStart);
}

void main()
{
    FragColor = texture(ourTexture, vertexIn.TexCoord);
    if (FragColor.a < 0.1) {
        discard;
    }
    
    float d = distance(cameraPos, vertexIn.pos);
    float fogFactor = getFogFactor(d);
    float alpha = 1 - fogFactor;

    FragColor.rgb *= vertexIn.light;
    FragColor.a *= alpha;
}
