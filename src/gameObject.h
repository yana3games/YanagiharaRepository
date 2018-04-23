#pragma once
//=============================================================================
// File : gameObject.h
// Date : 2018/02/07(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "transform.h"
#include <Windows.h>
#include <vector>
#include <list>
#include <string>
#include "renderer.h"
#include "shaderManager.h"


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class ViewingFrustum;
class Transform;
class CCamera;


///////////////////////////////////////////////////////////////////////////////
// �N���X�̒�`
class GameObject
{
 public:
  GameObject();
  virtual ~GameObject();

  virtual HRESULT Init(void) { return S_OK; };
  virtual void Update() {};
  virtual void Render(CCamera*) {};
  // ������J�����O
  virtual bool ViewingFrustumCulling(ViewingFrustum* frustum) { return true; }

  // �m�[�h�֘A
  static void UpdateAll(void);
  static void ReleaseAll(void);
  static GameObject* GetRoot(void);
  void AddChild(GameObject* child);
  void SetParent(GameObject*);
  GameObject* GetParent(void) const;
  GameObject* FindChildrenFromTag(const std::string& tag);
  void SetTag(const std::string& tag);

  // �`���
  void SetVisible(bool flag);
  bool IsVisible(void) const;

  // ���W�n
  Transform* GetTransform(void);

 protected:
  // ���g�̍폜�\��
  void Release(void);
  D3D11Renderer::eLayer      m_layer;         // �`�惌�C���[
  ShaderManager::eShaderType m_shaderType;    // �V�F�[�_�̃^�C�v

 private:
  // ���g�Ƃ��̎q���̍폜
  void Delete(void);
  // �e����X�V���Ă���
  void RecursiveUpdate(void);

  // �m�[�h�\��
  GameObject* m_parent;                       // �����̐e�m�[�h
  std::list<GameObject*> m_children;          // �����̎q�m�[�h�Q
  static GameObject* m_root;                  // ���[�g�m�[�h
  static std::list<GameObject*> m_releaseList;// �폜�v���Ώۃ��X�g

  Transform m_transform;                      // ���W�ϊ��Ǘ�
  bool m_visible;                             // �`��t���O
  std::string m_tag;                          // �����p�^�O
};

