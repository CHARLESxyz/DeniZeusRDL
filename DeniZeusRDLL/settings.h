#pragma once
#include <Windows.h>

namespace settings 
{
	bool aimbot_enabled = true;
	bool glow_enabled = true;
	bool bhop_enabled = true;

	bool aimbot_friendly_fire = false;
	float aimbot_smoothness = 20.f;
	float aimbot_fov = 10.f;
	int aimbot_aim_mode = 0;
	int aimbot_aim_bone = 8;
	DWORD aimbot_aim_key = 0x5;

	bool glow_ally = false;
	bool glow_enemy = true;
}