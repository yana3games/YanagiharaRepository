//=============================================================================
// File : skydome.cpp
// Date : 2017/12/12(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "appManager.h"
#include "skydome.h"
#include "renderer.h"
#include "textureManager.h"
#include "Texture.h"
#include "appUtility.h"
#include "shaderManager.h"
#include "shader.h"
#include "camera.h"
#include "cameraMan.h"


///////////////////////////////////////////////////////////////////////////////
// �n�`�쐬
Skydome* Skydome::Create(float fRadius, float width, float height, const Vector3& pos)
{
  // �n�`�V�K�쐬
  Skydome *pRet = new Skydome;
  if (pRet == nullptr) return nullptr;

  // 1�u���b�N�̑傫�����o��
  pRet->m_radius = fRadius;
  pRet->m_numDivision = Vector2(width, height);
  pRet->m_vertexCnt = ((width + 1) * (height - 1) + 2);
  pRet->m_indexCnt = ((2 + 2 * width)*(height - 2) + (height - 3) * 2);
  pRet->GetTransform()->SetPosition( pos);
  pRet->Init();

  return pRet;
}

///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
Skydome::Skydome()
{
  m_vertexBuffer = nullptr;
  m_indexBuffer = nullptr;
  m_layer = D3D11Renderer::eLayerSky;
  m_shaderType = ShaderManager::eShaderTypeSky;
  m_texture = nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// �R�s�[�R���X�g���N�^
Skydome::Skydome(const Skydome&)
{
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
Skydome::~Skydome()
{
  std::wstring filename = L"./data/TEXTURE/skydome/skydome.jpg";
  AppManager::GetTextureManager()->Release(filename);

  SAFE_RELEASE(m_indexBuffer);
  SAFE_RELEASE(m_vertexBuffer);
}


///////////////////////////////////////////////////////////////////////////////
// ����������
HRESULT Skydome::Init()
{
  bool result;
  std::wstring filename = L"./data/TEXTURE/skydome/skydome.jpg";
  result = AppManager::GetTextureManager()->Load(filename);
  if (!result) { return E_FAIL; }
  m_texture = AppManager::GetTextureManager()->GetResource(filename);

  HRESULT hr;
  hr = CreateVertexBuffer();
  if (FAILED(hr)) return E_FAIL;
  hr = CreateIndexBuffer();
  if (FAILED(hr)) return E_FAIL;
  
    //ID3D11RasterizerState* hpRasterizerState = NULL;
    //D3D11_RASTERIZER_DESC hRasterizerDesc = {
    //  D3D11_FILL_WIREFRAME,
    //  D3D11_CULL_NONE,	//�|���S���̗��\�𖳂���
    //  FALSE,
    //  0,
    //  0.0f,
    //  FALSE,
    //  FALSE,
    //  FALSE,
    //  FALSE,
    //  FALSE
    //};
    //if (FAILED(D3D11Renderer::GetInstance()->GetDevice()->CreateRasterizerState(&hRasterizerDesc, &hpRasterizerState))) {
    //  MessageBox(NULL, "RastrerizerCreate", "error!", MB_OK);
    //}
    //
    ////���X�^���C�U�[���R���e�L�X�g�ɐݒ�
    //D3D11Renderer::GetInstance()->GetDeviceContext()->RSSetState(hpRasterizerState);

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// �X�V����
void Skydome::Update()
{
  GetTransform()->Rotate(Vector3(0.0f, 0.0001f, 0.0f));
  GetTransform()->SetPosition(Vec3(CCameraMan::GetInstance()->GetCamera()->GetPos()));
}


///////////////////////////////////////////////////////////////////////////////
// �`�揈��
void Skydome::Render(CCamera* pCamera)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  // �v���~�e�B�u�̎�ނ�ݒ�
  pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // ���̓A�Z���u���ɒ��_�o�b�t�@��ݒ�
  UINT stride = sizeof(Vertex);
  UINT offset = 0;
  pDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

  // ���̓A�Z���u���ɃC���f�b�N�X�o�b�t�@��ݒ�
  pDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

  // �V�F�[�_���̃Z�b�g
  Shader* shader = AppManager::GetShaderManager()->GetShader(m_shaderType);
  shader->SetParameters(
    GetTransform()->GetWorldMatrix(),
    *pCamera->GetViewMtx(),
    *pCamera->GetProjMtx(),
    m_texture
  );

  // �`��
  pDeviceContext->DrawIndexed(m_indexCnt, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////
// ���_�o�b�t�@�쐬
HRESULT Skydome::CreateVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  // �f�o�C�X�̎擾
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  // ���_�̈ꎞ�o�b�t�@�m��
  Vertex* vertices = new Vertex[m_vertexCnt];

  float fTheta = 0.0f;
  float fPhi = 0.0f;
  vertices[0].pos = Vector3(0.0f, m_radius, 0.0f);
  vertices[0].tex = Vector2(0.5f, 0.0f);
  for (int height = 1; height < m_numDivision.y; height++)
  {
    fTheta = (DirectX::XM_PI / m_numDivision.y) * height + (DirectX::XM_PI*0.5f);
    for (int width = 0; width <= m_numDivision.x; width++)
    {
      fPhi = (2 * DirectX::XM_PI / m_numDivision.x) * width;
      vertices[((height - 1)*((int)m_numDivision.x + 1)) + width + 1].pos = Vector3(m_radius*cos(fTheta)*sin(fPhi), m_radius*sin(fTheta), m_radius*cos(fTheta)*cos(fPhi));
      vertices[((height - 1)*((int)m_numDivision.x + 1)) + width + 1].tex = Vector2(1.0f / m_numDivision.x*width, 1.0f / m_numDivision.y*height);
    }
  }
  vertices[m_vertexCnt - 1].pos = Vector3(0.0f, -m_radius, 0.0f);
  vertices[m_vertexCnt - 1].tex = Vector2(0.5f, 1.0f);

  // ���_�o�b�t�@�̐ݒ�
  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(Vertex)* m_vertexCnt;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  // �T�u���\�[�X�\���̂ɒ��_�f�[�^�ւ̃|�C���^���i�[
  vertexData.pSysMem = vertices;
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  // ���_�o�b�t�@�쐬
  result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
  delete[] vertices;
  // ���_�m�ۂ����s
  if (FAILED(result)) {
    return E_FAIL;
  }
  return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////
// �C���f�b�N�X�o�b�t�@�쐬
HRESULT Skydome::CreateIndexBuffer(void)
{
  HRESULT result;
  D3D11_BUFFER_DESC indexBufferDesc;
  D3D11_SUBRESOURCE_DATA indexData;

  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  unsigned long* index = new unsigned long[m_indexCnt];
 
  // ���_���W�̐ݒ�
  for (int height = 0; height < m_numDivision.y - 2; height++)
  {
    if (height != 0)
    {
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height - 2] = ((height - 1)*(m_numDivision.x + 1)) + m_numDivision.x + 1;
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height - 1] = ((height + 1)*(m_numDivision.x + 1)) + 1;
    }
    index[(2 + 2 + 2 * (int)m_numDivision.x)*height + 0] = (height + 1) * (m_numDivision.x + 1) + 1;
    index[(2 + 2 + 2 * (int)m_numDivision.x)*height + 1] = height * (m_numDivision.x + 1) + 1;

    for (int width = 1; width <= m_numDivision.x; width++)
    {
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height + width * 2 + 0] = ((height + 1) * (m_numDivision.x + 1)) + width + 1;
      index[(2 + 2 + 2 * (int)m_numDivision.x)*height + width * 2 + 1] = (height * (m_numDivision.x + 1)) + width + 1;
    }
  }
  // �ÓI�C���f�b�N�X�o�b�t�@�̐�����ݒ肵�܂��B 
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCnt;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  // �T�u���\�[�X�\���̂ɃC���f�b�N�X�f�[�^�ւ̃|�C���^��n���܂��B 
  indexData.pSysMem = index;
  indexData.SysMemPitch = 0;
  indexData.SysMemSlicePitch = 0;

  //�C���f�b�N�X�o�b�t�@���쐬���܂��B
  result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
  if (FAILED(result)) {
    return E_FAIL;
  }
  return S_OK;
}

