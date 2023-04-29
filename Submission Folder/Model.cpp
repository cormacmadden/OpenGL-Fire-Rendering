#include "Model.h"
#include "stb_image.h"
#include "Mesh.h"
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace)
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2
#define COLOR_TEXTURE_UNIT              GL_TEXTURE0
#define COLOR_TEXTURE_UNIT_INDEX		0
#define SPECULAR_EXPONENT_UNIT             GL_TEXTURE6

using namespace std;

Model::Model() {};

Model::Model(const char* file_name) {
	filename = file_name;
	loadScene();
}

void Model::loadScene() {
	Assimp::Importer Importer;

	//glGenVertexArrays(1, &m_VAO);
	//glBindVertexArray(m_VAO);

	//glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	const aiScene* scene = Importer.ReadFile(filename, ASSIMP_LOAD_FLAGS);

	bool Ret = false;
	if (scene) {
		Ret = InitFromScene(scene);
	}
	glBindVertexArray(0);
	std::string name = std::string(filename);
	size_t pos = 0;
	pos = name.find(".");
	std::string token = name.substr(0, pos);
	std::cout << "Model Name: " << token << '\n';
	std::cout << "Model Filename: " << name << '\n';
	vec3 trans = vec3(0.0f, 0.0f, 0.0f);
}

void Model::countVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices)
{
	//for (unsigned int i = 0; i < m_Meshes.size(); i++) {
	//	m_Meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
	//	m_Meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
	//	m_Meshes[i].BaseVertex = NumVertices;
	//	m_Meshes[i].BaseIndex = NumIndices;
	//	NumVertices += pScene->mMeshes[i]->mNumVertices;
	//	NumIndices += m_Meshes[i].NumIndices;
	//}
	//for (unsigned int i = 0; i < meshes.size(); i++) {
	//	NumVertices += pScene->mMeshes[i]->mNumVertices;
	//	NumIndices += m_Meshes[i].NumIndices;
	//}
}

bool Model::InitFromScene(const aiScene* pScene)
{
	//m_Meshes.resize(pScene->mNumMeshes);
	meshes.resize(pScene->mNumMeshes);
	m_Materials.resize(pScene->mNumMaterials);

	//countVerticesAndIndices(pScene, NumVertices, NumIndices);

	InitAllMeshes(pScene);

 	if (!InitMaterials(pScene)) {
		return false;
	}

	PopulateMeshBuffers();
	return true;
}
void Model::InitAllMeshes(const aiScene* pScene)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		meshes[i] = Mesh(paiMesh, ModelVertices, ModelIndices);
		ModelVertices += pScene->mMeshes[i]->mNumVertices;
		ModelIndices += meshes[i].NumIndices;
	}
	std::cout << "Num Vertices: " << ModelVertices << '\n';
	std::cout << "Num Indices:" << ModelIndices << '\n';
}

void Model::InitSingleMesh(const aiMesh* paiMesh, int index, unsigned int& NumVertices, unsigned int& NumIndices)
{

}

void Model::PopulateBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_Positions[0]) * m_Positions.size(), &m_Positions[0], GL_STATIC_DRAW);
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

}

void Model::PopulateMeshBuffers(){
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].PopulateBuffers();
	}
}

void Model::RenderModel(mat4 transform, unsigned int shaderID)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].RenderMesh(transform, shaderID);
	}
}

string GetDirFromFilename(const string& Filename)
{
	// Extract the directory part from the file name
	string::size_type SlashIndex;
	SlashIndex = Filename.find_last_of("\\");
	string Dir;
	if (SlashIndex == -1) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}
	return Dir;
}

const Material& Model::GetMaterial()
{
	for (unsigned int i = 0; i < m_Materials.size(); i++) {
		if ((m_Materials[i].AmbientColor.v[0] != 0.0f) && (m_Materials[i].AmbientColor.v[1] != 0.0f) && (m_Materials[i].AmbientColor.v[2] != 0.0f)) {
			return m_Materials[i];
		}
	}
	return m_Materials[0];
}

void Model::LoadColors(const aiMaterial* pMaterial, int index)
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

void Model::LoadTextures(const string& Dir, const aiMaterial* pMaterial, int index)
{
	LoadDiffuseTexture(Dir, pMaterial, index);
	LoadSpecularTexture(Dir, pMaterial, index);
	LoadNormalTexture(Dir, pMaterial, index);
}

void Model::LoadDiffuseTexture(const string& Dir, const aiMaterial* pMaterial, int index)
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

void Model::LoadNormalTexture(const string& Dir, const aiMaterial* pMaterial, int index)
{
	m_Materials[index].pNormal = NULL;

	if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
		aiString Path;

		if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
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

void Model::LoadSpecularTexture(const string& Dir, const aiMaterial* pMaterial, int index)
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

bool Model::InitMaterials(const aiScene* pScene)
{
	string Dir = GetDirFromFilename(filename);

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].InitMeshMaterials(Dir, pScene);
	}
	return true;
}

int Model::getNumberMeshes() {
	return meshes.size();
}


Mesh processMesh(aiMesh* mesh, const aiScene* scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.v[0] = mesh->mVertices[i].x;
		vector.v[1] = mesh->mVertices[i].y;
		vector.v[2] = mesh->mVertices[i].z;
		vertex.m_Positions = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.v[0] = mesh->mNormals[i].x;
			vector.v[1] = mesh->mNormals[i].y;
			vector.v[2] = mesh->mNormals[i].z;
			vertex.m_Normals = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.v[0] = mesh->mTextureCoords[0][i].x;
			vec.v[1] = mesh->mTextureCoords[0][i].y;
			vertex.m_TexCoords = vec;
			// tangent
			vector.v[0] = mesh->mTangents[i].x;
			vector.v[1] = mesh->mTangents[i].y;
			vector.v[2] = mesh->mTangents[i].z;
			vertex.m_Tangents = vector;
			// bitangent
			vector.v[0] = mesh->mBitangents[i].x;
			vector.v[1] = mesh->mBitangents[i].y;
			vector.v[2] = mesh->mBitangents[i].z;
			vertex.m_Bit_Tangents = vector;
		}
		else
			vertex.m_TexCoords = vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	// 1. diffuse maps
	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.m_fileName = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
	string filename = string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}