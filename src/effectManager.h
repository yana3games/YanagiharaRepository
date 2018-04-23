#pragma once
//=============================================================================
// File : effectManager.h
// Date : 2018/02/01(木)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#pragma comment( lib, "Effekseer.lib" )
#pragma comment( lib, "EffekseerRendererDX11" )
#pragma comment( lib, "EffekseerSoundXAudio2.lib" )

#include <Effekseer.h>
#include <EffekseerRendererDX11.h>
#include <EffekseerSoundXAudio2.h>
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include "Vector.h"


// エフェクシアー管理クラス
class EffectManager
{
  struct Res
  {
    Effekseer::Effect* effect;
    unsigned int nCnt;
  };
 public:
  EffectManager();
  ~EffectManager();

  void Draw(void);
  void Update(void);

  bool Load(const std::wstring& key);
  Effekseer::Handle Play(const std::wstring& key, const Vector3& pos);

  bool Release(std::wstring key);
  bool AllRelease(void);

 private:
  Effekseer::Matrix44 XM( DirectX::XMMATRIX&);

  static const int m_MaxEffects = 10000;
  ::EffekseerRenderer::Renderer* m_renderer;
  ::EffekseerSound::Sound* m_sound;
  ::Effekseer::Manager* m_manager;

  std::unordered_map< std::wstring, Res> m_res; // エフェクトリソース
};