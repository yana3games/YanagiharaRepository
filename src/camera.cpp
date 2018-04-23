//=============================================================================
// File : camera.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include "camera.h"
#include "appConfig.h"
#include "renderer.h"


///////////////////////////////////////////////////////////////////////////////
//	�f�t�H���g�R���X�g���N�^
CCamera::CCamera() : m_posCamera(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)), m_posAt(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)), m_vecUp(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)), m_fFov(1.0f), m_width((float)AppConfig::GetInstance()->GetScreenWidth()), m_height((float)AppConfig::GetInstance()->GetScreenHeight()), m_far(5000.0f), m_near(0.01f)
{
  m_distance = DirectX::XMVectorSubtract(m_posAt, m_posCamera);
  m_olddistance = m_distance;
  ComputationZiku();
}


///////////////////////////////////////////////////////////////////////////////
//	�R���X�g���N�^
CCamera::CCamera(const DirectX::XMVECTOR& posCamera, const DirectX::XMVECTOR& posAt) :m_posCamera(posCamera), m_posAt(posAt), m_vecUp(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)), m_fFov(1.0f), m_width((float)AppConfig::GetInstance()->GetScreenWidth()), m_height((float)AppConfig::GetInstance()->GetScreenHeight()), m_far(5000.0f), m_near(0.01f)
{
  m_distance = DirectX::XMVectorSubtract(m_posAt, m_posCamera);
  m_olddistance = m_distance;
  ComputationZiku();
}


///////////////////////////////////////////////////////////////////////////////
//	�f�X�g���N�^
CCamera::~CCamera()
{
}


///////////////////////////////////////////////////////////////////////////////
//	�e�����v�Z
void CCamera::ComputationZiku()
{
  m_distance = DirectX::XMVectorSubtract(m_posAt, m_posCamera);
  m_Zziku = DirectX::XMVector3Normalize(m_distance);
  m_Xziku = DirectX::XMVector3Cross(m_Zziku, m_vecUp);
  m_Xziku = DirectX::XMVector3Normalize(m_Xziku);
  m_Yziku = DirectX::XMVector3Cross(m_Xziku, m_Zziku);
  m_Yziku = DirectX::XMVector3Normalize(m_Yziku);
}


///////////////////////////////////////////////////////////////////////////////
//	�Y�[���C��
void CCamera::ZoomOut()
{
  m_fFov -= 0.01f;
  m_fFov = max(m_fFov, 0.01f);
}


///////////////////////////////////////////////////////////////////////////////
//	�Y�[���A�E�g
void CCamera::ZoomIn()
{
  m_fFov += 0.01f;
  m_fFov = min(m_fFov, DirectX::XM_PI - 0.01f);
}


///////////////////////////////////////////////////////////////////////////////
//	�Z�b�g�J����
void CCamera::SetMtxCamera()
{
  //�r���[�s��ւ̊i�[
  m_mtxView = DirectX::XMMatrixLookAtLH(m_posCamera, m_posAt, m_vecUp);

  //�p�[�X�̕������v���W�F�N�V�����s��̍쐬 ( �i�[��@��p�@�A�X��@�j�A�[�@�t�@�[)   0 < near
  m_mtxProj = DirectX::XMMatrixPerspectiveFovLH(m_fFov, m_width / m_height, m_near, m_far);
}


///////////////////////////////////////////////////////////////////////////////
//	�r���[�}�g���N�X�̃Q�b�^�[
DirectX::XMMATRIX* CCamera::GetViewMtx(void)
{
  return &m_mtxView;
}


///////////////////////////////////////////////////////////////////////////////
//	�v���W�F�N�V�����}�g���N�X�̃Q�b�^�[
DirectX::XMMATRIX* CCamera::GetProjMtx(void)
{
  return &m_mtxProj;
}


///////////////////////////////////////////////////////////////////////////////
//	�J������Z���̃Q�b�^�[
DirectX::XMVECTOR* CCamera::GetCameraZ(void)
{
  return &m_Zziku;
}

///////////////////////////////////////////////////////////////////////////////
//	�J������X���̃Q�b�^�[
DirectX::XMVECTOR* CCamera::GetCameraX(void)
{
  return &m_Xziku;
}

///////////////////////////////////////////////////////////////////////////////
//	�J������Y���̃Q�b�^�[
DirectX::XMVECTOR* CCamera::GetCameraY(void)
{
  return &m_Yziku;
}

///////////////////////////////////////////////////////////////////////////////
//	�J������XYZ���s�ړ�
void CCamera::ParallelShiftXYZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
  m_posCamera = DirectX::XMVectorAdd( m_posCamera, DirectX::XMVectorScale( *VecDirect, movelate));
  m_posAt = DirectX::XMVectorAdd(m_posAt, DirectX::XMVectorScale(*VecDirect, movelate));
}

