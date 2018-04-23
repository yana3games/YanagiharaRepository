#pragma once
//=============================================================================
// File : renderer.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "viewingFrustum.h"
#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include <list>

///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class GameObject;
class Sun;
class CascadeShadowManager;
class CCamera;


///////////////////////////////////////////////////////////////////////////////
// クラスの定義

// レンダラークラス
class D3D11Renderer
{
 public:
  // 描画順レイヤー
  typedef enum
  {
    eLayerSky = 0,           // 背景
    eLayerNormalObject,      // オブジェクト
    eLayerTranslucentObject, // 半透明オブジェクト
    eLayerEffect,            // エフェクト類
    eLayerBillboard,         // ビルボード
    eLayerUI,                // UI
    eLayerFade,              // フェードイン
    eLayerMax                // レイヤー数
  }eLayer;

  ~D3D11Renderer();
  HRESULT Init(HWND hWnd);
  void DrawBegin();
  void Draw();
  void DrawEnd();

  // 描画関連
  void AddDrawLayerList(eLayer layer, GameObject* object);
  void DrawAll(CCamera*);
  void DrawLayer(CCamera*, eLayer start, eLayer end);
  void ReleaseAll(void);
  void Release(GameObject* object, eLayer layer);

  // 各種ゲッター
  ID3D11Device* GetDevice(void);
  ID3D11DeviceContext* GetDeviceContext(void);
  ID3D11RenderTargetView** GetRenderTargetView(void) { return &m_pRTV; }
  ID3D11DepthStencilView** GetDepthStencilView(void) { return &m_pDSV; }
  Sun* GetSun(void) { return m_sun; }

 private:
  // オブジェクト単位の描画
  void DrawObject(GameObject* object, CCamera* pCamera);

  // 描画リスト
  std::vector<std::list<GameObject*>> m_drawObjectLayerList;

  // 視錐台カリングマネージャ
  ViewingFrustum m_viewingFrustum;

  // 影用の太陽データ
  Sun* m_sun;

  // カスケードシャドウ描画用マネージャ
  CascadeShadowManager* m_pCascadeShadowManager;


  ID3D11Device*              m_pDevice;               // DirectX11のデバイス
  ID3D11DeviceContext*       m_pDeviceContext;        // DirectX11のデバイスコンテキスト
  IDXGISwapChain*            m_pSwapChain;            // スワップチェイン

  D3D_DRIVER_TYPE            m_DriverType;            // ドライバータイプ
  D3D_FEATURE_LEVEL          m_FeatureLevel;          // 機能レベルの一番高いの格納
  UINT                       m_MultiSampleCount;      // マルチサンプルのカウント
  UINT                       m_MultiSampleQuality;    // マルチサンプルの品質
  UINT                       m_MultiSampleMaxQuality; // マルチサンプルの最大品質
  UINT                       m_SwapChainCount;        // スワップチェインのカウント
  DXGI_FORMAT                m_SwapChainFormat;       // スワップチェインのフォーマット
  DXGI_FORMAT                m_DepthStencilFormat;    // 深度ステンシルのフォーマット

  ID3D11Texture2D*           m_pRTT;                  // レンダリングターゲットのテクスチャ
  ID3D11RenderTargetView*    m_pRTV;                  // レンダーターゲットビュー？
  ID3D11ShaderResourceView*  m_pRTSRV;                // 情報として使ったりとか　レンダーターゲットのシェーダーリソースビュー
  ID3D11Texture2D*           m_pDST;                  // 深度ステンシルテクスチャ
  ID3D11DepthStencilView*    m_pDSV;                  // 深度ステンシルビュー
  ID3D11ShaderResourceView*  m_pDSSRV;                // 深度ステンシルのシェーダリソースッビュー
  FLOAT                      m_ClearColor[4];         // クリアカラー

  bool                       m_createFlag;            // 生成フラグ
};