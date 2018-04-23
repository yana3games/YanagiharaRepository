#pragma once
//=============================================================================
// File : fbxModel.h
// Date : 2018/02/20(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <d3d11.h>
#include "gameObject.h"
#include <string>
#include <DirectXMath.h>


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class Animation;


///////////////////////////////////////////////////////////////////////////////
// FBXモデルクラス
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
  std::string    m_filename;  // モデルのファイルネーム

  // スキンメッシュ用
  std::vector< Animation> m_bones;
  ID3D11Buffer*  m_boneCB;
};
