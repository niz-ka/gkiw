#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

class Model {
private:
	
	std::vector <Mesh> meshes;
	glm::mat4 modelMatrix;
	std::vector <GLuint> extraTextures; //tekstury wspólne dla wielu meshy

public:

	Model(std::vector<glm::vec4> verts,
		std::vector<glm::vec2> texCoords,
		std::vector<unsigned int> indices) : modelMatrix(glm::mat4(1.0f)) {
		
		meshes.push_back(Mesh(verts, texCoords, indices));
	}
	
	//Wczytywanie modelu z pliku za pomoc¹ biblioteki assimp
	Model(const char* filename) : modelMatrix(glm::mat4(1.f)) {
		
		printf("=============================\nINFO::MODEL.H::MODEL()::LOADING MODEL FROM FILE::%s\n", filename);
		
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
		
		std::string errorString = importer.GetErrorString();
		if (errorString != "") {
			std::cout << "ERROR::MODEL.H::MODEL()::ASSIMP ERROR::" << errorString << "\n";
			exit(-1);
		}
		
		if (scene->HasMeshes()) {
			printf("INFO::MODEL.H::MODEL()::NUMBER OF MESHES: %d\n", scene->mNumMeshes);
		}

		else {
			printf("ERROR:MODEL.H::MODEL()::MODEL HAS NOT MESHES\n");
			exit(-1);
		}
	

		for (int i = 0; i < scene->mNumMeshes; i++) {

			this->meshes.push_back(Mesh());
			unsigned int meshIndex = meshes.size() - 1;

			aiMesh* mesh = scene->mMeshes[i];
			
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			printf("INFO::MODEL.H::MODEL()::MATERIAL HAS BEEN FOUND::%s\n", material->GetName().C_Str());

			
			unsigned int liczba_zest = mesh->GetNumUVChannels();

			if (liczba_zest > 1) {
				printf("WARNING:MODEL.H::MODEL()::MODEL HAS MULTIPLE TEXTURE SETS::%d\n", liczba_zest);
			}

			else if(liczba_zest == 0) {
				printf("WARNING::MODEL.H::MODEL()::MODEL HAS NOT TEXTURE SET\n");
			}

			if (mesh->mNumUVComponents[0] > 2) {
				printf("WARNING:MODEL.H::MODEL()::MODEL HAS 3D (OR MORE) TEX COORDS\n");
			}

			if (!mesh->HasNormals()) {
				printf("WARNING::MODEL.H::MODEL()::MODEL HAS NOT NORMALS\n");
			}


			for (int i = 0; i < mesh->mNumVertices; i++) {
				
				//Jeœli model nie posiada wspó³rzêdnych wierzcho³ków, normalnych b¹dŸ
				//wspó³rzêdnych teksturowania to poni¿sze linijki spowoduj¹ b³¹d krytyczny
				
				aiVector3D vertex = mesh->mVertices[i];
				meshes[meshIndex].pushVerts(glm::vec4(vertex.x, vertex.y, vertex.z, 1));
	
			
				aiVector3D normal = mesh->mNormals[i];
				meshes[meshIndex].pushNorms(glm::vec4(normal.x, normal.y, normal.z, 0));
				

				aiVector3D texCoord = mesh->mTextureCoords[0][i];
				meshes[meshIndex].pushTexCoords(glm::vec2(texCoord.x, texCoord.y));

				//tutaj mo¿na rozbudowaæ funkcjê o wczytywanie np. materia³ów, tekstur,
				//tangents, bitangents oraz wiele innych...

			}

			//Wczytaj krawêdzie
			for (int i= 0; i < mesh->mNumFaces; i++) {
				aiFace& face = mesh->mFaces[i];

				for (int j = 0; j < face.mNumIndices; j++) {
					meshes[meshIndex].pushIndices(face.mIndices[j]);
				}
			}
		
		}
	}
	
	//Ustaw po jednej teksturze na ka¿dy mesh
	void setTextures(std::vector<const char*> filenames) {
		
		if (filenames.size() > meshes.size()) {
			printf("ERROR::MODEL.H::SET_TEXTURES()::TOO MANY TEXTURES\n");
			exit(-1);
		}

		else if (filenames.size() < meshes.size()) {
			printf("WARNING::MODEL.H::SET_TEXTURES()::TOO FEW TEXTURES\n");
		}

		//Jeœli podano za ma³o tekstur - zacznij przypisywanie cyklicznie od pocz¹tku
		for (size_t i = 0; i < meshes.size(); i++) {
			meshes[i].setTexture(filenames[i % filenames.size()]);
		}
	}

	//Ustaw specjalne tekstury modelu, np. specular, roughness... za pomoc¹ biblioteki soil2
	void setExtraTextures(std::vector<const char*> filenames) {

		for (int i = 0; i < filenames.size(); ++i) {
			extraTextures.push_back(0);
			unsigned index = extraTextures.size() - 1;

			glGenTextures(1, &this->extraTextures[index]);
			glBindTexture(GL_TEXTURE_2D, this->extraTextures[index]);

			int width = 0;
			int height = 0;

			unsigned char* image = SOIL_load_image(filenames[i], &width, &height, NULL, SOIL_LOAD_RGBA);

			if (!image) {
				std::cout << "ERROR::MODEL.H::SET_EXTRA_TEXTURES()::COULD NOT LOAD THE TEXTURE " << filenames[i] << "\n";
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

			std::cout << "INFO::MODEL.H::SET_EXTRA_TEXTURES()::TEXTURE HAS LOADED::" << filenames[i] << "\n";
		}
	}

	Mesh* getMesh(int index) {
		
		if (index >= meshes.size()) {
			printf("ERROR::MODEL.H::GET_MESH()::INDEX OUT OF RANGE\n");
			exit(-1);
		}

		return &meshes[index];
	}

	size_t getNrOfMeshes() const {
		return meshes.size();
	}

	GLuint getExtraTexture(unsigned int index) {
		if (index >= extraTextures.size()) {
			printf("ERROR::MODEL.H::GET_EXTRA_TEXTURE()::INDEX OUT OF RANGE\n");
			exit(-1);
		}

		return extraTextures[index];
	}

	size_t getExtraTexturesSize() const {
		return extraTextures.size();
	}

	void rotateModel(float degrees, float x, float y, float z) {
		modelMatrix = rotate(modelMatrix, glm::radians(degrees), glm::vec3(x, y, z));
	}

	void scaleModel(float x, float y, float z) {
		modelMatrix = scale(modelMatrix, glm::vec3(x, y, z));
	}

	void translateModel(float x, float y, float z) {
		modelMatrix = translate(modelMatrix, glm::vec3(x, y, z));
	}

	glm::mat4 getModelMatrix() const {
		return modelMatrix;
	}

};