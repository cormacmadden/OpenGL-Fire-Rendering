
#include "Mesh.h"
#include <vector>
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2
#define COLOR_TEXTURE_UNIT              GL_TEXTURE0
#define NORMAL_TEXTURE_UNIT              GL_TEXTURE1
#define COLOR_TEXTURE_UNIT_INDEX		0
#define SPECULAR_EXPONENT_UNIT             GL_TEXTURE6


using namespace std;

Mesh::Mesh() {};
Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

/*
Mesh::Mesh(const aiMesh* mesh, unsigned int& TotalVertices, unsigned int& TotalIndices) {
	printf("%i vertices in mesh\n", mesh->mNumVertices);
	//mPointCount += mesh->mNumVertices;
	//name = aiName;
	std::cout << mesh->mName.data << '\n';
	m_Positions.reserve(mesh->mNumVertices);
	m_Normals.reserve(mesh->mNumVertices);
	m_TexCoords.reserve(mesh->mNumVertices);
	m_Indices.reserve(mesh->mNumFaces * 3);
	MaterialIndex = mesh->mMaterialIndex;
	NumIndices = mesh->mNumFaces * 3;
	BaseVertex = TotalVertices;
	BaseIndex = TotalIndices;

	for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if (mesh->HasPositions()) {
			const aiVector3D& vp = mesh->mVertices[v_i];
			m_Positions.push_back(vec3(vp.x, vp.y, vp.z));
		}
		if (mesh->HasNormals()) {
			//printf("norm!\n");
			const aiVector3D& vn = mesh->mNormals[v_i];
			m_Normals.push_back(vec3(vn.x, vn.y, vn.z));
		}
		if (mesh->HasTextureCoords(0)) {
			//printf("tex!");
			const aiVector3D& vt = mesh->mTextureCoords[0][v_i];
			m_TexCoords.push_back(vec2(vt.x, vt.y));
		}
		if (mesh->HasTangentsAndBitangents()) {
			const aiVector3D& vtg = mesh->mTangents[v_i];
			const aiVector3D& vbtg = mesh->mBitangents[v_i];
			m_Tangents.push_back(vec3(vtg.x, vtg.y, vtg.z));
			m_Bit_Tangents.push_back(vec3(vbtg.x, vbtg.y, vbtg.z));
		}
	};
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		const aiFace& Face = mesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		m_Indices.push_back(Face.mIndices[0]);
		m_Indices.push_back(Face.mIndices[1]);
		m_Indices.push_back(Face.mIndices[2]);
	}
	//std::cout << name << "\n";
	MaterialIndex = mesh->mMaterialIndex;
}
*/

void Mesh::PopulateBuffers()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	glBindBuffer(GL_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &m_Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords[0]) * m_TexCoords.size(), &m_TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Normals[0]) * m_Normals.size(), &m_Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices[0]) * m_Indices.size(), &m_Indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void Mesh::RenderMesh(mat4 transform, unsigned int shaderID)
{
	glBindVertexArray(VAO);
	int matrix_location = glGetUniformLocation(shaderID, "model");
	assert(MaterialIndex < m_Materials.size());
	//add a check for normal map, atm it breaks if theres no normal map in the .mtl file
	int samplerLoc = glGetUniformLocation(shaderID, "gSampler");
	int normalLoc = glGetUniformLocation(shaderID, "normalMap");
	int materialpDiffuseLoc = glGetUniformLocation(shaderID, "gMaterial.pDiffuse");

	glUniform1i(samplerLoc, COLOR_TEXTURE_UNIT_INDEX);
	
	if (m_Materials[MaterialIndex].pDiffuse != NULL) {
		m_Materials[MaterialIndex].pDiffuse->Bind(GL_TEXTURE0);
		glUniform1i(samplerLoc, 0);
		glUniform1i(materialpDiffuseLoc, 1);
	}
	if (m_Materials[MaterialIndex].pNormal != NULL) {
		m_Materials[MaterialIndex].pNormal->Bind(GL_TEXTURE1);
		glUniform1i(normalLoc, 1);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, 0); // << default texture object
		int DiffuseColorLoc = glGetUniformLocation(shaderID, "gMaterial.DiffuseColor");
		int AmbientColorLoc = glGetUniformLocation(shaderID, "gMaterial.AmbientColor");
		int SpecularColorLoc = glGetUniformLocation(shaderID, "gMaterial.SpecularColor");
		glUniform3f(DiffuseColorLoc,m_Materials[MaterialIndex].DiffuseColor.v[0], m_Materials[MaterialIndex].DiffuseColor.v[1], m_Materials[MaterialIndex].DiffuseColor.v[2]);
		glUniform3f(AmbientColorLoc, m_Materials[MaterialIndex].AmbientColor.v[0], m_Materials[MaterialIndex].AmbientColor.v[1], m_Materials[MaterialIndex].AmbientColor.v[2]);
		glUniform3f(SpecularColorLoc, m_Materials[MaterialIndex].SpecularColor.v[0], m_Materials[MaterialIndex].SpecularColor.v[1], m_Materials[MaterialIndex].SpecularColor.v[2]);
		glUniform1i(materialpDiffuseLoc, 0);
	}
	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, transform.m);

	glDrawElementsBaseVertex(GL_TRIANGLES,
		NumIndices,
		GL_UNSIGNED_INT,
		(void*)(sizeof(unsigned int) * BaseIndex),
		BaseVertex);
	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
}

