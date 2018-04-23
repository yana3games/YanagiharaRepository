//=============================================================================
// File : viewingFrustum.cpp
// Date : 2017/12/14(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "viewingFrustum.h"

using namespace DirectX;


///////////////////////////////////////////////////////////////////////////////
// デフォルトコンストラクタ
ViewingFrustum::ViewingFrustum()
{

}


///////////////////////////////////////////////////////////////////////////////
// コピーコンストラクタ
ViewingFrustum::ViewingFrustum( const ViewingFrustum&)
{
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
ViewingFrustum::~ViewingFrustum()
{
}


///////////////////////////////////////////////////////////////////////////////
// 視錐台を表す平面の設定
void ViewingFrustum::SetupViewingFrustum(const float& depth, XMMATRIX projMtx, XMMATRIX viewMtx)
{
  float zMin, r;
  XMMATRIX matrix; // 視錐台行列

  // 視錐台の最小Z距離を計算
  zMin = XMVectorGetZ(projMtx.r[3]) / XMVectorGetZ(projMtx.r[2]);
  r = depth / (depth - zMin);

  XMVectorSetZ(projMtx.r[2], r);
  XMVectorSetZ(projMtx.r[3], -r * zMin);
  
  // ビュー行列と改良したプロジェクション行列で視錐台行列を作る
  matrix = XMMatrixMultiply(viewMtx, projMtx);

  // 視錐台の手前の平面を計算
  m_planes[0] = XMVectorSetX(m_planes[0], XMVectorGetW(matrix.r[0]) + XMVectorGetZ(matrix.r[0])); //mat14+13
  m_planes[0] = XMVectorSetY(m_planes[0], XMVectorGetW(matrix.r[1]) + XMVectorGetZ(matrix.r[1])); //mat24+23
  m_planes[0] = XMVectorSetZ(m_planes[0], XMVectorGetW(matrix.r[2]) + XMVectorGetZ(matrix.r[2])); //mat34+33
  m_planes[0] = XMVectorSetW(m_planes[0], XMVectorGetW(matrix.r[3]) + XMVectorGetZ(matrix.r[3])); //mat44+43
  m_planes[0] = XMPlaneNormalize(m_planes[0]);

  // 視錐台の奥の平面を計算
  m_planes[1] = XMVectorSetX(m_planes[1], XMVectorGetW(matrix.r[0]) - XMVectorGetZ(matrix.r[0])); // mat14-13
  m_planes[1] = XMVectorSetY(m_planes[1], XMVectorGetW(matrix.r[1]) - XMVectorGetZ(matrix.r[1])); // mat24-23
  m_planes[1] = XMVectorSetZ(m_planes[1], XMVectorGetW(matrix.r[2]) - XMVectorGetZ(matrix.r[2])); // mat34-33
  m_planes[1] = XMVectorSetW(m_planes[1], XMVectorGetW(matrix.r[3]) - XMVectorGetZ(matrix.r[3])); // mat44-43
  m_planes[1] = XMPlaneNormalize(m_planes[1]);

  // 視錐台の左の平面を計算
  m_planes[2] = XMVectorSetX(m_planes[2], XMVectorGetW(matrix.r[0]) + XMVectorGetX(matrix.r[0])); // mat14+11
  m_planes[2] = XMVectorSetY(m_planes[2], XMVectorGetW(matrix.r[1]) + XMVectorGetX(matrix.r[1])); // mat24+21
  m_planes[2] = XMVectorSetZ(m_planes[2], XMVectorGetW(matrix.r[2]) + XMVectorGetX(matrix.r[2])); // mat34+31
  m_planes[2] = XMVectorSetW(m_planes[2], XMVectorGetW(matrix.r[3]) + XMVectorGetX(matrix.r[3])); // mat44+41
  m_planes[2] = XMPlaneNormalize(m_planes[2]);

  // 視錐台の右の平面を計算
  m_planes[3] = XMVectorSetX(m_planes[3], XMVectorGetW(matrix.r[0]) - XMVectorGetX(matrix.r[0])); // mat14-11
  m_planes[3] = XMVectorSetY(m_planes[3], XMVectorGetW(matrix.r[1]) - XMVectorGetX(matrix.r[1])); // mat24-21
  m_planes[3] = XMVectorSetZ(m_planes[3], XMVectorGetW(matrix.r[2]) - XMVectorGetX(matrix.r[2])); // mat34-31
  m_planes[3] = XMVectorSetW(m_planes[3], XMVectorGetW(matrix.r[3]) - XMVectorGetX(matrix.r[3])); // mat44-41
  m_planes[3] = XMPlaneNormalize(m_planes[3]);

  // 視錐台の上の平面を計算
  m_planes[4] = XMVectorSetX(m_planes[4], XMVectorGetW(matrix.r[0]) - XMVectorGetY(matrix.r[0])); // mat14-12
  m_planes[4] = XMVectorSetY(m_planes[4], XMVectorGetW(matrix.r[1]) - XMVectorGetY(matrix.r[1])); // mat24-22
  m_planes[4] = XMVectorSetZ(m_planes[4], XMVectorGetW(matrix.r[2]) - XMVectorGetY(matrix.r[2])); // mat34-32
  m_planes[4] = XMVectorSetW(m_planes[4], XMVectorGetW(matrix.r[3]) - XMVectorGetY(matrix.r[3])); // mat44-42
  m_planes[4] = XMPlaneNormalize(m_planes[4]);

  // 視錐台の下の平面を計算
  m_planes[5] = XMVectorSetX(m_planes[5], XMVectorGetW(matrix.r[0]) + XMVectorGetY(matrix.r[0])); // mat14+12
  m_planes[5] = XMVectorSetY(m_planes[5], XMVectorGetW(matrix.r[1]) + XMVectorGetY(matrix.r[1])); // mat24+22
  m_planes[5] = XMVectorSetZ(m_planes[5], XMVectorGetW(matrix.r[2]) + XMVectorGetY(matrix.r[2])); // mat34+32
  m_planes[5] = XMVectorSetW(m_planes[5], XMVectorGetW(matrix.r[3]) + XMVectorGetY(matrix.r[3])); // mat44+42
  m_planes[5] = XMPlaneNormalize(m_planes[5]);
}


///////////////////////////////////////////////////////////////////////////////
// 頂点が視錐台内に居るかチェック
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
// キューブが視錐台内に居るかチェック
bool ViewingFrustum::CheckCube()
{
  return false;
}


///////////////////////////////////////////////////////////////////////////////
// 球体が視錐台内に居るかチェック
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
// 頂点が視錐台内に居るかチェック
bool ViewingFrustum::CheckRectangle()
{
  return false;
}