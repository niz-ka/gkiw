#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#include <soil2.h>

class Mesh {

private:

	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;
	GLuint texture;

public:

	Mesh() : texture(0) {}

	Mesh(std::vector<glm::vec4> verts, 
		std::vector<glm::vec2> texCoords,
		std::vector<unsigned int> indices) 
		: verts(verts), texCoords(texCoords), indices(indices), texture(0) {}

	inline void pushVerts(glm::vec4 vertex) {
		verts.push_back(vertex);
	}

	inline void pushNorms(glm::vec4 normal) {
		norms.push_back(normal);
	}

	inline void pushTexCoords(glm::vec2 texCoord) {
		texCoords.push_back(texCoord);
	}

	inline void pushIndices(unsigned int index) {
		indices.push_back(index);
	}

	//Wczytywanie tekstury za pomoc¹ biblioteki soil2
	void setTexture(const char* filename) {

		glGenTextures(1, &this->texture);
		glBindTexture(GL_TEXTURE_2D, this->texture);

		int width = 0;
		int height = 0;

		unsigned char* image = SOIL_load_image(filename, &width, &height, NULL, SOIL_LOAD_RGBA);

		if (!image) {
			std::cout << "ERROR::MESH.H::SET_TEXTURE()::COULD NOT LOAD THE TEXTURE " << filename << "\n";
			exit(-1);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		SOIL_free_image_data(image);

		std::cout << "INFO::MESH.H::SET_TEXTURE()::TEXTURE HAS LOADED::" << filename << "\n";

	}

	GLuint getTexture() const {
		return texture;
	}
	
	
	glm::vec4* getVerts() {
		return verts.data();
	}

	glm::vec4* getNorms() {
		return norms.data();
	}

	glm::vec2* getTexCoords() {
		return texCoords.data();
	}

	unsigned int* getIndices() {
		return indices.data();
	}

	size_t getIndicesSize() const {
		return indices.size();
	}

};



