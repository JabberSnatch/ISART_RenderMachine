#include "Transform.hpp"

auto
Transform::GetMatrix() const -> Matrix
{
	Matrix translation, scale;

	translation = Matrix::Translation(Position.x, Position.y, Position.z);
	scale = Matrix::Scale(Scale.x, Scale.y, Scale.z);

	//return scale * rotation * translation;//translation * rotation * scale;
	return scale * Rotation.getMatrix() * translation;
}


auto
Transform::GetInverseMatrix() const -> Matrix
{
	Matrix translation, scale;

	translation = Matrix::Translation(-Position.x, -Position.y, -Position.z);
	scale = Matrix::Scale(1.f / Scale.x, 1.f / Scale.y, 1.f / Scale.z);

	//return scale * rotation * translation;
	return scale * Rotation.conjugate().getMatrix() * translation;
}