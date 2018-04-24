#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <directxmath.h>
#include <d3d11.h>

using namespace DirectX;

class Light
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetRoughness(float);
	void SetFresnel(float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetDirection();
	float GetRoughness();
	float GetFresnel();
	
private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
	XMFLOAT4 m_ambientColor;
	float m_roughness;
	float m_fresnel;
};

#endif