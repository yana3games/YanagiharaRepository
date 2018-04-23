#pragma once
//=============================================================================
// File : fbxPart.h
// Date : 2018/02/20(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <d3d11.h>
#include "gameObject.h"
#include <string>
#include <DirectXMath.h>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class FBXModel;
class Texture;


///////////////////////////////////////////////////////////////////////////////
// FBXモデルクラス
class FBXPart : public GameObject
{
 public:
  FBXPart();
  ~FBXPart();
  HRESULT Init();
  void Update() {};
  void Render() {};

 private:
  FBXModel* m_model;                // 本体のポインタ
  DirectX::XMMATRIX m_mixMatrix;    // 親から自分までのマトリクス
  std::vector<Texture*> m_textures; // 適応されているテクスチャ
};
