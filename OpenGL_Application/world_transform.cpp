#include "world_transform.h"

void WorldTransfrom::SetScale(float scale)
{
	m_scale = scale;
}

void WorldTransfrom::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

void WorldTransfrom::SetPosition(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void WorldTransfrom::Rotate(float x, float y, float z)
{
	m_rotation.x += x;
	m_rotation.y += y;
	m_rotation.z += z;
}

Matrix4f WorldTransfrom::GetMatrix()
{
	Matrix4f Scale;
	Scale.InitScaleTransform(m_scale, m_scale, m_scale);

	Matrix4f Rotation;
	Rotation.InitRotateTransform(m_rotation.x, m_rotation.y, m_rotation.z);

	Matrix4f Translation;
	Translation.InitTranslationTransform(m_pos.x, m_pos.y, m_pos.z);

	Matrix4f WorldTransformation = Translation * Rotation * Scale;

	return WorldTransformation;
}
