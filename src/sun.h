#pragma once
//=============================================================================
// File  : sun.h
// Date  : 2018/02/23(金)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "transform.h"

///////////////////////////////////////////////////////////////////////////////
// 前方宣言


///////////////////////////////////////////////////////////////////////////////
// クラス定義
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
  Transform m_transform;    // 座標系
  DirectX::XMMATRIX m_view; // シャドウ用ビュー行列
  Vector3 m_posAt;
  Vector3 m_vecUp;
  Vector3 m_lightVec;       // 光の方向
};