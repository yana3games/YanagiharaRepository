//=============================================================================
// File : sound.cpp
// Date : 2017/12/07(木)
// Author: Kazuaki Yanagihara.
//=============================================================================


///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "sound.h"

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
DirectSound::DirectSound()
  : m_pSoundDevice(nullptr)
  , m_primaryBuffer(nullptr)
  , m_bgmVolume(0)
  , m_seVolume(0)
{
  m_bgm.bLoop = 0;
  m_bgm.buffer = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// デストラクタ
DirectSound::~DirectSound()
{
  // 全セカンダリバッファの開放
  EndWaveFile(&m_bgm.buffer);
  for (auto&& se : m_seBufferList) {
    EndWaveFile(&se);
  }

  // DirectSoundAPIの終了
  EndDirectSoundAPI();
}

///////////////////////////////////////////////////////////////////////////////
// 初期化処理
bool DirectSound::Init( HWND hWnd)
{
  // DirectSound本体とプライマリバッファの初期化
  if (!InitDirectSoundAPI(hWnd)) return false;

  IDirectSoundBuffer8* se1;
  IDirectSoundBuffer8* se2;
  IDirectSoundBuffer8* se3;
  if( LoadWaveFile( "./data/SE/attack_blade.wav", &se1))  m_seBufferList.push_back(se1);
  if (LoadWaveFile("./data/SE/explosion02.wav", &se2))    m_seBufferList.push_back(se2);
  if (LoadWaveFile("./data/BGM/result_bgm.wav", &se3))    m_seBufferList.push_back(se3);
  LoadWaveFile("./data/BGM/result_bgm.wav", &m_bgm.buffer);
}

///////////////////////////////////////////////////////////////////////////////
// DirectSound本体の初期化等
bool DirectSound::InitDirectSoundAPI(HWND hWnd)
{
  // デバイス作成
  DirectSoundCreate8(NULL, &m_pSoundDevice, NULL);

  // 強調レベル設定
  m_pSoundDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);

  // プライマリバッファの作成
  DSBUFFERDESC PrimaryBufferDesc;
  PrimaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);   // 構造体の大きさ
  PrimaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME; // デバイスの付加能力
  PrimaryBufferDesc.dwBufferBytes = 0;               // バッファのサイズ 書き込み波形データ次第
  PrimaryBufferDesc.dwReserved = 0;                  // 予約域
  PrimaryBufferDesc.lpwfxFormat = nullptr;           // フォーマット
  PrimaryBufferDesc.guid3DAlgorithm = GUID_NULL;     // 仮想3DエフェクトのGUID

  // デフォルトサウンドデバイスのプライマリサウンドバッファのコントロールを取得
  if (FAILED(m_pSoundDevice->CreateSoundBuffer(&PrimaryBufferDesc, &m_primaryBuffer, NULL))){
    return false;
  }

  // プライマリバッファの波形のフォーマット(非圧縮CDオーディオ品質)
  WAVEFORMATEX waveFormat;  // 波形のフォーマット
  waveFormat.wFormatTag = WAVE_FORMAT_PCM; // 圧縮技術のフォーマットタグ
  waveFormat.nChannels = 2;                // ステレオ・モノラル
  waveFormat.nSamplesPerSec = 44100;       // サンプリングレート(音質 Hz)
  waveFormat.wBitsPerSample = 16;
  waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8); // フォーマット形式の最小単位(byte)
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; // 平均データ転送速度
  waveFormat.cbSize = 0;

  // プライマリバッファを上記波形形式で設定
  if (FAILED(m_primaryBuffer->SetFormat(&waveFormat))) return false;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// DirectSoundの終了
