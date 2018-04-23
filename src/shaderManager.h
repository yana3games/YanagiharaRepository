#pragma once
//=============================================================================
// File  : shaderManager.h
// Date  : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <d3d11.h>
#include <unordered_map>
#include <DirectXMath.h>


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class Shader;
class RenderTexture;


///////////////////////////////////////////////////////////////////////////////
// �V�F�[�_�[�Ǘ��N���X
class ShaderManager
{
 public:
  // ���_�V�F�[�_�Ɠ��̓��C�A�E�g�̃��b�v
  struct VertexShader {
    ID3D11VertexShader* pVS;
    ID3D11InputLayout* pIL;
  };

  // �V�F�[�_�[��K������I�u�W�F�N�g�̎��
  typedef enum {
    eShaderTypeSky = 0,
    eShaderTypeRegid,
    eShaderTypeSkin,
    eShaderTypeTerrain,
    eShaderTypeSprite,
    eShaderTypeMax
  }eShaderType;
  ShaderManager();
  ~ShaderManager();

  // �g�p����V�F�[�_�N���X�쐬
  void Initialize(void);

  // �Z�b�g����Ă�V�F�[�_�̎擾
  Shader* GetShader( eShaderType type);

  // ���ʂ̃V�F�[�_�[�̃Z�b�g
  void SetDefaultShader(void);

  // �[�x���i�[�p�V�F�[�_�[�̃Z�b�g
  void SetDepthShader(void);

  // �V���h�E�}�b�v�𗘗p����V�F�[�_�[�̃Z�b�g
  void SetShadowShader(void);

  // �J�X�P�[�h�Ή��̐[�x���i�[�p�V�F�[�_�[�̃Z�b�g
  void SetCascadeDepthShader(const DirectX::XMMATRIX& proj, const DirectX::XMMATRIX& view);

  // �J�X�P�[�h�V���h�E�}�b�v�𗘗p����V�F�[�_�[�̃Z�b�g
  void SetCascadeShader(DirectX::XMMATRIX* proj, DirectX::XMMATRIX* view, RenderTexture** zbuffer);
  
  // �x�������_�����O�̃}���`�����_�[�`��V�F�[�_�[�̃Z�b�g
  void SetDeferredShader(void);

  // �x�������_�����O�̍ŏI�`��p�V�F�[�_�[�Z�b�g
  void SetDeferredBlendShader(void);

  // VertexShader
  bool LoadVS(const std::string& key, D3D11_INPUT_ELEMENT_DESC* layout, int numElements);
  VertexShader GetVS(const std::string& key);
  
  // PixelShader
  bool LoadPS(const std::string& key);
  ID3D11PixelShader* GetPS(const std::string& key);

  // GeometryShader
  bool LoadGS(const std::string& key);
  ID3D11GeometryShader* GetGS(const std::string& key);

  // HullShader
  bool LoadHS(const std::string& key);
  ID3D11HullShader* GetHS(const std::string& key);

  // DomainShader
  bool LoadDS(const std::string& key);
  ID3D11DomainShader* GetDS(const std::string& key);

  // AllShader
  bool AllRelease(void);

 private:
  bool CreateVertexShader(  const std::string& csoName, ID3D11VertexShader**   resVS, ID3D11InputLayout** resIL, D3D11_INPUT_ELEMENT_DESC* layout, int numElements); // ���_�V�F�[�_�쐬
  bool CreatePixelShader(   const std::string& csoName, ID3D11PixelShader**    resPS); // �s�N�Z���V�F�[�_�쐬
  bool CreateGeometryShader(const std::string& csoName, ID3D11GeometryShader** resGS); // �W�I���g���V�F�[�_�쐬
  bool CreateHullShader(    const std::string& csoName, ID3D11HullShader**     resHS); // �n���V�F�[�_�쐬
  bool CreateDomainShader(  const std::string& csoName, ID3D11DomainShader**   resDS); // �h���C���V�F�[�_�쐬

  std::unordered_map< std::string, VertexShader>          m_resVS;  // ���_�V�F�[�_�o�b�t�@�}�b�v
  std::unordered_map< std::string, ID3D11PixelShader*>    m_resPS;  // �s�N�Z���V�F�[�_�}�b�v
  std::unordered_map< std::string, ID3D11GeometryShader*> m_resGS;  // �W�I���g���V�F�[�_�o�b�t�@�}�b�v
  std::unordered_map< std::string, ID3D11HullShader*>     m_resHS;  // �n���V�F�[�_�o�b�t�@�}�b�v
  std::unordered_map< std::string, ID3D11DomainShader*>   m_resDS;  // �h���C���V�F�[�_�o�b�t�@�}�b�v

  std::unordered_map< std::string, Shader*>               m_resShader; // �V�F�[�_�[�N���X�o�b�t�@�}�b�v
  std::vector< Shader*>                                   m_useShader; // �g�p����V�F�[�_�[�̃o�b�t�@
};