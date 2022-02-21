#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include "shaderprogram.h"
#include "myCube.h" //warto pozostawić, np. na potrzeby testów
#include "Model.h"

//---------Zmienne na potrzeby kamery------------------------
int framebufferWidth = 0;
int framebufferHeight = 0;

glm::vec3 cameraPos = glm::vec3(5.783205, 2.186728, 0.123923);
glm::vec3 cameraFront = glm::vec3(0.953177, 0.057564, 0.296884);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;
float lastFrame = 0.0f;



bool tabKeyOn = true; //flaga - czy rysować info o TAB?

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

glm::mat4 V = glm::mat4(1.f);
glm::mat4 P = glm::mat4(1.f);

//-----------------------------------------------------

//Shadery
ShaderProgram *sculptureShader, *altarShader, *plateShader, *infoShader;

//Modele
Model *churchModel, *sculptureModel, *plateModel, *infoModel,*swordModel, *candelabraModel[2], *benchModel[2], *altarModel ;

//Wskaźnik specjalny na poruszanie wybranym modelem
Model* movingModel;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Obsługa klawisza ESC (wyjście z programu) oraz klawiszy ruchu (W, A, S, D)
void updateInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		cameraPos = glm::vec3(8.059840, 1.866094, 1.474472);
		cameraFront = glm::vec3(0.100878, -0.059306, -0.993130);
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		cameraPos = glm::vec3(9.242101, 2.189203, 0.598326);
		cameraFront = glm::vec3(0.959383, -0.029667, 0.280543);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		cameraPos = glm::vec3(1.165836, 3.965870, 2.664492);
		cameraFront = glm::vec3(0.972183, -0.231748, -0.033947);
	}
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	
	glClearColor(0.1f,0.1f,0.1f,1);
	glEnable(GL_DEPTH_TEST);

	//Załaduj shadery
	sculptureShader = new ShaderProgram("v_sculpture.glsl",NULL,"f_sculpture.glsl");
	altarShader = new ShaderProgram("v_church.glsl", NULL, "f_church.glsl");
	plateShader = new ShaderProgram("v_plate.glsl", NULL, "f_plate.glsl");
	infoShader = new ShaderProgram("v_info.glsl", NULL, "f_info.glsl");

	
	//candelabra
	for (int i = 0; i < 2; i++) {
		candelabraModel[i] = new Model("resources/candelabra/candelabra.obj");
		candelabraModel[i]->setExtraTextures(std::vector<const char*>{
			"resources/candelabra/candelabrum_basecolor.png",
				"resources/candelabra/candelabrum_normal.png",
				"resources/candelabra/candelabrum_roughness.png",
				"resources/candelabra/candelabrum_metallic.png",
				"resources/candelabra/candle_ambient_occlusion_map_from_mesh.png",
				"resources/candelabra/candle_normal_map_from_mesh.png"

		});
	}
	candelabraModel[0]->translateModel(16.028753, 0.2, 0.977976);
	candelabraModel[1]->translateModel(16.028753, 0.2, 4.428006);
	candelabraModel[0]->scaleModel(0.015, 0.015, 0.015);
	candelabraModel[1]->scaleModel(0.015, 0.015, 0.015);
	//Sword
	swordModel = new Model("resources/sword/sword.obj");
	swordModel->setExtraTextures(std::vector<const char*>{
		
			"resources/sword/Basecolor.png", 
			"resources/sword/Diffuse.png",
			"resources/sword/Normal_OpenGL.png",
			"resources/sword/Roughness.png",
			"resources/sword/Specular_F0.png",
				"resources/sword/Glossiness.png",
				"resources/sword/Metallic.png",
				"resources/sword/AO.png"
	});
	swordModel->translateModel(11.582589, 0.402083, 4.510784);
	swordModel->scaleModel(0.001, 0.001, 0.001);
	swordModel->rotateModel(90, 0, -1, 0);
	swordModel->rotateModel(27, -1, 0, 0);

	//Bench 
	for (int i = 0;i < 2;i++) {

		benchModel[i] = new Model("resources/bench/bench.fbx");
		

		benchModel[i]->rotateModel(90, 0, 1, 0);
		if (i < 4) {
			benchModel[i]->translateModel(-1, 0.2, 12 - i * 2);
		}
		else {
			benchModel[i]->translateModel(-4.3, 0.2, 12 - (i - 4) * 2);
		}
		benchModel[i]->scaleModel(0.15, 0.15, 0.15);
	}
	benchModel[0]->setExtraTextures(std::vector<const char*>{
		"resources/bench/Madieval_bench_m_medieval_bench_Diffuse.jpg",
			"resources/bench/Madieval_bench_m_medieval_bench_Height.jpg",
			"resources/bench/Madieval_bench_m_medieval_bench_ior.jpg",
			"resources/bench/Madieval_bench_m_medieval_bench_Normal.jpg",
			"resources/bench/Madieval_bench_m_medieval_bench_Roughness.jpg",
			"resources/bench/Madieval_bench_m_medieval_bench_Specular.jpg"


	});

	//altar
	altarModel = new Model("resources/altar/Altar.obj");
	altarModel->setTextures(std::vector<const char*>{
			"resources/altar/Stone.png"
	});
	altarModel->translateModel(16, 0.2, 2.6);
	
	//Model główny - kościół
	churchModel = new Model("resources/church/model.obj");

	
	//Tekstury kościoła (zwykłe)
	churchModel->setTextures(std::vector<const char*>{
		"resources/church/tex1.jpg",
		"resources/church/tex2.jpg",
		"resources/church/tex3.jpg",
		"resources/church/tex4.jpg"
	});

	//Odpowiednie ustawienie kościoła
	churchModel->rotateModel(90, 0, 0, 1);
	churchModel->rotateModel(90, 0, 1, 0); 
	churchModel->rotateModel(90, 0, 0, 1);
	churchModel->translateModel(-6.000000, 2.800000, 0.000000);

	//Model - rzeźba
	sculptureModel = new Model("resources/sculpture/model.obj");
	
	//Tekstura rzeźby (zwykła), dwa razy ta sama
	sculptureModel->setTextures(std::vector<const char*>{
		"resources/sculpture/tex.jpg",
		"resources/sculpture/tex.jpg"
	});

	//Odpowiednie ustawienie rzeźby
	sculptureModel->translateModel(8.980000, 1.110000, -3.960000);
	sculptureModel->scaleModel(0.25, 0.25, 0.25);
	sculptureModel->rotateModel(90, 0, 1, 0);
	sculptureModel->rotateModel(90, 0, -1, 0);


	//Model - napierśnik
	plateModel = new Model("resources/plate/model.dae");
	
	//Napierśnik - tekstury (PBR)
	plateModel->setExtraTextures(std::vector<const char*>{
		"resources/plate/albedo.jpeg",
		"resources/plate/normal.png",
		"resources/plate/metallic.jpeg",
		"resources/plate/roughness.jpeg",
		"resources/plate/ao.jpeg"
	});

	//Odpowiednie ustawienie napierśnika
	plateModel->translateModel(16,1.45,2.6);
	plateModel->scaleModel(0.5f, 0.5f, 0.5f);
	plateModel->rotateModel(90, 0, -1, 0);

	//Model 2D - info o klawiszu TAB - konstruktor new Model(vertex, texCoords, indices)
	infoModel = new Model(
		std::vector <glm::vec4> //wierzchołki
		{
			glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
			glm::vec4(-1.0f, 0.8f, 0.0f, 1.0f),
			glm::vec4(-0.8f, 0.8f, 0.0f, 1.0f),
			glm::vec4(-0.8f, 1.0f, 0.0f, 1.0f)
		},
		
		std::vector <glm::vec2> //współrzędne teksturowania
			{
				glm::vec2(0.0f, 1.0f),
				glm::vec2(0.0f, 0.0f),
				glm::vec2(1.0f, 0.0f),
				glm::vec2(1.0f, 1.0f)
			},

		std::vector <unsigned int> //krawędzie
			{
				0, 1, 2,
				0, 2, 3
			}
	);
	
	//Tekstura informacji o klawiszu TAB (lewy górny róg)
	infoModel->setTextures(std::vector<const char*>{"resources/info/tab.png"});

	//Ustawienie tego wskaźnika na obiekt Model umożliwia
	//poruszanie (translate) wybranym modelem podczas trwania programu
	//za pomocą klawiszy strzałek (góra, dół, prawo, lewo) oraz page up i page down
	//w konsoli następuje też pomocne przy rozmieszczaniu modelu wypisanie macierzy modelu (M)
	movingModel = swordModel;

}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    
	//Usuń shadery
	delete sculptureShader, altarShader, plateShader, infoShader;
	
	//Usuń modele
	delete sculptureModel, churchModel, plateModel, infoModel, swordModel, candelabraModel, benchModel, altarModel;
}



