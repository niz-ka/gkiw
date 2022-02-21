#version 330

in vec4 vertex;
in vec2 texCoords;

out vec2 f_texCoords;

void main() {
	//domyœlnie tekstury maj¹ odwrócn¹ górê/dó³
	f_texCoords = vec2(texCoords.x, texCoords.y * -1.f); 
	
	gl_Position = vertex;
}