#version 330 core
out vec4 FragColor;

uniform vec4 lamp_fs;

void main()
{
    FragColor = vec4(1.0) * lamp_fs; // set alle 4 vector values to 1.0
}