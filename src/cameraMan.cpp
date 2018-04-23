//=============================================================================
// File : camera.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include "appManager.h"
#include "input.h"
#include "cameraMan.h"
#include "camera.h"

///////////////////////////////////////////////////////////////////////////////
//	カメラマンの更新処理
void CCameraMan::Update()
{
  if (!m_pCamera) return;

  //	カメラの操作
  CameraOperation();

  //	カメラの軸情報の計算
  m_pCamera->ComputationZiku();
}

///////////////////////////////////////////////////////////////////////////////
//	カメラマンの更新処理
void CCameraMan::Set()
{
  //	カメラのセット
  SetVPMtx();
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの操作
void CCameraMan::CameraOperation()
{
  if (!m_pCamera) return;
  // 注視点方向移動
  //	前
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_W)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraZ(), m_movelate);
  }
  //	後ろ
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_S)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraZ(), -m_movelate);
  }
  //	右
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_A)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraX(), m_movelate);
  }
  //	左
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_D)) {
    m_pCamera->ParallelShiftXYZ(m_pCamera->GetCameraX(), -m_movelate);
  }

  // カメラ旋回移動
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_Z))
  {
    m_pCamera->SpiralUp(-m_rotatelate);
  }
  //	後ろ
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_C))
  {
    m_pCamera->SpiralDown(m_rotatelate);
  }
  //	右
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_Q))
  {
    m_pCamera->SpiralSide(-m_rotatelate);
  }
  //	左
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_E))
  {
    m_pCamera->SpiralSide(m_rotatelate);
  }

  // 注視点移動
  //	前
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_UP)) {
    m_pCamera->TurningUp(m_rotatelate);
  }
  //	後ろ
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_DOWN)) {
    m_pCamera->TurningDown(-m_rotatelate);
  }
  //	右
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_LEFT)) {
    m_pCamera->TurningSide(-m_rotatelate);
  }
  //	左
  if (AppManager::GetKeyboard()->GetKeyPress(DIK_RIGHT)) {
    m_pCamera->TurningSide(m_rotatelate);
  }
}

///////////////////////////////////////////////////////////////////////////////
//	カメラマンの持ってるカメラのマトリクスをセット
void CCameraMan::SetVPMtx()
{
  if (!m_pCamera) return;
  m_pCamera->SetMtxCamera();
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのプロジェクションマトリクス取得
DirectX::XMMATRIX* CCameraMan::GetProjMtx(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetProjMtx();
}


///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのプロジェクションマトリクス情報のセット
void CCameraMan::SetProjMtx(float width, float height, float Near)
{
	if (!m_pCamera) return;
	m_pCamera->SetProjMtx(width, height, Near);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのビューマトリクス取得
DirectX::XMMATRIX* CCameraMan::GetViewMtx(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetViewMtx();
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのZ軸取得
DirectX::XMVECTOR* CCameraMan::GetCameraZ(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetCameraZ();
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのX軸取得
DirectX::XMVECTOR* CCameraMan::GetCameraX(void)
{
	if (!m_pCamera) return nullptr;
	return m_pCamera->GetCameraX();
}

///////////////////////////////////////////////////////////////////////////////
//	カメラマンにカメラのセット
void CCameraMan::CameraSet( CCamera* pCamera)
{
	m_pCamera = pCamera;
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのXYZ平行移動
void CCameraMan::ParallelShiftCameraXYZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
	if (!m_pCamera) return;
	m_pCamera->ParallelShiftXYZ(VecDirect, movelate);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラのXZ平面移動
void CCameraMan::ParallelShiftCameraXZ(DirectX::XMVECTOR *VecDirect, float movelate)
{
	if (!m_pCamera) return;
	m_pCamera->ParallelShiftXZ(VecDirect, movelate);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの螺旋操作↑
void CCameraMan::SpiralCameraUp(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->SpiralUp(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの螺旋操作↓
void CCameraMan::SpiralCameraDown(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->SpiralDown(rotatelate);
}


///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの左右螺旋操作
void CCameraMan::SpiralCameraSide(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->SpiralSide(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの旋回操作↑
void CCameraMan::TurningCameraUp(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->TurningUp(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの旋回操作↓
void CCameraMan::TurningCameraDown(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->TurningDown(rotatelate);
}

///////////////////////////////////////////////////////////////////////////////
//	持っているカメラの左右旋回操作
void CCameraMan::TurningCameraSide(float rotatelate)
{
	if (!m_pCamera) return;
	m_pCamera->TurningSide(rotatelate);
}

