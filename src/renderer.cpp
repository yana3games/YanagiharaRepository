//=============================================================================
// File : renderer.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include "renderer.h"
#include "gameObject.h"
#include "appManager.h"
#include "appConfig.h"
#include "appUtility.h"
#include "sun.h"
#include "cascadeShadowManager.h"
#include "cameraMan.h"
#include "camera.h"


///////////////////////////////////////////////////////////////////////////////
// ポリゴンの初期化処理
HRESULT D3D11Renderer::Init(HWND hWnd)
{
  // 描画リストをレイヤー分確保
  m_drawObjectLayerList.resize(eLayerMax);

  // デバイスの初期化
  m_pDevice               = nullptr;                       // DirectX11のデバイス
  m_pDeviceContext        = nullptr;                       // DirectX11のデバイスコンテキスト
  m_pSwapChain            = nullptr;                       // スワップチェイン

  m_DriverType            = D3D_DRIVER_TYPE_NULL;          // ドライバータイプ
  m_FeatureLevel          = D3D_FEATURE_LEVEL_11_0;        // 機能レベルの一番高いの格納ti
  m_MultiSampleCount      = 4;                             // マルチサンプルのカウント
  m_MultiSampleQuality    = 0;                             // マルチサンプルの品質
  m_MultiSampleMaxQuality = 0;                             // マルチサンプルの最大品質
  m_SwapChainCount        = 2;                             // スワップチェインのカウント
  m_SwapChainFormat       = DXGI_FORMAT_R8G8B8A8_UNORM;    // スワップチェインのフォーマット
  m_DepthStencilFormat    = DXGI_FORMAT_D24_UNORM_S8_UINT; // 深度ステンシルのフォーマット

  m_pRTT                  = nullptr;                       // レンダリングターゲットのテクスチャ
  m_pRTV                  = nullptr;                       // レンダーターゲットビュー
  m_pRTSRV                = nullptr;                       // 情報として使ったりとか　レンダーターゲットのシェーダーリソースビュー
  m_pDST                  = nullptr;                       // 深度ステンシルテクスチャ
  m_pDSV                  = nullptr;                       // 深度ステンシルビュー
  m_pDSSRV                = nullptr;                       // 深度ステンシルのシェーダリソースッビュー
  m_ClearColor[0]         = 0.5f;                          // クリアカラー
  m_ClearColor[1]         = 0.8f;
  m_ClearColor[2]         = 0.2f;
  m_ClearColor[3]         = 1.0f;

  // デバイス生成フラグ.
  UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif//defined(DEBUG) || deifned(_DEBUG)

  // ドライバータイプ.
  D3D_DRIVER_TYPE driverTypes[] = {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };
  UINT numDriverTytpes = sizeof(driverTypes) / sizeof(driverTypes[0]);

  // 機能レベル.
  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
  };
  UINT numFeatureLevels = sizeof(featureLevels) / sizeof(featureLevels[0]);

  // スワップチェインの構成設定.
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(DXGI_SWAP_CHAIN_DESC));
  sd.BufferCount                        = m_SwapChainCount;     //バッファ数（ダブルバッファ
  sd.BufferDesc.Width                   = AppConfig::GetInstance()->GetScreenWidth();         //ディスプレイバッファ縦幅横幅
  sd.BufferDesc.Height                  = AppConfig::GetInstance()->GetScreenHeight();
  sd.BufferDesc.Format                  = m_SwapChainFormat;    //DXGI_FORMAT_R8G8B8A8_UNORM; // リソースデータフォーマット
  sd.BufferDesc.RefreshRate.Numerator   = 60;                   //分子 60fpsでリフレッシュ
  sd.BufferDesc.RefreshRate.Denominator = 1;                    //分母
  sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;//ディスプレイバッファの使われ方。レンダーターゲットとして使用・レンダリングテクスチャ
  sd.OutputWindow                       = hWnd;                 //ウィンドウハンドル
  //アンチエイリアシング使う場合は↓の数字いじると勝手にやってくれるっぽい？
  sd.SampleDesc.Count                   = m_MultiSampleCount;   //マルチサンプルのサンプリングカウント数
  sd.SampleDesc.Quality                 = m_MultiSampleQuality; //値が高いほど品質up
  sd.Windowed                           = TRUE;                 //TRUE(ウィンドウモード) FALSE(フルスクリーンモード)

  for (UINT idx = 0; idx < numDriverTytpes; ++idx)
  {
    // ドライバータイプ設定.
    m_DriverType = driverTypes[idx];

    // デバイスとスワップチェインの生成.
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
      NULL,               //ビデオアダプターへのポインタ
      m_DriverType,       //作成するデバイスの種類
      NULL,               //ソフトウェアラスタライザーを実装するDLLのハンドル
      createDeviceFlags,  //有効にするランタイムレイヤー
      featureLevels,      //作成を試みる機能レベルの順序
      numFeatureLevels,   //フューチャーレベルの要素数？
      D3D11_SDK_VERSION,  //SDKのバージョン
      &sd,                //スワップチェーンの初期化パラメータ
      &m_pSwapChain,      //レンダリングに使用するスワップチェーン
      &m_pDevice,         //作成されたデバイス	ID3D11Device
      &m_FeatureLevel,    //このデバイスでサポートされてる機能レベルの一番高いやつ？
      &m_pDeviceContext   //デバイスコンテキスト ID3D11DeviceContext
      );
    // 成功したらループを脱出.
    if (SUCCEEDED(hr)){
      break;
    }

    // 失敗していないかチェック.
    if (FAILED(hr)){
      return false;
    }
  }

  HRESULT hr = S_OK;

  // マルチサンプルの最大品質値を取得.
  hr = m_pDevice->CheckMultisampleQualityLevels(m_SwapChainFormat, m_MultiSampleCount, &m_MultiSampleMaxQuality);
  if (FAILED(hr)){
    MessageBox(hWnd, "マルチサンプルの最大品質値の取得に失敗しました！", "警告！", MB_ICONWARNING);
    return false;
  }

  // バックバッファを取得
  hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_pRTT);
  if (FAILED(hr)){
    MessageBox(hWnd, "バックバッファの取得に失敗しました！", "警告！", MB_ICONWARNING);
    return false;
  }

  // レンダーターゲットを生成.
  hr = m_pDevice->CreateRenderTargetView(m_pRTT, NULL, &m_pRTV);
  if (FAILED(hr)){
    MessageBox(hWnd, "レンダーターゲットの生成に失敗しました！", "警告！", MB_ICONWARNING);
    return false;
  }

  // レンダーターゲットのシェーダリソースビューを生成.
  hr = m_pDevice->CreateShaderResourceView(m_pRTT, NULL, &m_pRTSRV);
  if (FAILED(hr)){
    MessageBox(hWnd, "レンダーターゲットのシェーダリソースビューの生成に失敗しました！", "警告！", MB_ICONWARNING);
    return false;
  }

  DXGI_FORMAT textureFormat = m_DepthStencilFormat;
  DXGI_FORMAT resourceFormat = m_DepthStencilFormat;

  // テクスチャとシェーダリソースビューのフォーマットを適切なものに変更.
  switch (m_DepthStencilFormat)
  {
  case DXGI_FORMAT_D16_UNORM:
  {
    textureFormat = DXGI_FORMAT_R16_TYPELESS;
    resourceFormat = DXGI_FORMAT_R16_UNORM;
  }
  break;

  case DXGI_FORMAT_D24_UNORM_S8_UINT:
  {
    textureFormat = DXGI_FORMAT_R24G8_TYPELESS;
    resourceFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
  }
  break;

  case DXGI_FORMAT_D32_FLOAT:
  {
    textureFormat = DXGI_FORMAT_R32_TYPELESS;
    resourceFormat = DXGI_FORMAT_R32_FLOAT;
  }
  break;

  case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
  {
    textureFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
    resourceFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
  }
  break;
  }
  D3D11_TEXTURE2D_DESC td;

  // 深度ステンシルテクスチャの生成.
  ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
  td.Width					= AppConfig::GetInstance()->GetScreenWidth();
  td.Height					= AppConfig::GetInstance()->GetScreenHeight();
  td.MipLevels				= 1;
  td.ArraySize				= 1;
  td.Format					= textureFormat;
  td.SampleDesc.Count			= m_MultiSampleCount;
  td.SampleDesc.Quality		= m_MultiSampleQuality;
  td.Usage					= D3D11_USAGE_DEFAULT;
  td.BindFlags				= D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
  td.CPUAccessFlags			= 0;
  td.MiscFlags				= 0;

  // 深度ステンシルテクスチャの生成.
  hr = m_pDevice->CreateTexture2D(&td, NULL, &m_pDST);
  if (FAILED(hr)){
  	MessageBox(hWnd, "深度ステンシルテクスチャの生成に失敗しました！", "警告！", MB_ICONWARNING);
  	return false;
  }

  // 深度ステンシルビューの設定.
  //マルチサンプルの数がゼロでないときは
  //D3D11_DSV_DIMENSION_TEXTURE2DMSや
  //D3D11_SRV_DIMENSION_TEXTURE2DMSを設定しないといけない
  D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
  ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
  dsvd.Format = m_DepthStencilFormat;
  if (m_MultiSampleCount == 0){
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvd.Texture2D.MipSlice = 0;
  }
  else{
    dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
  }


  // 深度ステンシルビューの生成.
  hr = m_pDevice->CreateDepthStencilView(m_pDST, &dsvd, &m_pDSV);
  if (FAILED(hr)) {
    MessageBox(hWnd, "深度ステンシルビューの生成に失敗しました！", "警告！", MB_ICONWARNING);
    return false;
  }

  // シェーダリソースビューの設定.
  D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
  ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
  srvd.Format = resourceFormat;

  if (m_MultiSampleCount == 0){
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MostDetailedMip = 0;
    srvd.Texture2D.MipLevels = 1;
  }
  else{
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
  }

  // シェーダリソースビューを生成.
  hr = m_pDevice->CreateShaderResourceView(m_pDST, &srvd, &m_pDSSRV);
  if (FAILED(hr)){
    MessageBox(hWnd, "シェーダーリソースビューの生成に失敗しました！", "警告！", MB_ICONWARNING);
    return false;
  }

  // デバイスコンテキストにレンダーターゲットを設定.
  m_pDeviceContext->OMSetRenderTargets(1, &m_pRTV, m_pDSV);

  // ビューポートの設定.
  D3D11_VIEWPORT vp;
  vp.Width = (float)AppConfig::GetInstance()->GetScreenWidth();
  vp.Height = (float)AppConfig::GetInstance()->GetScreenHeight();
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;

  // デバイスコンテキストにビューポートを設定.
  m_pDeviceContext->RSSetViewports(1, &vp);
  
  // ブレンドステート作成
  //ID3D11BlendState* hpBlendState = nullptr;
  //D3D11_BLEND_DESC BlendStateDesc;
  //BlendStateDesc.AlphaToCoverageEnable = false;
  //BlendStateDesc.IndependentBlendEnable = false;
  //for (int i = 0; i < 8; i++) {
  //  BlendStateDesc.RenderTarget[i].BlendEnable = true;
  //  BlendStateDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  //  BlendStateDesc.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  //  BlendStateDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
  //  BlendStateDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
  //  BlendStateDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
  //  BlendStateDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  //  BlendStateDesc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  //}
  //m_pDevice->CreateBlendState(&BlendStateDesc, &hpBlendState);
  //
  //// ブレンドステートをコンテキストに設定
  //float blendFactor[4] = { D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
  //m_pDeviceContext->OMSetBlendState(hpBlendState, blendFactor, 0xffffffff);


  // 影用太陽データ生成
  m_sun = new Sun(Vector3(1000.0f, 1000.0f, 0.0f), Vector3(-1.0f, -1.0f, 0.0f));

  // カスケードシャドウ用マネージャの生成
  m_pCascadeShadowManager = new CascadeShadowManager;
  m_pCascadeShadowManager->SunSet(m_sun);

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// レンダラーの終了処理
D3D11Renderer::~D3D11Renderer(void)
{
  if (m_pDeviceContext){
    m_pDeviceContext->ClearState();
    m_pDeviceContext->Flush();
  }

  SAFE_RELEASE(m_pRTSRV)
  SAFE_RELEASE(m_pRTV)
  SAFE_RELEASE(m_pRTT)

  SAFE_RELEASE(m_pDSSRV)
  SAFE_RELEASE(m_pDSV)
  SAFE_RELEASE(m_pDST)

  SAFE_RELEASE(m_pSwapChain)
  SAFE_RELEASE(m_pDeviceContext)
  SAFE_RELEASE(m_pDevice)

  if (m_sun) delete m_sun;
  if (m_pCascadeShadowManager) delete m_pCascadeShadowManager;
}


