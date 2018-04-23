#pragma once
//=============================================================================
// File : camera.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include <DirectXMath.h>

///////////////////////////////////////////////////////////////////////////////
//	マクロ定義
static const float CAMERA_EXCURSION = 0.99f;


///////////////////////////////////////////////////////////////////////////////
// カメラクラス
class CCamera
{
 private:
  DirectX::XMVECTOR m_posCamera;               // カメラの座標
  DirectX::XMVECTOR m_posAt;                   // 注視点の座標
  DirectX::XMVECTOR m_vecUp;                   // 上の方向
  float m_fFov;                                // 画角の広さ
  DirectX::XMVECTOR m_Xziku, m_Yziku, m_Zziku; // X軸Y軸Z軸
  DirectX::XMVECTOR m_distance, m_olddistance; // 距離･1f前の距離
  DirectX::XMMATRIX m_mtxView;                 // ビューマトリクス
  DirectX::XMMATRIX m_mtxProj;                 // プロジェクションマトリクス
  float m_width;                               // プロジェクション行列横幅
  float m_height;                              // プロジェクション行列縦幅
  float m_near;                                // プロジェクション行列手前
  float m_far;                                 // プロジェクション行列奥行き
 protected:
  ///   NOTHING   ///

 public:
  CCamera();
  CCamera(const DirectX::XMMATRIX& proj, const DirectX::XMMATRIX& view) : m_mtxProj(proj), m_mtxView(view) {}
  CCamera(const DirectX::XMVECTOR& posCamera, const DirectX::XMVECTOR& posAt);
  ~CCamera();

  // セッター
  void SetMtxCamera();
  void SetProjMtx(float width, float height, float Near);
  void SetProjMtx(DirectX::XMMATRIX);
  void SetFar(float);
  void SetViewMtx(DirectX::XMMATRIX);
  void SetNear(float);

  // ゲッター
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

  // 操作
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
