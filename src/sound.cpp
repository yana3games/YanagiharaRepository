//=============================================================================
// File : sound.cpp
// Date : 2017/12/07(��)
// Author: Kazuaki Yanagihara.
//=============================================================================


///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include "sound.h"

///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
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
// �f�X�g���N�^
DirectSound::~DirectSound()
{
  // �S�Z�J���_���o�b�t�@�̊J��
  EndWaveFile(&m_bgm.buffer);
  for (auto&& se : m_seBufferList) {
    EndWaveFile(&se);
  }

  // DirectSoundAPI�̏I��
  EndDirectSoundAPI();
}

///////////////////////////////////////////////////////////////////////////////
// ����������
bool DirectSound::Init( HWND hWnd)
{
  // DirectSound�{�̂ƃv���C�}���o�b�t�@�̏�����
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
// DirectSound�{�̂̏�������
bool DirectSound::InitDirectSoundAPI(HWND hWnd)
{
  // �f�o�C�X�쐬
  DirectSoundCreate8(NULL, &m_pSoundDevice, NULL);

  // �������x���ݒ�
  m_pSoundDevice->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);

  // �v���C�}���o�b�t�@�̍쐬
  DSBUFFERDESC PrimaryBufferDesc;
  PrimaryBufferDesc.dwSize = sizeof(DSBUFFERDESC);   // �\���̂̑傫��
  PrimaryBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME; // �f�o�C�X�̕t���\��
  PrimaryBufferDesc.dwBufferBytes = 0;               // �o�b�t�@�̃T�C�Y �������ݔg�`�f�[�^����
  PrimaryBufferDesc.dwReserved = 0;                  // �\���
  PrimaryBufferDesc.lpwfxFormat = nullptr;           // �t�H�[�}�b�g
  PrimaryBufferDesc.guid3DAlgorithm = GUID_NULL;     // ���z3D�G�t�F�N�g��GUID

  // �f�t�H���g�T�E���h�f�o�C�X�̃v���C�}���T�E���h�o�b�t�@�̃R���g���[�����擾
  if (FAILED(m_pSoundDevice->CreateSoundBuffer(&PrimaryBufferDesc, &m_primaryBuffer, NULL))){
    return false;
  }

  // �v���C�}���o�b�t�@�̔g�`�̃t�H�[�}�b�g(�񈳏kCD�I�[�f�B�I�i��)
  WAVEFORMATEX waveFormat;  // �g�`�̃t�H�[�}�b�g
  waveFormat.wFormatTag = WAVE_FORMAT_PCM; // ���k�Z�p�̃t�H�[�}�b�g�^�O
  waveFormat.nChannels = 2;                // �X�e���I�E���m����
  waveFormat.nSamplesPerSec = 44100;       // �T���v�����O���[�g(���� Hz)
  waveFormat.wBitsPerSample = 16;
  waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample / 8); // �t�H�[�}�b�g�`���̍ŏ��P��(byte)
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; // ���σf�[�^�]�����x
  waveFormat.cbSize = 0;

  // �v���C�}���o�b�t�@����L�g�`�`���Őݒ�
  if (FAILED(m_primaryBuffer->SetFormat(&waveFormat))) return false;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// DirectSound�̏I��
