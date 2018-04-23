//=============================================================================
// File : sprite3D.cpp
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
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
//  コンストラクタ
Sprite3D::Sprite3D()
{
  m_layer = D3D11Renderer::eLayerNormalObject;
  m_shaderType = ShaderManager::eShaderTypeRegid;
  m_texture = nullptr;
}


//////////////////////////////////////////////////////////////////////////////
//  コンストラクタ
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
// デストラクタ
Sprite3D::~Sprite3D()
{
  if (m_texture) {// テクスチャのオブジェクト
    delete m_texture;
    m_texture = nullptr;
  }
}


///////////////////////////////////////////////////////////////////////////////
//	初期化処理
HRESULT Sprite3D::Init()
{
  return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//	描画処理
void Sprite3D::Update(void)
{
}


///////////////////////////////////////////////////////////////////////////////
//	描画処理
void Sprite3D::Render(CCamera* pCamera)
{
  // シェーダを設定して描画
  ID3D11DeviceContext* pDeviceContext = AppManager::GetRenderer()->GetDeviceContext();

  // プリミティブの種類を設定
  pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  // シェーダ情報のセット
  Shader* shader = AppManager::GetShaderManager()->GetShader(m_shaderType);
  shader->SetParameters(GetTransform()->GetWorldMatrix(),
    *pCamera->GetViewMtx(),
    *pCamera->GetProjMtx(),
    m_texture
  );
  
  // 描画
  pDeviceContext->Draw(4, 0);
}

///////////////////////////////////////////////////////////////////////////////
//	スプライト3D作成クラス
Sprite3D* Sprite3D::Create(Vector3 pos, Vector3 size, Vector3 rot)
{
	Sprite3D *pSprite3D;
	pSprite3D = new Sprite3D(pos, size, rot);
	pSprite3D->Init();
	return pSprite3D;
}


///////////////////////////////////////////////////////////////////////////////
// テクスチャのセット
void Sprite3D::SetTexture(Texture* texture)
{
  m_texture = texture;
}


///////////////////////////////////////////////////////////////////////////////
// テクスチャのセット
void Sprite3D::SetTexture(std::wstring filename)
{
  AppManager::GetTextureManager()->Load(filename);
  m_texture = AppManager::GetTextureManager()->GetResource(filename);
}