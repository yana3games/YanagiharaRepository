#pragma once
//=============================================================================
// File : cascadeShadowManager.h
// Date : 2018/02/23(金)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <d3d11.h>
#include <DirectXMath.h>
#include "camera.h"


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class RenderTexture;
class Sun;


///////////////////////////////////////////////////////////////////////////////
// クラス定義
class CascadeShadowManager
{
 public:
   CascadeShadowManager();
  ~CascadeShadowManager();
  void Draw(void);
  void SunSet(Sun* sun) { m_sun = sun; }

  static const int NumDivision = 4; // シャドウマップ分割数
  static constexpr float farTable[NumDivision + 1] = {
    0.0f,
    0.05f,
    0.2f,
    0.5f,
    1.0f
  };
  static constexpr float ShadowMapSize = 2048;

 private:
  void ComputeOrthoProj(void);  // 深度値描画範囲の計算
  void RenderDepth(void);       // デプスバッファの生成
  void RenderObject(void);      // 影付きオブジェクトの描画

   // カスケード分割バッファ
  RenderTexture* m_ZBuffer[NumDivision];

  // 影の描画範囲
  CCamera m_camera[NumDivision];

  // 影の方向等
  Sun* m_sun;
};