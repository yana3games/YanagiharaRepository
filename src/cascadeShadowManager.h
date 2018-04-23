#pragma once
//=============================================================================
// File : cascadeShadowManager.h
// Date : 2018/02/23(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>
#include <DirectXMath.h>
#include "camera.h"


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class RenderTexture;
class Sun;


///////////////////////////////////////////////////////////////////////////////
// �N���X��`
class CascadeShadowManager
{
 public:
   CascadeShadowManager();
  ~CascadeShadowManager();
  void Draw(void);
  void SunSet(Sun* sun) { m_sun = sun; }

  static const int NumDivision = 4; // �V���h�E�}�b�v������
  static constexpr float farTable[NumDivision + 1] = {
    0.0f,
    0.05f,
    0.2f,
    0.5f,
    1.0f
  };
  static constexpr float ShadowMapSize = 2048;

 private:
  void ComputeOrthoProj(void);  // �[�x�l�`��͈͂̌v�Z
  void RenderDepth(void);       // �f�v�X�o�b�t�@�̐���
  void RenderObject(void);      // �e�t���I�u�W�F�N�g�̕`��

   // �J�X�P�[�h�����o�b�t�@
  RenderTexture* m_ZBuffer[NumDivision];

  // �e�̕`��͈�
  CCamera m_camera[NumDivision];

  // �e�̕�����
  Sun* m_sun;
};