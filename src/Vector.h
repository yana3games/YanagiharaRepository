#pragma once
//=============================================================================
// File : Vector.h
// Date : 2017/12/06(êÖ)
// Author: Kazuaki Yanagihara.
//=============================================================================
#include <DirectXMath.h>

class Vector2
{
 public:
  float x, y;

  Vector2() :x(0.0f), y(0.0f) {}

  Vector2(float x, float y) {
    (this->x) = x;
    (this->y) = y;
  };

  Vector2 operator + (const Vector2 src) const {
    Vector2 vec;
    vec.x = (this->x) + src.x;
    vec.y = (this->y) + src.y;
    return vec;
  }

  Vector2 operator - (const Vector2 src) const {
    Vector2 vec;
    vec.x = ((this)->x) - src.x;
    vec.y = ((this)->y) - src.y;
    return vec;
  };

  Vector2 operator * (float scl) const {
    Vector2 vec;
    vec.x = ((this)->x) * scl;
    vec.y = ((this)->y) * scl;
    return vec;
  };

  Vector2 operator / (float scl) const {
    Vector2 vec;
    if (scl == 0.0f) return *this;
    vec.x = ((this)->x) / scl;
    vec.y = ((this)->y) / scl;
    return vec;
  };

  Vector2& operator += (const Vector2 &src) {
    ((this)->x) += src.x;
    ((this)->y) += src.y;
    return *this;
  };

  Vector2& operator -= (const Vector2 &src) {
    ((this)->x) -= src.x;
    ((this)->y) -= src.y;
    return *this;
  };

  Vector2& operator *= (float scl) {
    ((this)->x) *= scl;
    ((this)->y) *= scl;
    return *this;
  };

  Vector2& operator /= (float scl) {
    if (scl == 0.0f) return *this;
    ((this)->x) /= scl;
    ((this)->y) /= scl;
    return *this;
  };

};


class Vector3
{
 public:
  float x, y, z;

  Vector3() :x(0.0f), y(0.0f), z(0.0f) {}

  Vector3(float x, float y, float z) {
    ((this)->x) = x;
    ((this)->y) = y;
    ((this)->z) = z;
  };
  Vector3 operator + (const Vector3 src) const {
    Vector3 vec;
    vec.x = ((this)->x) + src.x;
    vec.y = ((this)->y) + src.y;
    vec.z = ((this)->z) + src.z;
    return vec;
  }

  Vector3 operator - (const Vector3 src) const {
    Vector3 vec;
    vec.x = ((this)->x) - src.x;
    vec.y = ((this)->y) - src.y;
    vec.z = ((this)->z) - src.z;
    return vec;
  }

  Vector3 operator * (float scl) const {
    Vector3 vec;
    vec.x = ((this)->x) * scl;
    vec.y = ((this)->y) * scl;
    vec.z = ((this)->z) * scl;
    return vec;
  };

  Vector3 operator / (float scl) const {
    Vector3 vec;
    if (scl == 0.0f) return *this;
    vec.x = ((this)->x) / scl;
    vec.y = ((this)->y) / scl;
    vec.z = ((this)->z) / scl;
    return vec;
  };

  Vector3& operator += (const Vector3 &src) {
    ((this)->x) += src.x;
    ((this)->y) += src.y;
    ((this)->z) += src.z;
    return *this;
  };

  Vector3& operator -= (const Vector3 &src) {
    ((this)->x) -= src.x;
    ((this)->y) -= src.y;
    ((this)->z) -= src.z;
    return *this;
  };

  Vector3& operator *= (float scl) {
    ((this)->x) *= scl;
    ((this)->y) *= scl;
    ((this)->z) *= scl;
    return *this;
  };

  Vector3& operator /= (float scl) {
    if (scl == 0.0f) return *this;
    ((this)->x) /= scl;
    ((this)->y) /= scl;
    ((this)->z) /= scl;
    return *this;
  };

  static void Normalize(Vector3 *pOut, Vector3 *pV)
  {
    double len;
    double x, y, z;

    x = (double)(pV->x);
    y = (double)(pV->y);
    z = (double)(pV->z);
    len = sqrt(x * x + y * y + z * z);

    if (len < (1e-6)) return;

    len = 1.0 / len;
    x *= len;
    y *= len;
    z *= len;

    pOut->x = (float)x;
    pOut->y = (float)y;
    pOut->z = (float)z;
  }

  static float Dot(Vector3* pV1, Vector3* pV2)
  {
    return ((pV1->x)*(pV2->x) + (pV1->y)*(pV2->y) + (pV1->z)*(pV2->z));
  }

