#version 330

in vec4 vertex;
in vec2 texCoords;

out vec2 f_texCoords;

void main() {
	//domy�lnie tekstury maj� odwr�cn� g�r�/d�
	f_texCoords = vec2(texCoords.x, texCoords.y * -1.f); 
	
	gl_Position = vertex;
}