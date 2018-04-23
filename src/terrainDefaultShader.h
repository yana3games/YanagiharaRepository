#pragma once
//=============================================================================
// File : terrainDefaultShader.h
// Date : 2018/02/19(月)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "shader.h"
#include "Vector.h"

// スカイドーム通常描画用シェーダ
class TerrainDefaultShader : public Shader
{
 public:
  TerrainDefaultShader();
  ~TerrainDefaultShader();
  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture);

  struct Vertex
  {
    Vector3 pos;
    Vector3 Nrm;
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
  ID3D11Buffer* m_matCB;    // マトリクスのコンスタントバッファ
};
