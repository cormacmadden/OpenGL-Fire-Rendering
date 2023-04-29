#pragma once

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

#include "maths_funcs.h"
//#include "obj_parser.h"

#include "Material.h"
#include "Shader.h"
using namespace std;
struct Vertex {
	vec3 m_Positions;
	vec3 m_Normals;
	vec2 m_TexCoords;
	vec3 m_Tangents;
	vec3 m_Bit_Tangents;
	std::vector<unsigned int> m_Indices;

};
#define INVALID_MATERIAL 0xFFFFFFFF
class Mesh {

	public:
		vector<Vertex> vertices;
		const char* name;
		vec3 transformMat;
		Mesh();
		Mesh(const aiMesh* mesh, unsigned int& TotalVertices, unsigned int& TotalIndices);
		Mesh(const vector<Vertex> vertices, const vector<unsigned int> indices, const vector<Texture> textures);

		//void generateObjectBufferMesh(Shader shaderProgram);
		void PopulateBuffers();
		void RenderMesh(mat4 transform, unsigned int shaderID);

		void LoadDiffuseTexture(const string& Dir, const aiMaterial* pMaterial, int index);

		void LoadNormalTexture(const string& Dir, const aiMaterial* pMaterial, int index);

		void LoadSpecularTexture(const string& Dir, const aiMaterial* pMaterial, int index);

		void LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index);

		void LoadColors(const aiMaterial* pMaterial, int index);

		void InitMeshMaterials(const string& Dir, const aiScene* pScene);
		std::vector<Material> m_Materials;
		int NumIndices = 0;
		unsigned int MaterialIndex = INVALID_MATERIAL;
	private:
		unsigned int VBO, EBO;

		enum BUFFER_TYPE {
			INDEX_BUFFER = 0,
			POS_VB = 1,
			TEXCOORD_VB = 2,
			NORMAL_VB = 3,
			WVP_MAT_VB = 4,
			WORLD_MAT_VB = 5,
			NUM_BUFFERS = 6
		};
		GLuint VAO = 0;
		GLuint m_Buffers[NUM_BUFFERS] = { 0 };
		
		unsigned int BaseVertex = 0;
		unsigned int BaseIndex = 0;
		




};