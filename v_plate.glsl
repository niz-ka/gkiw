#version 330
in vec4 aPos; //a
in vec2 aTexCoords; //a
in vec4 aNormal; //a

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projection; //a
uniform mat4 view; //a
uniform mat4 model; //a

void main()
{
    TexCoords = aTexCoords;
    WorldPos = vec3(model * aPos);
    Normal = mat3(model) * aNormal.xyz;   

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}