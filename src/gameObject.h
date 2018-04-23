#pragma once
//=============================================================================
// File : gameObject.h
// Date : 2018/02/07(水)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "transform.h"
#include <Windows.h>
#include <vector>
#include <list>
#include <string>
#include "renderer.h"
#include "shaderManager.h"


///////////////////////////////////////////////////////////////////////////////
// 前方宣言
class ViewingFrustum;
class Transform;
class CCamera;


///////////////////////////////////////////////////////////////////////////////
// クラスの定義
class GameObject
{
 public:
  GameObject();
  virtual ~GameObject();

  virtual HRESULT Init(void) { return S_OK; };
  virtual void Update() {};
  virtual void Render(CCamera*) {};
  // 視錐台カリング
  virtual bool ViewingFrustumCulling(ViewingFrustum* frustum) { return true; }

  // ノード関連
  static void UpdateAll(void);
  static void ReleaseAll(void);
  static GameObject* GetRoot(void);
  void AddChild(GameObject* child);
  void SetParent(GameObject*);
  GameObject* GetParent(void) const;
  GameObject* FindChildrenFromTag(const std::string& tag);
  void SetTag(const std::string& tag);

  // 描画可否
  void SetVisible(bool flag);
  bool IsVisible(void) const;

  // 座標系
  Transform* GetTransform(void);

 protected:
  // 自身の削除予約
  void Release(void);
  D3D11Renderer::eLayer      m_layer;         // 描画レイヤー
  ShaderManager::eShaderType m_shaderType;    // シェーダのタイプ

 private:
  // 自身とその子供の削除
  void Delete(void);
  // 親から更新していく
  void RecursiveUpdate(void);

  // ノード構造
  GameObject* m_parent;                       // 自分の親ノード
  std::list<GameObject*> m_children;          // 自分の子ノード群
  static GameObject* m_root;                  // ルートノード
  static std::list<GameObject*> m_releaseList;// 削除要求対象リスト

  Transform m_transform;                      // 座標変換管理
  bool m_visible;                             // 描画フラグ
  std::string m_tag;                          // 検索用タグ
};

