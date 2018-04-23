#pragma once
//=============================================================================
// File : textureManager.h
// Date : 2017/12/10(日)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <d3d11.h>
#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class Texture;

///////////////////////////////////////////////////////////////////////////////
// シェーダー管理クラス
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
  std::unordered_map< std::wstring, Res> m_res; // テクスチャマップ
};
