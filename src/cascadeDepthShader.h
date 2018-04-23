#pragma once
//=============================================================================
// File : cascadeDepthShader.h
// Date : 2018/02/27(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "shader.h"


///////////////////////////////////////////////////////////////////////////////
// �O���錾


// �J�X�P�[�h�V���h�E�p���̂̐[�x�`��V�F�[�_
class CascadeDepthShader : public Shader
{
public:
  CascadeDepthShader();
  ~CascadeDepthShader();

  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, Texture* texture);

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

private:
  void CreateMatrixBuffer(void);
  void CreateShader(void);
  ID3D11Buffer* m_matCB;        // �}�g���N�X�̃R���X�^���g�o�b�t�@
};