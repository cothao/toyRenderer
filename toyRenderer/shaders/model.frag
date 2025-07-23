#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D base_map1;

void main()
{    
    FragColor = texture(base_map1, TexCoords);
}