#pragma once

#include "assimp/scene.h"
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "maths_funcs.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"

// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <cassert>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <fstream>
#include <iostream>
#include <sstream>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/Importer.hpp> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "Mesh.h"
#include "Camera.h"
#include "maths_funcs.h"
//#include "obj_parser.h"
#include "Shader.h"

using namespace std;
class Model {
public:
	//static vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	//Not Used
	std::vector<Mesh> meshes;
	//std::vector<Mesh> textures;
	mat4 location;
	Model();

	Model(const char* file_name);

	std::vector<Mesh> loadScene(const char* file_name);

	void RenderModel(mat4 transform, unsigned int shaderID);
	void RenderMesh(mat4 transform, unsigned int shaderID, int meshIndex);
	int getNumberMeshes();
	const Material& GetMaterial();
	void UpdatePos(int x, int y, Camera camera);

	void ProcessKeyboard(MovementDir direction, float deltaTime);

	float getTextureXOffset() {
		int column = textureIndex % m_Materials[0].pDiffuse.getNumberOfRows();
		return (float)column/(float)m_Materials[0].pDiffuse.getNumberOfRows();
	}

	float getTextureYOffset() {
		int row = textureIndex / m_Materials[0].pDiffuse.getNumberOfRows();
		return (float)row / (float)m_Materials[0].pDiffuse.getNumberOfRows();
	}
	std::vector<Material> m_Materials;
	int textureIndex = 6;
private:
	
	enum BUFFER_TYPE {
		INDEX_BUFFER = 0,
		POS_VB = 1,
		TEXCOORD_VB = 2,
		NORMAL_VB = 3,
		WVP_MAT_VB = 4,
		WORLD_MAT_VB = 5,
		NUM_BUFFERS = 6
	};
	GLuint m_VAO = 0;
	GLuint m_Buffers[NUM_BUFFERS] = { 0 };
	struct BasicMeshEntry {
		BasicMeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};

	std::vector<BasicMeshEntry> m_Meshes;
	

	// Temporary space for vertex stuff before we load them into the GPU
	std::vector<vec3> m_Positions;
	std::vector<vec3> m_Normals;
	std::vector<vec2> m_TexCoords;
	std::vector<unsigned int> m_Indices;

	
	void countVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);
	void setLoc(vec3 loc);
	bool InitFromScene(const aiScene* pScene, const char*& Filename);
	void ReserveSpace(unsigned int NumVertices, unsigned int NumIndices);
	void InitAllMeshes(const aiScene* pScene);
	void InitSingleMesh(const aiMesh* paiMesh);
	void PopulateBuffers();
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);

	void LoadColors(const aiMaterial* pMaterial, int index);

	void LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index);

	void LoadDiffuseTexture(const string& Dir, const aiMaterial* pMaterial, int index);

	void LoadSpecularTexture(const string& Dir, const aiMaterial* pMaterial, int index);

};