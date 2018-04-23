///////////////////////////////////////////////////////////////////////////////
// File : gameObject.h
// Date : 2018/02/07(��)
// Author: Kazuaki Yanagihara.
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//	�C���N���[�h�t�@�C��
#include "gameObject.h"
#include "renderer.h"
#include <stdio.h>
#include <algorithm>
#include "appManager.h"


///////////////////////////////////////////////////////////////////////////////
// �ÓI�����o��
GameObject* GameObject::m_root = new GameObject;
std::list<GameObject*> GameObject::m_releaseList;


///////////////////////////////////////////////////////////////////////////////
// �R���X�g���N�^( �f�t�H���g)
GameObject::GameObject()
{
  m_shaderType = ShaderManager::eShaderTypeRegid;
  m_layer = D3D11Renderer::eLayerSky;
  m_parent = nullptr;
  m_visible = true;
  m_tag = "none";
}


///////////////////////////////////////////////////////////////////////////////
// �f�X�g���N�^( �f�t�H���g)
GameObject::~GameObject()
{
}


///////////////////////////////////////////////////////////////////////////////
// �S�ẴI�u�W�F�N�g�̍X�V����
void GameObject::UpdateAll(void)
{
  // �S�Ẵm�[�h�̃A�b�v�f�[�g�����s
  if (!m_root) return;
  m_root->RecursiveUpdate();

  // �폜�v���̗L���������폜
  std::for_each(m_releaseList.begin(), m_releaseList.end(),
    [&](GameObject* pChild) { pChild->Delete(); });
  // �폜�v�����X�g�̃N���A
  m_releaseList.clear();
}


///////////////////////////////////////////////////////////////////////////////
// �e����X�V���Ă���
void GameObject::RecursiveUpdate(void)
{
  Update();

  std::for_each(m_children.begin(), m_children.end(),
    [&](GameObject* pChild) { pChild->RecursiveUpdate(); });
}


///////////////////////////////////////////////////////////////////////////////
// �S�ẴI�u�W�F�N�g�̏I������
void GameObject::ReleaseAll(void)
{
  std::for_each(m_root->m_children.begin(), m_root->m_children.end(),
    [](GameObject* pChild) { pChild->Delete(); });
}


///////////////////////////////////////////////////////////////////////////////
// ���[�g�m�[�h�̃Q�b�^�[
GameObject* GameObject::GetRoot(void)
{
  return m_root;
}


///////////////////////////////////////////////////////////////////////////////
// ���g�Ǝq���̍폜
void GameObject::Delete(void)
{
  std::for_each(m_children.begin(), m_children.end(),
    [](GameObject* pChild) { pChild->Delete(); });
  // �`�惊�X�g����폜
  AppManager::GetRenderer()->Release(this, m_layer);

  // �e�����݂���ΐe�̎q�����X�g����폜
  if (m_parent) m_parent->m_children.erase(std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this));

  // ���g�̍폜
  delete this;
}


///////////////////////////////////////////////////////////////////////////////
// �q�m�[�h�̒ǉ�
void GameObject::AddChild(GameObject* child)
{
  m_children.push_back(child);
  child->SetParent(this);
}



///////////////////////////////////////////////////////////////////////////////
// �e�m�[�h�̃Z�b�g
void GameObject::SetParent(GameObject* parent)
{
  m_parent = parent;
}


///////////////////////////////////////////////////////////////////////////////
// �e�m�[�h�̃Q�b�g
GameObject* GameObject::GetParent(void) const
{
  return m_parent;
}


///////////////////////////////////////////////////////////////////////////////
// �q����^�O�̃I�u�W�F�N�g�����邩����
GameObject* GameObject::FindChildrenFromTag(const std::string& tag)
{
  GameObject* result = nullptr;
  for (auto&& child : m_children) {
    result = child->FindChildrenFromTag(tag);
    // nullptr�ȊO���A���Ă�����ŏ��̌Ăяo���܂Ŏ����ċA��
    if (result) return result;
  }
  // �^�O����v����Ύ��g��Ԃ�
  if (m_tag == tag) return this;
  return nullptr;
}


///////////////////////////////////////////////////////////////////////////////
// �^�O�̐ݒ�
void GameObject::SetTag(const std::string& tag)
{
  m_tag = tag; 
}


///////////////////////////////////////////////////////////////////////////////
// �`���Ԃ̐ݒ�
void GameObject::SetVisible(bool flag)
{
  m_visible = flag;
}


///////////////////////////////////////////////////////////////////////////////
// �`��̉�
bool GameObject::IsVisible(void) const
{
  return m_visible;
}


///////////////////////////////////////////////////////////////////////////////
// ���W�n���̃Q�b�g
Transform* GameObject::GetTransform(void)
{
  return &m_transform;
}


///////////////////////////////////////////////////////////////////////////////
// ���g���폜���X�g�ɒǉ�
void GameObject::Release(void)
{
  m_releaseList.push_back(this);
}