  static void Cross(Vector3 *pOut, Vector3 *pV1, Vector3 *pV2)
  {
    Vector3 vec;
    double x1, y1, z1, x2, y2, z2;

    x1 = (double)(pV1->x);
    y1 = (double)(pV1->y);
    z1 = (double)(pV1->z);
    x2 = (double)(pV2->x);
    y2 = (double)(pV2->y);
    z2 = (double)(pV2->z);

    vec.x = (float)(y1 * z2 - z1 * y2);
    vec.y = (float)(z1 * x2 - x1 * z2);
    vec.z = (float)(x1 * y2 - y1 * x2);
    *pOut = vec;
  }
};


class Vector4
{
 public:
   float x, y, z, w;

   Vector4() :x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

   Vector4(float x, float y, float z) {
     ((this)->x) = x;
     ((this)->y) = y;
     ((this)->z) = z;
     ((this)->w) = w;
   };

   Vector4 operator + (const Vector4 src) const {
     Vector4 vec;
     vec.x = ((this)->x) + src.x;
     vec.y = ((this)->y) + src.y;
     vec.z = ((this)->z) + src.z;
     vec.w = ((this)->w) + src.w;
     return vec;
   }

   Vector4 operator - (const Vector4 src) const {
     Vector4 vec;
     vec.x = ((this)->x) - src.x;
     vec.y = ((this)->y) - src.y;
     vec.z = ((this)->z) - src.z;
     vec.w = ((this)->w) - src.w;
     return vec;
   }

   Vector4 operator * (float scl) const {
     Vector4 vec;
     vec.x = ((this)->x) * scl;
     vec.y = ((this)->y) * scl;
     vec.z = ((this)->z) * scl;
     vec.w = ((this)->w) * scl;
     return vec;
   };

   Vector4 operator / (float scl) const {
     Vector4 vec;
     if (scl == 0.0f) return *this;
     vec.x = ((this)->x) / scl;
     vec.y = ((this)->y) / scl;
     vec.z = ((this)->z) / scl;
     vec.w = ((this)->w) / scl;
     return vec;
   };

   Vector4& operator += (const Vector4 &src) {
     ((this)->x) += src.x;
     ((this)->y) += src.y;
     ((this)->z) += src.z;
     ((this)->w) += src.w;
     return *this;
   };

   Vector4& operator -= (const Vector4 &src) {
     ((this)->x) -= src.x;
     ((this)->y) -= src.y;
     ((this)->z) -= src.z;
     ((this)->w) -= src.w;
     return *this;
   };

   Vector4& operator *= (float scl) {
     ((this)->x) *= scl;
     ((this)->y) *= scl;
     ((this)->z) *= scl;
     ((this)->w) *= scl;
     return *this;
   };

   Vector4& operator /= (float scl) {
     if (scl == 0.0f) return *this;
     ((this)->x) /= scl;
     ((this)->y) /= scl;
     ((this)->z) /= scl;
     ((this)->w) /= scl;
     return *this;
   };
};

inline Vector2 Vec2( const DirectX::XMVECTOR& XM)
{
  Vector2 vec;
  vec.x = DirectX::XMVectorGetX(XM);
  vec.y = DirectX::XMVectorGetY(XM);
  return vec;
}

inline Vector3 Vec3(const DirectX::XMVECTOR& XM)
{
  Vector3 vec;
  vec.x = DirectX::XMVectorGetX(XM);
  vec.y = DirectX::XMVectorGetY(XM);
  vec.z = DirectX::XMVectorGetZ(XM);
  return vec;
}

inline Vector4 Vec4(const DirectX::XMVECTOR& XM)
{
  Vector4 vec;
  vec.x = DirectX::XMVectorGetX(XM);
  vec.y = DirectX::XMVectorGetY(XM);
  vec.z = DirectX::XMVectorGetZ(XM);
  vec.w = DirectX::XMVectorGetW(XM);
  return vec;
}

inline DirectX::XMVECTOR XM(const Vector2& vec2)
{
  return DirectX::XMVectorSet(vec2.x, vec2.y, 0.0f, 1.0f);
}

inline DirectX::XMVECTOR XM(const Vector3& vec3)
{
  return DirectX::XMVectorSet(vec3.x, vec3.y, vec3.z, 1.0f);
}

inline DirectX::XMVECTOR XM(const Vector4& vec4)
{
  return DirectX::XMVectorSet(vec4.x, vec4.y, vec4.z, vec4.w);
}
