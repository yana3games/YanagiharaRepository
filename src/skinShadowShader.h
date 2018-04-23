#pragma once
//=============================================================================
// File : skinShadowShader.h
// Date : 2018/01/26(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "shader.h"
#include "Vector.h"

class Texture;

// �������V���h�E�X�L�����b�V���V�F�[�_�[�N���X
class SkinShadowShader : public Shader
{
 public:
  SkinShadowShader();
  ~SkinShadowShader();

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
    DirectX::XMMATRIX wit;
  };

 private:
  ID3D11Buffer*          m_matCB;    // �R���X�^���g�o�b�t�@
};