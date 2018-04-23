#pragma once
//=============================================================================
// File : sound.h
// Date : 2017/12/07(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <dsound.h>
#include <string>
#include <vector>


// �_�C���N�g�T�E���h�N���X
class DirectSound
{
 public:
   DirectSound();
  ~DirectSound();
  bool Init(HWND hWnd);

  bool PlaySE( int i);// �w����@�l����
  bool PlayBGM();

  void SetVolumeSE(const LONG& value);  // SE���ʐݒ�
  void SetVolumeBGM(const LONG& value); // BGM���ʐݒ�
  LONG GetVolumeSE(void);               // SE���ʎ擾
  LONG GetVolumeBGM(void);              // BGM���ʎ擾

 private:
  bool InitDirectSoundAPI(HWND hWnd); // DirectSound�{�̂̍쐬��
  void EndDirectSoundAPI();           // DirectSound�{�̂̊J��

  bool LoadWaveFile(const std::string& filename, IDirectSoundBuffer8** SecondaryBuffer); // Wave�t�@�C���̓ǂݍ���
  void EndWaveFile(IDirectSoundBuffer8** SecondaryBuffer); // Wave�t�@�C���P�̂̊J��

  struct WaveHeaderType
  {// ���������̂œK���R�����g
    char           chunkId[4];      // �`�����NID
    unsigned long  chunkSize;       // �`�����N�T�C�Y
    char           format[4];       // �t�H�[�}�b�g
    char           subChunkId[4];   // �T�u�`�����NID
    unsigned long  subChunkSize;    // �T�u�`�����N�T�C�Y
    unsigned short audioFormat;     // �t�H�[�}�b�g
    unsigned short numChannels;     // �`�����l����
    unsigned long  sampleRate;      // �T���v�����O���[�g
    unsigned long  bytesPerSecond;  // �P�b������̃o�C�g��
    unsigned short blockAlign;      // �u���b�N�̃A���C����
    unsigned short bitsPerSample;   // �T���v�����O������̃o�C�g��
    char           dataChunkId[4];  // �f�[�^�̃`�����NID
    unsigned long  dataSize;        // �f�[�^�T�C�Y
  };

  struct BGMData
  {
    bool bLoop;                        // ���[�v���邩
    IDirectSoundBuffer8* buffer;       // BGM�p�Z�J���_���[�o�b�t�@
  };

  IDirectSound8*      m_pSoundDevice;  // Direct�T�E���h�{��
  IDirectSoundBuffer* m_primaryBuffer; // �v���C�}���[�o�b�t�@
  std::vector<IDirectSoundBuffer8*> m_seBufferList; // SE�Z�J���_���[�o�b�t�@�Q
  BGMData m_bgm;                       // BGM�p�Z�J���_���[�o�b�t�@
  LONG m_bgmVolume;                    // BGM����
  LONG m_seVolume;                     // SE����
};