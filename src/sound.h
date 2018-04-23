#pragma once
//=============================================================================
// File : sound.h
// Date : 2017/12/07(木)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <dsound.h>
#include <string>
#include <vector>


// ダイレクトサウンドクラス
class DirectSound
{
 public:
   DirectSound();
  ~DirectSound();
  bool Init(HWND hWnd);

  bool PlaySE( int i);// 指定方法考え中
  bool PlayBGM();

  void SetVolumeSE(const LONG& value);  // SE音量設定
  void SetVolumeBGM(const LONG& value); // BGM音量設定
  LONG GetVolumeSE(void);               // SE音量取得
  LONG GetVolumeBGM(void);              // BGM音量取得

 private:
  bool InitDirectSoundAPI(HWND hWnd); // DirectSound本体の作成等
  void EndDirectSoundAPI();           // DirectSound本体の開放

  bool LoadWaveFile(const std::string& filename, IDirectSoundBuffer8** SecondaryBuffer); // Waveファイルの読み込み
  void EndWaveFile(IDirectSoundBuffer8** SecondaryBuffer); // Waveファイル単体の開放

  struct WaveHeaderType
  {// 説明無いので適当コメント
    char           chunkId[4];      // チャンクID
    unsigned long  chunkSize;       // チャンクサイズ
    char           format[4];       // フォーマット
    char           subChunkId[4];   // サブチャンクID
    unsigned long  subChunkSize;    // サブチャンクサイズ
    unsigned short audioFormat;     // フォーマット
    unsigned short numChannels;     // チャンネル数
    unsigned long  sampleRate;      // サンプリングレート
    unsigned long  bytesPerSecond;  // １秒当たりのバイト数
    unsigned short blockAlign;      // ブロックのアライン数
    unsigned short bitsPerSample;   // サンプリング当たりのバイト数
    char           dataChunkId[4];  // データのチャンクID
    unsigned long  dataSize;        // データサイズ
  };

  struct BGMData
  {
    bool bLoop;                        // ループするか
    IDirectSoundBuffer8* buffer;       // BGM用セカンダリーバッファ
  };

  IDirectSound8*      m_pSoundDevice;  // Directサウンド本体
  IDirectSoundBuffer* m_primaryBuffer; // プライマリーバッファ
  std::vector<IDirectSoundBuffer8*> m_seBufferList; // SEセカンダリーバッファ群
  BGMData m_bgm;                       // BGM用セカンダリーバッファ
  LONG m_bgmVolume;                    // BGM音量
  LONG m_seVolume;                     // SE音量
};