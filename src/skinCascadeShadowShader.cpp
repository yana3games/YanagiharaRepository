//=============================================================================
// File : regidCascadeShadowShader.cpp
// Date : 2018/02/26(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "appManager.h"
#include "skinCascadeShadowShader.h"
#include "shaderManager.h"
#include <d3d11.h>
#include "renderer.h"
#include "Texture.h"
#include "renderTexture.h"
#include "appConfig.h"


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
SkinCascadeShadowShader::SkinCascadeShadowShader(void)
{
  CreateMatrixBuffer();
  CreateSunBuffer();
  CreateShader();
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
SkinCascadeShadowShader::~SkinCascadeShadowShader(void)
{
  if (m_matCB) m_matCB->Release();
  if (m_sunCB) m_sunCB->Release();
}


///////////////////////////////////////////////////////////////////////////////
// �p�����[�^�̃Z�b�g
void SkinCascadeShadowShader::SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();
  pDeviceContext->VSSetShader(m_pVS, NULL, 0);
  pDeviceContext->GSSetShader(m_pGS, NULL, 0);
  pDeviceContext->HSSetShader(m_pHS, NULL, 0);
  pDeviceContext->DSSetShader(m_pDS, NULL, 0);
  pDeviceContext->PSSetShader(m_pPS, NULL, 0);
  pDeviceContext->IASetInputLayout(m_pIL);

  // �萔�o�b�t�@�̐ݒ�.
  Matrix cb;
  cb.world = World;
  cb.view = View;
  cb.proj = Proj;
  cb.WIT = WIT;
  // �T�u���\�[�X���X�V.
  pDeviceContext->UpdateSubresource(m_matCB, 0, NULL, &cb, 0, 0);
  // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
  pDeviceContext->VSSetConstantBuffers(0, 1, &m_matCB);
  // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
  pDeviceContext->VSSetConstantBuffers(1, 1, &boneCB);

  SunMatrix suncb;
  for (int i = 0; i < CascadeShadowManager::NumDivision; i++) {
    suncb.sunProj[i] = *(m_sunProj + i);
  }
  suncb.sunView = m_sunView;
  for (int i = 0; i < CascadeShadowManager::NumDivision; i++) {
    suncb.divisionFar[i] = AppConfig::GetInstance()->GetFar() * CascadeShadowManager::farTable[i + 1];
  }
  // �T�u���\�[�X���X�V.
  pDeviceContext->UpdateSubresource(m_sunCB, 0, NULL, &suncb, 0, 0);
  // �s�N�Z���V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
  pDeviceContext->PSSetConstantBuffers(0, 1, &m_sunCB);

  // �e�N�X�`���̃Z�b�g
 if (texture != nullptr) pDeviceContext->PSSetShaderResources(0, 1, texture->GetShaderResouceView().GetAddressOf());

  for (int i = 0; i < CascadeShadowManager::NumDivision; i++) {
    pDeviceContext->PSSetShaderResources(i + 1, 1, m_ZBuffers[i]->GetSRView().GetAddressOf());
  }
}



///////////////////////////////////////////////////////////////////////////////
// �s��R���X�^���g�o�b�t�@�̍쐬
void SkinCascadeShadowShader::CreateMatrixBuffer(void)
{
  // �}�g���N�X�o�b�t�@�̐ݒ�.
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
// ���z�R���X�^���g�o�b�t�@�̍쐬
void SkinCascadeShadowShader::CreateSunBuffer(void)
{
  // �}�g���N�X�o�b�t�@�̐ݒ�.
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
  bd.ByteWidth = sizeof(SunMatrix);
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;

  // �}�g���N�X�o�b�t�@�𐶐�.
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateBuffer(&bd, NULL, &m_sunCB);
  if (FAILED(hr)) {
    MessageBox(NULL, "FailedCreateConstantBuffer", "error!", MB_OK);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �V�F�[�_�̍쐬
void SkinCascadeShadowShader::CreateShader(void)
{
  ShaderManager* shaderManager = AppManager::GetShaderManager();
  // ���̓��C�A�E�g�̒�`
  D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL"  , 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "WEIGHT"  , 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "INDEX"   , 0, DXGI_FORMAT_R32G32B32A32_UINT , 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
  };

  UINT numElements = sizeof(layout) / sizeof(layout[0]);
  shaderManager->LoadVS("./data/SHADER/VertexShader/skinCascadeShadowVS.cso", layout, numElements);
  ShaderManager::VertexShader VS = shaderManager->GetVS("./data/SHADER/VertexShader/skinCascadeShadowVS.cso");
  m_pVS = VS.pVS;
  m_pIL = VS.pIL;
  shaderManager->LoadPS("./data/SHADER/PixelShader/skinCascadeShadowPS.cso");
  m_pPS = shaderManager->GetPS("./data/SHADER/PixelShader/skinCascadeShadowPS.cso");
}
