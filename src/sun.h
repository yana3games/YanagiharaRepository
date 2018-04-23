#pragma once
//=============================================================================
// File  : sun.h
// Date  : 2018/02/23(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "transform.h"

///////////////////////////////////////////////////////////////////////////////
// �O���錾


///////////////////////////////////////////////////////////////////////////////
// �N���X��`
class Sun
{
 public:
   Sun(Vector3 pos, Vector3 vec) {
     m_transform.SetPosition(pos);
     m_lightVec = Vec3(DirectX::XMVector3Normalize(XM(vec)));
     m_posAt = pos + vec;
     m_vecUp = Vector3(0.0f, 1.0f, 0.0f);
     m_view = DirectX::XMMatrixLookAtLH(XM(pos), XM(m_posAt), XM(m_vecUp));
  }

  Transform* GetTransform() { return &m_transform; }
  DirectX::XMMATRIX GetViewMatrix() { return m_view; }
  Vector3 GetLightVec() { return m_lightVec; }

 private:
  Sun() {};
  Transform m_transform;    // ���W�n
  DirectX::XMMATRIX m_view; // �V���h�E�p�r���[�s��
  Vector3 m_posAt;
  Vector3 m_vecUp;
  Vector3 m_lightVec;       // ���̕���
};