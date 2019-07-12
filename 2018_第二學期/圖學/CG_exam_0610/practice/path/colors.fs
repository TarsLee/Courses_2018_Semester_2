#version 330 core
out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;

uniform vec4 color_fs;

void main()
{
    FragColor = vec4(lightColor * objectColor, 1.0) * color_fs;
}