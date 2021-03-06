//=============================================================================
// File : regidCascadeDepthShader.cpp
// Date : 2018/02/26(金)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "appManager.h"
#include "skinCascadeDepthShader.h"
#include "shaderManager.h"
#include <d3d11.h>
#include "renderer.h"
#include "Texture.h"
#include "renderTexture.h"
#include "appConfig.h"


///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
SkinCascadeDepthShader::SkinCascadeDepthShader(void)
{
  CreateMatrixBuffer();
  CreateShader();
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
SkinCascadeDepthShader::~SkinCascadeDepthShader(void)
{
  if (m_matCB) m_matCB->Release();
}


///////////////////////////////////////////////////////////////////////////////
// パラメータのセット
void SkinCascadeDepthShader::SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture)
{
  // シェーダを設定して描画
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();
  pDeviceContext->VSSetShader(m_pVS, NULL, 0);
  pDeviceContext->GSSetShader(m_pGS, NULL, 0);
  pDeviceContext->HSSetShader(m_pHS, NULL, 0);
  pDeviceContext->DSSetShader(m_pDS, NULL, 0);
  pDeviceContext->PSSetShader(m_pPS, NULL, 0);
  pDeviceContext->IASetInputLayout(m_pIL);

  // 定数バッファの設定.
  Matrix cb;
  cb.world = World;
  cb.view = View;
  cb.proj = Proj;

  // サブリソースを更新.
  pDeviceContext->UpdateSubresource(m_matCB, 0, NULL, &cb, 0, 0);

  // 頂点シェーダに定数バッファを設定.
  pDeviceContext->VSSetConstantBuffers(0, 1, &m_matCB);

  // 頂点シェーダに定数バッファを設定.
  pDeviceContext->VSSetConstantBuffers(1, 1, &boneCB);
}


///////////////////////////////////////////////////////////////////////////////
// 行列コンスタントバッファの作成
void SkinCascadeDepthShader::CreateMatrixBuffer(void)
{
  // マトリクスバッファの設定.
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
  bd.ByteWidth = sizeof(Matrix);
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;

  // マトリクスバッファを生成.
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateBuffer(&bd, NULL, &m_matCB);
  if (FAILED(hr)) {
    MessageBox(NULL, "FailedCreateConstantBuffer", "error!", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// シェーダの作成
void SkinCascadeDepthShader::CreateShader(void)
{
  ShaderManager* shaderManager = AppManager::GetShaderManager();
  // 入力レイアウトの定義
  D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "WEIGHT"  , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "INDEX"   , 0, DXGI_FORMAT_R32G32B32A32_UINT , 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  UINT numElements = sizeof(layout) / sizeof(layout[0]);
  shaderManager->LoadVS("./data/SHADER/VertexShader/skinCascadeDepthVS.cso", layout, numElements);
  ShaderManager::VertexShader VS = shaderManager->GetVS("./data/SHADER/VertexShader/skinCascadeDepthVS.cso");
  m_pVS = VS.pVS;
  m_pIL = VS.pIL;
  shaderManager->LoadPS("./data/SHADER/PixelShader/skinCascadeDepthPS.cso");
  m_pPS = shaderManager->GetPS("./data/SHADER/PixelShader/skinCascadeDepthPS.cso");
}
