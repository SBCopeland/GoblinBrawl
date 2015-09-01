#pragma once
#include <DirectXMath.h>
#include "Lighting.h"

struct ID3DX11Effect;
struct ID3DX11EffectTechnique;
struct ID3DX11EffectMatrixVariable;
struct ID3D11ShaderResourceView;

class ModelLoader;
class Mesh;
struct ID3D11DeviceContext;
class ID3D11Device;

class Walls {
public:
	Walls();
	~Walls();
	bool Init( ModelLoader* modelLoader, ID3D11Device* device );
	void XM_CALLCONV Draw( DirectX::FXMMATRIX viewProj, FXMVECTOR cameraPos, std::vector<PointLight> pointLights, ID3D11DeviceContext* context );
private:
	Mesh*							mesh;
	ID3D11ShaderResourceView*		diffuseView;
	Material						mat;
};