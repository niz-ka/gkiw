#version 330

out vec4 color;
in vec2 f_texCoords;

uniform sampler2D texture0;

void main() {
	color = texture(texture0, f_texCoords);
}