//=============================================================================
// File : appManager.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include "appManager.h"
#include "renderer.h"
#include "input.h"
#include "sound.h"
#include "shaderManager.h"
#include "textureManager.h"
#include "imgui_impl_dx11.h"
#include "gameObject.h"
#include "cameraMan.h"

// mode
#include "mode.h"
#include "title.h"
#include "game.h"
#include "result.h"


///////////////////////////////////////////////////////////////////////////////
//	静的メンバ変数
ModeContext*    AppManager::m_modeContext = nullptr;
D3D11Renderer*  AppManager::m_renderer = nullptr;
CInputMouse*    AppManager::m_mouse = nullptr;
CInputKeyboard* AppManager::m_keyboard = nullptr;
DirectSound*    AppManager::m_sound = nullptr;
ShaderManager*  AppManager::m_shaderManager = nullptr;
TextureManager* AppManager::m_textureManager = nullptr;


///////////////////////////////////////////////////////////////////////////////
//	コンストラクタ( デフォルト)
AppManager::AppManager()
{
}


///////////////////////////////////////////////////////////////////////////////
//	デストラクタ( デフォルト)
AppManager::~AppManager()
{
}


///////////////////////////////////////////////////////////////////////////////
// 初期化処理
HRESULT AppManager::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
  // レンダラーの生成
  m_renderer = new D3D11Renderer;
  m_renderer->Init(hWnd);

  // マウスの初期化
  m_mouse = new CInputMouse;
  m_mouse->Init(hInstance, hWnd);

  // キーボードの初期化
  m_keyboard = new CInputKeyboard;
  m_keyboard->Init(hInstance, hWnd);

  // サウンドの初期化
  m_sound = new DirectSound;
  m_sound->Init(hWnd);

  // シェーダマネージャの生成
  m_shaderManager = new ShaderManager;
  m_shaderManager->Initialize();
  m_shaderManager->SetDefaultShader();

  // テクスチャマネージャの生成
  m_textureManager = new TextureManager;

  // ImGUIの初期化
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  ImGui_ImplDX11_Init(hWnd, m_renderer->GetDevice(), m_renderer->GetDeviceContext());
  // Setup style
  //ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

  // モード管理人生成
  m_modeContext = new ModeContext;
  m_modeContext->SetMode(new GameMode);
  m_modeContext->NextMode();

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// 終了処理
void AppManager::Uninit(void)
{
  // 現在モードの破棄
  if (m_modeContext != NULL) {
    delete m_modeContext;
    m_modeContext = NULL;
  }
  
  // ImGUIの終了
  ImGui_ImplDX11_Shutdown();
  ImGui::DestroyContext();

  // テクスチャマネージャの破棄
  if (m_textureManager) delete m_textureManager;

  // シェーダマネージャの破棄
  if (m_shaderManager) delete m_shaderManager;

  // サウンドの破棄
  if (m_sound) delete m_sound;

  // キーボードの破棄
  if (m_keyboard) delete m_keyboard;

  // マウスの破棄
  if(m_mouse) delete m_mouse;

  // レンダラーの破棄
  if (m_renderer) delete m_renderer;
}


///////////////////////////////////////////////////////////////////////////////
// 更新処理
void AppManager::Update(void)
{
  // システム面の更新
  ImGui_ImplDX11_NewFrame();
  m_keyboard->Update();
  m_mouse->Update();
  CCameraMan::GetInstance()->Update();

  // 各更新
  GameObject::UpdateAll();
  m_modeContext->update();

  if (m_keyboard->GetKeyPress(DIK_F1)) m_modeContext->SetMode(new TitleMode);
  if (m_keyboard->GetKeyPress(DIK_F2)) m_modeContext->SetMode(new GameMode);
  if (m_keyboard->GetKeyPress(DIK_F3)) m_modeContext->SetMode(new ResultMode);
}


///////////////////////////////////////////////////////////////////////////////
// 描画処理
void AppManager::Draw(void)
{
  // 描画
  m_renderer->DrawBegin();
  
  m_renderer->Draw();
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  m_renderer->DrawEnd();

  // 描画後にモードの切り替え
  m_modeContext->NextMode();
}


///////////////////////////////////////////////////////////////////////////////
// モード管理クラスの取得
ModeContext* AppManager::GetModeContext(void)
{
  return m_modeContext;
}


///////////////////////////////////////////////////////////////////////////////
// レンダラーの取得
D3D11Renderer* AppManager::GetRenderer(void)
{
  return m_renderer;
}


///////////////////////////////////////////////////////////////////////////////
// キーボードの取得
CInputKeyboard* AppManager::GetKeyboard(void)
{
  return m_keyboard;
}


///////////////////////////////////////////////////////////////////////////////
// マウスの取得
CInputMouse* AppManager::GetMouse(void)
{
  return m_mouse;
}


///////////////////////////////////////////////////////////////////////////////
// シェーダーマネージャの取得
ShaderManager* AppManager::GetShaderManager(void)
{
  return m_shaderManager;
}


///////////////////////////////////////////////////////////////////////////////
// テクスチャマネージャの取得
TextureManager* AppManager::GetTextureManager(void)
{
  return m_textureManager;
}
