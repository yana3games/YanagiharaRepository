//=============================================================================
// File : appManager.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
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
//	�ÓI�����o�ϐ�
ModeContext*    AppManager::m_modeContext = nullptr;
D3D11Renderer*  AppManager::m_renderer = nullptr;
CInputMouse*    AppManager::m_mouse = nullptr;
CInputKeyboard* AppManager::m_keyboard = nullptr;
DirectSound*    AppManager::m_sound = nullptr;
ShaderManager*  AppManager::m_shaderManager = nullptr;
TextureManager* AppManager::m_textureManager = nullptr;


///////////////////////////////////////////////////////////////////////////////
//	�R���X�g���N�^( �f�t�H���g)
AppManager::AppManager()
{
}


///////////////////////////////////////////////////////////////////////////////
//	�f�X�g���N�^( �f�t�H���g)
AppManager::~AppManager()
{
}


///////////////////////////////////////////////////////////////////////////////
// ����������
HRESULT AppManager::Init( HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
  // �����_���[�̐���
  m_renderer = new D3D11Renderer;
  m_renderer->Init(hWnd);

  // �}�E�X�̏�����
  m_mouse = new CInputMouse;
  m_mouse->Init(hInstance, hWnd);

  // �L�[�{�[�h�̏�����
  m_keyboard = new CInputKeyboard;
  m_keyboard->Init(hInstance, hWnd);

  // �T�E���h�̏�����
  m_sound = new DirectSound;
  m_sound->Init(hWnd);

  // �V�F�[�_�}�l�[�W���̐���
  m_shaderManager = new ShaderManager;
  m_shaderManager->Initialize();
  m_shaderManager->SetDefaultShader();

  // �e�N�X�`���}�l�[�W���̐���
  m_textureManager = new TextureManager;

  // ImGUI�̏�����
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  ImGui_ImplDX11_Init(hWnd, m_renderer->GetDevice(), m_renderer->GetDeviceContext());
  // Setup style
  //ImGui::StyleColorsDark();
  ImGui::StyleColorsClassic();

  // ���[�h�Ǘ��l����
  m_modeContext = new ModeContext;
  m_modeContext->SetMode(new GameMode);
  m_modeContext->NextMode();

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// �I������
void AppManager::Uninit(void)
{
  // ���݃��[�h�̔j��
  if (m_modeContext != NULL) {
    delete m_modeContext;
    m_modeContext = NULL;
  }
  
  // ImGUI�̏I��
  ImGui_ImplDX11_Shutdown();
  ImGui::DestroyContext();

  // �e�N�X�`���}�l�[�W���̔j��
  if (m_textureManager) delete m_textureManager;

  // �V�F�[�_�}�l�[�W���̔j��
  if (m_shaderManager) delete m_shaderManager;

  // �T�E���h�̔j��
  if (m_sound) delete m_sound;

  // �L�[�{�[�h�̔j��
  if (m_keyboard) delete m_keyboard;

  // �}�E�X�̔j��
  if(m_mouse) delete m_mouse;

  // �����_���[�̔j��
  if (m_renderer) delete m_renderer;
}


///////////////////////////////////////////////////////////////////////////////
// �X�V����
void AppManager::Update(void)
{
  // �V�X�e���ʂ̍X�V
  ImGui_ImplDX11_NewFrame();
  m_keyboard->Update();
  m_mouse->Update();
  CCameraMan::GetInstance()->Update();

  // �e�X�V
  GameObject::UpdateAll();
  m_modeContext->update();

  if (m_keyboard->GetKeyPress(DIK_F1)) m_modeContext->SetMode(new TitleMode);
  if (m_keyboard->GetKeyPress(DIK_F2)) m_modeContext->SetMode(new GameMode);
  if (m_keyboard->GetKeyPress(DIK_F3)) m_modeContext->SetMode(new ResultMode);
}


///////////////////////////////////////////////////////////////////////////////
// �`�揈��
void AppManager::Draw(void)
{
  // �`��
  m_renderer->DrawBegin();
  
  m_renderer->Draw();
  ImGui::Render();
  ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

  m_renderer->DrawEnd();

  // �`���Ƀ��[�h�̐؂�ւ�
  m_modeContext->NextMode();
}


///////////////////////////////////////////////////////////////////////////////
// ���[�h�Ǘ��N���X�̎擾
ModeContext* AppManager::GetModeContext(void)
{
  return m_modeContext;
}


///////////////////////////////////////////////////////////////////////////////
// �����_���[�̎擾
D3D11Renderer* AppManager::GetRenderer(void)
{
  return m_renderer;
}


///////////////////////////////////////////////////////////////////////////////
// �L�[�{�[�h�̎擾
CInputKeyboard* AppManager::GetKeyboard(void)
{
  return m_keyboard;
}


///////////////////////////////////////////////////////////////////////////////
// �}�E�X�̎擾
CInputMouse* AppManager::GetMouse(void)
{
  return m_mouse;
}


///////////////////////////////////////////////////////////////////////////////
// �V�F�[�_�[�}�l�[�W���̎擾
ShaderManager* AppManager::GetShaderManager(void)
{
  return m_shaderManager;
}


///////////////////////////////////////////////////////////////////////////////
// �e�N�X�`���}�l�[�W���̎擾
TextureManager* AppManager::GetTextureManager(void)
{
  return m_textureManager;
}
