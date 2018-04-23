//=============================================================================
// File : expSpriteDefaultShader.cpp
// Date : 2018/02/09(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "appManager.h"
#include "expSpriteDefaultShader.h"
#include "shaderManager.h"
#include <d3d11.h>
#include "renderer.h"
#include "Texture.h"


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
ExpSpriteDefaultShader::ExpSpriteDefaultShader(void)
{
  CreateVertexBuffer();
  CreateMatrixBuffer();
  CreateShader();
}

///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
ExpSpriteDefaultShader::~ExpSpriteDefaultShader(void)
{
  if (m_matCB) m_matCB->Release();
  if (m_pVB) m_pVB->Release();
}


///////////////////////////////////////////////////////////////////////////////
// �p�����[�^�̃Z�b�g
void ExpSpriteDefaultShader::SetParameters( DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();
  pDeviceContext->VSSetShader(m_pVS, NULL, 0);
  pDeviceContext->GSSetShader(m_pGS, NULL, 0);
  pDeviceContext->HSSetShader(m_pHS, NULL, 0);
  pDeviceContext->DSSetShader(m_pDS, NULL, 0);
  pDeviceContext->PSSetShader(m_pPS, NULL, 0);
  pDeviceContext->IASetInputLayout(m_pIL);

  // ���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &stride, &offset);

  // �萔�o�b�t�@�̐ݒ�.
  Matrix cb;
  cb.world = World;
  cb.view = View;
  cb.proj = Proj;
  // �T�u���\�[�X���X�V.
  pDeviceContext->UpdateSubresource(m_matCB, 0, NULL, &cb, 0, 0);
  // ���_�V�F�[�_�ɒ萔�o�b�t�@��ݒ�.
  pDeviceContext->VSSetConstantBuffers(0, 1, &m_matCB);

  // �e�N�X�`���̃Z�b�g
  if (texture != nullptr) pDeviceContext->PSSetShaderResources(0, 1, texture->GetShaderResouceView().GetAddressOf());
  else pDeviceContext->PSSetShaderResources(0, 1, nullptr);
}


///////////////////////////////////////////////////////////////////////////////
// ���_�o�b�t�@�̐ݒ�
void ExpSpriteDefaultShader::CreateVertexBuffer(void)
{
  // ���_�̒�`.
  Vertex vertices[4] = {
    { Vector3(-0.5f,  0.5f, 0.0f), Vector2(0.0f, 0.0f) },
    { Vector3(+0.5f,  0.5f, 0.0f), Vector2(1.0f, 0.0f) },
    { Vector3(-0.5f, -0.5f, 0.0f), Vector2(0.0f, 1.0f) },
    { Vector3(+0.5f, -0.5f, 0.0f), Vector2(1.0f, 1.0f) }
  };
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(Vertex) * 4;
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = 0;

  // �T�u���\�[�X�̐ݒ�.
  D3D11_SUBRESOURCE_DATA initData;
  ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
  initData.pSysMem = vertices;

  // ���_�o�b�t�@�̐���.
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateBuffer(&bd, &initData, &m_pVB);
  if (FAILED(hr))
  {
    MessageBox(NULL, "ID3D11Device::CreateBuffer() Failed.", "�x���I", MB_ICONWARNING);
  }
}


///////////////////////////////////////////////////////////////////////////////
// �s��R���X�^���g�o�b�t�@�̍쐬
void ExpSpriteDefaultShader::CreateMatrixBuffer(void)
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
// �V�F�[�_�̍쐬
void ExpSpriteDefaultShader::CreateShader(void)
{
  ShaderManager* shaderManager = AppManager::GetShaderManager();
  // ���̓��C�A�E�g�̒�`
  D3D11_INPUT_ELEMENT_DESC layout[] = {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT   , 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT      , 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  UINT numElements = sizeof(layout) / sizeof(layout[0]);
  shaderManager->LoadVS("./data/SHADER/VertexShader/testVS.cso", layout, numElements);
  ShaderManager::VertexShader VS = shaderManager->GetVS("./data/SHADER/VertexShader/testVS.cso");
  m_pVS = VS.pVS;
  m_pIL = VS.pIL;
  shaderManager->LoadPS("./data/SHADER/PixelShader/testPS.cso");
  m_pPS = shaderManager->GetPS("./data/SHADER/PixelShader/testPS.cso");
}
