///////////////////////////////////////////////////////////////////////////////
// File : gameObject.h
// Date : 2018/02/07(水)
// Author: Kazuaki Yanagihara.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include "gameObject.h"
#include "renderer.h"
#include <stdio.h>
#include <algorithm>
#include "appManager.h"


///////////////////////////////////////////////////////////////////////////////
// 静的メンバ変
GameObject* GameObject::m_root = new GameObject;
std::list<GameObject*> GameObject::m_releaseList;


///////////////////////////////////////////////////////////////////////////////
// コンストラクタ( デフォルト)
GameObject::GameObject()
{
  m_shaderType = ShaderManager::eShaderTypeRegid;
  m_layer = D3D11Renderer::eLayerSky;
  m_parent = nullptr;
  m_visible = true;
  m_tag = "none";
}


///////////////////////////////////////////////////////////////////////////////
// デストラクタ( デフォルト)
GameObject::~GameObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// 全てのオブジェクトの更新処理
void GameObject::UpdateAll(void)
{
  // 全てのノードのアップデートを実行
  if (!m_root) return;
  m_root->RecursiveUpdate();

  // 削除要求の有った物を削除
  std::for_each(m_releaseList.begin(), m_releaseList.end(),
    [&](GameObject* pChild) { pChild->Delete(); });
  // 削除要求リストのクリア
  m_releaseList.clear();
}


///////////////////////////////////////////////////////////////////////////////
// 親から更新していく
void GameObject::RecursiveUpdate(void)
{
  Update();

  std::for_each(m_children.begin(), m_children.end(),
    [&](GameObject* pChild) { pChild->RecursiveUpdate(); });
}


///////////////////////////////////////////////////////////////////////////////
// 全てのオブジェクトの終了処理
void GameObject::ReleaseAll(void)
{
  std::for_each(m_root->m_children.begin(), m_root->m_children.end(),
    [](GameObject* pChild) { pChild->Delete(); });
}


///////////////////////////////////////////////////////////////////////////////
// ルートノードのゲッター
GameObject* GameObject::GetRoot(void)
{
  return m_root;
}


///////////////////////////////////////////////////////////////////////////////
// 自身と子供の削除
void GameObject::Delete(void)
{
  std::for_each(m_children.begin(), m_children.end(),
    [](GameObject* pChild) { pChild->Delete(); });
  // 描画リストから削除
  AppManager::GetRenderer()->Release(this, m_layer);

  // 親が存在すれば親の子供リストから削除
  if (m_parent) m_parent->m_children.erase(std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this));

  // 自身の削除
  delete this;
}


///////////////////////////////////////////////////////////////////////////////
// 子ノードの追加
void GameObject::AddChild(GameObject* child)
{
  m_children.push_back(child);
  child->SetParent(this);
}



///////////////////////////////////////////////////////////////////////////////
// 親ノードのセット
void GameObject::SetParent(GameObject* parent)
{
  m_parent = parent;
}


///////////////////////////////////////////////////////////////////////////////
// 親ノードのゲット
GameObject* GameObject::GetParent(void) const
{
  return m_parent;
}


///////////////////////////////////////////////////////////////////////////////
// 子からタグのオブジェクトがあるか検索
GameObject* GameObject::FindChildrenFromTag(const std::string& tag)
{
  GameObject* result = nullptr;
  for (auto&& child : m_children) {
    result = child->FindChildrenFromTag(tag);
    // nullptr以外が帰ってきたら最初の呼び出しまで持って帰る
    if (result) return result;
  }
  // タグが一致すれば自身を返す
  if (m_tag == tag) return this;
  return nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// タグの設定
void GameObject::SetTag(const std::string& tag)
{
  m_tag = tag; 
}


///////////////////////////////////////////////////////////////////////////////
// 描画状態の設定
void GameObject::SetVisible(bool flag)
{
  m_visible = flag;
}


///////////////////////////////////////////////////////////////////////////////
// 描画の可否
bool GameObject::IsVisible(void) const
{
  return m_visible;
}


///////////////////////////////////////////////////////////////////////////////
// 座標系情報のゲット
Transform* GameObject::GetTransform(void)
{
  return &m_transform;
}


///////////////////////////////////////////////////////////////////////////////
// 自身を削除リストに追加
void GameObject::Release(void)
{
  m_releaseList.push_back(this);
}