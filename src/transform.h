#pragma once
//=============================================================================
// File : transform.h
// Date : 2018/02/07(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "Vector.h"


// 座標変換クラス
class Transform
{
 public:
  Transform() : m_scl(Vector3(1.0f,1.0f,1.0f)), m_rot(Vector3(0.0f,0.0f,0.0f)), m_pos(Vector3(0.0f, 0.0f, 0.0f)) {}
  ~Transform() {}

  // 各種ゲッター
  Vector3 GetScale(void) const { return m_scl;}
  Vector3 GetRotation(void) const { return m_rot; }
  Vector3 GetPosition(void) const { return m_pos; }

  DirectX::XMMATRIX GetWorldMatrix(void) {
    DirectX::XMMATRIX mtxScl = DirectX::XMMatrixScaling(m_scl.x, m_scl.y, m_scl.z);
    DirectX::XMMATRIX mtxRotate = DirectX::XMMatrixRotationRollPitchYaw(m_rot.x, m_rot.y, m_rot.z);
    DirectX::XMMATRIX mtxTrans = DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
    m_mtxWorld = mtxScl * mtxRotate* mtxTrans;
    return m_mtxWorld;
  }

  // 各種セッター
  void SetScale(const Vector3& scl) { m_scl = scl; }
  void SetScale(const float x, const float y, const float z) { m_scl = Vector3(x, y, z); }
  void SetScaleX(const float x) { m_scl.x = x; }
  void SetScaleY(const float y) { m_scl.y = y; }
  void SetScaleZ(const float z) { m_scl.z = z; }
  void Scaling(const Vector3& value) { m_scl += value; }

  void SetRotation(const Vector3& rot) { m_rot = rot; }
  void SetRotation(const float x, const float y, const float z) { m_rot = Vector3(x, y, z); }
  void SetRotationX(const float x) { m_rot.x = x; }
  void SetRotationY(const float y) { m_rot.y = y; }
  void SetRotationZ(const float z) { m_rot.z = z; }
  void Rotate(const Vector3& value) { m_rot += value; }

  void SetPosition(const Vector3& pos) { m_pos = pos; }
  void SetPosition(const float x, const float y, const float z) { m_pos = Vector3(x, y, z); }
  void SetPositionX(const float x) { m_pos.x = x; }
  void SetPositionY(const float y) { m_pos.y = y; }
  void SetPositionZ(const float z) { m_pos.z = z; }
  void Move(const Vector3& value) { m_pos += value; }

 private:
  DirectX::XMMATRIX m_mtxWorld;
  Vector3 m_scl;
  Vector3 m_rot;
  Vector3 m_pos;
};
