#version 330

in vec4 vertex;
in vec4 normal;
in vec2 texCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
    FragPos = vec3(M * vertex);
    Normal = mat3(transpose(inverse(M))) * normal.xyz;  
    TexCoords = texCoords;
    
    gl_Position = P * V * vec4(FragPos, 1.0);
}