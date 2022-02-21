#version 330

out vec4 pixelColor;

in vec2 gTexCoords;

uniform sampler2D texture0;

void main(void) {
	vec4 churchDarkness = vec4(0.6, 0.6, 0.6, 1);
	pixelColor = texture(texture0, gTexCoords) * churchDarkness;
}