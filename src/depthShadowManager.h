#pragma once
//=============================================================================
// File : depthShadowManager.h
// Date : 2018/01/31(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>

///////////////////////////////////////////////////////////////////////////////
// �O���錾
class RenderTexture;

///////////////////////////////////////////////////////////////////////////////
// �N���X��`
class DepthShadowManager
{
 public:

 private:
  RenderTexture* m_ZBuffer;
};