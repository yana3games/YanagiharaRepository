#pragma once
//=============================================================================
// File : game.h
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "mode.h"

///////////////////////////////////////////////////////////////////////////////
// �O���錾
class Terrain;
class Skydome;
class FBXFile;
class EffectManager;

// �Q�[�����[�h�N���X
class GameMode : public CMode
{
 private:
  Terrain* m_pTerrain;
  Skydome* m_pSkydome;
  FBXFile* m_pFbxFile;
  EffectManager* m_pEffect;

 public:
  GameMode();
  ~GameMode();
  void Init(void) override;
  void Update(void) override;
  void Draw(void) override;
};




