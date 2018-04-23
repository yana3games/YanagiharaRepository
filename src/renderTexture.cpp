//=============================================================================
// File : terrain.h
// Date : 2017/12/10(日)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "renderTexture.h"
#include "appManager.h"
#include "renderer.h"

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
RenderTexture::RenderTexture( float width, float height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT depthFormat = DXGI_FORMAT_D24_UNORM_S8_UINT)
{
  // デバイスの取得
  ID3D11Device* device = AppManager::GetRenderer()->GetDevice();

  // 2次元テクスチャの設定
  D3D11_TEXTURE2D_DESC texDesc;
  memset(&texDesc, 0, sizeof(texDesc));
  texDesc.Usage = D3D11_USAGE_DEFAULT;
  texDesc.Format = format;
  texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
  texDesc.Width = width;
  texDesc.Height = height;
  texDesc.CPUAccessFlags = 0;
  texDesc.MipLevels = 1;
  texDesc.ArraySize = 1;
  texDesc.SampleDesc.Count = 1;
  texDesc.SampleDesc.Quality = 0;

  // 2次元テクスチャの生成
  HRESULT hr = device->CreateTexture2D(&texDesc, NULL, &m_pRTT);
  if (FAILED(hr))
  {
    MessageBox(NULL, "レンダーターゲットの生成に失敗しました！", "警告！", MB_ICONWARNING);
  }

  // レンダーターゲットビューの設定
  D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
  memset(&rtvDesc, 0, sizeof(rtvDesc));
  rtvDesc.Format = format;
  rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

  // レンダーターゲットビューの生成
  hr = device->CreateRenderTargetView(m_pRTT.Get(), &rtvDesc, &m_pRTV);
  if (FAILED(hr))
  {
    MessageBox(NULL, "RTのビューの生成に失敗しました！", "警告！", MB_ICONWARNING);
  }

  // シェーダリソースビューの設定
  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  memset(&srvDesc, 0, sizeof(srvDesc));
  srvDesc.Format = rtvDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = 1;

  // シェーダリソースビューの生成
  hr = device->CreateShaderResourceView(m_pRTT.Get(), &srvDesc, &m_pRTSRV);
  if (FAILED(hr))
  {
    MessageBox(NULL, "RTのシェーダーリソースビューの生成に失敗しました！", "警告！", MB_ICONWARNING);
  }

  // 深度ステンシルテクスチャの生成.
  D3D11_TEXTURE2D_DESC td;
  ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
  td.Width = width;
  td.Height = height;
  td.MipLevels = 1;
  td.ArraySize = 1;
  td.Format = depthFormat;
  td.SampleDesc.Count = 1;
  td.SampleDesc.Quality = 0;
  td.Usage = D3D11_USAGE_DEFAULT;
  td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  td.CPUAccessFlags = 0;
  td.MiscFlags = 0;
  
  // 深度ステンシルテクスチャの生成.
  hr = device->CreateTexture2D(&td, NULL, &m_pDST);
  if (FAILED(hr)) {
    MessageBox(NULL, "深度ステンシルテクスチャの生成に失敗しました！", "警告！", MB_ICONWARNING);
  }

  // 深度ステンシルビューの設定
  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
  ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
  dsvd.Format = depthFormat;
  dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

  // 深度ステンシルビューの生成.
  hr = device->CreateDepthStencilView(m_pDST.Get(), &dsvd, &m_pDSV);
  if (FAILED(hr)) {
    MessageBox(NULL, "深度ステンシルビューの生成に失敗しました！", "警告！", MB_ICONWARNING);
  }
}

