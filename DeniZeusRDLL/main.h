#pragma once
#include <io.h>
#include <string>
#include <thread>
#include <future>
#include <tchar.h>
#include <fstream>
#include <sstream>
#include <urlmon.h>
#include <wininet.h>
#include <Windows.h>
#include <experimental/filesystem>


#include "json.h"
#include "math.h"
#include "util.h"
#include "memory.h"
#include "settings.h"
#include "iniparser.h"
#include "consolehook.h"

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "wininet.lib")

namespace filesystem = std::experimental::filesystem;

DWORD pID, dwClient, dwEngine;

DWORD clientState;

bool is_pressed(int i)
{
	bool out = false;
	static HWND csgo_window = FindWindowA(NULL, "Counter-Strike: Global Offensive");
	short key = GetAsyncKeyState(i);

	if ((key & 0x8000 || key & 0x7FFF) && GetForegroundWindow() == csgo_window)
		out = true;

	return out;

}

inline void stopGlow()
{
	int m_flDetectedByEnemySensorTime = netvars["netvars"]["m_bGunGameImmunity"].get<int>() + 52;
	if (local.base)
	{
		for (int i = 1; i < 65; i++)
		{
			if (players[i].ignore || players[i].is_dormant || !players[i].alive)
				continue;

			mem.WriteMemory(players[i].base + m_flDetectedByEnemySensorTime, 0.f);
		}
	}

}

bool stop_glow = false;
inline void entityLoop()
{
	int dwLocalPlayer = netvars["signatures"]["dwLocalPlayer"].get<int>();
	int dwClientState = netvars["signatures"]["dwClientState"].get<int>();
	int dwEntityList = netvars["signatures"]["dwEntityList"].get<int>();
	int m_bDormant = netvars["signatures"]["m_bDormant"].get<int>();
	int m_iHealth = netvars["netvars"]["m_iHealth"].get<int>();
	int m_iTeamNum = netvars["netvars"]["m_iTeamNum"].get<int>();
	int m_vecOrigin = netvars["netvars"]["m_vecOrigin"].get<int>();
	int m_vecViewOffset = netvars["netvars"]["m_vecViewOffset"].get<int>();
	int m_bSpottedByMask = netvars["netvars"]["m_bSpottedByMask"].get<int>();
	while (true)
	{
		local.base = mem.ReadMemory<DWORD>(dwClient + dwLocalPlayer);
		clientState = mem.ReadMemory<DWORD>(dwEngine + dwClientState);
		if (local.base)
		{
			local.health = mem.ReadMemory<int>(local.base + m_iHealth);
			local.team = mem.ReadMemory<DWORD>(local.base + m_iTeamNum);
			local.origin = mem.ReadMemory<Vector>(local.base + m_vecOrigin);
			local.head_origin = local.origin + mem.ReadMemory<Vector>(local.base + m_vecViewOffset);
			local.alive = local.health > 0;

			for (int i = 1; i < 65; i++)
			{
				players[i].base = mem.ReadMemory<DWORD>(dwClient + dwEntityList + ((i) * 0x10));
				players[i].ignore = true;
				if (players[i].base == local.base || !players[i].base)
					continue;
				players[i].ignore = false;
				players[i].team = mem.ReadMemory<DWORD>(players[i].base + m_iTeamNum);
				players[i].is_dormant = mem.ReadMemory<DWORD>(players[i].base + m_bDormant);
				players[i].health = mem.ReadMemory<int>(players[i].base + m_iHealth);
				players[i].alive = players[i].health > 0;
				players[i].is_spotted = ((mem.ReadMemory<int>(players[i].base + m_bSpottedByMask) & (1 << (mem.ReadMemory<int>(local.base + 0x64) - 1))) > 0);
			}
		}

		if (GetAsyncKeyState(VK_DELETE) & 0x8000)
		{
			stop_glow = true;
			stopGlow();
			exit(0);
		}

		if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
			Console::ToggleConsole();
			Sleep(200);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}


inline void glowLoop()
{
	int m_flDetectedByEnemySensorTime = netvars["netvars"]["m_bGunGameImmunity"].get<int>() + 52;
	while (true)
	{
		if (!local.base)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(256));
			continue;
		}

		if (stop_glow)
			continue;

		for (int i = 1; i < 65; i++)
		{
			if (players[i].ignore || players[i].is_dormant || !players[i].alive)
				continue;

			if ((players[i].team != local.team && !settings::glow_enemy) || (players[i].team == local.team && !settings::glow_ally))
				continue;

			mem.WriteMemory(players[i].base + m_flDetectedByEnemySensorTime, 86400.f);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(64));
	}
}