void DirectSound::EndDirectSoundAPI()
{
  // プライマリサウンドバッファの解放
  if (m_primaryBuffer) {
    m_primaryBuffer->Release();
    m_primaryBuffer = nullptr;
  }

  // DirectSound本体の解放
  if (m_pSoundDevice) {
    m_pSoundDevice->Release();
    m_pSoundDevice = nullptr;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Waveファイル読み込み (mp3とか対応するなら同じようなものを作らないといけない)
bool DirectSound::LoadWaveFile(const std::string& filename, IDirectSoundBuffer8** SecondaryBuffer)
{
  int error;
  FILE* filePtr;
  unsigned int count;
  WaveHeaderType waveFileHeader;
  WAVEFORMATEX waveFormat;
  DSBUFFERDESC DSBufferDesc;
  HRESULT result;
  IDirectSoundBuffer * tempBuffer;
  unsigned char* waveData;
  unsigned char* bufferPtr;
  unsigned long bufferSize;

  // バイナリでwaveファイルを開く
  error = fopen_s(&filePtr, filename.c_str(), "rb");
  if (error != 0) return false;

  // Waveファイルのヘッダ読み込み
  count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
  if (count != 1) return false;

  //チャンクIDがRIFF形式であるか？
  if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
    (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
  {
    return false;
  }

  //ファイル形式がWAVE形式であるか？
  if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A')|| 
  (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
  {
    return false;
  }

  //サブチャンクIDがfmt形式であるか？ 
  if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm')|| 
  (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
  {
    return false;
  }

  //オーディオフォーマットがWAVE_FORMAT_PCMであるか？
  if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
  {
    return false;
  }

  // Waveファイルがステレオフォーマットで記録されたか？
  if(waveFileHeader.numChannels != 2)
  {
    MessageBox(NULL, "ステレオフォーマットじゃないです...", filename.c_str(), MB_OK);
    return false;
  }

  //ウェーブファイルが44.1 KHzのサンプルレートで記録されたか？
  if(waveFileHeader.sampleRate != 44100)
  {
    MessageBox(NULL, "サンプルレートが44.1KHZじゃないです...", filename.c_str(), MB_OK);
    return false;
  }

  // Waveファイルが16ビット形式で記録されたか？
  if(waveFileHeader.bitsPerSample != 16)
  {
    MessageBox(NULL, "16bit形式じゃないです...", filename.c_str(), MB_OK);
    return false;
  }

  //データチャンクヘッダーのチェック
  if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
  (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
  {
    return false;
  }

  // waveヘッダファイルの検証完了

  // 波形のフォーマット
  waveFormat.wFormatTag = WAVE_FORMAT_PCM; // 圧縮技術のフォーマットタグ
  waveFormat.nChannels = 2;                // ステレオ・モノラル
  waveFormat.nSamplesPerSec = 44100;       // サンプリングレート(音質 Hz)
  waveFormat.wBitsPerSample = 16;
  waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8; // フォーマット形式の最小単位(byte)
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; // 平均データ転送速度
  waveFormat.cbSize = 0;

  // セカンダリバッファの設定作成
  DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);           // 構造体の大きさ
  DSBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;            // デバイスの付加能力
  DSBufferDesc.dwBufferBytes = waveFileHeader.dataSize; // セカンダリバッファのサイズ 書き込み波形データ次第
  DSBufferDesc.dwReserved = 0;                          // 予約域
  DSBufferDesc.lpwfxFormat = &waveFormat;               // フォーマット
  DSBufferDesc.guid3DAlgorithm = GUID_NULL;             // 仮想3DエフェクトのGUID

  // 上記のバッファ設定で一時的なサウンドバッファを作成
  IDirectSoundBuffer* pTempBuf = nullptr;
  result = m_pSoundDevice->CreateSoundBuffer(&DSBufferDesc, &pTempBuf, NULL);
  if( FAILED( result)) return false;

  // セカンダリバッファの作成
  pTempBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**)SecondaryBuffer);

  // 一時バッファ解放
  pTempBuf->Release();
  pTempBuf = nullptr;

  // データチャンクヘッダーの最後で始まるウェーブデータの先頭に移動します。 
  fseek( filePtr, sizeof(WaveHeaderType), SEEK_SET);

  // ウェーブファイルデータを保持用一時バッファを作成
  waveData = new unsigned char[waveFileHeader.dataSize];
  if( !waveData) return false;

  // バッファにWaveファイルの内容を格納
  count = fread( waveData, 1, waveFileHeader.dataSize, filePtr);
  if(count != waveFileHeader.dataSize) return false;

  // ファイルクローズ
  error = fclose( filePtr);
  if( error != 0) return false;

  // セカンダリバッファのロック
  result = (*SecondaryBuffer)->Lock( 0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
  if( FAILED( result)) return false;

  // セカンダリバッファにWaveデータ格納
  memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

  // セカンダリバッファアンロック 
  result = (*SecondaryBuffer)->Unlock( (void*)bufferPtr, bufferSize, NULL, 0);
  if (FAILED(result)) return false;

  // ウェーブデータ解放
  delete[] waveData;
  waveData = nullptr;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// 指定のWaveファイルの破棄
void DirectSound::EndWaveFile(IDirectSoundBuffer8** SecondaryBuffer)
{
  // セカンダリバッファの解放
  if (*SecondaryBuffer) {
    (*SecondaryBuffer)->Release();
    *SecondaryBuffer = nullptr;
  }
}

///////////////////////////////////////////////////////////////////////////////
// SE再生
bool DirectSound::PlaySE(int i)
{
  HRESULT result;

  // サウンダリバッファの先頭にシーク
  result = m_seBufferList[i]->SetCurrentPosition(0);
  if( FAILED( result)) return false;

  // SEの音量をセット
  result = m_seBufferList[i]->SetVolume(m_seVolume);
  if( FAILED( result)) return false;
  
  // セカンダリサウンドバッファ再生
  result = m_seBufferList[i]->Play(0, 0, 0);
  if( FAILED( result)) return false;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// BGM再生
bool DirectSound::PlayBGM()
{
  HRESULT result;

  // サウンダリバッファの先頭にシーク
  result = m_bgm.buffer->SetCurrentPosition(0);
  if (FAILED(result)) return false;

  // SEの音量をセット
  result = m_bgm.buffer->SetVolume(m_seVolume);
  if (FAILED(result)) return false;

  // セカンダリサウンドバッファ再生
  result = m_bgm.buffer->Play(0, 0, DSBPLAY_LOOPING);
  if (FAILED(result)) return false;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SE音量設定
void DirectSound::SetVolumeSE(const LONG& value)
{
  m_seVolume = min(DSBVOLUME_MAX , max(DSBVOLUME_MIN, value));
}

///////////////////////////////////////////////////////////////////////////////
// BGM音量設定
void DirectSound::SetVolumeBGM(const LONG& value)
{
  m_bgmVolume = min( DSBVOLUME_MAX, max( DSBVOLUME_MIN, value));
}

///////////////////////////////////////////////////////////////////////////////
// SE音量取得
LONG DirectSound::GetVolumeSE(void)
{
  return m_seVolume;
}

///////////////////////////////////////////////////////////////////////////////
// BGM音量取得
LONG DirectSound::GetVolumeBGM(void)
{
  return m_bgmVolume;
}