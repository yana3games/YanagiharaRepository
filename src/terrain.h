#pragma once
//=============================================================================
// File : terrain.h
// Date : 2017/12/10(��)
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


// �n�`�N���X
class Terrain : public GameObject
{
 public:
  Terrain();
  Terrain(const Terrain&);
  ~Terrain();

  HRESULT Init(void);
  void Update(void);
  void Render(CCamera*);
  static Terrain* Create(float fWidth, float fHeight, int nBlockX, int nBlockY);

 private:
  HRESULT CreateVertexBuffer(void);
  HRESULT CreateIndexBuffer(void);

  void SetNormalAll( Vector3* normal);

  struct Vertex
  {
    Vector3 pos;
    Vector3 nrm;
    Vector2 tex;
  };
  ID3D11Buffer *m_vertexBuffer, *m_indexBuffer; // �o�b�t�@�[
  int m_vertexCnt, m_indexCnt; // �o�b�t�@�[�̗v�f��
  Vector2 m_fieldSize;         // �S�̂̍L��
  Vector2 m_blockSize;         // 1�u���b�N�̍L��
  Vector2 m_numBlock;          // �u���b�N��
  Vector3 m_LTPos;             // �ŏ��̒��_�̈ʒu(����
  Texture* m_texture;          // �e�N�X�`��

};