inline void aimbotLoop()
{
	int dwClientState_ViewAngles = netvars["signatures"]["dwClientState_ViewAngles"].get<int>();
	int m_aimPunchAngle = netvars["netvars"]["m_aimPunchAngle"].get<int>();
	while (true)
	{
		if (!local.base)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(128));
			continue;
		}

		if (settings::aimbot_aim_mode == 1)
		{
			if (!is_pressed(0x1))
				continue;
		}
		else if (settings::aimbot_aim_mode == 2)
		{
			if (!is_pressed(0x1) && !is_pressed(settings::aimbot_aim_key))
				continue;
		}

		float lowest_fov = settings::aimbot_fov;
		int lowest_index = 0;
		QAngle lowest_calculated;
		QAngle view_angle = mem.ReadMemory<QAngle>(clientState + dwClientState_ViewAngles);

		for (int i = 1; i < 65; i++)
		{
			if (players[i].ignore || players[i].is_dormant || !players[i].alive)
				continue;

			if ((players[i].team == local.team && !settings::aimbot_friendly_fire))
				continue;

			if (!players[i].is_spotted)
				continue;

			players[i].origin = players[i].get_bone(settings::aimbot_aim_bone);
			QAngle calc_angle = Math::CalcAngle(local.head_origin, players[i].origin);
			float fov = Math::GetFOV(view_angle, calc_angle);
			if (fov < lowest_fov)
			{
				lowest_fov = fov;
				lowest_index = i;
				lowest_calculated = calc_angle;
			}
		}

		if (lowest_index)
		{
			QAngle recoil = mem.ReadMemory<QAngle>(local.base + m_aimPunchAngle) * 2.f;
			QAngle smoothed;
			Math::SmoothAngle(settings::aimbot_smoothness, view_angle, lowest_calculated - recoil, smoothed);
			Math::NormalizeClamp(smoothed);
			mem.WriteMemory(clientState + dwClientState_ViewAngles, smoothed);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

inline void bhopLoop()
{
	int dwForceJump = netvars["signatures"]["dwForceJump"].get<int>();
	int m_fFlags = netvars["netvars"]["m_fFlags"].get<int>();
	while (true) {
		if (!local.base || !local.alive)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(128));
			continue;
		}

		if (is_pressed(VK_SPACE) && mem.ReadMemory<int>(local.base + m_fFlags) & (1 << 0))
		{
			mem.WriteMemory(dwClient + dwForceJump, 6);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(7));
	}
}

