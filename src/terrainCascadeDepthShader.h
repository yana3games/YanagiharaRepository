#pragma once
//=============================================================================
// File : terrainCascadeDepthShader.h
// Date : 2018/02/27(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "shader.h"


///////////////////////////////////////////////////////////////////////////////
// 前方宣言


// カスケードシャドウ用地形の深度描画シェーダ
class TerrainCascadeDepthShader : public Shader
{
public:
  TerrainCascadeDepthShader();
  ~TerrainCascadeDepthShader();

  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture);

  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
  };

  struct Matrix
  {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
  };

private:
  void CreateMatrixBuffer(void);
  void CreateShader(void);
  ID3D11Buffer* m_matCB;        // マトリクスのコンスタントバッファ
};