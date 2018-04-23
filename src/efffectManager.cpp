//=============================================================================
// File : effectManager.h
// Date : 2018/02/01(木)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "effectManager.h"
#include "appManager.h"
#include "renderer.h"
#include "cameraMan.h"
#include "camera.h"

using namespace DirectX;
using namespace Effekseer;

///////////////////////////////////////////////////////////////////////////////
// コンストラクタ
EffectManager::EffectManager(void)
{
  // 描画管理インスタンスの生成
  m_renderer = ::EffekseerRendererDX11::Renderer::Create(AppManager::GetRenderer()->GetDevice(), AppManager::GetRenderer()->GetDeviceContext(), m_MaxEffects);
  // サウンド管理インスタンスの生成
//  ::EffekseerSound::Sound* sound = ::EffekseerSound::Sound::Create(IXAudio2*, モノラル再生用ボイス数, ステレオ再生用ボイス数);
  // エフェクト管理用インスタンスの生成
  m_manager = Manager::Create(m_MaxEffects);

  // 描画方法を指定します。独自に拡張することもできます。
  m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
  m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
  m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
  m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
  m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

  // テクスチャ画像の読込方法の指定(パッケージ等から読み込む場合拡張する必要があります。)
  m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());

  // サウンド再生用インスタンスの指定
  //m_manager->SetSoundPlayer(m_sound->CreateSoundPlayer());

  // サウンドデータの読込方法の指定(圧縮フォーマット、パッケージ等から読み込む場合拡張する必要があります。)
  //m_manager->SetSoundLoader(m_sound->CreateSoundLoader());

  // 座標系の指定(RHで右手系、LHで左手系)
  m_manager->SetCoordinateSystem(CoordinateSystem::LH);

  // カリングを行う範囲を設定
  m_manager->CreateCullingWorld(1000.0f, 1000.0f, 1000.0f, 5);
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ
EffectManager::~EffectManager(void)
{
  // エフェクトの解放
  AllRelease();
  // エフェクト管理用インスタンスを破棄
  m_manager->Destroy();
  // サウンド用インスタンスを破棄
  m_sound->Destroy();
  // 描画用インスタンスを破棄
  m_renderer->Destroy();
}


///////////////////////////////////////////////////////////////////////////////
// エフェクト読み込み
bool EffectManager::Load(const std::wstring& key)
{
  // エフェクトを検索
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it == m_res.end()) {            // 見つからなかった場合
    Effect* effect = Effect::Create(m_manager, (EFK_CHAR*)key.c_str());
    if (!effect) {
      MessageBox(NULL, "FailedLoadEffect!", "LoadError!", MB_OK);
      return false;
    }
    Res res;
    res.nCnt = 1;
    res.effect = effect;
    m_res[key] = res;
    return true;
  }
  it->second.nCnt += 1;
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// エフェクトの再生
Handle EffectManager::Play(const std::wstring& key, const Vector3& pos)
{
  // エフェクトを検索
  std::unordered_map<std::wstring, Res>::iterator it = m_res.find(key);
  if (it != m_res.end()) {       // 見つかった場合
    Handle handle= m_manager->Play( it->second.effect, pos.x, pos.y, pos.z);
    return handle;
  }
  else return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// エフェクトの開放
bool EffectManager::Release(std::wstring key)
{
  // エフェクトを検索
  std::unordered_map<std::wstring, Res>::iterator itr = m_res.find(key);
  if (itr == m_res.end()) return false;
  if ((itr->second.nCnt--) == 0) {       // 見つかった場合
    ES_SAFE_RELEASE( itr->second.effect);
    m_res.erase(itr);
    return true;
  }
  return true;
}


///////////////////////////////////////////////////////////////////////////////
//	全ての画像の開放
bool EffectManager::AllRelease()
{
  for (auto&& res : m_res) {
    ES_SAFE_RELEASE( res.second.effect);
  }
  m_res.clear();
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// 描画処理
void EffectManager::Draw()
{
  // カリングの計算
  m_manager->CalcCulling(m_renderer->GetCameraProjectionMatrix(), false);
  m_renderer->BeginRendering();
  m_manager->Draw();
  m_renderer->EndRendering();
}


///////////////////////////////////////////////////////////////////////////////
// 更新処理
void EffectManager::Update()
{
  m_renderer->SetProjectionMatrix( XM( *CCameraMan::GetInstance()->GetProjMtx() ) );
  // カメラ行列の更新
  m_renderer->SetCameraMatrix( XM( *CCameraMan::GetInstance()->GetViewMtx() ) );
  // 3Dサウンド用リスナー設定の更新
  //m_sound->SetListener(リスナー位置, 注目点, 上方向ベクトル);
  // 再生中のエフェクトの移動等(::Effekseer::Manager経由で様々なパラメーターが設定できます。)
  //m_manager->AddLocation(handle, ::Effekseer::Vector3D);
  // 全てのエフェクトの更新
  m_manager->Update();
}


///////////////////////////////////////////////////////////////////////////////
// 適当キャスト
Matrix44 EffectManager::XM(XMMATRIX& xm)
{
  Matrix44 matrix;
  for (int i = 0; i < 4; i++)
  {
    matrix.Values[i][0] = XMVectorGetX(xm.r[i]);
    matrix.Values[i][1] = XMVectorGetY(xm.r[i]);
    matrix.Values[i][2] = XMVectorGetZ(xm.r[i]);
    matrix.Values[i][3] = XMVectorGetW(xm.r[i]);
  }
  return matrix;
}

