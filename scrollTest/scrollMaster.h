#pragma once

#include <Windows.h>
#include <stdio.h>

#define ISBREAK		GetAsyncKeyState(SCROLL_BREAK) & 0x8000 // Returns the state of the SCROLL_BREAK key (up or down)

/* This is setup as a singleton class, so some things may look a bit odd at first glance */

namespace ScrollMaster {
	// Pick the scroll and break keys
	/********************************************************/
	const unsigned __int8 SCROLL_ACTION = VK_OEM_3;		// Using '~' - reference this for a list of alternative codes: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes?redirectedfrom=MSDN
	const unsigned __int8 SCROLL_BREAK = VK_CONTROL;	// Using the CTRL key
	/********************************************************/

	enum class INPUT_TYPE {
		MOUSE,
		KB
	};

	class ScrollHook
	{
	private:
		/* Private variables */
		KBDLLHOOKSTRUCT *kbh = nullptr;
		bool gb_scrollPressed = false;
		bool gb_isbreak = false;

		/* Singleton variables */
		static ScrollHook *instance;
		ScrollHook() {};				// This needs to be private so no new objects can be created
	public:
		/* Singleton functions */
		static ScrollHook *getInstance();

		/* Getters and Setters */
		KBDLLHOOKSTRUCT *getKBHookStruct() { return kbh; }
		void setKBHookStruct(KBDLLHOOKSTRUCT *lParam) { kbh = lParam; }
		bool getIsScrollPressed() { return gb_scrollPressed; }
		void setIsScrollPressed(bool isPressed) { gb_scrollPressed = isPressed; }
		bool getIsBreakPressed() { return gb_isbreak; }
		void setIsBreakPressed(bool isPressed) { gb_isbreak = isPressed; }

		DWORD WINAPI ScrollThread();
		void SendCustomInput(INPUT_TYPE inputType);
	};
}

extern ScrollMaster::ScrollHook *g_hook;