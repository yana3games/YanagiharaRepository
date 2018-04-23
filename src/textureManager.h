#pragma once
//=============================================================================
// File : textureManager.h
// Date : 2017/12/10(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>
#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////
// �O���錾
class Texture;

///////////////////////////////////////////////////////////////////////////////
// �V�F�[�_�[�Ǘ��N���X
class TextureManager
{
  struct Res
  {
    Texture*     tex;
    unsigned int nCnt;
  };
 public:
  ~TextureManager();

  bool Load(std::wstring& key);
  Texture* GetResource(const std::wstring& key);

  bool Release(std::wstring key);
  bool AllRelease(void);

 private:
  std::unordered_map< std::wstring, Res> m_res; // �e�N�X�`���}�b�v
};
