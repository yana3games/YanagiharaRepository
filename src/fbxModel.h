#pragma once
//=============================================================================
// File : fbxModel.h
// Date : 2018/02/20(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>
#include "gameObject.h"
#include <string>
#include <DirectXMath.h>


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class Animation;


///////////////////////////////////////////////////////////////////////////////
// FBX���f���N���X
class FBXModel : public GameObject
{
 public:
  FBXModel(std::string filename);
  ~FBXModel();
  HRESULT Init();
  void Update() {};
  void Render() {};

 private:
  HRESULT Load(std::string filename);
  HRESULT CreateConstantBuffer(void);
  FBXModel();
  std::string    m_filename;  // ���f���̃t�@�C���l�[��

  // �X�L�����b�V���p
  std::vector< Animation> m_bones;
  ID3D11Buffer*  m_boneCB;
};
