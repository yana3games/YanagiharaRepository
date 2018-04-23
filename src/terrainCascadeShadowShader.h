#pragma once
//=============================================================================
// File : terrainCascadeShadowShader.h
// Date : 2018/03/01(木)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "shader.h"
#include "Vector.h"
#include "cascadeShadowManager.h"


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class RenderTexture;


// カスケードシャドウ付き地形描画シェーダ
class TerrainCascadeShadowShader : public Shader
{
public:
  TerrainCascadeShadowShader();
  ~TerrainCascadeShadowShader();

  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture);
  void Set(DirectX::XMMATRIX* proj, DirectX::XMMATRIX* view, RenderTexture** zbuffer) {
    m_sunProj = proj;
    m_sunView = *view;
    m_ZBuffers = zbuffer;
  }

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

  struct SunMatrix
  {
    DirectX::XMMATRIX sunView;
    DirectX::XMMATRIX sunProj[CascadeShadowManager::NumDivision];
    float divisionFar[CascadeShadowManager::NumDivision];
  };

private:
  void CreateMatrixBuffer(void);
  void CreateSunBuffer(void);
  void CreateShader(void);
  DirectX::XMMATRIX* m_sunProj; // 太陽のプロジェクション行列配列の先頭ポインタ
  DirectX::XMMATRIX m_sunView;  // 太陽のビューマトリクス
  ID3D11Buffer* m_matCB;        // マトリクスのコンスタントバッファ
  ID3D11Buffer* m_sunCB;        // 太陽行列のコンスタントバッファ
  RenderTexture** m_ZBuffers;   // 深度バッファ配列の先頭ポインタ
};