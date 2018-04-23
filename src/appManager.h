#pragma once
//=============================================================================
// File : appManager.h
// Date : 2017/12/06(水)
// Author: Kazuaki Yanagihara.
//=============================================================================


///////////////////////////////////////////////////////////////////////////////
//	インクルードファイル
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <iostream>
#include <algorithm>
#include <assert.h>

#include "appConfig.h"
#include "appUtility.h"
#include "Vector.h"


///////////////////////////////////////////////////////////////////////////////
//	前方宣言
class ModeContext;
class CMode;
class D3D11Renderer;
class CInputMouse; 
class CInputKeyboard;
class DirectSound;
class ShaderManager;
class TextureManager;


///////////////////////////////////////////////////////////////////////////////
//	クラスの定義

// アプリのマネージャクラス
class AppManager
{
 public:
  AppManager();
  ~AppManager();
  HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
  void Uninit(void);
  void Update(void);
  void Draw(void);

  static ModeContext*    GetModeContext(void);
  static D3D11Renderer*  GetRenderer(void);
  static CInputMouse*    GetMouse(void);
  static CInputKeyboard* GetKeyboard(void);
  static DirectSound*    GetSound(void);
  static ShaderManager*  GetShaderManager(void);
  static TextureManager* GetTextureManager(void);

 private:
  static ModeContext*      m_modeContext;
  static D3D11Renderer*    m_renderer;
  static CInputMouse*      m_mouse;
  static CInputKeyboard*   m_keyboard;
  static DirectSound*      m_sound;
  static ShaderManager*    m_shaderManager;
  static TextureManager*   m_textureManager;
};
