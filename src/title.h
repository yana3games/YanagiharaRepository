#pragma once
//=============================================================================
// File : sprite3D.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "mode.h"

// �^�C�g�����[�h�N���X
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




