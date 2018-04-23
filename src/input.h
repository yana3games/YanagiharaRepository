//=============================================================================
//
// 入力処理 [input.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#include <dinput.h>

#define DIRECTINPUT_VERSION ( 0x0800)

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX		(256)	// キー最大数

//*********************************************************
// 入力クラス
//*********************************************************
class CInput
{
 public:
  CInput();
  virtual ~CInput();

  virtual HRESULT Init(HINSTANCE hInst, HWND hWnd);
  virtual void Update(void) = 0;		// ---> 純粋仮想関数化

 protected:
  static LPDIRECTINPUT8 m_pDInput;	// DirectInputオブジェクト
  LPDIRECTINPUTDEVICE8 m_pDIDevice;	// Deviceオブジェクト(入力に必要)
};

//*********************************************************
// キーボード入力クラス
//*********************************************************
class CInputKeyboard : public CInput
{
 public:
  CInputKeyboard();
  ~CInputKeyboard();
  HRESULT Init(HINSTANCE hInst, HWND hWnd);
  void Update(void) override;

  BOOL GetKeyPress(int nKey);
  BOOL GetKeyTrigger(int nKey);
  BOOL GetKeyRelease(int nKey);
  BOOL GetKeyRepeat(int nKey);
  void FlushKeyTrigger(int nKey);

 private:
  BYTE m_aKeyState[NUM_KEY_MAX];          // キーボードの状態を受け取るワーク
  BYTE m_aKeyStateTrigger[NUM_KEY_MAX];   // トリガーワーク
  BYTE m_aKeyStateRelease[NUM_KEY_MAX];   // リリースワーク
  BYTE m_aKeyStateRepeat[NUM_KEY_MAX];    // リピートワーク
  int  m_aKeyStateRepeatCnt[NUM_KEY_MAX]; // リピートカウンタ
};

//*********************************************************
// マウス入力クラス
//*********************************************************
class CInputMouse : public CInput
{
 public:
  ~CInputMouse() {};
  HRESULT Init(HINSTANCE hInst, HWND hWnd);
  void Update(void) override;

  bool GetLeftClickPress(void);
  bool GetLeftClickTrigger(void);
  bool GetLeftClickRelease(void);
  bool GetRightClickPress(void);
  bool GetRightClickTrigger(void);
  bool GetRightClickRelease(void);
  int GetMouseWheel(void);
  int GetMouseMoveX(void);
  int GetMouseMoveY(void);

 private:
  BYTE m_aMouseLeftClick;           // マウスの左クリック
  BYTE m_aMouseLeftClickTrigger;    // マウスのトリガー情報ワーク
  BYTE m_aMouseLeftClickRelease;    // マウスのリリース情報ワーク
  BYTE m_aMouseRightClick;          // マウスの右クリック
  BYTE m_aMouseRightClickTrigger;   // マウスのトリガー情報ワーク
  BYTE m_aMouseRightClickRelease;   // マウスのリリース情報ワーク
  LONG m_lX;                        // マウスのX軸（カーソル）
  LONG m_lY;                        // マウスのY軸（カーソル）
  LONG m_lZ;                        // マウスのZ軸（ホイール）
};
#endif