void Mesh::LoadDiffuseTexture(const string& Dir, const aiMaterial* pMaterial, int index)
{
	m_Materials[index].pDiffuse = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			string p(Path.data);

			if (p.substr(0, 2) == ".\\") {
				p = p.substr(2, p.size() - 2);
			}

			string FullPath = Dir + '\\' + p;

			m_Materials[index].pDiffuse = new Texture(GL_TEXTURE_2D, FullPath.c_str());

			if (!m_Materials[index].pDiffuse->Load()) {
				printf("Error loading diffuse texture '%s'\n", FullPath.c_str());
				exit(0);
			}
			else {
				printf("Loaded diffuse texture '%s'\n", FullPath.c_str());
			}
		}
	}
}

void Mesh::LoadNormalTexture(const string& Dir, const aiMaterial* pMaterial, int index)
{
	m_Materials[index].pNormal = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_HEIGHT) > 0) {
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_HEIGHT, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			string p(Path.data);

			if (p.substr(0, 2) == ".\\") {
				p = p.substr(2, p.size() - 2);
			}

			string FullPath = Dir + '\\' + p;

			m_Materials[index].pNormal = new Texture(GL_TEXTURE_2D, FullPath.c_str());

			if (!m_Materials[index].pNormal->Load()) {
				printf("Error loading normal texture '%s'\n", FullPath.c_str());
				exit(0);
			}
			else {
				printf("Loaded diffuse normal '%s'\n", FullPath.c_str());
			}
		}
	}
}

void Mesh::LoadSpecularTexture(const string& Dir, const aiMaterial* pMaterial, int index)
{
	m_Materials[index].pSpecularExponent = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0) {
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
			string p(Path.data);

			if (p.substr(0, 2) == ".\\") {
				p = p.substr(2, p.size() - 2);
			}

			string FullPath = Dir + '/' + p;

			m_Materials[index].pSpecularExponent = new Texture(GL_TEXTURE_2D, FullPath.c_str());

			if (!m_Materials[index].pSpecularExponent->Load()) {
				printf("Error loading specular texture '%s'\n", FullPath.c_str());
				exit(0);
			}
			else {
				printf("Loaded specular texture '%s'\n", FullPath.c_str());
			}
		}
	}
}

void Mesh::LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index)
{
	LoadDiffuseTexture(Dir, pMaterial, index);
	LoadSpecularTexture(Dir, pMaterial, index);
	LoadNormalTexture(Dir, pMaterial, index);
}
void Mesh::LoadColors(const aiMaterial* pMaterial, int index)
{
	aiColor3D AmbientColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, AmbientColor) == AI_SUCCESS) {
		printf("Loaded ambient color [%f %f %f]\n", AmbientColor.r, AmbientColor.g, AmbientColor.b);
		m_Materials[index].AmbientColor.v[0] = AmbientColor.r;
		m_Materials[index].AmbientColor.v[1] = AmbientColor.g;
		m_Materials[index].AmbientColor.v[2] = AmbientColor.b;
	}

	aiColor3D DiffuseColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, DiffuseColor) == AI_SUCCESS) {
		printf("Loaded diffuse color [%f %f %f]\n", DiffuseColor.r, DiffuseColor.g, DiffuseColor.b);
		m_Materials[index].DiffuseColor.v[0] = DiffuseColor.r;
		m_Materials[index].DiffuseColor.v[1] = DiffuseColor.g;
		m_Materials[index].DiffuseColor.v[2] = DiffuseColor.b;
	}

	aiColor3D SpecularColor(0.0f, 0.0f, 0.0f);

	if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, SpecularColor) == AI_SUCCESS) {
		printf("Loaded specular color [%f %f %f]\n", SpecularColor.r, SpecularColor.g, SpecularColor.b);
		m_Materials[index].SpecularColor.v[0] = SpecularColor.r;
		m_Materials[index].SpecularColor.v[1] = SpecularColor.g;
		m_Materials[index].SpecularColor.v[2] = SpecularColor.b;
	}
}
void Mesh::InitMeshMaterials(const string& Dir, const aiScene* pScene)
{
	// Initialize the materials
	m_Materials.resize(pScene->mNumMaterials);
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		LoadTextures(Dir, pMaterial, i);
		LoadColors(pMaterial, i);
	}
}
