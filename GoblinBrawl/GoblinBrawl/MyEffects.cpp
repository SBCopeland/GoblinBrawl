#include "stdafx.h"
#include "MyEffects.h"
#include <iostream>
#include <fstream>
#include <vector>

MyEffect::MyEffect( ID3D11Device* device, const std::wstring& filename ) :
fx( nullptr ) {
	std::ifstream fin( filename, std::ios::binary );
	fin.seekg( 0, std::ios_base::end );
	int size = (int)fin.tellg();
	fin.seekg( 0, std::ios_base::beg );
	std::vector<char> compiledShader( size );
	fin.read( &compiledShader[0], size );
	fin.close();
	HR( D3DX11CreateEffectFromMemory( &compiledShader[0], size, 0, device, &fx ) );
}

MyEffect::~MyEffect() {
	ReleaseCOM( fx );
}

SimpleEffect::SimpleEffect( ID3D11Device* device, const std::wstring& filename ) :
MyEffect( device, filename ) {
	simpleTechnique = fx->GetTechniqueByName( "SimpleTech" );
	worldViewProj = fx->GetVariableByName( "gWorldViewProj" )->AsMatrix();
}
SimpleEffect::~SimpleEffect() {}

TerrainEffect::TerrainEffect( ID3D11Device* device, const std::wstring& filename ) :
MyEffect( device, filename ) {
	terrainTechnique = fx->GetTechniqueByName( "TerrainTech" );
	world = fx->GetVariableByName( "gWorld" )->AsMatrix();
	worldInvTranspose = fx->GetVariableByName( "gWorldInvTranspose" )->AsMatrix();
	worldViewProj = fx->GetVariableByName( "gWorldViewProj" )->AsMatrix();
	diffuseMap = fx->GetVariableByName( "gDiffuseMap" )->AsShaderResource();
}
TerrainEffect::~TerrainEffect() {}

StaticGeomEffect::StaticGeomEffect( ID3D11Device* device, const std::wstring& filename ) :
MyEffect( device, filename ) {
	staticGeomLight5Tech = fx->GetTechniqueByName( "StaticGeomLight5" );
	world = fx->GetVariableByName( "gWorld" )->AsMatrix();
	worldInvTranspose = fx->GetVariableByName( "gWorldInvTranspose" )->AsMatrix();
	worldViewProj = fx->GetVariableByName( "gWorldViewProj" )->AsMatrix();
	diffuseMap = fx->GetVariableByName( "gDiffuseMap" )->AsShaderResource();
	eyePosW = fx->GetVariableByName( "gEyePosW" )->AsVector();
	pointLights = fx->GetVariableByName( "gPointLights" );
	mat = fx->GetVariableByName( "gMaterial" );
}
StaticGeomEffect::~StaticGeomEffect() {}

CharacterEffect::CharacterEffect( ID3D11Device* device, const std::wstring& filename ) :
MyEffect( device, filename ) {
	characterLight5Tech = fx->GetTechniqueByName( "CharacterLight5" );
	world = fx->GetVariableByName( "gWorld" )->AsMatrix();
	worldInvTranspose = fx->GetVariableByName( "gWorldInvTranspose" )->AsMatrix();
	worldViewProj = fx->GetVariableByName( "gWorldViewProj" )->AsMatrix();
	diffuseMap = fx->GetVariableByName( "gDiffuseMap" )->AsShaderResource();
	eyePosW = fx->GetVariableByName( "gEyePosW" )->AsVector();
	pointLights = fx->GetVariableByName( "gPointLights" );
	mat = fx->GetVariableByName( "gMaterial" );
}
CharacterEffect::~CharacterEffect() {}

CharacterSkinnedEffect::CharacterSkinnedEffect( ID3D11Device* device, const std::wstring& filename ) :
MyEffect( device, filename ) {
	characterSkinnedLight5Tech = fx->GetTechniqueByName( "CharacterSkinnedLight5" );
	world = fx->GetVariableByName( "gWorld" )->AsMatrix();
	worldInvTranspose = fx->GetVariableByName( "gWorldInvTranspose" )->AsMatrix();
	worldViewProj = fx->GetVariableByName( "gWorldViewProj" )->AsMatrix();
	diffuseMap = fx->GetVariableByName( "gDiffuseMap" )->AsShaderResource();
	eyePosW = fx->GetVariableByName( "gEyePosW" )->AsVector();
	pointLights = fx->GetVariableByName( "gPointLights" );
	mat = fx->GetVariableByName( "gMaterial" );
	boneTransforms = fx->GetVariableByName( "gBoneTransforms" )->AsMatrix();
}
CharacterSkinnedEffect::~CharacterSkinnedEffect() {}

SimpleEffect* MyEffects::SimpleFX = 0;
TerrainEffect* MyEffects::TerrainFX = 0;
StaticGeomEffect* MyEffects::StaticGeomFX = 0;
CharacterEffect* MyEffects::CharacterFX = 0;
CharacterSkinnedEffect* MyEffects::CharacterSkinnedFX = 0;

void MyEffects::InitAll( ID3D11Device* device ) {
	SimpleFX = new SimpleEffect( device, L"fx/simple.fxo" );
	TerrainFX = new TerrainEffect( device, L"fx/terrain.fxo" );
	StaticGeomFX = new StaticGeomEffect( device, L"fx/staticGeom.fxo" );
	CharacterFX = new CharacterEffect( device, L"fx/character.fxo" );
	CharacterSkinnedFX = new CharacterSkinnedEffect( device, L"fx/characterSkinned.fxo" );
}

void MyEffects::DestroyAll() {
	SafeDelete( SimpleFX );
	SafeDelete( TerrainFX );
	SafeDelete( StaticGeomFX );
	SafeDelete( CharacterFX );
	SafeDelete( CharacterSkinnedFX );
}