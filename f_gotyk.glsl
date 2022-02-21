#version 330

out vec4 pixelColor;

in vec2 gTexCoords;

uniform sampler2D texture0;

void main(void) {
	pixelColor = texture(texture0, gTexCoords) ;
}