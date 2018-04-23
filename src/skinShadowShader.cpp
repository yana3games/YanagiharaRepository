//=============================================================================
// File : skinShadowShader.cpp
// Date : 2018/01/26(金)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "appManager.h"
#include "skinShadowShader.h"
#include <d3d11.h>
#include "renderer.h"
#include "Texture.h"


///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
SkinShadowShader::SkinShadowShader(void)
{ // マトリクスバッファの設定.
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
// デストラクタ
SkinShadowShader::~SkinShadowShader(void)
{
  if(m_matCB) m_matCB->Release();
}


///////////////////////////////////////////////////////////////////////////////
// パラメータのセット
void SkinShadowShader::SetParameters( DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture)
{
  // シェーダを設定して描画
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();
  pDeviceContext->VSSetShader(m_pVS, NULL, 0);
  pDeviceContext->GSSetShader(nullptr, NULL, 0);
  pDeviceContext->HSSetShader(nullptr, NULL, 0);
  pDeviceContext->DSSetShader(nullptr, NULL, 0);
  pDeviceContext->PSSetShader(m_pPS, NULL, 0);
  pDeviceContext->IASetInputLayout(m_pIL);

  // 頂点シェーダに定数バッファを設定.
  pDeviceContext->VSSetConstantBuffers(1, 1, &boneCB);

  // 定数バッファの設定.
  Matrix cb;
  cb.world = World;
  cb.view = View;
  cb.proj = Proj;
  cb.wit = WIT;
  // テクスチャのセット
  if (texture != nullptr) pDeviceContext->PSSetShaderResources(0, 1, texture->GetShaderResouceView().GetAddressOf());
  else pDeviceContext->PSSetShaderResources(0, 1, nullptr);

  // サブリソースを更新.
  pDeviceContext->UpdateSubresource(m_matCB, 0, NULL, &cb, 0, 0);

  // 頂点シェーダに定数バッファを設定.
  pDeviceContext->VSSetConstantBuffers(0, 1, &m_matCB);
}

