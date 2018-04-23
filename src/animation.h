#pragma once
//=============================================================================
// File : animation.h
// Date : 2018/02/20(火)
// Author: Kazuaki Yanagihara.
//=============================================================================

///////////////////////////////////////////////////////////////////////////////
// インクルードファイル
#include "Vector.h"
#include <vector>
#include <string>


class Animation
{
  
 private:
  std::string name;
  int m_allFrame;
  std::vector<std::vector<DirectX::XMMATRIX>> m_bones;
};