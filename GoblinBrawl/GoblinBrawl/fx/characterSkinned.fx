#include "lightHelper.fx"

cbuffer cbPerFrame {
	PointLight	gPointLights[5];
	float3		gEyePosW;
};

cbuffer cbPerObject {
	float4x4	gWorld;
	float4x4	gWorldInvTranspose;
	float4x4	gWorldViewProj;
	Material	gMaterial;
};

cbuffer cbSkinned {
	float4x4 gBoneTransforms[96]; // limited to 96 bone max
};

Texture2D	gDiffuseMap;

SamplerState samAnisotropic {
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn {
	float3 PosL		:	POSITION;
	float3 NormalL	:	NORMAL;
	float2 Tex		:	TEXCOORD;
	float4 Weights    : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct VertexOut {
	float4 PosH		:	SV_POSITION;
	float3 PosW		:	POSITION;
	float3 NormalW	:	NORMAL;
	float2 Tex		:	TEXCOORD;
	float4 DebugCol :	DEBUGCOL;
};

VertexOut VS( VertexIn vin ) {
	VertexOut vout;

	// Init array or else we get strange warnings about SV_POSITION
	float weights[4] = { 0.f, 0.f, 0.f, 0.f };
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f-weights[0]-weights[1]-weights[2];

	vin.NormalL = normalize( vin.NormalL.zyx );

	// Vertex blending
	float3 posL = float3(0.f, 0.f, 0.f);
	float3 normalL = float3(0.f, 0.f, 0.f);
	[unroll]
	for( int i = 0; i<4; ++i ) {
		posL += weights[i]*mul( float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]] ).xyz;
		normalL += weights[i]*mul( vin.NormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]] );
	}
	
	vout.DebugCol = float4(normalL, 1.f);

	// Transform to world space
	vout.PosW = mul( float4(posL, 1.0f), gWorld ).xyz;
	vout.NormalW = normalL.xyz;//mul( normalL, (float3x3)gWorldInvTranspose );

	// Transform to homogeneous clip space.
	vout.PosH = mul( float4(posL, 1.0f), gWorldViewProj );

	// Output vertex attributes for interpolation across triangle
	vout.Tex = vin.Tex;

	return vout;
}

float4 PS( VertexOut pin, uniform int gLightCount ) : SV_Target{

	//return pin.DebugCol;

	// Interpolating normal can unnormalize it
	pin.NormalW = normalize( pin.NormalW );

	float4 texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );

	float3 toEye = gEyePosW-pin.PosW;

	float distToEye = length( toEye );
	toEye /= distToEye;
		
		///
		// Lighting
		///

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
	[unroll]
	for( int i = 0; i<gLightCount; ++i ) {
		float4 A, D, S;
		ComputePointLight( gMaterial, gPointLights[i], pin.PosW, pin.NormalW, toEye, A, D, S );
		ambient += A;
		diffuse += D;
		spec += S;
	}
	
	float4 litColor = texColor * (ambient+diffuse)+spec;
	litColor.a = gMaterial.Diffuse.a;

	//return float4(pin.NormalW, 1.0f);
	return litColor;
}

technique11 CharacterSkinnedLight5 {
	pass P0 {
		SetVertexShader( CompileShader(vs_5_0, VS()) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_5_0, PS(5)) );
	}
}