#include "Camera.h"

Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other)
{
}


Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 Camera::GetPosition()
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 Camera::GetRotation()
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render()
{
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	//����w���x�N�g����ݒ�@Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//�����XMVECTOR�\���̂Ƀ��[�h�@Setup the position of the camera in the world.
	upVector = XMLoadFloat3(&up);

	//�J�����̃��[���h���W��ݒ�@Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	positionVector = XMLoadFloat3(&position);

	//�f�t�H���g�ŃJ������������ꏊ��ݒ� Setup where the camera is looking by default.
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAtVector = XMLoadFloat3(&lookAt);

	//	���[�iY���j�A�s�b�`�iX���j�A���[���iZ���j��]�����W�A���Őݒ� Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//���[�A�s�b�`�A���[���̒l�����]�s����쐬 Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//lookAt��up�x�N�g������]�s��ŕϊ����A�r���[�����_�Ő�������]����悤�ɂ��� Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//��]�����J�����̈ʒu���r���[�A�̈ʒu�ɕϊ� Translate the rotated camera position to the location of the viewer.
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//�Ō�ɁA�X�V���ꂽ3�̃x�N�g������r���[�s����쐬 Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	//�r���[�s����쐬���邽�߂�Render()���Ăяo���ꂽ��A����GetViewMatrix�֐����g�p���āA
	//�X�V���ꂽ�r���[�s����Ăяo���֐��ɒ񋟂��邱�Ƃ��ł��܂��B
	//�r���[�s��́AHLSL���_�V�F�[�_�Ŏg�p�����3�̎�v�ȃ}�g���b�N�X��1�ɂȂ�܂��B

	viewMatrix = m_viewMatrix;
	return;
}