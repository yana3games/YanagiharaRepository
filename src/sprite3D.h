#pragma once
//=============================================================================
// File : sprite3D.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <d3d11.h>
#include "gameObject.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class Texture;
class CCamera;

///////////////////////////////////////////////////////////////////////////////
// sprite3Dクラス
class Sprite3D : public GameObject
{
 public:
  Sprite3D();
  Sprite3D(Vector3 pos, Vector3 size, Vector3 rot);
  ~Sprite3D();
  HRESULT Init();
  void Update();
  void Render(CCamera*);
  static Sprite3D* Create(Vector3 pos, Vector3 size, Vector3 rot);
  void SetTexture(Texture* texture);
  void SetTexture(std::wstring filename);

 private:
  Texture*                   m_texture;     // テクスチャー
};
