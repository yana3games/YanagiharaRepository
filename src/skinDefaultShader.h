#pragma once
//=============================================================================
// File : skinDefaultShader.h
// Date : 2018/02/29(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "shader.h"
#include "Vector.h"


// �X�L�����b�V���`��V�F�[�_
class SkinDefaultShader : public Shader
{
public:
  SkinDefaultShader();
  ~SkinDefaultShader();

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