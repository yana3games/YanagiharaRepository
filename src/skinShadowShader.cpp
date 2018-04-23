//=============================================================================
// File : skinShadowShader.cpp
// Date : 2018/01/26(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "appManager.h"
#include "skinShadowShader.h"
#include <d3d11.h>
#include "renderer.h"
#include "Texture.h"


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
SkinShadowShader::SkinShadowShader(void)
{ // �}�g���N�X�o�b�t�@�̐ݒ�.
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
  bd.ByteWidth = sizeof(Matrix);
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;

  // �}�g���N�X�o�b�t�@�𐶐�.
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateBuffer(&bd, NULL, &m_matCB);
  if (FAILED(hr)) {
    MessageBox(NULL, "FailedCreateConstantBuffer", "error!", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
SkinShadowShader::~SkinShadowShader(void)
{
  if(m_matCB) m_matCB->Release();
}


///////////////////////////////////////////////////////////////////////////////
// �p�����[�^�̃Z�b�g
void SkinShadowShader::SetParameters( DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();
  pDeviceContext->VSSetShader(m_pVS, NULL, 0);
  pDeviceContext->GSSetShader(nullptr, NULL, 0);
  pDeviceContext->HSSetShader(nullptr, NULL, 0);
  pDeviceContext->DSSetShader(nullptr, NULL, 0);
  pDeviceContext->PSSetShader(m_pPS, NULL, 0);
  pDeviceContext->IASetInputLayout(m_pIL);

  // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
  pDeviceContext->VSSetConstantBuffers(1, 1, &boneCB);

  // �萔�o�b�t�@�̐ݒ�.
  Matrix cb;
  cb.world = World;
  cb.view = View;
  cb.proj = Proj;
  cb.wit = WIT;
  // �e�N�X�`���̃Z�b�g
  if (texture != nullptr) pDeviceContext->PSSetShaderResources(0, 1, texture->GetShaderResouceView().GetAddressOf());
  else pDeviceContext->PSSetShaderResources(0, 1, nullptr);

  // �T�u���\�[�X���X�V.
  pDeviceContext->UpdateSubresource(m_matCB, 0, NULL, &cb, 0, 0);

  // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
  pDeviceContext->VSSetConstantBuffers(0, 1, &m_matCB);
}

