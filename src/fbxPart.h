#pragma once
//=============================================================================
// File : fbxPart.h
// Date : 2018/02/20(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>
#include "gameObject.h"
#include <string>
#include <DirectXMath.h>
#include <vector>


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class FBXModel;
class Texture;


///////////////////////////////////////////////////////////////////////////////
// FBX���f���N���X
class FBXPart : public GameObject
{
 public:
  FBXPart();
  ~FBXPart();
  HRESULT Init();
  void Update() {};
  void Render() {};

 private:
  FBXModel* m_model;                // �{�̂̃|�C���^
  DirectX::XMMATRIX m_mixMatrix;    // �e���玩���܂ł̃}�g���N�X
  std::vector<Texture*> m_textures; // �K������Ă���e�N�X�`��
};
