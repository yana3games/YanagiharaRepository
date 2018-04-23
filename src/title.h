#pragma once
//=============================================================================
// File : sprite3D.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "mode.h"

// タイトルモードクラス
class TitleMode : public CMode
{
 private:
  
 public:
  TitleMode();
  ~TitleMode();
  void Init(void) override;
  void Update(void) override;
  void Draw(void) override;
};




