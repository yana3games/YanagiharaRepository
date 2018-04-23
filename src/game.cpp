//=============================================================================
// File : game.cpp
// Date : 2017/12/06(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "game.h"
#include "gameObject.h"
#include "renderer.h"
#include "appManager.h"
#include "camera.h"
#include "cameraMan.h"
#include "terrain.h"
#include "skydome.h"
#include "fbxfile.h"
#include "sprite3D.h"
#include "effectManager.h"
#include "textureManager.h"


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^
GameMode::GameMode()
{

}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^
GameMode::~GameMode()
{
  m_pFbxFile->Uninit();

//  CCamera::ReleaseAll();
//
  //	�I�u�W�F�N�g�̔j��
  GameObject::ReleaseAll();
  AppManager::GetRenderer()->ReleaseAll();
}


///////////////////////////////////////////////////////////////////////////////
// ����������
void GameMode::Init(void)
{
//  m_pFbxFile = new FBXFile("./data/MODEL/FbxModel/SecretMaster.fbx");
//  m_pFbxFile = new FBXFile("./data/MODEL/dude/dude1.fbx");
//  m_pFbxFile = new FBXFile("./data/MODEL/can.fbx");
//  m_pFbxFile->Init();
  GameObject* root = GameObject::GetRoot();

  GameObject* sprite;
  sprite = (GameObject*)new Sprite3D(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
  AppManager::GetRenderer()->AddDrawLayerList(D3D11Renderer::eLayerNormalObject, sprite);
  std::wstring filename = L"./data/TEXTURE/skydome/skydome.jpg";
  AppManager::GetTextureManager()->Load(filename);
  Texture* texture = AppManager::GetTextureManager()->GetResource(filename);
  ((Sprite3D*)sprite)->SetTexture(texture);
  root->AddChild(sprite);

  GameObject* terrain = Terrain::Create(2000.0f, 2000.0f, 50,50);
  root->AddChild(terrain);
  AppManager::GetRenderer()->AddDrawLayerList(D3D11Renderer::eLayerNormalObject, terrain);
  
  terrain = Terrain::Create(200.0f, 200.0f, 5, 5);
  terrain->GetTransform()->SetPositionY(200.0f);
  terrain->GetTransform()->SetRotationY(DirectX::XM_PI / 4);
  root->AddChild(terrain);
  AppManager::GetRenderer()->AddDrawLayerList(D3D11Renderer::eLayerNormalObject, terrain);

  GameObject* skydome = Skydome::Create(AppConfig::GetInstance()->GetFar()*0.9f, 18, 18, Vector3(0.0f, 0.0f, 0.0f));
  root->AddChild(skydome);
  AppManager::GetRenderer()->AddDrawLayerList(D3D11Renderer::eLayerSky, skydome);

  GameObject* fbx;
//  fbx = (GameObject*)new FBXFile2("./data/MODEL/dude/dude.fbx");
  fbx = (GameObject*)new FBXFile2("./data/MODEL/kano.fbx");
  ((FBXFile2*)fbx)->Init();
  fbx->GetTransform()->SetPositionY(200.0f);
  fbx->GetTransform()->SetRotationY(DirectX::XM_PI / 4);
  AppManager::GetRenderer()->AddDrawLayerList(D3D11Renderer::eLayerNormalObject, fbx);
  root->AddChild(fbx);

  // �J��������
  CCamera* camera = new CCamera(DirectX::XMVectorSet(0.0, 50.0, -50.0, 0.0), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
  CCameraMan::GetInstance()->CameraSet(camera);

  m_pEffect = new EffectManager();
  m_pEffect->Load(L"./data/EFFECT/thunder.efk");
}


///////////////////////////////////////////////////////////////////////////////
// �X�V����
void GameMode::Update(void)
{
  static int cnt = 0;
  cnt++;
  if (cnt % 300 == 0) m_pEffect->Play(L"./data/EFFECT/thunder.efk", Vector3(0.0f, 0.0f, 0.0f));
  //m_pFbxFile->Update();
  //m_pEffect->Update();
  // �S�I�u�W�F�N�g�̍X�V
}


///////////////////////////////////////////////////////////////////////////////
// �`�揈��
void GameMode::Draw(void)
{
  CCameraMan::GetInstance()->Set();
  //m_pFbxFile->Draw();

  // �S�I�u�W�F�N�g�̕`��

  //m_pEffect->Draw();
}

