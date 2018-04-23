//=============================================================================
//
// ���͏��� [input.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#include <dinput.h>

#define DIRECTINPUT_VERSION ( 0x0800)

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_KEY_MAX		(256)	// �L�[�ő吔

//*********************************************************
// ���̓N���X
//*********************************************************
class CInput
{
 public:
  CInput();
  virtual ~CInput();

  virtual HRESULT Init(HINSTANCE hInst, HWND hWnd);
  virtual void Update(void) = 0;		// ---> �������z�֐���

 protected:
  static LPDIRECTINPUT8 m_pDInput;	// DirectInput�I�u�W�F�N�g
  LPDIRECTINPUTDEVICE8 m_pDIDevice;	// Device�I�u�W�F�N�g(���͂ɕK�v)
};

//*********************************************************
// �L�[�{�[�h���̓N���X
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
  BYTE m_aKeyState[NUM_KEY_MAX];          // �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
  BYTE m_aKeyStateTrigger[NUM_KEY_MAX];   // �g���K�[���[�N
  BYTE m_aKeyStateRelease[NUM_KEY_MAX];   // �����[�X���[�N
  BYTE m_aKeyStateRepeat[NUM_KEY_MAX];    // ���s�[�g���[�N
  int  m_aKeyStateRepeatCnt[NUM_KEY_MAX]; // ���s�[�g�J�E���^
};

//*********************************************************
// �}�E�X���̓N���X
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
  BYTE m_aMouseLeftClick;           // �}�E�X�̍��N���b�N
  BYTE m_aMouseLeftClickTrigger;    // �}�E�X�̃g���K�[��񃏁[�N
  BYTE m_aMouseLeftClickRelease;    // �}�E�X�̃����[�X��񃏁[�N
  BYTE m_aMouseRightClick;          // �}�E�X�̉E�N���b�N
  BYTE m_aMouseRightClickTrigger;   // �}�E�X�̃g���K�[��񃏁[�N
  BYTE m_aMouseRightClickRelease;   // �}�E�X�̃����[�X��񃏁[�N
  LONG m_lX;                        // �}�E�X��X���i�J�[�\���j
  LONG m_lY;                        // �}�E�X��Y���i�J�[�\���j
  LONG m_lZ;                        // �}�E�X��Z���i�z�C�[���j
};
#endif