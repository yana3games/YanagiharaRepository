//=============================================================================
// File : effectManager.h
// Date : 2018/02/01(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "effectManager.h"
#include "appManager.h"
#include "renderer.h"
#include "cameraMan.h"
#include "camera.h"

using namespace DirectX;
using namespace Effekseer;

///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
EffectManager::EffectManager(void)
{
  // �`��Ǘ��C���X�^���X�̐���
  m_renderer = ::EffekseerRendererDX11::Renderer::Create(AppManager::GetRenderer()->GetDevice(), AppManager::GetRenderer()->GetDeviceContext(), m_MaxEffects);
  // �T�E���h�Ǘ��C���X�^���X�̐���
//  ::EffekseerSound::Sound* sound = ::EffekseerSound::Sound::Create(IXAudio2*, ���m�����Đ��p�{�C�X��, �X�e���I�Đ��p�{�C�X��);
  // �G�t�F�N�g�Ǘ��p�C���X�^���X�̐���
  m_manager = Manager::Create(m_MaxEffects);

  // �`����@���w�肵�܂��B�Ǝ��Ɋg�����邱�Ƃ��ł��܂��B
  m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
  m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
  m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
  m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
  m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

  // �e�N�X�`���摜�̓Ǎ����@�̎w��(�p�b�P�[�W������ǂݍ��ޏꍇ�g������K�v������܂��B)
  m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());

  // �T�E���h�Đ��p�C���X�^���X�̎w��
  //m_manager->SetSoundPlayer(m_sound->CreateSoundPlayer());

  // �T�E���h�f�[�^�̓Ǎ����@�̎w��(���k�t�H�[�}�b�g�A�p�b�P�[�W������ǂݍ��ޏꍇ�g������K�v������܂��B)
  //m_manager->SetSoundLoader(m_sound->CreateSoundLoader());

  // ���W�n�̎w��(RH�ŉE��n�ALH�ō���n)
  m_manager->SetCoordinateSystem(CoordinateSystem::LH);

  // �J�����O���s���͈͂�ݒ�
  m_manager->CreateCullingWorld(1000.0f, 1000.0f, 1000.0f, 5);
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
EffectManager::~EffectManager(void)
{
  // �G�t�F�N�g�̉��
  AllRelease();
  // �G�t�F�N�g�Ǘ��p�C���X�^���X��j��
  m_manager->Destroy();
  // �T�E���h�p�C���X�^���X��j��
  m_sound->Destroy();
  // �`��p�C���X�^���X��j��
  m_renderer->Destroy();
}


///////////////////////////////////////////////////////////////////////////////
// �G�t�F�N�g�ǂݍ���
bool EffectManager::Load(const std::wstring& key)
{
  // �G�t�F�N�g������
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it == m_res.end()) {            // ������Ȃ������ꍇ
    Effect* effect = Effect::Create(m_manager, (EFK_CHAR*)key.c_str());
    if (!effect) {
      MessageBox(NULL, "FailedLoadEffect!", "LoadError!", MB_OK);
      return false;
    }
    Res res;
    res.nCnt = 1;
    res.effect = effect;
    m_res[key] = res;
    return true;
  }
  it->second.nCnt += 1;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �G�t�F�N�g�̍Đ�
Handle EffectManager::Play(const std::wstring& key, const Vector3& pos)
{
  // �G�t�F�N�g������
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it != m_res.end()) {       // ���������ꍇ
    Handle handle= m_manager->Play( it->second.effect, pos.x, pos.y, pos.z);
    return handle;
  }
  else return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// �G�t�F�N�g�̊J��
bool EffectManager::Release(std::wstring key)
{
  // �G�t�F�N�g������
  std::unordered_map<std::wstring, Res>::iterator itr = m_res.find(key);
  if (itr == m_res.end()) return false;
  if ((itr->second.nCnt--) == 0) {       // ���������ꍇ
    ES_SAFE_RELEASE( itr->second.effect);
    m_res.erase(itr);
    return true;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//	�S�Ẳ摜�̊J��
bool EffectManager::AllRelease()
{
  for (auto&& res : m_res) {
    ES_SAFE_RELEASE( res.second.effect);
  }
  m_res.clear();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �`�揈��
void EffectManager::Draw()
{
  // �J�����O�̌v�Z
  m_manager->CalcCulling(m_renderer->GetCameraProjectionMatrix(), false);
  m_renderer->BeginRendering();
  m_manager->Draw();
  m_renderer->EndRendering();
}


///////////////////////////////////////////////////////////////////////////////
// �X�V����
void EffectManager::Update()
{
  m_renderer->SetProjectionMatrix( XM( *CCameraMan::GetInstance()->GetProjMtx() ) );
  // �J�����s��̍X�V
  m_renderer->SetCameraMatrix( XM( *CCameraMan::GetInstance()->GetViewMtx() ) );
  // 3D�T�E���h�p���X�i�[�ݒ�̍X�V
  //m_sound->SetListener(���X�i�[�ʒu, ���ړ_, ������x�N�g��);
  // �Đ����̃G�t�F�N�g�̈ړ���(::Effekseer::Manager�o�R�ŗl�X�ȃp�����[�^�[���ݒ�ł��܂��B)
  //m_manager->AddLocation(handle, ::Effekseer::Vector3D);
  // �S�ẴG�t�F�N�g�̍X�V
  m_manager->Update();
}


///////////////////////////////////////////////////////////////////////////////
// �K���L���X�g
Matrix44 EffectManager::XM(XMMATRIX& xm)
{
  Matrix44 matrix;
  for (int i = 0; i < 4; i++)
  {
    matrix.Values[i][0] = XMVectorGetX(xm.r[i]);
    matrix.Values[i][1] = XMVectorGetY(xm.r[i]);
    matrix.Values[i][2] = XMVectorGetZ(xm.r[i]);
    matrix.Values[i][3] = XMVectorGetW(xm.r[i]);
  }
  return matrix;
}

