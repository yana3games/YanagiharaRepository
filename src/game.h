#pragma once
//=============================================================================
// File : game.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "mode.h"

///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class Terrain;
class Skydome;
class FBXFile;
class EffectManager;

// ゲームモードクラス
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




