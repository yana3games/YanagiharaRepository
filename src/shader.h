#pragma once
//=============================================================================
// File : shader.h
// Date : 2018/01/19(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include <d3d11.h>
#include "Vector.h"

// �O���錾
class Texture;

// �V�F�[�_�[�N���X�C���^�[�t�F�[�X
class Shader
{
 public:
  Shader() : m_pVS(nullptr), m_pIL(nullptr), m_pHS(nullptr), m_pDS(nullptr), m_pGS(nullptr), m_pPS(nullptr) {};
  virtual ~Shader() {};
  virtual void SetParameters() {};
  virtual void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, ID3D11Buffer* boneCB, Texture* texture) {};
  virtual void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, Texture* texture) {};
  virtual void SetParameters(DirectX::XMMATRIX& World, const DirectX::XMMATRIX& View, const DirectX::XMMATRIX& Proj, const DirectX::XMMATRIX& WIT, Texture* texture) {};

 protected:
  ID3D11VertexShader*    m_pVS;      // ���_�V�F�[�_
  ID3D11InputLayout*     m_pIL;      // ���̓��C�A�E�g
  ID3D11HullShader*      m_pHS;      // �n���V�F�[�_
  ID3D11DomainShader*    m_pDS;      // �h���C���V�F�[�_
  ID3D11GeometryShader*  m_pGS;      // �W�I���g���V�F�[�_
  ID3D11PixelShader*     m_pPS;      // �s�N�Z���V�F�[�_
};