void start_cheat()
{
	Console::Attach();
	SetConsoleTitle("DeniZeus External DLL Edition");
	Console::Print(" =================================\n");
	Console::Print(" DeniZeus External DLL Edition\n");
	Console::Print(" Searching CS:GO\n");
	while (!pID)
	{
		pID = mem.GetProcessIdByProcessName((char*)"csgo.exe");
		Sleep(100);
	}
	Console::Print(" CS:GO founded!\n");
	Console::Print(" Searching client_panorama.dll\n");
	while (!dwClient)
	{
		dwClient = mem.GetModuleBaseAddress(pID, (char*)"client_panorama.dll");
		Sleep(100);
	}
	Console::Print(" client_panorama.dll founded!\n");
	Console::Print(" Searching engine.dll\n");
	while (!dwEngine)
	{
		dwEngine = mem.GetModuleBaseAddress(pID, (char*)"engine.dll");
		Sleep(100);
	}
	Console::Print(" engine.dll founded!\n");
	Console::Print(" Getting handle of process...\n");
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "CS:GO handle can't opened!", "Error", 0);
		exit(EXIT_FAILURE);
	}
	mem.hProcess = hProcess;
	Console::Clear();
	Console::Print(" =================================\n");
	Console::Print(" DeniZeus loaded all dependencies!\n");
	Console::Print(" Getting netvars from github/hazedumper!\n");
	DeleteUrlCacheEntry("https://raw.githubusercontent.com/frk1/hazedumper/master/csgo.json");
	filesystem::remove("netvars.json");
	HRESULT hr = URLDownloadToFile(NULL, _T("https://raw.githubusercontent.com/frk1/hazedumper/master/csgo.json"), _T("netvars.json"), 0, NULL);
	if (hr != S_OK) {
		MessageBoxA(NULL, "Error occured on connection to github!", "Error", 0);
		exit(EXIT_FAILURE);
	}
	std::ifstream ifs("netvars.json");

	netvars = json::parse(ifs);

	if (!filesystem::exists("settings.ini"))
	{
		Console::Print(" settings file not founded. creating...\n");
		std::ofstream("settings.ini")
			<< R"#([DEFAULT]
aimbot_enabled = true
glow_enabled = true
bhop_enabled = true

[AIMBOT]
fov = 10.0
smoothness = 20.0
friendly_fire = false
aim_mode = 0 
# Aim modes | 0 = always | 1 = when fire | 2 = when fire or pressed key
aim_bone = 8 
# Aim bones | pelvis = 0 | stomatch = 3 |  spine = 5 | neck = 7 | head = 8
aim_key = 0x5 
# for Aim key list // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes




[GLOW]
glow_ally = false
glow_enemy = true
)#";
	}
	else
	{
		Console::Print(" \n");
		Console::Print(" loading settings...\n");
		inipp::Ini<char> ini;
		std::ifstream is("settings.ini");
		ini.parse(is);
		ini.default_section(ini.sections["DEFAULT"]);
		ini.interpolate();
		inipp::extract(ini.sections["DEFAULT"]["aimbot_enabled"], settings::aimbot_enabled);
		inipp::extract(ini.sections["DEFAULT"]["glow_enabled"], settings::glow_enabled);
		inipp::extract(ini.sections["DEFAULT"]["bhop_enabled"], settings::bhop_enabled);

		inipp::extract(ini.sections["AIMBOT"]["friendly_fire"], settings::aimbot_friendly_fire);
		inipp::extract(ini.sections["AIMBOT"]["smoothness"], settings::aimbot_smoothness);
		inipp::extract(ini.sections["AIMBOT"]["fov"], settings::aimbot_fov);
		inipp::extract(ini.sections["AIMBOT"]["aim_mode"], settings::aimbot_aim_mode);
		inipp::extract(ini.sections["AIMBOT"]["aim_bone"], settings::aimbot_aim_bone);
		unsigned int x;
		std::string input;
		std::stringstream ss;
		inipp::extract(ini.sections["AIMBOT"]["aim_key"], input);
		ss << std::hex << input;
		ss >> x;
		settings::aimbot_aim_key = x;

		inipp::extract(ini.sections["GLOW"]["glow_ally"], settings::glow_ally);
		inipp::extract(ini.sections["GLOW"]["glow_enemy"], settings::glow_enemy);

		Sleep(2000);
	}
	Console::Clear();
	Console::Print("==================================\n");
	Console::Print(" DeniZeus loaded!\n");
	Console::Print(settings::aimbot_enabled ? " Aimbot Enabled       : true\n" : " Aimbot Enabled       : false\n");
	Console::Print(settings::aimbot_friendly_fire ? " Aimbot Friendly Fire : true\n" : " Aimbot Friendly Fire : false\n");
	Console::Print(" Aimbot Smothness     : %g\n", settings::aimbot_smoothness);
	Console::Print(" Aimbot FOV           : %g\n", settings::aimbot_fov);
	Console::Print(" Aimbot Mode          : %i\n", settings::aimbot_aim_mode);
	Console::Print(" Aimbot Bone          : %i\n", settings::aimbot_aim_bone);
	Console::Print(" Aimbot Key           : 0x%x\n", settings::aimbot_aim_key);
	Console::Print(" \n");
	Console::Print(settings::glow_enabled ? " Glow Enabled         : true\n" : " Glow Enabled         : false\n");
	Console::Print(settings::glow_ally ? " Glow Ally            : true\n" : " Glow Ally            : false\n");
	Console::Print(settings::glow_enemy ? " Glow Enemy           : true\n" : " Glow Enemy           : false\n");
	Console::Print(" \n");
	Console::Print(settings::bhop_enabled ? " Bhop Enabled         : true\n" : " Bhop Enabled         : false\n");
	Console::Print(" \n");
	Console::Print(" Close Cheat          : Delete\n");
	Console::Print(" Hide Console         : Insert\n");

	std::future<void> threadGlow;
	std::future<void> threadAimbot;
	std::future<void> threadBhop;
	auto threadEntity = std::async(std::launch::async, entityLoop);


	if (settings::glow_enabled) {
		threadGlow = std::async(std::launch::async, glowLoop);
	}

	if (settings::aimbot_enabled) {
		threadAimbot = std::async(std::launch::async, aimbotLoop);
	}

	if (settings::bhop_enabled) {
		threadBhop = std::async(std::launch::async, bhopLoop);
	}

	threadGlow.get(), threadAimbot.get(), threadBhop.get(), threadEntity.get();

	Console::ReadKey();
}