#pragma once
//=============================================================================
// File : camera.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include <DirectXMath.h>

///////////////////////////////////////////////////////////////////////////////
//	�}�N����`
static const float CAMERA_EXCURSION = 0.99f;


///////////////////////////////////////////////////////////////////////////////
// �J�����N���X
class CCamera
{
 private:
  DirectX::XMVECTOR m_posCamera;               // �J�����̍��W
  DirectX::XMVECTOR m_posAt;                   // �����_�̍��W
  DirectX::XMVECTOR m_vecUp;                   // ��̕���
  float m_fFov;                                // ��p�̍L��
  DirectX::XMVECTOR m_Xziku, m_Yziku, m_Zziku; // X��Y��Z��
  DirectX::XMVECTOR m_distance, m_olddistance; // �����1f�O�̋���
  DirectX::XMMATRIX m_mtxView;                 // �r���[�}�g���N�X
  DirectX::XMMATRIX m_mtxProj;                 // �v���W�F�N�V�����}�g���N�X
  float m_width;                               // �v���W�F�N�V�����s�񉡕�
  float m_height;                              // �v���W�F�N�V�����s��c��
  float m_near;                                // �v���W�F�N�V�����s���O
  float m_far;                                 // �v���W�F�N�V�����s�񉜍s��
 protected:
  ///   NOTHING   ///

 public:
  CCamera();
  CCamera(const DirectX::XMMATRIX& proj, const DirectX::XMMATRIX& view) : m_mtxProj(proj), m_mtxView(view) {}
  CCamera(const DirectX::XMVECTOR& posCamera, const DirectX::XMVECTOR& posAt);
  ~CCamera();

  // �Z�b�^�[
  void SetMtxCamera();
  void SetProjMtx(float width, float height, float Near);
  void SetProjMtx(DirectX::XMMATRIX);
  void SetFar(float);
  void SetViewMtx(DirectX::XMMATRIX);
  void SetNear(float);

  // �Q�b�^�[
  DirectX::XMMATRIX* GetProjMtx(void);
  DirectX::XMMATRIX* GetViewMtx(void);
  DirectX::XMVECTOR* GetCameraZ(void);
  DirectX::XMVECTOR* GetCameraX(void);
  DirectX::XMVECTOR* GetCameraY(void);
  float GetAspect(void) { return m_width / m_height; }
  float GetFar(void) { return m_far; }
  float GetNear(void) { return m_near; }
  float GetFov(void) { return m_fFov; }
  DirectX::XMVECTOR GetPos(void) { return m_posCamera; }

  // ����
  void ComputationZiku();
  void ZoomIn();
  void ZoomOut();
  void ParallelShiftXYZ(DirectX::XMVECTOR *VecDirect, float movelate);
  void ParallelShiftXZ(DirectX::XMVECTOR *VecDirect, float movelate);
  void SpiralUp(float rotatelate);
  void SpiralDown(float rotatelate);
  void SpiralSide(float rotatelate);
  void TurningUp(float rotatelate);
  void TurningDown(float rotatelate);
  void TurningSide(float rotatelate);
};
