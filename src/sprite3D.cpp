//=============================================================================
// File : sprite3D.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include "appManager.h"
#include "Vector.h"
#include "gameObject.h"
#include "appUtility.h"
#include "renderer.h"
#include "sprite3D.h"
#include "game.h"
#include "camera.h"
#include "Texture.h"
#include "textureManager.h"
#include "shaderManager.h"
#include "shader.h"


///////////////////////////////////////////////////////////////////////////////
//  �R���X�g���N�^
Sprite3D::Sprite3D()
{
  m_layer = D3D11Renderer::eLayerNormalObject;
  m_shaderType = ShaderManager::eShaderTypeRegid;
  m_texture = nullptr;
}


//////////////////////////////////////////////////////////////////////////////
//  �R���X�g���N�^
Sprite3D::Sprite3D(Vector3 pos, Vector3 scl, Vector3 rot)
{
  GetTransform()->SetPosition(pos);
  GetTransform()->SetScale(scl);
  GetTransform()->SetRotation(rot);
  m_layer = D3D11Renderer::eLayerNormalObject;
  m_shaderType = ShaderManager::eShaderTypeRegid;
  m_texture = nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
Sprite3D::~Sprite3D()
{
  if (m_texture) {// �e�N�X�`���̃I�u�W�F�N�g
    delete m_texture;
    m_texture = nullptr;
  }
}


///////////////////////////////////////////////////////////////////////////////
//	����������
HRESULT Sprite3D::Init()
{
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//	�`�揈��
void Sprite3D::Update(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//	�`�揈��
void Sprite3D::Render(CCamera* pCamera)
{
  // �V�F�[�_��ݒ肵�ĕ`��
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  // �v���~�e�B�u�̎�ނ�ݒ�
  pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // �V�F�[�_���̃Z�b�g
  Shader* shader = AppManager::GetShaderManager()->GetShader(m_shaderType);
  shader->SetParameters(GetTransform()->GetWorldMatrix(),
    *pCamera->GetViewMtx(),
    *pCamera->GetProjMtx(),
    m_texture
  );
  
  // �`��
  pDeviceContext->Draw(4, 0);
}

///////////////////////////////////////////////////////////////////////////////
//	�X�v���C�g3D�쐬�N���X
Sprite3D* Sprite3D::Create(Vector3 pos, Vector3 size, Vector3 rot)
{
	Sprite3D *pSprite3D;
	pSprite3D = new Sprite3D(pos, size, rot);
	pSprite3D->Init();
	return pSprite3D;
}


///////////////////////////////////////////////////////////////////////////////
// �e�N�X�`���̃Z�b�g
void Sprite3D::SetTexture(Texture* texture)
{
  m_texture = texture;
}


///////////////////////////////////////////////////////////////////////////////
// �e�N�X�`���̃Z�b�g
void Sprite3D::SetTexture(std::wstring filename)
{
  AppManager::GetTextureManager()->Load(filename);
  m_texture = AppManager::GetTextureManager()->GetResource(filename);
}