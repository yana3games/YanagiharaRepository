#pragma once
//=============================================================================
// File : renderTexture.h
// Date : 2018/02/05(月)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "renderer.h"
#include <wrl/client.h>

// レンダーテクスチャクラス
class RenderTexture
{
 public:
  RenderTexture(float width, float height, DXGI_FORMAT format, DXGI_FORMAT depthFormat);

  // レンダーターゲットのビュー取得
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetView(void) { return m_pRTV; }
  // レンダーターゲットのシェーダーリソースビュー取得
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRView(void) { return m_pRTSRV; }
  // レンダーターゲット用のデプスステンシルビュー取得
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthView(void) { return m_pDSV; }
  // レンダーターゲット用のデプスステンシルシェーダーリソースビュー取得
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetDepthSRView(void) { return m_pDSSRV; }

 private:
  RenderTexture();
  Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_pRTT;        // レンダリングターゲットのテクスチャ
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    m_pRTV;        // レンダーターゲットビュー
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_pRTSRV;      // 情報として使ったりとか　レンダーターゲットのシェーダーリソースビュー
  Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_pDST;        // 深度ステンシルテクスチャ
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    m_pDSV;        // 深度ステンシルビュー
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_pDSSRV;      // 深度ステンシルのシェーダリソースッビュー
};