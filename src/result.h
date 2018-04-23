#pragma once
//=============================================================================
// File : result.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "mode.h"

// タイトルモードクラス
class ResultMode : public CMode
{
 private:

 public:
  ResultMode();
  ~ResultMode();
  void Init(void) override;
  void Update(void) override;
  void Draw(void) override;
};




