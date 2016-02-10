#include "Transform.hpp"

auto
Transform::GetMatrix() -> Matrix
{
	Matrix result, rotation, translation, scale;

	rotation = Matrix::Rotation(Rotation.x, Rotation.y, Rotation.z);
	translation = Matrix::Translation(Position.x, Position.y, Position.z);
	scale = Matrix::Scale(Scale.x, Scale.y, Scale.z);

	return scale * rotation * translation;//translation * rotation * scale;
}


auto
Transform::GetInverseMatrix() -> Matrix
{
	Matrix result, rotation, translation, scale;

	rotation = Matrix::Rotation(-Rotation.x, -Rotation.y, -Rotation.z);
	translation = Matrix::Translation(-Position.x, -Position.y, -Position.z);
	scale = Matrix::Scale(1.f / Scale.x, 1.f / Scale.y, 1.f / Scale.z);

	return scale * rotation * translation;
}