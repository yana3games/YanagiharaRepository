#pragma once
//=============================================================================
// File : result.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "mode.h"

// �^�C�g�����[�h�N���X
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




