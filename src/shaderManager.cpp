//=============================================================================
// File : shaderManager.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <Windows.h>
#include "appManager.h"
#include "shaderManager.h"
#include "shader.h"
#include "renderer.h"
#include <algorithm>


// default
#include "skydomeDefaultShader.h"
#include "terrainDefaultShader.h"
#include "expSpriteDefaultShader.h"

// depth shadow
#include "skinShadowShader.h"

// cascade shadow
#include "cascadeDepthShader.h"
#include "skinCascadeDepthShader.h"
#include "terrainCascadeDepthShader.h"
#include "regidCascadeShadowShader.h"
#include "skinCascadeShadowShader.h"
#include "terrainCascadeShadowShader.h"


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
ShaderManager::ShaderManager()
{
  // �Ώۂ̎�ޕ������z��m��
  m_useShader.resize(eShaderTypeMax);
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
ShaderManager::~ShaderManager()
{
  // �S�V�F�[�_�[�f�[�^�폜
  AllRelease();
}


///////////////////////////////////////////////////////////////////////////////
// �g�p����V�F�[�_�N���X�̍쐬
void ShaderManager::Initialize()
{
  // �Q�[���Ɏg�p����V�F�[�_�[�̓ǂݍ���
  m_resShader["skinShadow"]     = new SkinShadowShader;
  m_resShader["regidDefault"]   = new ExpSpriteDefaultShader;
  m_resShader["terrainDefault"] = new TerrainDefaultShader;
  m_resShader["skyDefault"]     = new SkydomeDefaultShader;
  //m_resShader["skinDepth"]      = new SkinDepthShader;
  //m_resShader["skinDepth"]      = new SkinDepthShader;


  m_resShader["cascadeDepth"]       = new CascadeDepthShader;
  m_resShader["skinCascadeDepth"]   = new SkinCascadeDepthShader;
  m_resShader["terrainCascadeDepth"]= new TerrainCascadeDepthShader;
  m_resShader["regidCascadeShadow"] = new RegidCascadeShadowShader;
  m_resShader["skinCascadeShadow"]  = new SkinCascadeShadowShader;
  m_resShader["terrainCascadeShadow"] = new TerrainCascadeShadowShader;
}


///////////////////////////////////////////////////////////////////////////////
// ���݃Z�b�g���̃^�C�v���̃V�F�[�_�[���擾
Shader*  ShaderManager::GetShader(eShaderType type)
{
  return m_useShader[type];
}


///////////////////////////////////////////////////////////////////////////////
// ���ʂ̃V�F�[�_�[�̃Z�b�g
void ShaderManager::SetDefaultShader(void)
{
  m_useShader[eShaderTypeSky]  = m_resShader["skyDefault"];
  m_useShader[eShaderTypeTerrain] = m_resShader["terrainDefault"];
  m_useShader[eShaderTypeRegid]  = m_resShader["regidDefault"];
//  m_useShader[eShaderTypeSkin]   = m_resShader["skinDefault"];
//  m_useShader[eShaderTypeSprite] = m_resShader["spriteDefault"];
}


///////////////////////////////////////////////////////////////////////////////
// �[�x��񏑂����ݗp�V�F�[�_�[�̃Z�b�g
void ShaderManager::SetDepthShader(void)
{
  m_useShader[eShaderTypeRegid]  = m_resShader["regidDepth"];
  m_useShader[eShaderTypeSkin]   = m_resShader["skinDepth"];
}


///////////////////////////////////////////////////////////////////////////////
// �V���h�E�}�b�v�𗘗p�����V�F�[�_�[�̃Z�b�g
void ShaderManager::SetShadowShader(void)
{
  m_useShader[eShaderTypeRegid]  = m_resShader["regidShadow"];
//  m_useShader[eShaderTypeSkin]   = m_resShader["skinShadow"];
}


///////////////////////////////////////////////////////////////////////////////
// �J�X�P�[�h�Ή��̐[�x��񏑂����ݗp�V�F�[�_�[�̃Z�b�g
void ShaderManager::SetCascadeDepthShader(const DirectX::XMMATRIX& proj, const DirectX::XMMATRIX& view)
{
  // �V�F�[�_�̃Z�b�g
  m_useShader[eShaderTypeRegid] = m_resShader["cascadeDepth"];
  m_useShader[eShaderTypeSkin] = m_resShader["skinCascadeDepth"];
  m_useShader[eShaderTypeTerrain] = m_resShader["terrainCascadeDepth"];
}


///////////////////////////////////////////////////////////////////////////////
// �J�X�P�[�h�V���h�E�}�b�v�𗘗p�����I�u�W�F�N�g�`��V�F�[�_�[�̃Z�b�g
void ShaderManager::SetCascadeShader(DirectX::XMMATRIX* proj, DirectX::XMMATRIX* view, RenderTexture** zbuffer)
{
  // �V�F�[�_�[�̃Z�b�g
  SetDefaultShader(); // �V���h�E�I�u�W�F�N�g�ΏۊO�͒ʏ�V�F�[�_�`�悷��
  m_useShader[eShaderTypeRegid] = m_resShader["regidCascadeShadow"];
  m_useShader[eShaderTypeSkin] = m_resShader["skinCascadeShadow"];
  m_useShader[eShaderTypeTerrain] = m_resShader["terrainCascadeShadow"];

  // �Œ���̃Z�b�g
  ((RegidCascadeShadowShader*)m_useShader[eShaderTypeRegid])->Set(proj, view, zbuffer);
  ((SkinCascadeShadowShader*)m_useShader[eShaderTypeSkin])->Set(proj, view, zbuffer);
  ((TerrainCascadeShadowShader*)m_useShader[eShaderTypeTerrain])->Set(proj, view, zbuffer);
}


///////////////////////////////////////////////////////////////////////////////
// �x�������_�����O�̃}���`�T���v�����O�`��V�F�[�_�[�̃Z�b�g
void ShaderManager::SetDeferredShader(void)
{
  m_useShader[eShaderTypeRegid]  = m_resShader["regidDeferred"];
  m_useShader[eShaderTypeSkin]   = m_resShader["skinDeferred"];
}


///////////////////////////////////////////////////////////////////////////////
// �x�������_�����O�̍ŏI�`��p�V�F�[�_�[�Z�b�g
void ShaderManager::SetDeferredBlendShader(void)
{
  m_useShader[eShaderTypeSprite] = m_resShader["spriteDefaultEnd"];
}


///////////////////////////////////////////////////////////////////////////////
// ���_�V�F�[�_���[�h
bool ShaderManager::LoadVS(const std::string& key, D3D11_INPUT_ELEMENT_DESC* layout, int numElements)
{
  std::unordered_map<std::string, VertexShader>::const_iterator itr = m_resVS.find(key);
  if (itr != m_resVS.end()) return true; // ���Ƀ��[�h����Ă���B

  VertexShader vertexShader;
  if (!CreateVertexShader(key, &vertexShader.pVS, &vertexShader.pIL, layout, numElements)) {
    MessageBox(NULL, key.c_str(), "MakeVertexShaderError!", MB_OK);
    return false; // �쐬���s
  }

  // ���[�h����
  m_resVS[key] = vertexShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �w��̒��_�V�F�[�_���\�[�X�̃Q�b�g
ShaderManager::VertexShader ShaderManager::GetVS(const std::string& key)
{
  std::unordered_map<std::string, VertexShader>::const_iterator itr = m_resVS.find(key);
  if (itr == m_resVS.end()) return VertexShader(); // ���̖��O�̃��\�[�X�����݂��Ȃ�

  // �擾����
  return itr->second;
}


///////////////////////////////////////////////////////////////////////////////
// �s�N�Z���V�F�[�_���[�h
bool ShaderManager::LoadPS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11PixelShader*>::const_iterator itr = m_resPS.find(key);
  if( itr != m_resPS.end()) return true; // ���Ƀ��[�h����Ă���B

  ID3D11PixelShader* pixelShader;
  if( !CreatePixelShader( key, &pixelShader)) {
    MessageBox(NULL, key.c_str(), "MakePicelShaderError!", MB_OK);
    return false; // �쐬���s
  }

  // ���[�h����
  m_resPS[ key] = pixelShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �w��̃s�N�Z���V�F�[�_���\�[�X�̃Q�b�g
ID3D11PixelShader* ShaderManager::GetPS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11PixelShader*>::const_iterator itr = m_resPS.find(key);
  if (itr == m_resPS.end()) return nullptr; // ���̖��O�̃��\�[�X�����݂��Ȃ�

  // �擾����
  return itr->second;
}


///////////////////////////////////////////////////////////////////////////////
// �W�I���g���V�F�[�_���[�h
bool ShaderManager::LoadGS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11GeometryShader*>::const_iterator itr = m_resGS.find(key);
  if (itr != m_resGS.end()) return true; // ���Ƀ��[�h����Ă���B

  ID3D11GeometryShader* geometryShader;
  if (!CreateGeometryShader(key, &geometryShader)) {
    MessageBox(NULL, key.c_str(), "MakeGeometryShaderError!", MB_OK);
    return false; // �쐬���s
  }

  // ���[�h����
  m_resGS[key] = geometryShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �w��̃W�I���g���V�F�[�_���\�[�X�̃Q�b�g
ID3D11GeometryShader* ShaderManager::GetGS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11GeometryShader*>::const_iterator itr = m_resGS.find(key);
  if (itr == m_resGS.end()) return nullptr; // ���̖��O�̃��\�[�X�����݂��Ȃ�

  // �擾����
  return itr->second;
}


///////////////////////////////////////////////////////////////////////////////
// �n���V�F�[�_���[�h
bool ShaderManager::LoadHS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11HullShader*>::const_iterator itr = m_resHS.find(key);
  if (itr != m_resHS.end()) return true; // ���Ƀ��[�h����Ă���B

  ID3D11HullShader* hullShader;
  if (!CreateHullShader(key, &hullShader)) {
    MessageBox(NULL, key.c_str(), "MakeHullShaderError!", MB_OK);
    return false; // �쐬���s
  }

  // ���[�h����
  m_resHS[key] = hullShader;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �w��̃n���V�F�[�_���\�[�X�̃Q�b�g
ID3D11HullShader* ShaderManager::GetHS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11HullShader*>::const_iterator itr = m_resHS.find(key);
  if (itr == m_resHS.end()) return nullptr; // ���̖��O�̃��\�[�X�����݂��Ȃ�

  // �擾����
  return itr->second;
}

///////////////////////////////////////////////////////////////////////////////
// �h���C���V�F�[�_���[�h
bool ShaderManager::LoadDS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11DomainShader*>::const_iterator itr = m_resDS.find(key);
  if (itr != m_resDS.end()) return true; // ���Ƀ��[�h����Ă���B

  ID3D11DomainShader* domainShader;
  if (!CreateDomainShader(key, &domainShader)) {
    MessageBox(NULL, key.c_str(), "MakeDomainShaderError!", MB_OK);
    return false; // �쐬���s
  }

  // ���[�h����
  m_resDS[key] = domainShader;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// �w��̃h���C���V�F�[�_���\�[�X�̃Q�b�g
ID3D11DomainShader* ShaderManager::GetDS(const std::string& key)
{
  std::unordered_map<std::string, ID3D11DomainShader*>::const_iterator itr = m_resDS.find(key);
  if (itr == m_resDS.end()) return nullptr; // ���̖��O�̃��\�[�X�����݂��Ȃ�

  // �擾����
  return itr->second;
}

///////////////////////////////////////////////////////////////////////////////
// �S�ẴV�F�[�_�[���\�[�X�̍폜 �G���[�ǂ�����ďo���΂����񂾁H ����k
bool ShaderManager::AllRelease(void)
{
  // �V�F�[�_�N���X���
  for (auto&& shader : m_resShader) {
    delete shader.second;
  }
  m_resShader.clear();
  m_useShader.clear();

  // ���_�V�F�[�_�E���̓��C�A�E�g�������
  for (auto&& vs : m_resVS) {
    vs.second.pVS->Release();
    vs.second.pIL->Release();
  }
  m_resVS.clear();

  // �s�N�Z���V�F�[�_�������
  for (auto&& ps : m_resPS) {
    ps.second->Release();
  }
  m_resPS.clear();

  // �W�I���g���V�F�[�_�������
  for (auto&& gs : m_resGS) {
    gs.second->Release();
  }
  m_resGS.clear();

  // �n���V�F�[�_�������
  for (auto&& hs : m_resHS) {
    hs.second->Release();
  }
  m_resHS.clear();

  // �h���C���V�F�[�_�������
  for (auto&& ds : m_resDS) {
    ds.second->Release();
  }
  m_resDS.clear();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// ���_�V�F�[�_�[�쐬
bool ShaderManager::CreateVertexShader(const std::string& csoName, ID3D11VertexShader** resVS, ID3D11InputLayout** resIL, D3D11_INPUT_ELEMENT_DESC* layout, int numElements)
{
  FILE* fp = fopen(csoName.c_str(), "rb");
  if (fp == NULL) return false;

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* cso_data = new unsigned char[cso_sz];
  fread(cso_data, cso_sz, 1, fp);
  fclose(fp);

  ID3D11VertexShader* pVertexShader;
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateVertexShader(cso_data, cso_sz, NULL, &pVertexShader);
  if (FAILED(hr)) return false;
  *resVS = pVertexShader;

  ID3D11InputLayout* pInputLayout;
  hr = AppManager::GetRenderer()->GetDevice()->CreateInputLayout(layout, numElements, cso_data, cso_sz, &pInputLayout);
  if (FAILED(hr)) return false;
  *resIL = pInputLayout;

  delete[] cso_data;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �s�N�Z���V�F�[�_�[�쐬
bool ShaderManager::CreatePixelShader(const std::string& csoName, ID3D11PixelShader** resPS)
{
  FILE* fp = fopen(csoName.c_str(), "rb");
  if (fp == NULL) return false;

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* cso_data = new unsigned char[cso_sz];
  fread(cso_data, cso_sz, 1, fp);
  fclose(fp);

  ID3D11PixelShader* pPixelShader;
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreatePixelShader(cso_data, cso_sz, NULL, &pPixelShader);
  if (FAILED(hr)) return false;
  *resPS = pPixelShader;

  delete[] cso_data;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �W�I���g���V�F�[�_�쐬
bool ShaderManager::CreateGeometryShader( const std::string& csoName, ID3D11GeometryShader** resGS)
{
  FILE* fp = fopen(csoName.c_str(), "rb");
  if (fp == NULL) return false;

  fseek(fp, 0, SEEK_END);
  long cso_sz = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  unsigned char* cso_data = new unsigned char[cso_sz];
  fread(cso_data, cso_sz, 1, fp);
  fclose(fp);

  ID3D11GeometryShader* pGeometryShader;
  HRESULT hr = AppManager::GetRenderer()->GetDevice()->CreateGeometryShader(cso_data, cso_sz, NULL, &pGeometryShader);
  if (FAILED(hr)) return false;
  *resGS = pGeometryShader;

  delete[] cso_data;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �n���V�F�[�_�[�쐬
bool ShaderManager::CreateHullShader(const std::string& csoName, ID3D11HullShader** resHS)
{
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// �h���C���V�F�[�_�[�쐬
bool ShaderManager::CreateDomainShader(const std::string& csoName, ID3D11DomainShader** resDS)
{
  return true;
}
