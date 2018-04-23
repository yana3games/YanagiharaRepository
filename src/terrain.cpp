//=============================================================================
// File : terrain.h
// Date : 2017/12/10(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "appManager.h"
#include "terrain.h"
#include "renderer.h"
#include "textureManager.h"
#include "Texture.h"
#include "appUtility.h"
#include "shaderManager.h"
#include "shader.h"

#include "camera.h"


///////////////////////////////////////////////////////////////////////////////
// �n�`�쐬
Terrain* Terrain::Create(float fWidth, float fHeight, int nBlockX, int nBlockY)
{
  // �n�`�V�K�쐬
  Terrain *pRet = new Terrain;
  if (pRet == nullptr) return nullptr;
  
  // 1�u���b�N�̑傫�����o��
  pRet->m_fieldSize = Vector2(fWidth, fHeight);
  pRet->m_numBlock = Vector2((float)nBlockX, (float)nBlockY);
  pRet->m_LTPos = Vector3(-fWidth  * 0.5f, 0.0f, fHeight * 0.5f); // �����̒��_�ʒu
  pRet->m_blockSize = Vector2(fWidth / nBlockX, fHeight / nBlockY);

  pRet->Init();

  return pRet;
}


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
Terrain::Terrain()
{
  m_vertexBuffer = nullptr;
  m_indexBuffer = nullptr;
  m_layer = D3D11Renderer::eLayerNormalObject;
  m_shaderType = ShaderManager::eShaderTypeTerrain;
  m_texture = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// �R�s�[�R���X�g���N�^
Terrain::Terrain(const Terrain&)
{
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
Terrain::~Terrain()
{
  std::wstring filename = L"Big_pebbles_pxr128.tif";
  AppManager::GetTextureManager()->Release(filename);

  SAFE_RELEASE(m_indexBuffer);
  SAFE_RELEASE(m_vertexBuffer);
}


///////////////////////////////////////////////////////////////////////////////
// ����������
HRESULT Terrain::Init()
{
  bool result;
  std::wstring filename = L"./data/TEXTURE/terrain/Big_pebbles_pxr128.tif";
  result = AppManager::GetTextureManager()->Load(filename);
  if (!result) { return E_FAIL; }
  m_texture = AppManager::GetTextureManager()->GetResource(filename);

  HRESULT hr;
  hr = CreateVertexBuffer();
  if (FAILED(hr)) return E_FAIL;
  hr = CreateIndexBuffer();
  if (FAILED(hr)) return E_FAIL;
//  ID3D11RasterizerState* hpRasterizerState = NULL;
//  D3D11_RASTERIZER_DESC hRasterizerDesc = {
//    D3D11_FILL_WIREFRAME,
//    D3D11_CULL_NONE,	//�|���S���̗��\�𖳂���
//    FALSE,
//    0,
//    0.0f,
//    FALSE,
//    FALSE,
//    FALSE,
//    FALSE,
//    FALSE
//  };
//  if (FAILED(AppManager::GetRenderer()->GetDevice()->CreateRasterizerState(&hRasterizerDesc, &hpRasterizerState))) {
//    MessageBox(NULL, "RastrerizerCreate", "error!", MB_OK);
//  }
//  
//  //���X�^���C�U�[���R���e�L�X�g�ɐݒ�
//  AppManager::GetRenderer()->GetDeviceContext()->RSSetState(hpRasterizerState);

  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
// �X�V����
void Terrain::Update()
{
}


///////////////////////////////////////////////////////////////////////////////
// �`�揈��
void Terrain::Render(CCamera* pCamera)
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
HRESULT Terrain::CreateVertexBuffer()
{
  HRESULT result;
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  // �f�o�C�X�̎擾
  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  // ���_���̌v�Z
  m_vertexCnt = ((int)m_numBlock.x + 1) * ((int)m_numBlock.y + 1);

  // ���_�̈ꎞ�o�b�t�@�m��
  Vertex* vertices = new Vertex[ m_vertexCnt];

//  float *pHeightMap = (float *)&m_aHeightMap[0];
  Vector3* normal = new Vector3[ m_vertexCnt];
  SetNormalAll(normal);
  int i = 0;
  for (int nY = 0; nY < m_numBlock.y + 1; nY++)
  {
    for (int nX = 0; nX < m_numBlock.x + 1; nX++)
    {
      // �ʒu
      vertices[i].pos.x = m_LTPos.x + m_blockSize.x * nX;
      vertices[i].pos.y = 0;
      vertices[i].pos.z = m_LTPos.z - (m_blockSize.y * nY);

      // �@��
      vertices[i].nrm = normal[i];

      // �e�N�X�`�����W
      vertices[i].tex = Vector2(1.0f * nX, 1.0f * nY);

      ++i;
    }
  }
  delete[] normal;

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

  //���_�o�b�t�@�쐬
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
HRESULT Terrain::CreateIndexBuffer(void)
{
  HRESULT result;
  D3D11_BUFFER_DESC indexBufferDesc;
  D3D11_SUBRESOURCE_DATA indexData;

  ID3D11Device* pDevice = AppManager::GetRenderer()->GetDevice();

  // �C���f�b�N�X�����v�Z
  m_indexCnt = (((int)m_numBlock.x + 1) * 2 + 2) * ((int)m_numBlock.y)-2;

  int nNumUpper = 0;
  int nNumLower = (int)m_numBlock.x + 1;

  int i = 0;
  unsigned long* index = new unsigned long[m_indexCnt];
  for (int nY = 0; nY < m_numBlock.y; nY++)
  {
    for (int nX = 0; nX < m_numBlock.x + 1; nX++)
    {
      // ���_
      index[i] = nNumLower;
      ++i;
      nNumLower++;

      // ��_
      index[i] = nNumUpper;
      ++i;

      nNumUpper++;
    }

    if (nY < m_numBlock.y - 1)
    {
      nNumUpper--;    // �_�~�[�_��ł��߂̒���

      // �_�~�[�_�P
      index[i] = nNumUpper;
      ++i;
      nNumUpper++;

      // �_�~�[�_�Q
      index[i] = nNumLower;
      ++i;
    }
  }
  //�ÓI�C���f�b�N�X�o�b�t�@�̐�����ݒ肵�܂��B 
  indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCnt;
  indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  indexBufferDesc.CPUAccessFlags = 0;
  indexBufferDesc.MiscFlags = 0;
  indexBufferDesc.StructureByteStride = 0;

  //�T�u���\�[�X�\���̂ɃC���f�b�N�X�f�[�^�ւ̃|�C���^��n���܂��B 
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


///////////////////////////////////////////////////////////////////////////////
//	�@���Z�b�c�@�[
void Terrain::SetNormalAll( Vector3* normal)
{
  // �����ʒu���v�Z����
  Vector3 Pos[4] =
  {
    Vector3( -m_blockSize.x * m_numBlock.x * 0.5f,                 0.0f, -m_blockSize.y * m_numBlock.y * 0.5f ),
    Vector3( -m_blockSize.x * m_numBlock.x * 0.5f,                 0.0f, -m_blockSize.y * m_numBlock.y * 0.5f + m_blockSize.y ),
    Vector3( -m_blockSize.x * m_numBlock.x * 0.5f + m_blockSize.x, 0.0f, -m_blockSize.y * m_numBlock.y * 0.5f ),
    Vector3(  m_blockSize.x * m_numBlock.x * 0.5f + m_blockSize.x, 0.0f,  m_blockSize.y * m_numBlock.y * 0.5f + m_blockSize.y )
  };

  // ���_�ɓ����@����������
  {
    int i = 0;

    while( i < m_vertexCnt )
    {
      normal[i++] = Vector3( 0.0f, 0.0f, 0.0f );
    }
  }

  // �ʖ@���̌v�Z�ƒ��_�ɓ����@�����v�Z����
  // �������A�P�ʉ��͌�ōs��
  {
    int i = 0;  // ��

    for (int y = 0; y < m_numBlock.y; ++y)
    {
      for (int x = 0; x < m_numBlock.x; ++x)
      {
        // �|���S�����̈�
        {
          Vector3 nor;

          // �����̒��_����̃x�N�g��1,2
          Vector3 v3Vec1 = Pos[1] - Pos[0];
          Vector3 v3Vec2 = Pos[2] - Pos[1];

          // �O�ς���@�������߂�
          Vector3::Cross(&nor, &v3Vec1, &v3Vec2);
          Vector3::Normalize(&nor, &nor);

          // �ΏۂƂȂ钸�_�ɓ����
          normal[x + 0 + ((int)m_numBlock.x + 1)*(y + 1)] += nor;
          normal[x + 0 + ((int)m_numBlock.x + 1)*(y + 0)] += nor;
          normal[x + 1 + ((int)m_numBlock.x + 1)*(y + 1)] += nor;

          // ���̖@����
          ++i;
        }

        // �|���S�����̂Q
        {
          Vector3 nor;

          // ����̒��_����̃x�N�g��1,2
          Vector3 v3Vec1 = Pos[3] - Pos[1];
          Vector3 v3Vec2 = Pos[2] - Pos[3];

          // �O�ς���@�������߂�
          Vector3::Cross(&nor, &v3Vec1, &v3Vec2);
          Vector3::Normalize(&nor, &nor);

          // �ΏۂƂȂ钸�_�ɓ����
          normal[x + 0 + ((int)m_numBlock.x + 1)*(y + 0)] += nor;
          normal[x + 1 + ((int)m_numBlock.x + 1)*(y + 0)] += nor;
          normal[x + 1 + ((int)m_numBlock.x + 1)*(y + 1)] += nor;
          // ���̖@����
          ++i;
        }

        // ���̃u���b�N�ւ̏�����
        Pos[0].y = 0;//m_heightMap[(y + 1)*(m_numBlock.x + 1) + x];
        Pos[1].y = 0;//m_heightMap[y*(m_numBlock.x + 1) + x];
        Pos[2].y = 0;//m_heightMap[(y + 1)*(m_numBlock.x + 1) + x + 1];
        Pos[3].y = 0;//m_heightMap[y*(m_numBlock.x + 1) + x + 1];
      }
    }
  }

  for (int i = 0; i < m_vertexCnt; ++i) {
    Vector3::Normalize(normal, normal);
  }
}

/////////////////////////////////////////////////////////////////////////////////
////	�C���N���[�h
//#include "meshfield.h"
//#include <stdlib.h>
//#include <time.h>
//#include "manager.h"
//#include "renderer.h"
//#include "scene.h"
//#include "textureManager.h"
//#include "math.h"
//
/////////////////////////////////////////////////////////////////////////////////
////	�萔
//const float CMeshField::HEIGHTMAP[51*51] =
//{
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  17.0f,  15.0f,   7.0f,   6.5f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  17.0f,  17.0f,  15.0f,  10.0f,   7.5f,   7.0f,   6.5f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  17.0f,  15.0f,  15.0f,  10.0f,   8.5f,   8.0f,   7.5f,   7.0f,   0.5f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  17.0f,  15.0f,   9.0f,   8.5f,   8.0f,   7.5f,   7.0f,   6.5f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  17.0f,  15.0f,   9.0f,   8.5f,   8.0f,   7.5f,   7.0f,   6.5f,   6.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  17.0f,  15.0f,   8.0f,   8.0f,   7.5f,   7.0f,   6.5f,   7.0f,   7.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  15.0f,   7.5f,   8.0f,   7.5f,   7.0f,   6.0f,   8.0f,   8.0f,   8.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   6.0f,   7.0f,   7.5f,   7.0f,   6.5f,   5.0f,   7.0f,   9.0f,   9.0f,   0.0f,   0.0f,   0.0f,   2.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   0.0f,   6.5f,   7.0f,   6.5f,   2.0f,   4.0f,   6.0f,   8.0f,  10.0f,   0.0f,   0.0f,   0.0f,   3.0f,   1.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   6.5f,   0.0f,   1.0f,   3.0f,   5.0f,   7.0f,   9.0f,   9.0f,   0.0f,   0.0f,   4.0f,   2.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   4.0f,   6.0f,   8.0f,   8.0f,   8.0f,   7.0f,   5.0f,   3.0f,   1.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   5.0f,   7.0f,   7.0f,   7.0f,   6.0f,   6.0f,   4.0f,   2.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   4.0f,   6.0f,   6.0f,   6.0f,   5.0f,   5.0f,   5.0f,   3.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   5.0f,   5.0f,   5.0f,   4.0f,   4.0f,   4.0f,   4.0f,   2.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   4.0f,   4.0f,   4.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   3.0f,   3.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   2.0f,   2.0f,   1.0f,   1.0f,   1.0f,   1.0f,   1.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,  10.0f,   8.0f,   6.0f,   5.0f,   4.0f,   3.0f,   3.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   9.0f,   5.0f,   6.0f,   7.0f,   6.0f,   5.0f,   4.0f,   3.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   8.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   7.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   6.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   3.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   2.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   3.0f,   3.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   3.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   2.0f,   0.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   4.0f,   4.0f,   4.0f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   2.0f,   2.0f,   2.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.5f,   5.5f,   3.5f,   3.5f,   3.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   2.0f,   3.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   5.0f,   6.0f,   7.0f,   8.0f,   5.0f,   3.0f,   3.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   2.0f,   2.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   6.0f,   7.0f,   8.0f,   5.0f,   2.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   2.0f,   2.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   6.0f,   6.0f,   7.0f,   5.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   3.0f,   4.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   6.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   3.0f,   4.0f,   3.0f,   1.0f,   0.0f,  20.0f,  20.0f,
//
//  20.0f,  20.0f,   0.0f,   0.0f,   5.0f,   5.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   4.0f,   4.0f,   3.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   1.0f,   1.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   4.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   1.0f,   1.0f,   1.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,   0.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  13.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  17.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,  20.0f,
//};
//
//float CMeshField::m_aHeightMap[51*51] = {0.0f};
//
//
//
/////////////////////////////////////////////////////////////////////////////////
////	�R���X�g���N�^
//CMeshField::CMeshField() : m_pVtxBuff(NULL), m_nMaxVtx(0), m_pIdxBuff(NULL), m_nMaxIdx(0), m_pTex(NULL), m_pNormalMap(NULL), m_pNormalVtxBuff(NULL)
//{
//  // �}�e���A��
//  m_Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//  m_Material.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.5f);
//  m_Material.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
//  m_Material.Specular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
//  m_Material.Power = 0.0f;
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	���b�V���t�B�[���h�쐬
//CMeshField *CMeshField::Create(const std::string& pFile, float fWidth, float fHeight, int nBlockX, int nBlockY)
//{
//  for (unsigned i = 0; i < 51 * 51; ++i)
//  {
//    m_aHeightMap[i] = HEIGHTMAP[i];
//    m_aHeightMap[i] *= 5.9f;
//  }
//
//  // ���b�V���t�B�[���h�V�K�쐬
//  CMeshField *pRet = new CMeshField;
//  if (pRet == NULL)
//  {
//    return NULL;
//  }
//
//  // 1�u���b�N�̑傫�����o��
//  pRet->m_nBlockX = nBlockX;
//  pRet->m_nBlockY = nBlockY;
//  pRet->m_v3Pos = DirectX::XMVectorSet(-fWidth  * 0.5f, 0.0f, fHeight * 0.5f, 0.0f); // �����̒��_�ʒu
//  pRet->m_v2Size = DirectX::XMVectorSet(fWidth / nBlockX, fHeight / nBlockY, 0.0f);
//
//  pRet->Init();
//
//  // �e�N�X�`���̓ǂݍ���
//  if (pFile.empty())
//  {
//    pRet->m_pTex = NULL;
//  }
//  else {
//    CManager::GetTextureManager()->Load(pFile);
//    pRet->m_pTex = CManager::GetTextureManager()->GetResource(pFile);
//  }
//  pRet->SetObjType(OBJTYPE_FIELD);
//
//  return pRet;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	����������
//HRESULT CMeshField::Init(void)
//{
//	// ���U���g
//	HRESULT hr;
//
//	// �f�o�C�X�̎擾
//	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
//	// ���_�ݒ�
//	{
//		// ���_���̌v�Z
//		m_nMaxVtx = (m_nBlockX + 1) * (m_nBlockY + 1);
//
//		// �o�b�t�@�̐���(�m��)
//		hr = pDevice->CreateVertexBuffer(
//			sizeof(VERTEX_3D) * m_nMaxVtx, // �o�b�t�@�T�C�Y
//			D3DUSAGE_WRITEONLY,                // ���_�o�b�t�@�̎g�p���@
//			FVF_VERTEX_3D,                     // �g�p���钸�_�t�H�[�}�b�g
//			D3DPOOL_MANAGED,                   // �o�b�t�@��ێ����郁�����N�G�X�g���w��
//			&m_pVtxBuff,                   // ���_�̃o�b�t�@�̐擪�A�h���X
//			NULL);
//
//		// ���_�m�ۂ����s
//		if (FAILED(hr))
//		{
//			Uninit();
//			return E_FAIL;
//		}
//
//		// ���_�ݒ�
//		
//		VERTEX_3D *pVtx;
//		float *pHeightMap = (float *)&m_aHeightMap[0];
//
//		hr = m_pVtxBuff->Lock(0, 0, (void**)&pVtx, D3DLOCK_NOSYSLOCK);
//		for (int nY = 0; nY < m_nBlockY + 1; nY++)
//		{
//			for (int nX = 0; nX < m_nBlockX + 1; nX++)
//			{
//				// �ʒu
//				pVtx->pos.x = m_v3Pos.x + m_v2Size.x * nX;
//				pVtx->pos.y = *pHeightMap;
//				pVtx->pos.z = m_v3Pos.z - (m_v2Size.y * nY);
//
//				// �e�N�X�`�����W
//				pVtx->tex = DirectX::XMVectorSet(1.0f * nX, 1.0f * nY, 0.0f, 0.0f);
//
//				// ���_�J���[
//				pVtx->color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//
//				// ���̒��_��
//				++pHeightMap;
//				++pVtx;
//			}
//		}
//
//		hr = m_pVtxBuff->Unlock(); // ���_�o�b�t�@�̌Œ������
//		// �@���}�b�v�̍쐬
//		{
//			// �̈�m��
//			m_pNormalMap = new D3DXVECTOR3[m_nBlockY*m_nBlockX * 2];
//			m_pNormalVtxBuff = new D3DXVECTOR3[m_nMaxVtx];
//
//			// �v�Z
//			SetNormalAll();
//		}
//	}
//
//	// �C���f�b�N�X���w��
//	{
//		// �C���f�b�N�X�����v�Z
//		m_nMaxIdx = ((m_nBlockX + 1) * 2 + 2) * (m_nBlockY)-2;
//
//		// ����(�m��)
//		hr = pDevice->CreateIndexBuffer(
//			sizeof(WORD) * m_nMaxIdx,       // �o�b�t�@�T�C�Y
//			D3DUSAGE_WRITEONLY,             // ���_�o�b�t�@�̎g�p���@
//			D3DFMT_INDEX16,                 // �g�p���钸�_�t�H�[�}�b�g
//			D3DPOOL_MANAGED,                // �o�b�t�@��ێ����郁�����N�G�X�g���w��
//			&m_pIdxBuff,                    // ���_�C���f�b�N�X�̃o�b�t�@�̐擪�A�h���X
//			NULL);
//
//		if (FAILED(hr))
//		{
//			Uninit();
//			return E_FAIL;
//		}
//
//		int nNumUpper = 0;
//		int nNumLower = m_nBlockX + 1;
//		WORD *pIndx;
//
//		hr = m_pIdxBuff->Lock(0, 0, (void **)&pIndx, D3DLOCK_NOSYSLOCK);
//		for (int nY = 0; nY < m_nBlockY; nY++)
//		{
//			for (int nX = 0; nX < m_nBlockX + 1; nX++)
//			{
//				// ���_
//				*pIndx = nNumLower;
//				++pIndx;
//
//				nNumLower++;
//
//				// ��_
//				*pIndx = nNumUpper;
//				++pIndx;
//
//				nNumUpper++;
//			}
//
//			if (nY < m_nBlockY - 1)
//			{
//				nNumUpper--;    // �_�~�[�_��ł��߂̒���
//
//				// �_�~�[�_�P
//				*pIndx = nNumUpper;
//				++pIndx;
//				nNumUpper++;
//
//				// �_�~�[�_�Q
//				*pIndx = nNumLower;
//				++pIndx;
//			}
//		}
//		hr = m_pIdxBuff->Unlock(); // �C���f�b�N�X�o�b�t�@�̌Œ������
//	}
//	return S_OK;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	�I������
//void CMeshField::Uninit( void )
//{
//	if( m_pVtxBuff )
//	{
//		m_pVtxBuff->Release();
//		m_pVtxBuff = NULL;
//	}
//
//	if( m_pIdxBuff )
//	{
//		m_pIdxBuff->Release();
//		m_pIdxBuff = NULL;
//	}
//
//	if( m_pNormalMap )
//	{
//		delete[] m_pNormalMap;
//		m_pNormalMap = NULL;
//	}
//
//	if( m_pNormalVtxBuff )
//	{
//		delete m_pNormalVtxBuff;
//		m_pNormalVtxBuff = NULL;
//	}
//	CScene::Uninit();
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	�X�V����
//void CMeshField::Update( void )
//{
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	�`�揈��
//void CMeshField::Draw( void )
//{
//	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();
//
//	///////////////////////////////////////////////////////////////////////////
//	//	���C�g�̍쐬	�i4�܂�
//	D3DLIGHT9 light;
//	//���g�̏�����
//	ZeroMemory(&light, sizeof(D3DLIGHT9));
//	//���C�g�̎�ނ̐ݒ�
//	light.Type = D3DLIGHT_DIRECTIONAL;
//	//�g�U���̐ݒ�
//	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
//	//�A���r�G���g���̐ݒ�
//	light.Ambient = D3DXCOLOR(0.3f, 0.2f, 0.4f, 1.0f);
//	//���C�g�̕���
//	D3DXVECTOR3 vecDir(0.0f, -1.0f, 1.0f);
//	//�P�ʃx�N�g���֕ϊ�
//	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);
//	//�f�o�C�X�Ƀ��C�g�̐ݒ� ( ���C�g�̃X���b�gIndex, �i�[���郉�C�g)
//	pDevice->SetLight(0, &light);
//	//���C�g��L���ɂ���	( ���C�g�̃X���b�gIndex, TRUE.FALSE)
//	pDevice->LightEnable(0, TRUE);
//	//�������C�g��L����
//	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
//
//	HRESULT hr;
//	// ���_�o�b�t�@���f�[�^�X�g���[���Ƀo�C���h
//	pDevice->SetStreamSource( 0 , m_pVtxBuff, 0, sizeof( VERTEX_3D ) );
//	// ���_�t�H�[�}�b�g�̐ݒ�
//	pDevice->SetFVF(FVF_VERTEX_3D);
//	// �C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���Ƀo�C���h
//	pDevice->SetIndices( m_pIdxBuff );
//	// �e�N�X�`���̐ݒ�
//	pDevice->SetTexture( 0, m_pTex );
//
//	// �s��̃Z�b�g
//	D3DXMATRIX mtxWorld;
//	D3DXMatrixIdentity(&mtxWorld);
//	hr = pDevice->SetTransform( D3DTS_WORLD, &mtxWorld );
//
//	// �}�e���A���̃Z�b�g
//	hr = pDevice->SetMaterial( &m_Material );
//
//	// �`��
//	hr = pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, m_nMaxVtx, 0, m_nMaxIdx - 2 );
//
//	#ifdef _DEBUG
//	#if 0
//	// ���C���[�t���[���\��
//	{
//		D3DMATERIAL9 Normal;
//		Normal.Diffuse = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Ambient = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Emissive = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Specular = D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
//		Normal.Power    = 1.0f;
//		pDevice->SetMaterial( &Normal );
//		DWORD nBeforeFillMode;
//		pDevice->GetRenderState( D3DRS_FILLMODE, &nBeforeFillMode );
//		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
//		pDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 0, m_nMaxVtx, 0, m_nMaxIdx - 2 );
//		pDevice->SetRenderState( D3DRS_FILLMODE, nBeforeFillMode );
//	}
//	#endif
//	#endif
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	�����擾
//float CMeshField::GetHeight( const float x, const float z ) const
//{
//	// �t�B�[���h�̃I�t�Z�b�g�l
//	const D3DXVECTOR2 offset = D3DXVECTOR2( m_v2Size.x * m_nBlockX * 0.5f, m_v2Size.y * m_nBlockY * 0.5f );
//	if( x < -offset.x || offset.x < x || z < -offset.y || offset.y < z )
//	{
//		return 0.0f;
//	}
//
//	// �t�B�[���h�̍��[�����_�Ƃ������_s������
//	const D3DXVECTOR2 s = D3DXVECTOR2( offset.x + x, offset.y + z );
//
//	// �ΏۂƂȂ�u���b�N���v�Z����
//	int nBlockX = (int)( s.x / m_v2Size.x );
//	int nBlockY = (int)( s.y / m_v2Size.y );
//
//	// �ΏۂƂȂ�u���b�N�̒��_��p�Ƃ���
//	const D3DXVECTOR3 p[4] =
//	{
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y ),
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y),
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	};
//	
//	// p0����̃x�N�g��a�𓾂�
//	D3DXVECTOR2 a = D3DXVECTOR2( x - p[1].x, z - p[1].z );
//	D3DXVec2Normalize( &a, &a );
//	if(a.x < 0.0f) a.x = -a.x;
//	if(a.y < 0.0f) a.y = -a.y;
//	
//	D3DXVECTOR3 nor;
//	if( a.x > a.y )
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[2], p[3] - p[2] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//	else
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[0], p[2] - p[1] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//	return p[1].y + ( nor.x * (  x - p[1].x ) + nor.z * ( z - p[1].z )) / -nor.y;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	���݈ʒu�̌X���Q�b�c�@�[
//D3DXQUATERNION CMeshField::GetQuaternion( const float x, const float z ) const
//{
//	// �t�B�[���h�̃I�t�Z�b�g�l
//	const D3DXVECTOR2 offset = D3DXVECTOR2( m_v2Size.x * m_nBlockX * 0.5f, m_v2Size.y * m_nBlockY * 0.5f );
//	if( x < -offset.x || offset.x < x || z < -offset.y || offset.y < z )
//	{
//		D3DXQUATERNION quat;
//		D3DXQuaternionIdentity(&quat);
//		return quat;
//	}
//
//	// �t�B�[���h�̍��[�����_�Ƃ������_s������
//	const D3DXVECTOR2 s = D3DXVECTOR2( offset.x + x, offset.y + z );
//
//	// �ΏۂƂȂ�u���b�N���v�Z����
//	int nBlockX = (int)( s.x / m_v2Size.x );
//	int nBlockY = (int)( s.y / m_v2Size.y );
//
//	// �ΏۂƂȂ�u���b�N�̒��_��p�Ƃ���
//	const D3DXVECTOR3 p[4] =
//	{
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y ),
//
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y),
//
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	};
//
//	// p0����̃x�N�g��a�𓾂�
//	D3DXVECTOR2 a = D3DXVECTOR2( x - p[1].x, z - p[1].z );
//	D3DXVec2Normalize( &a, &a );
//	if(a.x < 0.0f) a.x = -a.x;
//	if(a.y < 0.0f) a.y = -a.y;
//
//	// �@�������߂�
//	D3DXVECTOR3 nor;
//
//	if( a.x > a.y )
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[2], p[3] - p[2] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//
//	else
//	{
//		const D3DXVECTOR3 v[2] = { p[1] - p[0], p[2] - p[1] };
//		D3DXVec3Cross( &nor, &v[0], &v[1] );
//		D3DXVec3Normalize( &nor, &nor );
//	}
//
//	// ��]�������߂�
//	D3DXVECTOR3 axis;
//	const D3DXVECTOR3 def(0.0f,1.0f,0.0f);
//	D3DXVec3Cross(&axis,&def,&nor);
//
//	// ��]�p�����߂�
//	const float angle = acosf(D3DXVec3Dot(&def,&nor));
//
//	// �N�I�[�^�j�I�����쐬
//	D3DXQUATERNION quat;
//	D3DXQuaternionRotationAxis(&quat,&axis,angle);
//
//	return quat;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	�@���Z�b�c�@�[
//void CMeshField::SetNormalAll( void )
//{
//	// �����ʒu���v�Z����
//	D3DXVECTOR3 Pos[4] =
//	{
//		D3DXVECTOR3( -m_v2Size.x * m_nBlockX * 0.5f, m_aHeightMap[m_nBlockX + 1], -m_v2Size.y * m_nBlockY * 0.5f ),
//		D3DXVECTOR3( -m_v2Size.x * m_nBlockX * 0.5f, m_aHeightMap[0],             -m_v2Size.y * m_nBlockY * 0.5f + m_v2Size.y ),
//		D3DXVECTOR3( -m_v2Size.x * m_nBlockX * 0.5f + m_v2Size.x, m_aHeightMap[m_nBlockX + 2], -m_v2Size.y * m_nBlockY * 0.5f ),
//		D3DXVECTOR3(  m_v2Size.x * m_nBlockX * 0.5f + m_v2Size.x, m_aHeightMap[1],              m_v2Size.y * m_nBlockY * 0.5f + m_v2Size.y )
//	};
//	
//	// ���_�ɓ����@����������
//	{
//		int i = 0;
//
//		while( i < m_nMaxVtx )
//		{
//			m_pNormalVtxBuff[i++] = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
//		}
//	}
//	
//	// �ʖ@���̌v�Z�ƒ��_�ɓ����@�����v�Z����
//	// �������A�P�ʉ��͌�ōs��
//	{
//		int i = 0;  // ��
//
//		for( int y = 0 ; y < m_nBlockY ; ++y )
//		{
//			for( int x = 0 ; x < m_nBlockX ; ++x )
//			{
//				// �|���S�����̈�
//				{
//					D3DXVECTOR3 nor;
//
//					// �����̒��_����̃x�N�g��1,2
//					D3DXVECTOR3 v3Vec1 = Pos[1] - Pos[0];
//					D3DXVECTOR3 v3Vec2 = Pos[2] - Pos[1];
//
//					// �O�ς���@�������߂�
//					D3DXVec3Cross( &nor, &v3Vec1, &v3Vec2 );
//					D3DXVec3Normalize( &m_pNormalMap[i], &nor );
//
//					// �ΏۂƂȂ钸�_�ɓ����
//					m_pNormalVtxBuff[x+0+(m_nBlockX+1)*(y+1)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+0+(m_nBlockX+1)*(y+0)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+1+(m_nBlockX+1)*(y+1)] += m_pNormalMap[i];
//
//					// ���̖@����
//					++i;
//				}
//
//				// �|���S�����̂Q
//				{
//					D3DXVECTOR3 nor;
//
//					// ����̒��_����̃x�N�g��1,2
//					D3DXVECTOR3 v3Vec1 = Pos[3] - Pos[1];
//					D3DXVECTOR3 v3Vec2 = Pos[2] - Pos[3];
//
//					// �O�ς���@�������߂�
//					D3DXVec3Cross( &nor, &v3Vec1, &v3Vec2 );
//					D3DXVec3Normalize( &m_pNormalMap[i], &nor );
//
//					// �ΏۂƂȂ钸�_�ɓ����
//					m_pNormalVtxBuff[x+0+(m_nBlockX+1)*(y+0)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+1+(m_nBlockX+1)*(y+0)] += m_pNormalMap[i];
//					m_pNormalVtxBuff[x+1+(m_nBlockX+1)*(y+1)] += m_pNormalMap[i];
//					// ���̖@����
//					++i;
//				}
//
//				// ���̃u���b�N�ւ̏�����
//				Pos[0].y = m_aHeightMap[(y+1)*(m_nBlockX+1)+x];
//				Pos[1].y = m_aHeightMap[y*(m_nBlockX+1)+x];
//				Pos[2].y = m_aHeightMap[(y+1)*(m_nBlockX+1)+x+1];
//				Pos[3].y = m_aHeightMap[y*(m_nBlockX+1)+x+1];
//			}
//		}
//	}
//
//	// ���_�ɖ@��������
//	{
//		HRESULT hr;
//		VERTEX_3D *pVtx;
//		hr = m_pVtxBuff->Lock( 0, 0, (void**)&pVtx, D3DLOCK_NOSYSLOCK );
//		for( int i = 0 ; i < m_nMaxVtx ; ++i )
//		{
//			D3DXVec3Normalize( &pVtx[i].normal, &m_pNormalVtxBuff[i] );
//		}
//		hr = m_pVtxBuff->Unlock(); // ���_�o�b�t�@�̌Œ������
//	}
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	�����蔻��
//bool CMeshField::IsHit( const D3DXVECTOR3 &start, const D3DXVECTOR3 &end ) const
//{
//	return HitField(start,end,1.0f);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	���ݑ���Ƃ̓����蔻��
//bool CMeshField::HitField( const D3DXVECTOR3 &start, const D3DXVECTOR3 &end, float par ) const
//{
//	bool re;
//
//	// ��G�c�Ƀu���b�N�����
//	float x = start.x + ( end.x - start.x ) * par;
//	float z = start.z + ( end.z - start.z ) * par;
//	
//	// �t�B�[���h�̃I�t�Z�b�g�l
//	const D3DXVECTOR2 offset = D3DXVECTOR2( m_v2Size.x * m_nBlockX * 0.5f, m_v2Size.y * m_nBlockY * 0.5f );
//	if( x < -offset.x || offset.x < x || z < -offset.y || offset.y < z )
//	{
//		re = false;
//	}
//	
//	// �t�B�[���h�̍��[�����_�Ƃ������_s������
//	const D3DXVECTOR2 s = D3DXVECTOR2( offset.x + x, offset.y + z );
//	
//	// �ΏۂƂȂ�u���b�N���v�Z����
//	int nBlockX = (int)( s.x / m_v2Size.x );
//	int nBlockY = (int)( s.y / m_v2Size.y );
//	
//	// �ΏۂƂȂ�u���b�N�̒��_��p�Ƃ���
//	const D3DXVECTOR3 p[4] =
//	{
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y ),
//	
//		D3DXVECTOR3(
//			(nBlockX) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+0)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	
//		D3DXVECTOR3(
//			(nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY+0)*(m_nBlockX+1)],
//			(nBlockY) * m_v2Size.y - offset.y),
//	
//		D3DXVECTOR3( (nBlockX+1) * m_v2Size.x - offset.x,
//			m_aHeightMap[(nBlockX+1)+(m_nBlockY-nBlockY-1)*(m_nBlockX+1)],
//			(nBlockY+1) * m_v2Size.y - offset.y),
//	};
//	
//	// p0����̃x�N�g��a�𓾂�
//	const D3DXVECTOR2 a = D3DXVECTOR2( x - p[1].x, z - p[1].z );
//	D3DXVECTOR2 a_nor;
//	D3DXVec2Normalize( &a_nor, &a );
//	
//	// �����蔻��
//	if( a_nor.x < a_nor.y )
//	{
//		re = StepOverFrom3Vertex(start,end,&p[1],&p[3],&p[2]);
//	}
//	else
//	{
//		re = StepOverFrom3Vertex(start,end,&p[0],&p[1],&p[2]);
//	}	
//	if(re) return re;
//	
//	// ������Ȃ���Ύ���
//	static const float fineness = 1.0f / 6.0f;
//	par -= fineness;
//	if( par < 0.0f ) return false;
//
//	return HitField(start,end,par);
//}
//
//
/////////////////////////////////////////////////////////////////////////////////
////	�F�ݒ�
//void CMeshField::SetDiffuse( const D3DXCOLOR &_set ){
//	m_Material.Diffuse = _set;
//}
//
/////////////////////////////////////////////////////////////////////////////////
////	���ʂ��ׂ��ŋ��邩�H
//bool CMeshField::StepOverFrom3Vertex(const D3DXVECTOR3& start, const D3DXVECTOR3& end, const D3DXVECTOR3 *pVtx1, const D3DXVECTOR3 *pVtx2, const D3DXVECTOR3 *pVtx3) const
//{
//	D3DXVECTOR3 vec12, vec13, normal;
//	vec12 = *pVtx2 - *pVtx1;
//	vec13 = *pVtx3 - *pVtx1;
//	D3DXVec3Cross(&normal, &vec12, &vec13);
//	D3DXVec3Normalize(&normal, &normal);
//	float a = normal.x;
//	float b = normal.y;
//	float c = normal.z;
//	float d = (normal.x*(-pVtx1->x)) + (normal.y*(-pVtx1->y)) + (normal.z*(-pVtx1->z));
//
//	float startDot = a*start.x + b*start.y + c*start.z + d;
//	float endDot = a*end.x + b*end.y + c*end.z + d;
//
//	if (startDot * endDot <= 1) return true;
//	return false;
//}