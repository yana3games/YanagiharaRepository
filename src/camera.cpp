//=============================================================================
// File : camera.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include "camera.h"
#include "appConfig.h"
#include "renderer.h"


///////////////////////////////////////////////////////////////////////////////
//	デフォルトコンストラクタ
CCamera::CCamera() : m_posCamera(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)), m_posAt(DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)), m_vecUp(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)), m_fFov(1.0f), m_width((float)AppConfig::GetInstance()->GetScreenWidth()), m_height((float)AppConfig::GetInstance()->GetScreenHeight()), m_far(5000.0f), m_near(0.01f)
{
  m_distance = DirectX::XMVectorSubtract(m_posAt, m_posCamera);
  m_olddistance = m_distance;
  ComputationZiku();
}


///////////////////////////////////////////////////////////////////////////////
//	コンストラクタ
CCamera::CCamera(const DirectX::XMVECTOR& posCamera, const DirectX::XMVECTOR& posAt) :m_posCamera(posCamera), m_posAt(posAt), m_vecUp(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)), m_fFov(1.0f), m_width((float)AppConfig::GetInstance()->GetScreenWidth()), m_height((float)AppConfig::GetInstance()->GetScreenHeight()), m_far(5000.0f), m_near(0.01f)
{
  m_distance = DirectX::XMVectorSubtract(m_posAt, m_posCamera);
  m_olddistance = m_distance;
  ComputationZiku();
}


///////////////////////////////////////////////////////////////////////////////
//	デストラクタ
CCamera::~CCamera()
{
}


///////////////////////////////////////////////////////////////////////////////
//	各軸情報計算
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
//	ズームイン
void CCamera::ZoomOut()
{
  m_fFov -= 0.01f;
  m_fFov = max(m_fFov, 0.01f);
}


///////////////////////////////////////////////////////////////////////////////
//	ズームアウト
void CCamera::ZoomIn()
{
  m_fFov += 0.01f;
  m_fFov = min(m_fFov, DirectX::XM_PI - 0.01f);
}


///////////////////////////////////////////////////////////////////////////////
//	セットカメラ
void CCamera::SetMtxCamera()
{
  //ビュー行列への格納
  m_mtxView = DirectX::XMMatrixLookAtLH(m_posCamera, m_posAt, m_vecUp);

  //パースの聞いたプロジェクション行列の作成 ( 格納先　画角　アス比　ニアー　ファー)   0 < near
  m_mtxProj = DirectX::XMMatrixPerspectiveFovLH(m_fFov, m_width / m_height, m_near, m_far);
}


///////////////////////////////////////////////////////////////////////////////
//	ビューマトリクスのゲッター
DirectX::XMMATRIX* CCamera::GetViewMtx(void)
{
  return &m_mtxView;
}


///////////////////////////////////////////////////////////////////////////////
//	プロジェクションマトリクスのゲッター
DirectX::XMMATRIX* CCamera::GetProjMtx(void)
{
  return &m_mtxProj;
}


///////////////////////////////////////////////////////////////////////////////
//	カメラのZ軸のゲッター
DirectX::XMVECTOR* CCamera::GetCameraZ(void)
{
  return &m_Zziku;
}

///////////////////////////////////////////////////////////////////////////////
//	カメラのX軸のゲッター
DirectX::XMVECTOR* CCamera::GetCameraX(void)
{
  return &m_Xziku;
}

///////////////////////////////////////////////////////////////////////////////
//	カメラのY軸のゲッター
DirectX::XMVECTOR* CCamera::GetCameraY(void)
{
  return &m_Yziku;
}

///////////////////////////////////////////////////////////////////////////////
//	カメラのXYZ並行移動
void CCamera::ParallelShiftXYZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
  m_posCamera = DirectX::XMVectorAdd( m_posCamera, DirectX::XMVectorScale( *VecDirect, movelate));
  m_posAt = DirectX::XMVectorAdd(m_posAt, DirectX::XMVectorScale(*VecDirect, movelate));
}

///////////////////////////////////////////////////////////////////////////////
//	カメラのXZ平面並行移動
void CCamera::ParallelShiftXZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
  DirectX::XMVECTOR work = *VecDirect;
  DirectX::XMVectorSetY(work, 0.0f);
  work = DirectX::XMVector3Normalize(work);
  m_posCamera = DirectX::XMVectorAdd(m_posCamera, DirectX::XMVectorScale(work, movelate));
  m_posAt = DirectX::XMVectorAdd(m_posAt, DirectX::XMVectorScale(work, movelate));
}


///////////////////////////////////////////////////////////////////////////////
//	カメラの螺旋移動↑
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
//	カメラの螺旋移動↓
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
//	カメラの左右螺旋移動
void CCamera::SpiralSide(float rotatelate)
{
  DirectX::XMMATRIX work;
  work = DirectX::XMMatrixRotationAxis(m_vecUp, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  m_posCamera = DirectX::XMVectorSubtract(m_posAt, m_distance);
}


///////////////////////////////////////////////////////////////////////////////
//	カメラの旋回移動↑
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
//	カメラの旋回移動↓
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
//	カメラの左右旋回移動
void CCamera::TurningSide(float rotatelate)
{
  DirectX::XMMATRIX work;
  work = DirectX::XMMatrixRotationAxis(m_vecUp, rotatelate);
  m_distance = XMVector3TransformCoord(m_distance, work);
  m_posAt = DirectX::XMVectorAdd(m_distance, m_posCamera);
}

///////////////////////////////////////////////////////////////////////////////
//	プロジェクションマトリクス用情報のセット
void CCamera::SetProjMtx(float width, float height, float Near)
{
  m_width = width;
  m_height = height;
  m_near = Near;
}


///////////////////////////////////////////////////////////////////////////////
// プロジェクションマトリクスセット
void CCamera::SetProjMtx(DirectX::XMMATRIX proj)
{
  m_mtxProj = proj;
}


///////////////////////////////////////////////////////////////////////////////
// ビューマトリクスのセット
void CCamera::SetViewMtx(DirectX::XMMATRIX view)
{
  m_mtxView = view;
}


///////////////////////////////////////////////////////////////////////////////
// ファーのセット
void CCamera::SetFar(float Far)
{
  m_far = Far;
}


///////////////////////////////////////////////////////////////////////////////
// ニアーのセット
void CCamera::SetNear(float Near)
{
  m_near = Near;
}