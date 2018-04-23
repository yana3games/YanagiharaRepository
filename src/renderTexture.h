#pragma once
//=============================================================================
// File : renderTexture.h
// Date : 2018/02/05(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "renderer.h"
#include <wrl/client.h>

// �����_�[�e�N�X�`���N���X
class RenderTexture
{
 public:
  RenderTexture(float width, float height, DXGI_FORMAT format, DXGI_FORMAT depthFormat);

  // �����_�[�^�[�Q�b�g�̃r���[�擾
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetView(void) { return m_pRTV; }
  // �����_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�r���[�擾
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRView(void) { return m_pRTSRV; }
  // �����_�[�^�[�Q�b�g�p�̃f�v�X�X�e���V���r���[�擾
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDepthView(void) { return m_pDSV; }
  // �����_�[�^�[�Q�b�g�p�̃f�v�X�X�e���V���V�F�[�_�[���\�[�X�r���[�擾
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetDepthSRView(void) { return m_pDSSRV; }

 private:
  RenderTexture();
  Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_pRTT;        // �����_�����O�^�[�Q�b�g�̃e�N�X�`��
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    m_pRTV;        // �����_�[�^�[�Q�b�g�r���[
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_pRTSRV;      // ���Ƃ��Ďg������Ƃ��@�����_�[�^�[�Q�b�g�̃V�F�[�_�[���\�[�X�r���[
  Microsoft::WRL::ComPtr<ID3D11Texture2D>           m_pDST;        // �[�x�X�e���V���e�N�X�`��
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    m_pDSV;        // �[�x�X�e���V���r���[
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  m_pDSSRV;      // �[�x�X�e���V���̃V�F�[�_���\�[�X�b�r���[
};