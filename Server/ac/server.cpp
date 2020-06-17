/*
 * echo_selserv_win.c
 * Written by SW. YOON
 */
#include "MainManager.h"

int main(int argc, char **argv)
{
	MainManager::create();
	MainManager::GetInstance()->Run();
	MainManager::GetInstance()->Destroy();


	return 0;
}



