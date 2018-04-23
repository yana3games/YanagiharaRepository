#pragma once
//=============================================================================
// File : expSpriteDefaultShader.h
// Date : 2018/02/09(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "shader.h"
#include "Vector.h"

// �����p�V�F�[�_
class ExpSpriteDefaultShader : public Shader
{
public:
  ExpSpriteDefaultShader();
  ~ExpSpriteDefaultShader();
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
  void CreateVertexBuffer(void);
  void CreateShader(void);
  ID3D11Buffer* m_pVB;
  ID3D11Buffer* m_matCB;    // �}�g���N�X�̃R���X�^���g�o�b�t�@
};