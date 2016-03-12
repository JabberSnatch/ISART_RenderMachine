#ifndef __INPUT_HPP__
#define __INPUT_HPP__

#include "iVec2.hpp"

enum EINPUT_TYPE
{
	KEY, 
	MOUSE, 
	GAMEPAD, 
	INPUT_TYPE_COUNT
};

enum EKEY_CODE
{
	KEY_A = 0, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
	KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
	KEY_SPACE, KEY_RETURN, KEY_CTRL, KEY_SHIFT, KEY_ALT, KEY_ESCAPE, KEY_TAB, KEY_HOME, KEY_END, KEY_DELETE, KEY_BACKSPACE,
	KEY_CODE_COUNT
};

enum EMOUSE_BUTTON
{
	MOUSE_LEFT = 0, MOUSE_RIGHT, MOUSE_MIDDLE, 
	MOUSE_BUTTON_COUNT
};

struct InputSnapshot
{
	InputSnapshot()
		:m_MousePosition()
	{
		memset(m_KeyPresses, 0, sizeof(bool) * KEY_CODE_COUNT);
		memset(m_MouseButtons, 0, sizeof(bool) * MOUSE_BUTTON_COUNT);
	}

	InputSnapshot(const InputSnapshot& _other)
		:m_MousePosition(_other.m_MousePosition)
	{
		memcpy(m_KeyPresses, _other.m_KeyPresses, sizeof(bool) * KEY_CODE_COUNT);
		memcpy(m_MouseButtons, _other.m_MouseButtons, sizeof(bool) * MOUSE_BUTTON_COUNT);
	}

	InputSnapshot& operator = (const InputSnapshot& _other)
	{
		memcpy(m_KeyPresses, _other.m_KeyPresses, sizeof(bool) * KEY_CODE_COUNT);
		memcpy(m_MouseButtons, _other.m_MouseButtons, sizeof(bool) * MOUSE_BUTTON_COUNT);
		m_MousePosition = _other.m_MousePosition;
	}

	bool	m_KeyPresses[KEY_CODE_COUNT];
	bool	m_MouseButtons[MOUSE_BUTTON_COUNT];
	iVec2	m_MousePosition;
};

struct InputData
{
	EINPUT_TYPE type;
	
	union
	{
		struct
		{
		} Key;

		struct
		{
		} Mouse;

		struct
		{
		} GamePad;
	};
};


class InputManager
{
public:
	InputManager()
	{
		m_CurrentFrame = new InputSnapshot();
		m_PreviousFrame = new InputSnapshot();
	}
	InputManager(const InputManager&) = delete;
	InputManager(InputManager&&) = delete;
	~InputManager()
	{
		delete m_CurrentFrame;
		delete m_PreviousFrame;
	}

	void SwapSnapshots()
	{
		delete m_PreviousFrame;
		m_PreviousFrame = m_CurrentFrame;
		m_CurrentFrame = new InputSnapshot(*m_PreviousFrame);
	}

	void SetKeyValue(EKEY_CODE _key, bool _value)
	{
		m_CurrentFrame->m_KeyPresses[_key] = _value; 
	}
	void SetMouseButtonValue(EMOUSE_BUTTON _button, bool _value)
	{ 
		m_CurrentFrame->m_MouseButtons[_button] = _value; 
	}
	void SetMousePosition(int _x, int _y)
	{ 
		m_CurrentFrame->m_MousePosition.x = _x;
		m_CurrentFrame->m_MousePosition.y = _y;
	}

	bool IsKeyDown(EKEY_CODE _key) { return m_CurrentFrame->m_KeyPresses[_key]; }
	bool KeyWentDown(EKEY_CODE _key) { return m_CurrentFrame->m_KeyPresses[_key] && !m_PreviousFrame->m_KeyPresses[_key]; }
	bool IsMouseButtonDown(EMOUSE_BUTTON _button) { return m_CurrentFrame->m_MouseButtons[_button]; }
	iVec2 MousePosition() { return m_CurrentFrame->m_MousePosition; }
	iVec2 GetMouseDelta() { return m_CurrentFrame->m_MousePosition - m_PreviousFrame->m_MousePosition; }

	InputManager& operator = (const InputManager&) = delete;
	InputManager& operator = (InputManager&&) = delete;

private:
	InputSnapshot*	m_CurrentFrame;
	InputSnapshot*	m_PreviousFrame;

};


#endif // __INPUT_HPP__