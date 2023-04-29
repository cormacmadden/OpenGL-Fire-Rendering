#include "Mesh.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

Mesh::Mesh() {};

Mesh::Mesh(const aiMesh* mesh, vec3 transformation, const char* aiName) {
	printf("%i vertices in mesh\n", mesh->mNumVertices);
	//mPointCount += mesh->mNumVertices;
	transformMat = transformation;
	name = aiName;
	std::cout << mesh->mName.data << '\n';

	for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if (mesh->HasPositions()) {
			const aiVector3D* vp = &(mesh->mVertices[v_i]);
			//mPositions.push_back(vec3(vp->x, vp->y, vp->z));
		}
		if (mesh->HasNormals()) {
			//printf("norm!\n");
			const aiVector3D* vn = &(mesh->mNormals[v_i]);
			//mNormals.push_back(vec3(vn->x, vn->y, vn->z));
		}
		if (mesh->HasTextureCoords(0)) {
			//printf("tex!");
			const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
			//mTextureCoords.push_back(vec2(vt->x, vt->y));
		}
		if (mesh->HasTangentsAndBitangents()) {
			//const aiVector3D* vtg = &(mesh->mTangents[v_i]);
			//const aiVector3D* vbtg = &(mesh->mBitangents[v_i]);

			/* You can extract tangents and bitangents here              */
			/* Note that you might need to make Assimp generate this     */
			/* data for you. Take a look at the flags that aiImportFile  */
			/* can take.                                                 */
		}
	};
	std::cout << name << "\n";
}

void Mesh::draw(mat4 transform, GLuint matrix_location, GLuint texture, Shader shaderProgram) {
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, transform.m);
	glBindVertexArray(vao);
	glBindVertexArray(0);
}