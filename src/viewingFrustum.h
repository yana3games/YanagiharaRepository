#pragma once
//=============================================================================
// File : viewingFrustum.h
// Date : 2017/12/14(��)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// �C���N���[�h�t�@�C��
#include "Vector.h"


// ������J�����O�N���X
class ViewingFrustum
{
 public:
   ViewingFrustum();
   ViewingFrustum(const ViewingFrustum&);
   ~ViewingFrustum();

   void SetupViewingFrustum(const float& depth, DirectX::XMMATRIX projMtx, DirectX::XMMATRIX viewMtx);
   bool CheckPoint(const Vector3& pos);
   bool CheckCube();
   bool CheckSphere(const Vector3& pos, const float& radius);
   bool CheckRectangle();

 private:
  DirectX::XMVECTOR m_planes[6]; // ����p�̕���
};