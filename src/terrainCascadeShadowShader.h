#pragma once
//=============================================================================
// File : terrainCascadeShadowShader.h
// Date : 2018/03/01(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "shader.h"
#include "Vector.h"
#include "cascadeShadowManager.h"


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class RenderTexture;


// �J�X�P�[�h�V���h�E�t���n�`�`��V�F�[�_
class TerrainCascadeShadowShader : public Shader
{
public:
  TerrainCascadeShadowShader();
  ~TerrainCascadeShadowShader();

  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture);
  void Set(DirectX::XMMATRIX* proj, DirectX::XMMATRIX* view, RenderTexture** zbuffer) {
    m_sunProj = proj;
    m_sunView = *view;
    m_ZBuffers = zbuffer;
  }

  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
  };

  struct Matrix
  {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
  };

  struct SunMatrix
  {
    DirectX::XMMATRIX sunView;
    DirectX::XMMATRIX sunProj[CascadeShadowManager::NumDivision];
    float divisionFar[CascadeShadowManager::NumDivision];
  };

private:
  void CreateMatrixBuffer(void);
  void CreateSunBuffer(void);
  void CreateShader(void);
  DirectX::XMMATRIX* m_sunProj; // ���z�̃v���W�F�N�V�����s��z��̐擪�|�C���^
  DirectX::XMMATRIX m_sunView;  // ���z�̃r���[�}�g���N�X
  ID3D11Buffer* m_matCB;        // �}�g���N�X�̃R���X�^���g�o�b�t�@
  ID3D11Buffer* m_sunCB;        // ���z�s��̃R���X�^���g�o�b�t�@
  RenderTexture** m_ZBuffers;   // �[�x�o�b�t�@�z��̐擪�|�C���^
};