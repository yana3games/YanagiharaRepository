#pragma once
//=============================================================================
// File : skinCascadeDepthShader.h
// Date : 2018/02/28(��)
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


// �J�X�P�[�h�V���h�E�t�����̕`��V�F�[�_
class SkinCascadeDepthShader : public Shader
{
public:
  SkinCascadeDepthShader();
  ~SkinCascadeDepthShader();

  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture);

  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
    float weight[4];
    unsigned int boneIndex[4];
  };

  struct Matrix
  {
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX proj;
  };

private:
  void CreateMatrixBuffer(void);
  void CreateShader(void);
  ID3D11Buffer* m_matCB;        // �}�g���N�X�̃R���X�^���g�o�b�t�@
};