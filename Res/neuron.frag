#version 330 core
out vec4 FragColor;

uniform vec4 drawColor;

void main()
{
    FragColor = drawColor;
}