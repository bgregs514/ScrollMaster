#include <stdio.h>
#include "scrollMaster.h"

using namespace ScrollMaster;
ScrollHook *ScrollHook::instance = nullptr;

int main(int argc, TCHAR *argv[])
{
	/*
	* Setup our singleton class object and call ScrollThread to begin running our program
	*/
	ScrollHook *g_hook = ScrollHook::getInstance();
	g_hook->ScrollThread();

	return 0;
}