#pragma once
//=============================================================================
// File : cascadeDepthShader.h
// Date : 2018/02/27(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "shader.h"


///////////////////////////////////////////////////////////////////////////////
// 前方宣言


// カスケードシャドウ用剛体の深度描画シェーダ
class CascadeDepthShader : public Shader
{
public:
  CascadeDepthShader();
  ~CascadeDepthShader();

  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, Texture* texture);

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