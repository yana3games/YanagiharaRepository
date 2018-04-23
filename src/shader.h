#pragma once
//=============================================================================
// File : shader.h
// Date : 2018/01/19(金)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include <d3d11.h>
#include "Vector.h"

// 前方宣言
class Texture;

// シェーダークラスインターフェース
class Shader
{
 public:
  Shader() : m_pVS(nullptr), m_pIL(nullptr), m_pHS(nullptr), m_pDS(nullptr), m_pGS(nullptr), m_pPS(nullptr) {};
  virtual ~Shader() {};
  virtual void SetParameters() {};
  virtual void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture) {};
  virtual void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture) {};
  virtual void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, Texture* texture) {};

 protected:
  ID3D11VertexShader*    m_pVS;      // 頂点シェーダ
  ID3D11InputLayout*     m_pIL;      // 入力レイアウト
  ID3D11HullShader*      m_pHS;      // ハルシェーダ
  ID3D11DomainShader*    m_pDS;      // ドメインシェーダ
  ID3D11GeometryShader*  m_pGS;      // ジオメトリシェーダ
  ID3D11PixelShader*     m_pPS;      // ピクセルシェーダ
};