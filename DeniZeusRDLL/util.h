#pragma once
#include <Windows.h>
#include "math.h"
#include "json.h"
#include "memory.h"

struct s_bone {
	byte padding[12];
	float x;
	byte padding2[12];
	float y;
	byte padding3[12];
	float z;

};

class Player 
{
public:
	DWORD base;
	DWORD team;
	Vector origin;
	bool alive;
	bool ignore;
	bool is_spotted;
	bool is_dormant;
	int health;
	Vector head_origin;
	Vector view_origin;

	Vector get_bone(int i) {
		static int m_dwBoneMatrix = netvars["netvars"]["m_dwBoneMatrix"].get<int>();
		Vector bonevec;
		DWORD adrBone = mem.ReadMemory<DWORD>(base + m_dwBoneMatrix);
		s_bone bone = mem.ReadMemory<s_bone>(adrBone + (0x30 * i));
		bonevec.x = bone.x; bonevec.y = bone.y; bonevec.z = bone.z;
		return bonevec;
	}
};

Player players[64];
Player local;