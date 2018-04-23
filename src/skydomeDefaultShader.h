#pragma once
//=============================================================================
// File : skydomeDefaultShader.h
// Date : 2018/02/19(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "shader.h"
#include "Vector.h"

// �X�J�C�h�[���ʏ�`��p�V�F�[�_
class SkydomeDefaultShader : public Shader
{
public:
  SkydomeDefaultShader();
  ~SkydomeDefaultShader();
  void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture);

  struct Vertex
  {
    Vector3 pos;
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
  ID3D11Buffer* m_matCB;    // �}�g���N�X�̃R���X�^���g�o�b�t�@
};
