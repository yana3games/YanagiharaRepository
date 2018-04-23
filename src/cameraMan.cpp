//=============================================================================
// File : camera.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include "appManager.h"
#include "input.h"
#include "cameraMan.h"
#include "camera.h"

///////////////////////////////////////////////////////////////////////////////
//	�J�����}���̍X�V����
void CCameraMan::Update()
{
  if (!m_pCamera) return;

  //	�J�����̑���
  CameraOperation();

  //	�J�����̎����̌v�Z
  m_pCamera->ComputationZiku();
}

///////////////////////////////////////////////////////////////////////////////
//	�J�����}���̍X�V����
void CCameraMan::Set()
{
  //	�J�����̃Z�b�g
  SetVPMtx();
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̑���
void CCameraMan::CameraOperation()
{
  if (!m_pCamera) return;
  // �����_�����ړ�
  //	�O
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_W)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraZ(), m_movelate);
  }
  //	���
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_S)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraZ(), -m_movelate);
  }
  //	�E
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_A)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraX(), m_movelate);
  }
  //	��
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_D)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraX(), -m_movelate);
  }

  // �J��������ړ�
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_Z))
  {
    m_pCamera->SpiralUp(-m_rotatelate);
  }
  //	���
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_C))
  {
    m_pCamera->SpiralDown(m_rotatelate);
  }
  //	�E
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_Q))
  {
    m_pCamera->SpiralSide(-m_rotatelate);
  }
  //	��
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_E))
  {
    m_pCamera->SpiralSide(m_rotatelate);
  }

  // �����_�ړ�
  //	�O
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_UP)) {
    m_pCamera->TurningUp(m_rotatelate);
  }
  //	���
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_DOWN)) {
    m_pCamera->TurningDown(-m_rotatelate);
  }
  //	�E
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_LEFT)) {
    m_pCamera->TurningSide(-m_rotatelate);
  }
  //	��
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_RIGHT)) {
    m_pCamera->TurningSide(m_rotatelate);
  }
}

///////////////////////////////////////////////////////////////////////////////
//	�J�����}���̎����Ă�J�����̃}�g���N�X���Z�b�g
void CCameraMan::SetVPMtx()
{
  if (!m_pCamera) return;
  m_pCamera->SetMtxCamera();
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̃v���W�F�N�V�����}�g���N�X�擾
DirectX::XMMATRIX* CCameraMan::GetProjMtx(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetProjMtx();
}


///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̃v���W�F�N�V�����}�g���N�X���̃Z�b�g
void CCameraMan::SetProjMtx(float width, float height, float Near)
{
	if (!m_pCamera) return;
	m_pCamera->SetProjMtx(width, height, Near);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̃r���[�}�g���N�X�擾
DirectX::XMMATRIX* CCameraMan::GetViewMtx(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetViewMtx();
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J������Z���擾
DirectX::XMVECTOR* CCameraMan::GetCameraZ(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetCameraZ();
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J������X���擾
DirectX::XMVECTOR* CCameraMan::GetCameraX(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetCameraX();
}

///////////////////////////////////////////////////////////////////////////////
//	�J�����}���ɃJ�����̃Z�b�g
void CCameraMan::CameraSet( CCamera* pCamera)
{
	m_pCamera = pCamera;
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J������XYZ���s�ړ�
void CCameraMan::ParallelShiftCameraXYZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
	if (!m_pCamera) return;
	m_pCamera->ParallelShiftXYZ(VecDirect, movelate);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J������XZ���ʈړ�
void CCameraMan::ParallelShiftCameraXZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
	if (!m_pCamera) return;
	m_pCamera->ParallelShiftXZ(VecDirect, movelate);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̗������쁪
void CCameraMan::SpiralCameraUp(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->SpiralUp(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̗������쁫
void CCameraMan::SpiralCameraDown(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->SpiralDown(rotatelate);
}


///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̍��E��������
void CCameraMan::SpiralCameraSide(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->SpiralSide(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̐��񑀍쁪
void CCameraMan::TurningCameraUp(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->TurningUp(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̐��񑀍쁫
void CCameraMan::TurningCameraDown(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->TurningDown(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	�����Ă���J�����̍��E���񑀍�
void CCameraMan::TurningCameraSide(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->TurningSide(rotatelate);
}

