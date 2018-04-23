#pragma once
//=============================================================================
// File : sprite3D.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>
#include "gameObject.h"
#include <string>


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class Texture;
class CCamera;

///////////////////////////////////////////////////////////////////////////////
// sprite3D�N���X
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
  Texture*                   m_texture;     // �e�N�X�`���[
};
