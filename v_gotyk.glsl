#version 330

in vec4 vertex;
in vec2 texCoords;

out vec2 gTexCoords;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main(void) {
	gTexCoords = texCoords;
	gl_Position = P*V*M*vertex;
}