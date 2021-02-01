#include "scrollMaster.h"

namespace ScrollMaster
{
	ScrollHook *g_hook = ScrollHook::getInstance();

	/*
	* This is the main logic block that determines how the different keys should be interpreted.
	* 
	* In this case, we want to capture our SCROLL_ACTION key press and transform it into a middle mouse button click.
	* If the the ISBREAK key is pressed, break out of the scroll functionality and send the scroll key's regular value instead - this
	* is used so that you can still use whatever you chose as the scroll key for regular typing
	*/
	LRESULT CALLBACK SetScrollHook(int nCode, WPARAM wParam, LPARAM lParam)
	{
		g_hook->setKBHookStruct(reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam));
		if (!g_hook->getKBHookStruct())
			return CallNextHookEx(NULL, nCode, wParam, lParam);

		/*
		* The scan codes are constantly looping - we set the break key state to true in the previous iteration ("setIsBreakPressed(true)") to send the scroll key's value,
		* so now we need to turn the brake state back off so we can begin to scroll again
		*/
		if (g_hook->getIsBreakPressed()) {
			g_hook->setIsBreakPressed(false);
			return 1;
		}

		/*
		* This is probably the most important section of the code; since we are capturing every keyboard event, we first need to make sure that we only perform our scroll
		* action when the vkCode (virtual key code) is equal to our SCROLL_ACTION key
		*/
		if (g_hook->getKBHookStruct()->vkCode == SCROLL_ACTION) {
			/*
			* Now that we are sure we are supposed to be scrolling, we need to capture the state of the scroll key and tell Windows how to interpret it; every key has 2 states: up and down.
			* This is where you can let your creativity go wild.  For instance, I want my scroll function to activate on press, but deactivate as soon as I release the key (scroll while holding).  This means that
			* I need send a "middle mouse button click" on the initial key down event, and an additional "left mouse button click" on the key up event to clear the scrolling state.  This could
			* easily be changed to somthing more of a toggle, where you wouldn't need to hold the scroll key each time.
			* 
			* There is also the need to break out of the scroll state if we need to actually send the scroll key's original value.  In this case I use the ~ key to activate the scroll state so it's probably
			* not much of an issue, but if you chose to use the letter 'A', there will most likely be a time when you want to type 'A' without scrolling.  For this reason, we need have a second key dedicated to
			* "breaking" the scroll state temporarily so that we can send the original scroll key's value.
			*/
			switch (wParam) {
			case WM_KEYDOWN:
				/*
				* Ignore repeated key signals - duplicate key presses will exit out of the "middle click"
				* scroll functionality in some apps like web browsers
				*/
				if (g_hook->getIsScrollPressed() == true) {
					break;
				}
				/*
				* Hold the "break" key before pressing SCROLL_ACTION to "free" it from its
				* scroll behavior - this allows you to actually use the key
				*/
				if (ISBREAK) {
					g_hook->setIsBreakPressed(true);
					g_hook->SendCustomInput(INPUT_TYPE::KB);
					break;
				}
				g_hook->setIsScrollPressed(true);
				g_hook->SendCustomInput(INPUT_TYPE::MOUSE);
				break;
			case WM_KEYUP:
				if (g_hook->getIsScrollPressed() == true) {
					g_hook->setIsScrollPressed(false);
					g_hook->SendCustomInput(INPUT_TYPE::MOUSE);
				}
				break;
			default:
				break;
			}

			return 1;
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	/*
	* You probably saw this function a couple times in the main hook function and wondered what we were doing.  Since we are intercepting the keyboard signals, we are
	* basically in "manual" mode and need to tell Windows what to do after we press the scroll key.  Here we are using the "SendInput" function of the Windows API to
	* send either a mouse or keyboard signal, depending on what state our scroll key is in.
	* 
	* Full disclosure, I am not thrilled with the current state of this function and I know it can be cleaned up.  It's very hacky right now, but it works and it's
	* probably in the easiest to understand form that it could possibly be right now so I'll leave it alone.
	*/
	void ScrollHook::SendCustomInput(INPUT_TYPE inputType)
	{
		/*
		* The SendInput function expects an argument of type INPUT, which basically just contains flags that tell Windows
		* what and how we want to send our input.
		*/
		INPUT input[1];

		/*
		* Here we send our mouse input; aka - the middle mouse button scroll click (up and down)
		* 
		* Notice that we clear the middle mouse click by just sending a simple left click (up and down)
		*/
		if (inputType == INPUT_TYPE::MOUSE) {
			/* Send the scroll input */
			input[0].type = INPUT_MOUSE;
			input[0].mi.dx = 0;
			input[0].mi.dy = 0;
			if (gb_scrollPressed)
				input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP;
			else
				input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
			input[0].mi.mouseData = 0;
			input[0].mi.dwExtraInfo = NULL;
			input[0].mi.time = 0;
			SendInput(1, input, sizeof(INPUT));
		/*
		* Here we send our keyboard input, which we use to send the original value of the SCROLL_ACTION key.
		* 
		* First we issue the key up event of the break button.
		* Next we send the SCROLL_ACTION value.
		*/
		} else {
			/* Send the original SCROLL_ACTION value */
			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wScan = 0;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;
			input[0].ki.wVk = SCROLL_BREAK;
			input[0].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
			SendInput(1, input, sizeof(INPUT));

			input[0].type = INPUT_KEYBOARD;
			input[0].ki.wScan = 0;
			input[0].ki.time = 0;
			input[0].ki.dwExtraInfo = 0;
			input[0].ki.wVk = SCROLL_ACTION;
			input[0].ki.dwFlags = KEYEVENTF_UNICODE;
			SendInput(1, input, sizeof(INPUT));

			input[0].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
			SendInput(1, input, sizeof(INPUT));
		}
	}

	ScrollHook *ScrollHook::getInstance()
	{
		if (instance == nullptr)
			instance = new ScrollHook();

		return instance;
	}

	DWORD __stdcall ScrollHook::ScrollThread()
	{
		HHOOK handle = SetWindowsHookEx(WH_KEYBOARD_LL, SetScrollHook, NULL, 0);
		if (!handle) {
			printf("Couldn't hook mouse\n");
			return -1;
		}
		printf("Successfully hooked from main .exe\n");

		MSG message;
		while (GetMessageW(&message, NULL, 0, 0)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}

		UnhookWindowsHookEx(handle);

		return 0;
	}
}