///////////////////////////////////////////////////////////////////////////////
//	�J������XZ���ʕ��s�ړ�
void CCamera::ParallelShiftXZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
  DirectX::XMVECTOR work = *VecDirect;
  DirectX::XMVectorSetY(work, 0.0f);
  work = DirectX::XMVector3Normalize(work);
  m_posCamera = DirectX::XMVectorAdd(m_posCamera, DirectX::XMVectorScale(work, movelate));
  m_posAt = DirectX::XMVectorAdd(m_posAt, DirectX::XMVectorScale(work, movelate));
}


///////////////////////////////////////////////////////////////////////////////
//	�J�����̗����ړ���
void CCamera::SpiralUp( float rotatelate)
{
  DirectX::XMMATRIX work;
  work = DirectX::XMMatrixIdentity();
  DirectX::XMVECTOR olddistance = m_distance;
  work = DirectX::XMMatrixRotationAxis(m_Xziku, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_vecUp, m_Zziku)) < -CAMERA_EXCURSION)
  {
    m_distance = olddistance;
  }
  m_posCamera = DirectX::XMVectorSubtract(m_posAt, m_distance);
}


///////////////////////////////////////////////////////////////////////////////
//	�J�����̗����ړ���
void CCamera::SpiralDown(float rotatelate)
{
  DirectX::XMMATRIX work;
  DirectX::XMVECTOR olddistance = m_distance;
  work = DirectX::XMMatrixRotationAxis(m_Xziku, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_vecUp, m_Zziku)) > CAMERA_EXCURSION)
  {
    m_distance = olddistance;
  }
  m_posCamera = DirectX::XMVectorSubtract(m_posAt, m_distance);
}

///////////////////////////////////////////////////////////////////////////////
//	�J�����̍��E�����ړ�
void CCamera::SpiralSide(float rotatelate)
{
  DirectX::XMMATRIX work;
  work = DirectX::XMMatrixRotationAxis(m_vecUp, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  m_posCamera = DirectX::XMVectorSubtract(m_posAt, m_distance);
}


///////////////////////////////////////////////////////////////////////////////
//	�J�����̐���ړ���
void CCamera::TurningUp(float rotatelate)
{
  DirectX::XMMATRIX work;
  DirectX::XMVECTOR olddistance = m_distance;
  work = DirectX::XMMatrixRotationAxis(m_Xziku, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);

  m_Xziku = DirectX::XMVector3Normalize(m_distance);
  if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_vecUp, m_Zziku)) > CAMERA_EXCURSION)
  {
    m_distance = olddistance;
  }
  m_posAt = DirectX::XMVectorAdd(m_distance, m_posCamera);
}

///////////////////////////////////////////////////////////////////////////////
//	�J�����̐���ړ���
void CCamera::TurningDown(float rotatelate)
{
  DirectX::XMMATRIX work;
  DirectX::XMVECTOR olddistance = m_distance;
  work = DirectX::XMMatrixRotationAxis(m_Xziku, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  m_Xziku = DirectX::XMVector3Normalize(m_distance);
  if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_vecUp, m_Zziku)) < -CAMERA_EXCURSION)
  {
    m_distance = olddistance;
  }
  m_posAt = DirectX::XMVectorAdd(m_distance, m_posCamera);
}

///////////////////////////////////////////////////////////////////////////////
//	�J�����̍��E����ړ�
void CCamera::TurningSide(float rotatelate)
{
  DirectX::XMMATRIX work;
  work = DirectX::XMMatrixRotationAxis(m_vecUp, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  m_posAt = DirectX::XMVectorAdd(m_distance, m_posCamera);
}

///////////////////////////////////////////////////////////////////////////////
//	�v���W�F�N�V�����}�g���N�X�p���̃Z�b�g
void CCamera::SetProjMtx(float width, float height, float Near)
{
  m_width = width;
  m_height = height;
  m_near = Near;
}


///////////////////////////////////////////////////////////////////////////////
// �v���W�F�N�V�����}�g���N�X�Z�b�g
void CCamera::SetProjMtx(DirectX::XMMATRIX proj)
{
  m_mtxProj = proj;
}


///////////////////////////////////////////////////////////////////////////////
// �r���[�}�g���N�X�̃Z�b�g
void CCamera::SetViewMtx(DirectX::XMMATRIX view)
{
  m_mtxView = view;
}


///////////////////////////////////////////////////////////////////////////////
// �t�@�[�̃Z�b�g
void CCamera::SetFar(float Far)
{
  m_far = Far;
}


///////////////////////////////////////////////////////////////////////////////
// �j�A�[�̃Z�b�g
void CCamera::SetNear(float Near)
{
  m_near = Near;
}