//rysowanie miecz
void drawSword() {

	plateShader->use();

	glUniformMatrix4fv(plateShader->u("projection"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(plateShader->u("view"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(plateShader->u("model"), 1, false, glm::value_ptr(swordModel->getModelMatrix()));

	glUniform3fv(plateShader->u("camPos"), 1, glm::value_ptr(cameraPos));

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	glm::vec3 lightColors[] = {
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f)
	};

	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		std::string posString = "lightPositions[" + std::to_string(i) + "]";
		std::string colString = "lightColors[" + std::to_string(i) + "]";
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		glUniform3fv(plateShader->u(posString.c_str()), 1, glm::value_ptr(newPos));
		glUniform3fv(plateShader->u(colString.c_str()), 1, glm::value_ptr(lightColors[i]));
	}

	glUniform1i(plateShader->u("albedoMap"), 0);
	glUniform1i(plateShader->u("normalMap"), 1);
	glUniform1i(plateShader->u("metallicMap"), 2);
	glUniform1i(plateShader->u("roughnessMap"), 3);
	glUniform1i(plateShader->u("aoMap"), 4);

	for (int i = 0; i < swordModel->getNrOfMeshes(); ++i) {

		Mesh* mesh = swordModel->getMesh(i);

		for (unsigned j = 0; j < swordModel->getExtraTexturesSize(); ++j) {
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, swordModel->getExtraTexture(j));
		}

		glEnableVertexAttribArray(plateShader->a("aPos"));
		glVertexAttribPointer(plateShader->a("aPos"), 4, GL_FLOAT, false, 0, mesh->getVerts());

		glEnableVertexAttribArray(plateShader->a("aNormal"));
		glVertexAttribPointer(plateShader->a("aNormal"), 4, GL_FLOAT, false, 0, mesh->getNorms());

		glEnableVertexAttribArray(plateShader->a("aTexCoords"));
		glVertexAttribPointer(plateShader->a("aTexCoords"), 2, GL_FLOAT, false, 0, mesh->getTexCoords());


		glDrawElements(GL_TRIANGLES, mesh->getIndicesSize(), GL_UNSIGNED_INT, mesh->getIndices());

		glDisableVertexAttribArray(plateShader->a("aPos"));
		glDisableVertexAttribArray(plateShader->a("aNormal"));
		glDisableVertexAttribArray(plateShader->a("aTexCoords"));

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//rysowanie candelabra
void drawCandelabra(int k) {

	plateShader->use();

	glUniformMatrix4fv(plateShader->u("projection"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(plateShader->u("view"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(plateShader->u("model"), 1, false, glm::value_ptr(candelabraModel[k]->getModelMatrix()));

	glUniform3fv(plateShader->u("camPos"), 1, glm::value_ptr(cameraPos));

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	glm::vec3 lightColors[] = {
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f)
	};

	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		std::string posString = "lightPositions[" + std::to_string(i) + "]";
		std::string colString = "lightColors[" + std::to_string(i) + "]";
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		glUniform3fv(plateShader->u(posString.c_str()), 1, glm::value_ptr(newPos));
		glUniform3fv(plateShader->u(colString.c_str()), 1, glm::value_ptr(lightColors[i]));
	}

	glUniform1i(plateShader->u("albedoMap"), 0);
	glUniform1i(plateShader->u("normalMap"), 1);
	glUniform1i(plateShader->u("metallicMap"), 2);
	glUniform1i(plateShader->u("roughnessMap"), 3);
	glUniform1i(plateShader->u("aoMap"), 4);

	for (int i = 0; i < candelabraModel[k]->getNrOfMeshes(); ++i) {

		Mesh* mesh = candelabraModel[k]->getMesh(i);

		for (unsigned j = 0; j < candelabraModel[k]->getExtraTexturesSize(); ++j) {
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, candelabraModel[k]->getExtraTexture(j));
		}

		glEnableVertexAttribArray(plateShader->a("aPos"));
		glVertexAttribPointer(plateShader->a("aPos"), 4, GL_FLOAT, false, 0, mesh->getVerts());

		glEnableVertexAttribArray(plateShader->a("aNormal"));
		glVertexAttribPointer(plateShader->a("aNormal"), 4, GL_FLOAT, false, 0, mesh->getNorms());

		glEnableVertexAttribArray(plateShader->a("aTexCoords"));
		glVertexAttribPointer(plateShader->a("aTexCoords"), 2, GL_FLOAT, false, 0, mesh->getTexCoords());


		glDrawElements(GL_TRIANGLES, mesh->getIndicesSize(), GL_UNSIGNED_INT, mesh->getIndices());

		glDisableVertexAttribArray(plateShader->a("aPos"));
		glDisableVertexAttribArray(plateShader->a("aNormal"));
		glDisableVertexAttribArray(plateShader->a("aTexCoords"));

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//rysowanie bench
void drawBench(int k) {

	plateShader->use();

	glUniformMatrix4fv(plateShader->u("projection"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(plateShader->u("view"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(plateShader->u("model"), 1, false, glm::value_ptr(benchModel[k]->getModelMatrix()));

	glUniform3fv(plateShader->u("camPos"), 1, glm::value_ptr(cameraPos));

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};

	glm::vec3 lightColors[] = {
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f)
	};

	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		std::string posString = "lightPositions[" + std::to_string(i) + "]";
		std::string colString = "lightColors[" + std::to_string(i) + "]";
		glm::vec3 newPos = lightPositions[i] + glm::vec3(glfwGetTime() * 5.0 * (100%5), 0.0, 0.0);
		glUniform3fv(plateShader->u(posString.c_str()), 1, glm::value_ptr(newPos));
		glUniform3fv(plateShader->u(colString.c_str()), 1, glm::value_ptr(lightColors[i]));
	}

	glUniform1i(plateShader->u("albedoMap"), 0);
	glUniform1i(plateShader->u("normalMap"), 1);
	glUniform1i(plateShader->u("metallicMap"), 2);
	glUniform1i(plateShader->u("roughnessMap"), 3);
	glUniform1i(plateShader->u("aoMap"), 4);

	for (int i = 0; i < benchModel[k]->getNrOfMeshes(); ++i) {

		Mesh* mesh = benchModel[k]->getMesh(i);

		for (unsigned j = 0; j < benchModel[0]->getExtraTexturesSize(); ++j) {
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, benchModel[0]->getExtraTexture(j));
		}

		glEnableVertexAttribArray(plateShader->a("aPos"));
		glVertexAttribPointer(plateShader->a("aPos"), 4, GL_FLOAT, false, 0, mesh->getVerts());

		glEnableVertexAttribArray(plateShader->a("aNormal"));
		glVertexAttribPointer(plateShader->a("aNormal"), 4, GL_FLOAT, false, 0, mesh->getNorms());

		glEnableVertexAttribArray(plateShader->a("aTexCoords"));
		glVertexAttribPointer(plateShader->a("aTexCoords"), 2, GL_FLOAT, false, 0, mesh->getTexCoords());


		glDrawElements(GL_TRIANGLES, mesh->getIndicesSize(), GL_UNSIGNED_INT, mesh->getIndices());

		glDisableVertexAttribArray(plateShader->a("aPos"));
		glDisableVertexAttribArray(plateShader->a("aNormal"));
		glDisableVertexAttribArray(plateShader->a("aTexCoords"));

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//atar 
void drawAltar() {

	altarShader->use();

	glUniformMatrix4fv(altarShader->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(altarShader->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(altarShader->u("M"), 1, false, glm::value_ptr(altarModel->getModelMatrix()));

	glUniform1i(altarShader->u("texture0"), 0);

	for (int i = 0; i < altarModel->getNrOfMeshes(); i++) {

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, altarModel->getMesh(i)->getTexture());

		glEnableVertexAttribArray(altarShader->a("vertex"));
		glVertexAttribPointer(altarShader->a("vertex"), 4, GL_FLOAT, false, 0, altarModel->getMesh(i)->getVerts());

		glEnableVertexAttribArray(altarShader->a("texCoords"));
		glVertexAttribPointer(altarShader->a("texCoords"), 2, GL_FLOAT, false, 0, altarModel->getMesh(i)->getTexCoords());

		glDrawElements(GL_TRIANGLES, altarModel->getMesh(i)->getIndicesSize(), GL_UNSIGNED_INT, altarModel->getMesh(i)->getIndices());

		glDisableVertexAttribArray(altarShader->a("vertex"));
		glDisableVertexAttribArray(altarShader->a("texCoords"));

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//Procedura rysująca kościół
void drawChurch() {
	
	altarShader->use();

	glUniformMatrix4fv(altarShader->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(altarShader->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(altarShader->u("M"), 1, false, glm::value_ptr(churchModel->getModelMatrix()));

	glUniform1i(altarShader->u("texture0"), 0);

	for (int i = 0; i < churchModel->getNrOfMeshes(); i++) {
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, churchModel->getMesh(i)->getTexture());

		glEnableVertexAttribArray(altarShader->a("vertex"));
		glVertexAttribPointer(altarShader->a("vertex"), 4, GL_FLOAT, false, 0, churchModel->getMesh(i)->getVerts() );

		glEnableVertexAttribArray(altarShader->a("texCoords"));
		glVertexAttribPointer(altarShader->a("texCoords"), 2, GL_FLOAT, false, 0, churchModel->getMesh(i)->getTexCoords() );
		
		glDrawElements(GL_TRIANGLES, churchModel->getMesh(i)->getIndicesSize(), GL_UNSIGNED_INT, churchModel->getMesh(i)->getIndices() ); 

		glDisableVertexAttribArray(altarShader->a("vertex"));
		glDisableVertexAttribArray(altarShader->a("texCoords"));

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//Procedura rysująca rzeźbę
void drawSculpture() {

	sculptureShader->use();

	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sculptureShader->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(sculptureShader->u("V"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(sculptureShader->u("M"), 1, false, glm::value_ptr(sculptureModel->getModelMatrix()));

	glUniform3fv(sculptureShader->u("light.position"), 1, glm::value_ptr(cameraPos));
	glUniform3fv(sculptureShader->u("light.direction"), 1, glm::value_ptr(cameraFront));
	glUniform3fv(sculptureShader->u("viewPos"), 1, glm::value_ptr(cameraPos));

	glUniform1f(sculptureShader->u("light.cutOff"), glm::cos(glm::radians(12.5f)));
	glUniform1f(sculptureShader->u("light.outerCutOff"), glm::cos(glm::radians(17.5f)));

	glUniform3fv(sculptureShader->u("light.ambient"), 1, glm::value_ptr(glm::vec3(0.5f, 0.5f, 0.5f)));
	glUniform3fv(sculptureShader->u("light.diffuse"), 1, glm::value_ptr(glm::vec3(0.8f, 0.8f, 0.8f)));
	glUniform3fv(sculptureShader->u("light.specular"), 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

	glUniform1f(sculptureShader->u("light.constant"), 0.6f);
	glUniform1f(sculptureShader->u("light.linear"), 0.05f); //0.05
	glUniform1f(sculptureShader->u("light.quadratic"), 0.0003f);

	glUniform1f(sculptureShader->u("material.shininess"), 32.0f);

	glUniform1i(sculptureShader->u("material.diffuse"), 0);
	glUniform1i(sculptureShader->u("material.specular"), 1);

	for (int i = 0; i < sculptureModel->getNrOfMeshes(); ++i) {

		Mesh* mesh = sculptureModel->getMesh(i);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->getTexture());

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->getTexture()); 

		glEnableVertexAttribArray(sculptureShader->a("vertex")); 
		glVertexAttribPointer(sculptureShader->a("vertex"), 4, GL_FLOAT, false, 0, mesh->getVerts());

		glEnableVertexAttribArray(sculptureShader->a("texCoords"));
		glVertexAttribPointer(sculptureShader->a("texCoords"), 2, GL_FLOAT, false, 0, mesh->getTexCoords()); 

		glEnableVertexAttribArray(sculptureShader->a("normal")); 
		glVertexAttribPointer(sculptureShader->a("normal"), 4, GL_FLOAT, false, 0, mesh->getNorms()); 

		glDrawElements(GL_TRIANGLES, mesh->getIndicesSize(), GL_UNSIGNED_INT, mesh->getIndices());

		glDisableVertexAttribArray(sculptureShader->a("vertex")); 
		glDisableVertexAttribArray(sculptureShader->a("texCoords")); 
		glDisableVertexAttribArray(sculptureShader->a("normal")); 

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//Procedura rysująca napierśnik
void drawPlate() {
	
	plateShader->use();

	glUniformMatrix4fv(plateShader->u("projection"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(plateShader->u("view"), 1, false, glm::value_ptr(V));
	glUniformMatrix4fv(plateShader->u("model"), 1, false, glm::value_ptr(plateModel->getModelMatrix()));

	glUniform3fv(plateShader->u("camPos"), 1, glm::value_ptr(cameraPos));

	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f,  10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	
	glm::vec3 lightColors[] = {
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f),
		glm::vec3(1000.0f, 1000.0f, 1000.0f)
	};

	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		std::string posString = "lightPositions[" + std::to_string(i) + "]";
		std::string colString = "lightColors[" + std::to_string(i) + "]";
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		glUniform3fv(plateShader->u(posString.c_str()), 1, glm::value_ptr(newPos));
		glUniform3fv(plateShader->u(colString.c_str()), 1, glm::value_ptr(lightColors[i]));
	}

	glUniform1i(plateShader->u("albedoMap"), 0);
	glUniform1i(plateShader->u("normalMap"), 1);
	glUniform1i(plateShader->u("metallicMap"), 2);
	glUniform1i(plateShader->u("roughnessMap"), 3);
	glUniform1i(plateShader->u("aoMap"), 4);

	for (int i = 0; i < plateModel->getNrOfMeshes(); ++i) {

		Mesh* mesh = plateModel->getMesh(i);

		for (unsigned j = 0; j < plateModel->getExtraTexturesSize(); ++j) {
			glActiveTexture(GL_TEXTURE0 + j);
			glBindTexture(GL_TEXTURE_2D, plateModel->getExtraTexture(j));
		}

		glEnableVertexAttribArray(plateShader->a("aPos"));  
		glVertexAttribPointer(plateShader->a("aPos"), 4, GL_FLOAT, false, 0, mesh->getVerts()); 

		glEnableVertexAttribArray(plateShader->a("aNormal"));
		glVertexAttribPointer(plateShader->a("aNormal"), 4, GL_FLOAT, false, 0, mesh->getNorms()); 

		glEnableVertexAttribArray(plateShader->a("aTexCoords")); 
		glVertexAttribPointer(plateShader->a("aTexCoords"), 2, GL_FLOAT, false, 0, mesh->getTexCoords()); 


		glDrawElements(GL_TRIANGLES, mesh->getIndicesSize(), GL_UNSIGNED_INT, mesh->getIndices());

		glDisableVertexAttribArray(plateShader->a("aPos"));
		glDisableVertexAttribArray(plateShader->a("aNormal"));
		glDisableVertexAttribArray(plateShader->a("aTexCoords"));

		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);
}

//Procedura rysująca info o klawiszu TAB (lewy górny róg)
void drawInfo() {
	infoShader->use();

	for (int i = 0; i < infoModel->getNrOfMeshes(); ++i) {

		Mesh* mesh = infoModel->getMesh(i);
		
		glUniform1i(infoShader->u("texture0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->getTexture());

		glEnableVertexAttribArray(infoShader->a("vertex"));
		glVertexAttribPointer(infoShader->a("vertex"), 4, GL_FLOAT, false, 0, mesh->getVerts());

		glEnableVertexAttribArray(infoShader->a("texCoords"));
		glVertexAttribPointer(infoShader->a("texCoords"), 2, GL_FLOAT, false, 0, mesh->getTexCoords());

		glDrawElements(GL_TRIANGLES, mesh->getIndicesSize(), GL_UNSIGNED_INT, mesh->getIndices());

		glDisableVertexAttribArray(infoShader->a("vertex"));
		glDisableVertexAttribArray(infoShader->a("texCoords"));
	}
	glUseProgram(0);
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	P = glm::perspective(glm::radians(fov), static_cast<float>(framebufferWidth) / framebufferHeight, 0.1f, 50.0f);

	//Rysowanie modeli
	if(tabKeyOn) drawInfo();
	//drawGotyk();
	drawSword();
	drawAltar();
	drawCandelabra(0);
	drawCandelabra(1);
	for (int i = 0;i < 2;i++) {
		drawBench(i);
	}
	drawChurch();
	drawSculpture();
	drawPlate();
	//----------------

    glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

//Zmiana wielkości okna
void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH) {
	glViewport(0, 0, fbW, fbH);
}

//Obsługa myszy - poruszanie kamerą
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

//Obsługa myszy - zoom za pomocą kółka myszy
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 45.0f)
		fov = 45.0f;
}

//Obsługa klawisza TAB (odblokowanie myszy), wypisanie macierzy modelu na konsolę, obsługa
//poruszania modelem za pomocą strzałek i page up/down
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetScrollCallback(window, scroll_callback);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			tabKeyOn = false;
		}

		else {
			glfwSetCursorPosCallback(window, nullptr);
			glfwSetScrollCallback(window, nullptr);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			tabKeyOn = true;
		}

		std::string wynik = glm::to_string(movingModel->getModelMatrix());
		std::cout << "\nwynik: " << wynik << "\n";
		std::cout << "\npoyzcja kamery: " << glm::to_string(cameraPos) << "\n";
		std::cout << "\nfront kamery: " << glm::to_string(cameraFront) << "\n";
	}

	else if (key == GLFW_KEY_UP) {
		movingModel->translateModel(0, 1, 0);
	}

	else if (key == GLFW_KEY_DOWN) {
		movingModel->translateModel(0, -1, 0);
	}

	else if (key == GLFW_KEY_LEFT) {
		movingModel->translateModel(-1, 0, 0);
	}

	else if (key == GLFW_KEY_RIGHT) {
		movingModel->translateModel(1, 0, 0);
	}

	else if (key == GLFW_KEY_PAGE_UP) {
		movingModel->translateModel(0, 0, -1);
	}

	else if (key == GLFW_KEY_PAGE_DOWN) {
		movingModel->translateModel(0, 0, 1);
	}

}

int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów	

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1000, 600, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}
	glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla

	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
		
		updateInput(window);

		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);
	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}