///////////////////////////////////////////////////////////////////////////////
// レンダラーの描画処理
void D3D11Renderer::DrawBegin(void)
{
  // ビューをクリアする
  m_pDeviceContext->ClearRenderTargetView(m_pRTV, m_ClearColor);
  m_pDeviceContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


///////////////////////////////////////////////////////////////////////////////
// レンダラーの描画処理
void D3D11Renderer::Draw(void)
{
  CCameraMan::GetInstance()->Set();
  //DrawAll(CCameraMan::GetInstance()->GetCamera());
  m_pCascadeShadowManager->Draw();
}


///////////////////////////////////////////////////////////////////////////////
// レンダラーの描画処理
void D3D11Renderer::DrawEnd(void)
{
  // 描画結果を表示
  m_pSwapChain->Present(0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// 描画リストへオブジェクトを追加
void D3D11Renderer::AddDrawLayerList(eLayer layer, GameObject* object)
{
  m_drawObjectLayerList[layer].push_back(object);
}


///////////////////////////////////////////////////////////////////////////////
// オブジェクト単位の描画
void D3D11Renderer::DrawObject(GameObject* object, CCamera* pCamera)
{
  // 使用中か
  if (!object->IsVisible()) return;
  // 視錐台内に居るか
  if (!object->ViewingFrustumCulling(&m_viewingFrustum)) return;
  // 描画
  object->Render(pCamera);
}


///////////////////////////////////////////////////////////////////////////////
// 描画リスト内のオブジェクト全てを描画
void D3D11Renderer::DrawAll(CCamera* pCamera)
{
  // 視錐台カリング用クラスに視錐台情報をセット
  m_viewingFrustum.SetupViewingFrustum(pCamera->GetFar(), *pCamera->GetProjMtx(), *pCamera->GetViewMtx());

  // 描画
  for (auto&& layer : m_drawObjectLayerList) {
    for (auto&& object : layer) {
      DrawObject(object, pCamera);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// 特定のレイヤーからレイヤーまでの描画
void D3D11Renderer::DrawLayer( CCamera* pCamera, eLayer start, eLayer end)
{
  // 視錐台カリング用クラスに視錐台情報をセット
  m_viewingFrustum.SetupViewingFrustum(pCamera->GetFar(), *pCamera->GetProjMtx(), *pCamera->GetViewMtx());

  // 描画
  for (auto layer = m_drawObjectLayerList.begin() + (int)start; layer != m_drawObjectLayerList.begin() + (int)end + 1; ++layer) {
    for (auto&& object : (*layer)) {
      DrawObject(object, pCamera);
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// 全描画リスト削除
void D3D11Renderer::ReleaseAll(void)
{
  for (auto layer = m_drawObjectLayerList.begin(); layer != m_drawObjectLayerList.begin(); ++layer) {
    layer->clear();
  }
  m_drawObjectLayerList.clear();
}


///////////////////////////////////////////////////////////////////////////////
// 特定のオブジェクトを描画レイヤーから削除予約
void D3D11Renderer::Release(GameObject* object, eLayer layer)
{
  m_drawObjectLayerList[layer].erase( std::find(m_drawObjectLayerList[layer].begin(), m_drawObjectLayerList[layer].end(), object));
}


///////////////////////////////////////////////////////////////////////////////
// デバイスのゲッター
ID3D11Device* D3D11Renderer::GetDevice(void)
{
  return m_pDevice;
}


///////////////////////////////////////////////////////////////////////////////
// デバイスのゲッター
ID3D11DeviceContext* D3D11Renderer::GetDeviceContext(void)
{
  return m_pDeviceContext;
}

