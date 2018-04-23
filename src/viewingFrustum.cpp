//=============================================================================
// File : viewingFrustum.cpp
// Date : 2017/12/14(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "viewingFrustum.h"

using namespace DirectX;


///////////////////////////////////////////////////////////////////////////////
// �f�t�H���g�R���X�g���N�^
ViewingFrustum::ViewingFrustum()
{

}


///////////////////////////////////////////////////////////////////////////////
// �R�s�[�R���X�g���N�^
ViewingFrustum::ViewingFrustum( const ViewingFrustum&)
{
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
ViewingFrustum::~ViewingFrustum()
{
}


///////////////////////////////////////////////////////////////////////////////
// �������\�����ʂ̐ݒ�
void ViewingFrustum::SetupViewingFrustum(const float& depth, XMMATRIX projMtx, XMMATRIX viewMtx)
{
  float zMin, r;
  XMMATRIX matrix; // ������s��

  // ������̍ŏ�Z�������v�Z
  zMin = XMVectorGetZ(projMtx.r[3]) / XMVectorGetZ(projMtx.r[2]);
  r = depth / (depth - zMin);

  XMVectorSetZ(projMtx.r[2], r);
  XMVectorSetZ(projMtx.r[3], -r * zMin);
  
  // �r���[�s��Ɖ��ǂ����v���W�F�N�V�����s��Ŏ�����s������
  matrix = XMMatrixMultiply(viewMtx, projMtx);

  // ������̎�O�̕��ʂ��v�Z
  m_planes[0] = XMVectorSetX(m_planes[0], XMVectorGetW(matrix.r[0]) + XMVectorGetZ(matrix.r[0])); //mat14+13
  m_planes[0] = XMVectorSetY(m_planes[0], XMVectorGetW(matrix.r[1]) + XMVectorGetZ(matrix.r[1])); //mat24+23
  m_planes[0] = XMVectorSetZ(m_planes[0], XMVectorGetW(matrix.r[2]) + XMVectorGetZ(matrix.r[2])); //mat34+33
  m_planes[0] = XMVectorSetW(m_planes[0], XMVectorGetW(matrix.r[3]) + XMVectorGetZ(matrix.r[3])); //mat44+43
  m_planes[0] = XMPlaneNormalize(m_planes[0]);

  // ������̉��̕��ʂ��v�Z
  m_planes[1] = XMVectorSetX(m_planes[1], XMVectorGetW(matrix.r[0]) - XMVectorGetZ(matrix.r[0])); // mat14-13
  m_planes[1] = XMVectorSetY(m_planes[1], XMVectorGetW(matrix.r[1]) - XMVectorGetZ(matrix.r[1])); // mat24-23
  m_planes[1] = XMVectorSetZ(m_planes[1], XMVectorGetW(matrix.r[2]) - XMVectorGetZ(matrix.r[2])); // mat34-33
  m_planes[1] = XMVectorSetW(m_planes[1], XMVectorGetW(matrix.r[3]) - XMVectorGetZ(matrix.r[3])); // mat44-43
  m_planes[1] = XMPlaneNormalize(m_planes[1]);

  // ������̍��̕��ʂ��v�Z
  m_planes[2] = XMVectorSetX(m_planes[2], XMVectorGetW(matrix.r[0]) + XMVectorGetX(matrix.r[0])); // mat14+11
  m_planes[2] = XMVectorSetY(m_planes[2], XMVectorGetW(matrix.r[1]) + XMVectorGetX(matrix.r[1])); // mat24+21
  m_planes[2] = XMVectorSetZ(m_planes[2], XMVectorGetW(matrix.r[2]) + XMVectorGetX(matrix.r[2])); // mat34+31
  m_planes[2] = XMVectorSetW(m_planes[2], XMVectorGetW(matrix.r[3]) + XMVectorGetX(matrix.r[3])); // mat44+41
  m_planes[2] = XMPlaneNormalize(m_planes[2]);

  // ������̉E�̕��ʂ��v�Z
  m_planes[3] = XMVectorSetX(m_planes[3], XMVectorGetW(matrix.r[0]) - XMVectorGetX(matrix.r[0])); // mat14-11
  m_planes[3] = XMVectorSetY(m_planes[3], XMVectorGetW(matrix.r[1]) - XMVectorGetX(matrix.r[1])); // mat24-21
  m_planes[3] = XMVectorSetZ(m_planes[3], XMVectorGetW(matrix.r[2]) - XMVectorGetX(matrix.r[2])); // mat34-31
  m_planes[3] = XMVectorSetW(m_planes[3], XMVectorGetW(matrix.r[3]) - XMVectorGetX(matrix.r[3])); // mat44-41
  m_planes[3] = XMPlaneNormalize(m_planes[3]);

  // ������̏�̕��ʂ��v�Z
  m_planes[4] = XMVectorSetX(m_planes[4], XMVectorGetW(matrix.r[0]) - XMVectorGetY(matrix.r[0])); // mat14-12
  m_planes[4] = XMVectorSetY(m_planes[4], XMVectorGetW(matrix.r[1]) - XMVectorGetY(matrix.r[1])); // mat24-22
  m_planes[4] = XMVectorSetZ(m_planes[4], XMVectorGetW(matrix.r[2]) - XMVectorGetY(matrix.r[2])); // mat34-32
  m_planes[4] = XMVectorSetW(m_planes[4], XMVectorGetW(matrix.r[3]) - XMVectorGetY(matrix.r[3])); // mat44-42
  m_planes[4] = XMPlaneNormalize(m_planes[4]);

  // ������̉��̕��ʂ��v�Z
  m_planes[5] = XMVectorSetX(m_planes[5], XMVectorGetW(matrix.r[0]) + XMVectorGetY(matrix.r[0])); // mat14+12
  m_planes[5] = XMVectorSetY(m_planes[5], XMVectorGetW(matrix.r[1]) + XMVectorGetY(matrix.r[1])); // mat24+22
  m_planes[5] = XMVectorSetZ(m_planes[5], XMVectorGetW(matrix.r[2]) + XMVectorGetY(matrix.r[2])); // mat34+32
  m_planes[5] = XMVectorSetW(m_planes[5], XMVectorGetW(matrix.r[3]) + XMVectorGetY(matrix.r[3])); // mat44+42
  m_planes[5] = XMPlaneNormalize(m_planes[5]);
}


///////////////////////////////////////////////////////////////////////////////
// ���_����������ɋ��邩�`�F�b�N
bool ViewingFrustum::CheckPoint( const Vector3& pos)
{
  for (int i = 0; i < 6; i++){
    if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XM(pos))) < 0.0f) {
      return false;
    }
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �L���[�u����������ɋ��邩�`�F�b�N
bool ViewingFrustum::CheckCube()
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
// ���̂���������ɋ��邩�`�F�b�N
bool ViewingFrustum::CheckSphere(const Vector3& pos, const float& radius)
{
  for (int i = 0; i < 6; i++) {
    if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XM(pos))) < -radius) {
      return false;
    }
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// ���_����������ɋ��邩�`�F�b�N
bool ViewingFrustum::CheckRectangle()
{
  return false;
}