void DirectSound::EndDirectSoundAPI()
{
  // �v���C�}���T�E���h�o�b�t�@�̉��
  if (m_primaryBuffer) {
    m_primaryBuffer->Release();
    m_primaryBuffer = nullptr;
  }

  // DirectSound�{�̂̉��
  if (m_pSoundDevice) {
    m_pSoundDevice->Release();
    m_pSoundDevice = nullptr;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Wave�t�@�C���ǂݍ��� (mp3�Ƃ��Ή�����Ȃ瓯���悤�Ȃ��̂����Ȃ��Ƃ����Ȃ�)
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

  // �o�C�i����wave�t�@�C�����J��
  error = fopen_s(&filePtr, filename.c_str(), "rb");
  if (error != 0) return false;

  // Wave�t�@�C���̃w�b�_�ǂݍ���
  count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
  if (count != 1) return false;

  //�`�����NID��RIFF�`���ł��邩�H
  if((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
    (waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
  {
    return false;
  }

  //�t�@�C���`����WAVE�`���ł��邩�H
  if((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A')|| 
  (waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
  {
    return false;
  }

  //�T�u�`�����NID��fmt�`���ł��邩�H 
  if((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm')|| 
  (waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
  {
    return false;
  }

  //�I�[�f�B�I�t�H�[�}�b�g��WAVE_FORMAT_PCM�ł��邩�H
  if(waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
  {
    return false;
  }

  // Wave�t�@�C�����X�e���I�t�H�[�}�b�g�ŋL�^���ꂽ���H
  if(waveFileHeader.numChannels != 2)
  {
    MessageBox(NULL, "�X�e���I�t�H�[�}�b�g����Ȃ��ł�...", filename.c_str(), MB_OK);
    return false;
  }

  //�E�F�[�u�t�@�C����44.1 KHz�̃T���v�����[�g�ŋL�^���ꂽ���H
  if(waveFileHeader.sampleRate != 44100)
  {
    MessageBox(NULL, "�T���v�����[�g��44.1KHZ����Ȃ��ł�...", filename.c_str(), MB_OK);
    return false;
  }

  // Wave�t�@�C����16�r�b�g�`���ŋL�^���ꂽ���H
  if(waveFileHeader.bitsPerSample != 16)
  {
    MessageBox(NULL, "16bit�`������Ȃ��ł�...", filename.c_str(), MB_OK);
    return false;
  }

  //�f�[�^�`�����N�w�b�_�[�̃`�F�b�N
  if((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
  (waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
  {
    return false;
  }

  // wave�w�b�_�t�@�C���̌��؊���

  // �g�`�̃t�H�[�}�b�g
  waveFormat.wFormatTag = WAVE_FORMAT_PCM; // ���k�Z�p�̃t�H�[�}�b�g�^�O
  waveFormat.nChannels = 2;                // �X�e���I�E���m����
  waveFormat.nSamplesPerSec = 44100;       // �T���v�����O���[�g(���� Hz)
  waveFormat.wBitsPerSample = 16;
  waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8; // �t�H�[�}�b�g�`���̍ŏ��P��(byte)
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign; // ���σf�[�^�]�����x
  waveFormat.cbSize = 0;

  // �Z�J���_���o�b�t�@�̐ݒ�쐬
  DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);           // �\���̂̑傫��
  DSBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;            // �f�o�C�X�̕t���\��
  DSBufferDesc.dwBufferBytes = waveFileHeader.dataSize; // �Z�J���_���o�b�t�@�̃T�C�Y �������ݔg�`�f�[�^����
  DSBufferDesc.dwReserved = 0;                          // �\���
  DSBufferDesc.lpwfxFormat = &waveFormat;               // �t�H�[�}�b�g
  DSBufferDesc.guid3DAlgorithm = GUID_NULL;             // ���z3D�G�t�F�N�g��GUID

  // ��L�̃o�b�t�@�ݒ�ňꎞ�I�ȃT�E���h�o�b�t�@���쐬
  IDirectSoundBuffer* pTempBuf = nullptr;
  result = m_pSoundDevice->CreateSoundBuffer(&DSBufferDesc, &pTempBuf, NULL);
  if( FAILED( result)) return false;

  // �Z�J���_���o�b�t�@�̍쐬
  pTempBuf->QueryInterface(IID_IDirectSoundBuffer8, (void**)SecondaryBuffer);

  // �ꎞ�o�b�t�@���
  pTempBuf->Release();
  pTempBuf = nullptr;

  // �f�[�^�`�����N�w�b�_�[�̍Ō�Ŏn�܂�E�F�[�u�f�[�^�̐擪�Ɉړ����܂��B 
  fseek( filePtr, sizeof(WaveHeaderType), SEEK_SET);

  // �E�F�[�u�t�@�C���f�[�^��ێ��p�ꎞ�o�b�t�@���쐬
  waveData = new unsigned char[waveFileHeader.dataSize];
  if( !waveData) return false;

  // �o�b�t�@��Wave�t�@�C���̓��e���i�[
  count = fread( waveData, 1, waveFileHeader.dataSize, filePtr);
  if(count != waveFileHeader.dataSize) return false;

  // �t�@�C���N���[�Y
  error = fclose( filePtr);
  if( error != 0) return false;

  // �Z�J���_���o�b�t�@�̃��b�N
  result = (*SecondaryBuffer)->Lock( 0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
  if( FAILED( result)) return false;

  // �Z�J���_���o�b�t�@��Wave�f�[�^�i�[
  memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

  // �Z�J���_���o�b�t�@�A�����b�N 
  result = (*SecondaryBuffer)->Unlock( (void*)bufferPtr, bufferSize, NULL, 0);
  if (FAILED(result)) return false;

  // �E�F�[�u�f�[�^���
  delete[] waveData;
  waveData = nullptr;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// �w���Wave�t�@�C���̔j��
void DirectSound::EndWaveFile(IDirectSoundBuffer8** SecondaryBuffer)
{
  // �Z�J���_���o�b�t�@�̉��
  if (*SecondaryBuffer) {
    (*SecondaryBuffer)->Release();
    *SecondaryBuffer = nullptr;
  }
}

///////////////////////////////////////////////////////////////////////////////
// SE�Đ�
bool DirectSound::PlaySE(int i)
{
  HRESULT result;

  // �T�E���_���o�b�t�@�̐擪�ɃV�[�N
  result = m_seBufferList[i]->SetCurrentPosition(0);
  if( FAILED( result)) return false;

  // SE�̉��ʂ��Z�b�g
  result = m_seBufferList[i]->SetVolume(m_seVolume);
  if( FAILED( result)) return false;
  
  // �Z�J���_���T�E���h�o�b�t�@�Đ�
  result = m_seBufferList[i]->Play(0, 0, 0);
  if( FAILED( result)) return false;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// BGM�Đ�
bool DirectSound::PlayBGM()
{
  HRESULT result;

  // �T�E���_���o�b�t�@�̐擪�ɃV�[�N
  result = m_bgm.buffer->SetCurrentPosition(0);
  if (FAILED(result)) return false;

  // SE�̉��ʂ��Z�b�g
  result = m_bgm.buffer->SetVolume(m_seVolume);
  if (FAILED(result)) return false;

  // �Z�J���_���T�E���h�o�b�t�@�Đ�
  result = m_bgm.buffer->Play(0, 0, DSBPLAY_LOOPING);
  if (FAILED(result)) return false;

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// SE���ʐݒ�
void DirectSound::SetVolumeSE(const LONG& value)
{
  m_seVolume = min(DSBVOLUME_MAX , max(DSBVOLUME_MIN, value));
}

///////////////////////////////////////////////////////////////////////////////
// BGM���ʐݒ�
void DirectSound::SetVolumeBGM(const LONG& value)
{
  m_bgmVolume = min( DSBVOLUME_MAX, max( DSBVOLUME_MIN, value));
}

///////////////////////////////////////////////////////////////////////////////
// SE���ʎ擾
LONG DirectSound::GetVolumeSE(void)
{
  return m_seVolume;
}

///////////////////////////////////////////////////////////////////////////////
// BGM���ʎ擾
LONG DirectSound::GetVolumeBGM(void)
{
  return m_bgmVolume;
}