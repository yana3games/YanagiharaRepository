#pragma once
//=============================================================================
// File : skydome.h
// Date : 2017/12/12(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vector.h"
#include "gameObject.h"


///////////////////////////////////////////////////////////////////////////////
// �O���錾
class Texture;
class CCamera;


// �X�J�C�h�[���N���X
class Skydome : public GameObject
{
public:
  Skydome();
  Skydome(const Skydome&);
  ~Skydome();

  HRESULT Init(void);
  void Update(void);
  void Render(CCamera*);
  static Skydome* Create(float fRadius, float width, float height, const Vector3& pos);

private:
  HRESULT CreateVertexBuffer(void);
  HRESULT CreateIndexBuffer(void);
  struct Vertex
  {
    Vector3 pos;
    Vector2 tex;
  };

  ID3D11Buffer *m_vertexBuffer, *m_indexBuffer; // �o�b�t�@�[
  int m_vertexCnt, m_indexCnt; // �o�b�t�@�[�̗v�f��

  float m_radius;              // ���a
  Vector2 m_numDivision;       // ������

  Texture* m_texture;          // �e�N�X�`��
};