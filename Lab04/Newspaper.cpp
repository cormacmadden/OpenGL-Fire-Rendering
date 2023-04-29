#include "Newspaper.h"

Newspaper::Newspaper(vec3 position)
{
	newspaper = Model("D:\\Personal\\College\\5thYear\\ComputerGraphics\\Models\\Newspaper.obj");
	Pos = position;
	MovementSpeed = 5.0f;
	Front = vec3(1.0f, 0.0f, 0.0f);
	Right = vec3(0.0f, 0.0f, 1.0f);
}

Newspaper::Newspaper()
{
}

void Newspaper::renderNewspaper(mat4 pos, int shaderID) {

	//Transform the Newspaper
	mat4 newspaper_pos = identity_mat4();
	newspaper_pos = translate(newspaper_pos, vec3(0.0f, 0.0f, 0.0f));
	newspaper_pos = rotate_y_deg(newspaper_pos, Yaw);
	newspaper_pos = translate(newspaper_pos, Pos);

	int world_loc = glGetUniformLocation(shaderID, "world");
	glUniformMatrix4fv(world_loc, 1, GL_FALSE, newspaper_pos.m);

	newspaper.RenderModel(newspaper_pos, shaderID);

}

void Newspaper::ProcessKeyboard(MovementDir direction, float deltaTime)
{
	float speed = MovementSpeed * deltaTime;
	if (direction == FORWARD)Pos += Front * speed;
	if (direction == BACKWARD)Pos -= Front * speed;
	if (direction == LEFT) {
		Yaw += 20.0f * speed;
	}
	if (direction == RIGHT) {
		Yaw -= 20.0f * speed;
	}
	updateNewspaperVectors();
}

void Newspaper::MoveNewspaper(int x, int y, float deltaTime) {

	float speed = MovementSpeed * deltaTime;
	Pos += Front * speed;
	if (x == 1)Yaw += 20.0f * speed;
	if (x == -1)Yaw -= 20.0f * speed;
	updateNewspaperVectors();

}

mat4 Newspaper::GetViewMatrix()
{
	return look_at(Pos, Pos + Front, Up);
}

void Newspaper::updateNewspaperVectors()
{
	vec3 front;
	front.v[0] = cos(convert(-Yaw)) * cos(convert(Pitch));
	front.v[1] = sin(convert(Pitch));
	front.v[2] = sin(convert(-Yaw)) * cos(convert(Pitch));

	Front = normalise(front);
	Right = normalise(cross(Front, WorldUp));
	Up = normalise(cross(Right, Front));
}

void Newspaper::throwPapers(Model paper, Shader myShader, int remainingPapers) {
	std::cout << "Throwing Papers, papers remaining: " << remainingPapers << '\n';
	if (remainingPapers > 0) {
		mat4 paper_mat = identity_mat4();
		paper_mat = translate(paper_mat, vec3(0.0, 0.0, 0.0));
		std::cout << "If statement Reached" << '\n';
		paper.RenderModel(paper_mat, myShader.ID);
		std::cout << "Model Rendered: " << '\n';
		this->remainingPapers--;
	}
}
