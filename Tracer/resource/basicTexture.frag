#version 150 core

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D image;

void main()
{
    outColor = texture(image, Texcoord);
}
