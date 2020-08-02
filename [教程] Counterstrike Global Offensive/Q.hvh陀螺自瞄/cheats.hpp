#pragma once

#include "address.hpp"
#include "memory.hpp"
#include "bsp_parser.hpp"

enum CSWeaponID_t : uint16_t //enum  ItemSchemaIndex_e
{
	WEAPON_NONE = 0,

	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER
};

class cheats
{
private:
	address m_address;			//地址管理器
	memory m_memory;		//内存管理器
	rn::bsp_parser m_bsp;		//地图管理器

	std::string m_path;			//保存CSGO的路径

	DWORD m_client;			//基址
	DWORD m_engine;
	DWORD m_vstdlib;

	DWORD dwLocalPlayer;
	DWORD m_iTeamNum;
	DWORD dwGlowObjectManager;
	DWORD dwEntityList;
	DWORD m_bGunGameImmunity;
	DWORD m_iGlowIndex;
	DWORD dwClientState;
	DWORD dwClientState_Map;
	DWORD dwClientState_MapDirectory;
	DWORD dwGameDir;
	DWORD m_vecOrigin;
	DWORD m_vecViewOffset;
	DWORD m_dwBoneMatrix;
	DWORD m_aimPunchAngle;
	DWORD dwClientState_ViewAngles;
	DWORD m_iHealth;
	DWORD dwForceAttack;
	DWORD m_iItemDefinitionIndex;
	DWORD dwForceAttack2;
	DWORD m_bIsScoped;
	DWORD m_hActiveWeapon;
	DWORD dwForceBackward;
	DWORD dwForceForward;
	DWORD dwForceJump;
	DWORD dwForceLeft;
	DWORD dwForceRight;
	DWORD m_fFlags;
	DWORD m_lifeState;
	DWORD interface_engine_cvar;
	DWORD convar_name_hash_table;
	DWORD m_iFOVStart;

private:
	/* 获取骨骼位置 */
	D3DXVECTOR3 get_bone_pos(DWORD base, int index)
	{
		typedef struct {
			float Matrix[3][4];
		} Matrix3x4_t;

		DWORD addr = m_memory.read<DWORD>(base + m_dwBoneMatrix);
		if (addr)
		{
			Matrix3x4_t result = m_memory.read<Matrix3x4_t>(addr + index * 0x30);

			return{
				result.Matrix[0][3],
				result.Matrix[1][3],
				result.Matrix[2][3]
			};
		}

		return {  };
	}

	/* 获取阵营 */
	int get_team(DWORD base)
	{
		return m_memory.read<int>(base + m_iTeamNum);
	}

	/* 获取买枪状态 */
	bool get_immunity(DWORD base)
	{
		return m_memory.read<bool>(base + m_bGunGameImmunity);
	}

	/* 获取开镜状态 */
	bool get_scoped(DWORD base)
	{
		return m_memory.read<bool>(base + m_bIsScoped);
	}

	/* 获取开镜状态 */
	bool get_two_scoped(DWORD base)
	{
		return 90 != m_memory.read<int>(base + m_iFOVStart);
	}

	/* 获取存活状态 */
	bool get_life_state(DWORD base)
	{
		int health = m_memory.read<int>(base + m_iHealth);
		unsigned char state = m_memory.read<unsigned char>(base + m_lifeState);
		return health && !state;
	}

	/* 获取玩家是否死亡 */
	bool get_player_life(int index)
	{
		DWORD entity = m_memory.read<DWORD>(m_client + dwEntityList + index * 0x10);
		if (entity == false) return false;

		return get_life_state(entity);
	}

	/* 判断当前武器是否能开镜 */
	bool IsIDSniper(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_AWP:
			return true;
		case WEAPON_G3SG1:
			return true;
		case WEAPON_SCAR20:
			return true;
		case WEAPON_SSG08:
			return true;
		default:
			return false;
		}
	}

	/* 判断当前武器是否是小刀 */
	bool IsIDKnife(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_KNIFE:
			return true;
		case WEAPON_KNIFEGG:
			return true;
		case WEAPON_KNIFE_T:
			return true;
		case WEAPON_KNIFE_GUT:
			return true;
		case WEAPON_KNIFE_FLIP:
			return true;
		case WEAPON_BAYONET:
			return true;
		case WEAPON_KNIFE_M9_BAYONET:
			return true;
		case WEAPON_KNIFE_KARAMBIT:
			return true;
		case WEAPON_KNIFE_TACTICAL:
			return true;
		case WEAPON_KNIFE_BUTTERFLY:
			return true;
		case WEAPON_KNIFE_FALCHION:
			return true;
		case WEAPON_KNIFE_PUSH:
			return true;
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
			return true;
		default:
			return false;
		}
	}

	/* 判断当前武器是否是其它 */
	bool IsIDOther(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_NONE:
			return true;
		case WEAPON_TASER:
			return true;
		case WEAPON_FLASHBANG:
			return true;
		case WEAPON_HEGRENADE:
			return true;
		case WEAPON_SMOKEGRENADE:
			return true;
		case WEAPON_MOLOTOV:
			return true;
		case WEAPON_DECOY:
			return true;
		case WEAPON_INCGRENADE:
			return true;
		case WEAPON_C4:
			return true;
		default:
			return false;
		}
	}

	/* 获取当前武器索引 */
	short get_weapon_index(DWORD base)
	{
		int index = m_memory.read<DWORD>(base + m_hActiveWeapon) & 0xfff;
		base = m_memory.read<DWORD>(m_client + dwEntityList + (index - 1) * 0x10);
		return m_memory.read<short>(base + m_iItemDefinitionIndex);
	}

	/* 计算自瞄角度 */
	D3DXVECTOR3 get_aimbot_angle(D3DXVECTOR3& vecOrigin, D3DXVECTOR3& vecOther)
	{
		D3DXVECTOR3 vecAngles{};
		D3DXVECTOR3 vecDelta = D3DXVECTOR3((vecOrigin[0] - vecOther[0]), (vecOrigin[1] - vecOther[1]), (vecOrigin[2] - vecOther[2]));
		float hyp = sqrtf(vecDelta[0] * vecDelta[0] + vecDelta[1] * vecDelta[1]);

		float  M_PI = 3.14159265358979323846f;
		vecAngles[0] = (float)atan(vecDelta[2] / hyp)		*(float)(180.f / M_PI);
		vecAngles[1] = (float)atan(vecDelta[1] / vecDelta[0])	*(float)(180.f / M_PI);
		vecAngles[2] = (float)0.f;

		if (vecDelta[0] >= 0.f) vecAngles[1] += 180.0f;
		return vecAngles;
	}

	/* 角度归一化 */
	void angle_normalize(D3DXVECTOR3& vAngles)
	{
		for (int i = 0; i < 3; i++)
		{
			if (vAngles[i] < -180.0f) vAngles[i] += 360.0f;
			if (vAngles[i] > 180.0f) vAngles[i] -= 360.0f;
		}

		if (vAngles.x < -89.0f) vAngles.x = 89.0f;
		if (vAngles.x > 89.0f) vAngles.x = 89.0f;

		vAngles.z = 0;
	}

	/*  */
	void clamp_angles(D3DXVECTOR3& vAngles)
	{
		while (vAngles.y < -180.0f) vAngles.y += 360.0f;
		while (vAngles.y > 180.0f) vAngles.y -= 360.0f;

		if (vAngles.x < -89.0f) vAngles.x = 89.0f;
		if (vAngles.x > 89.0f) vAngles.x = 89.0f;

		vAngles.z = 0;
	}

	/* 获取字符串hash */
	int get_string_hash(const char* name)
	{
		struct help { int code[256]{ 0 }; };
		help buf = m_memory.read<help>(m_vstdlib + convar_name_hash_table);

		int v2 = 0, v3 = 0, size = strlen(name);
		for (int i = 0; i < size; i += 2)
		{
			v3 = buf.code[v2 ^ toupper(name[i])];
			if (i + 1 == size) break;
			v2 = buf.code[v3 ^ toupper(name[i + 1])];
		}
		return v2 | (v3 << 8);
	}

	/* 获取虚表的地址 */
	DWORD get_convar_address(const char* name)
	{
		int hash = get_string_hash(name);
		DWORD convar_engine = m_memory.read<DWORD>(m_vstdlib + interface_engine_cvar);
		if (convar_engine == 0) return 0;

		DWORD addr = m_memory.read<DWORD>(convar_engine + 0x34);
		if (addr == 0) return 0;

		DWORD point = m_memory.read<DWORD>(addr + ((unsigned char)hash * 4));
		while (point)
		{
			if (m_memory.read<DWORD>(point) == hash)
			{
				DWORD convar_point = m_memory.read<DWORD>(point + 0x4);
				if (convar_point)
				{
					struct help { char value[32]{ 0 }; };
					DWORD temp = m_memory.read<DWORD>(convar_point + 0xC);
					if (temp)
					{
						help buf = m_memory.read<help>(temp);
						if (strcmp(buf.value, name) == 0)
							return convar_point;
					}
				}
			}

			point = m_memory.read<DWORD>(point + 0xC);
		}

		return 0;
	}

	/* 设置虚表数值 */
	void set_convar_value(const char* name, int value)
	{
		DWORD addr = get_convar_address(name);
		if (addr) m_memory.write<int>(addr + 0x30, value ^ addr);
	}

	/* 鼠标左键按下 */
	inline void mouse_left_down()
	{
		m_memory.write<bool>(m_client + dwForceAttack, true);
		Sleep(5);
		m_memory.write<bool>(m_client + dwForceAttack, false);
	}

public:
	/* 获取角度 */
	D3DXVECTOR3 get_angle()
	{
		DWORD addr = m_memory.read<DWORD>(m_engine + dwClientState);
		if (addr)
		{
			return m_memory.read<D3DXVECTOR3>(addr + dwClientState_ViewAngles);
		}
		return {};
	}

	/* 设置角度 */
	void set_angle(D3DXVECTOR3& angle)
	{
		DWORD addr = m_memory.read<DWORD>(m_engine + dwClientState);
		if (addr)
		{
			m_memory.write<D3DXVECTOR3>(addr + dwClientState_ViewAngles, angle);
		}
	}

public:
	cheats() {}
	~cheats() {}

	/* 初始化 */
	bool initialize()
	{
		/* 尝试获取CSGO进程ID */
		if (m_memory.attach("csgo.exe") == false) return false;
		std::cout << "[+] 当前游戏ID : " << m_memory.get_pid() << std::endl;

		/* 尝试获取基址 */
		MODULEENTRY32 client = m_memory.find_module("client.dll");
		MODULEENTRY32 engine = m_memory.find_module("engine.dll");
		MODULEENTRY32 vstdlib = m_memory.find_module("vstdlib.dll");
		if (client.modBaseSize == 0 || engine.modBaseSize == 0 || vstdlib.modBaseSize == 0) return false;
		m_client = (DWORD)client.modBaseAddr;
		m_engine = (DWORD)engine.modBaseAddr;
		m_vstdlib = (DWORD)vstdlib.modBaseAddr;
		std::cout << "[+] client基址 : 0x" << std::hex << m_client << std::endl;
		std::cout << "[+] engine基址 : 0x" << std::hex << m_engine << std::endl;
		std::cout << "[+] vstdlib基址 : 0x" << std::hex << m_vstdlib << std::endl;

		/* 尝试获取地址和偏移 */
		if (m_address.Initialize() == false) return false;
		dwLocalPlayer = m_address.get_address("dwLocalPlayer");
		m_iTeamNum = m_address.get_address("m_iTeamNum");
		dwGlowObjectManager = m_address.get_address("dwGlowObjectManager");
		dwEntityList = m_address.get_address("dwEntityList");
		m_bGunGameImmunity = m_address.get_address("m_bGunGameImmunity");
		m_iGlowIndex = m_address.get_address("m_iGlowIndex");
		dwClientState = m_address.get_address("dwClientState");
		dwClientState_Map = m_address.get_address("dwClientState_Map");
		dwClientState_MapDirectory = m_address.get_address("dwClientState_MapDirectory");
		dwGameDir = m_address.get_address("dwGameDir");
		m_vecOrigin = m_address.get_address("m_vecOrigin");
		m_vecViewOffset = m_address.get_address("m_vecViewOffset");
		m_dwBoneMatrix = m_address.get_address("m_dwBoneMatrix");
		m_aimPunchAngle = m_address.get_address("m_aimPunchAngle");
		dwClientState_ViewAngles = m_address.get_address("dwClientState_ViewAngles");
		m_iHealth = m_address.get_address("m_iHealth");
		dwForceAttack = m_address.get_address("dwForceAttack");
		m_iItemDefinitionIndex = m_address.get_address("m_iItemDefinitionIndex");
		dwForceAttack2 = m_address.get_address("dwForceAttack2");
		m_bIsScoped = m_address.get_address("m_bIsScoped");
		m_hActiveWeapon = m_address.get_address("m_hActiveWeapon");
		dwForceBackward = m_address.get_address("dwForceBackward");
		dwForceForward = m_address.get_address("dwForceForward");
		dwForceJump = m_address.get_address("dwForceJump");
		dwForceLeft = m_address.get_address("dwForceLeft");
		dwForceRight = m_address.get_address("dwForceRight");
		m_fFlags = m_address.get_address("m_fFlags");
		m_lifeState = m_address.get_address("m_lifeState");
		interface_engine_cvar = m_address.get_address("interface_engine_cvar");
		convar_name_hash_table = m_address.get_address("convar_name_hash_table");
		m_iFOVStart = m_address.get_address("m_iFOVStart");

		std::string temp = m_memory.read_string(m_engine + dwGameDir, 150);
		m_path = temp.substr(0, strlen(temp.c_str())) + "\\maps";
		std::cout << "[+] 当前游戏路径 : " << m_path << std::endl;

		return true;
	}

	/* 更新地图 */
	void update_map()
	{
		static std::string last_map = "";
		static bool state = false;

		DWORD addr = m_memory.read<DWORD>(m_engine + dwClientState);
		if (addr)
		{
			std::string temp = m_memory.read_string(addr + dwClientState_Map, 64);
			std::string current_map = temp.substr(0, strlen(temp.c_str())) + ".bsp";
			if (state == false || last_map != current_map)
			{
				last_map = current_map;
				state = m_bsp.load_map(m_path, current_map);
				std::cout << "[+] 当前游戏地图 : " << last_map << " - " << (state == true ? "成功" : "失败") << std::endl;
				Sleep(300);
			}
		}
	}

	/* 人物辉光 */
	void glow()
	{
		/* 获取自己基址 */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return;

		/* 获取自己存活状态 */
		if (get_life_state(local_player) == false) return;

		/* 获取自己阵营 */
		int local_team = get_team(local_player);

		/* 获取自己眼睛位置 */
		D3DXVECTOR3 local_pos = m_memory.read<D3DXVECTOR3>(local_player + m_vecOrigin);
		local_pos += m_memory.read<D3DXVECTOR3>(local_player + m_vecViewOffset);

		/* 获取辉光管理器 */
		DWORD glow_manager = m_memory.read<DWORD>(m_client + dwGlowObjectManager);
		if (glow_manager == 0) return;

		/* 遍历玩家 */
		for (int i = 0; i < 64; i++)
		{
			/* 获取玩家基址 */
			DWORD entity = m_memory.read<DWORD>(m_client + dwEntityList + i * 0x10);
			if (entity == 0 || entity == local_player) continue;

			/* 获取玩家血量 */
			if (get_life_state(entity) == false) continue;

			/* 获取玩家阵营 */
			int entity_team = get_team(entity);
			if (entity_team == local_team) continue;

			/* 获取玩家买枪状态 */
			if (get_immunity(entity)) continue;

			/* 获取玩家辉光索引 */
			int entity_glow = m_memory.read<int>(entity + m_iGlowIndex);

			/* 获取玩家头部骨骼 */
			D3DXVECTOR3 bone = get_bone_pos(entity, 8);

			/* 颜色 */
			float color = 2.0f;

			/* 判断玩家可见状态 */
			if (m_bsp.is_visible(local_pos, bone)) color = 0.0f;

			/* 实现辉光 */
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0x4), 2.0f);
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0x8), color);
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0xc), 0.0f);
			m_memory.write<float>(glow_manager + ((entity_glow * 0x38) + 0x10), 0.5f);

			m_memory.write<bool>(glow_manager + ((entity_glow * 0x38) + 0x24), true);
			m_memory.write<bool>(glow_manager + ((entity_glow * 0x38) + 0x25), false);
		}
	}

	/* 人物自瞄 */
	bool  aimbot()
	{
		/* 获取自己基址 */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return false;

		/* 获取自己存活状态 */
		if (get_life_state(local_player) == false) return false;

		/* 设置一下视角 */
		m_memory.write<DWORD>(local_player + 0x332c, 120);

		/* 获取当前武器索引 */
		short weapon_index = get_weapon_index(local_player);
		if (IsIDKnife(weapon_index) || IsIDOther(weapon_index)) return false;

		/* 获取自己眼睛位置 */
		D3DXVECTOR3 local_pos = m_memory.read<D3DXVECTOR3>(local_player + m_vecOrigin);
		local_pos += m_memory.read<D3DXVECTOR3>(local_player + m_vecViewOffset);

		/* 获取自己阵营 */
		int local_team = get_team(local_player);

		/* 遍历玩家 */
		for (int i = 0; i < 64; i++)
		{
			/* 获取玩家基址 */
			DWORD entity = m_memory.read<DWORD>(m_client + dwEntityList + i * 0x10);
			if (entity == 0 || entity == local_player) continue;

			/* 判断血量 */
			if (get_life_state(entity) == false) continue;

			/* 获取玩家阵营 */
			if (local_team == get_team(entity)) continue;

			/* 获取玩家买枪状态 */
			if (get_immunity(entity)) continue;

			/* 获取玩家头部骨骼 */
			D3DXVECTOR3 bone = get_bone_pos(entity, 8);

			/* 判断是否可见 */
			if (m_bsp.is_visible(local_pos, bone) == false) continue;

			/* 计算自瞄角度 */
			D3DXVECTOR3 angle = get_aimbot_angle(local_pos, bone);

			/* 获取后坐力 */
			D3DXVECTOR3 punch = m_memory.read<D3DXVECTOR3>(local_player + m_aimPunchAngle) * 2;
			if (abs(punch.x) > 1.0f) continue;
			angle -= punch;

			/* 归一化角度 */
			angle_normalize(angle);
			//clamp_angles(angle);

			/* 设置角度 */
			set_angle(angle);

			/* 是否顶置窗口 */
			bool top = FindWindowA(nullptr, "Counter-Strike: Global Offensive") == GetForegroundWindow();

			/* 开镜 */
			if (top && IsIDSniper(weapon_index))
			{
				if (get_scoped(local_player) == false)
				{
					mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
					Sleep(150);//小狙
					//Sleep(400);//大狙
					return false;
				}
				//else if (get_two_scoped(local_player) == false) mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}

			/* 开枪 */
			mouse_left_down();
			return true;
		}

		return false;
	}

	/* 静默自瞄 */
	bool silent()
	{
		static bool last_status = false;
		static D3DXVECTOR3 angle;
		if (last_status == false) angle = get_angle();
		else
		{
			set_angle(angle);
			last_status = false;
		}
		last_status = aimbot();
		return last_status;
	}

	/* 人物连跳 */
	void bunnyhop()
	{
#define FORCE_JUMP_BITMASK (1<<0)

		/* 获取自己基址 */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return;

		/* 获取自己存活状态 */
		if (get_life_state(local_player) == false) return;

		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			int state = m_memory.read<int>(m_client + dwForceJump);
			unsigned char ground = m_memory.read<unsigned char>(local_player + m_fFlags);
			if (ground) state |= FORCE_JUMP_BITMASK;
			else state &= ~FORCE_JUMP_BITMASK;
			m_memory.write<int>(m_client + dwForceJump, state);
		}
	}

	/* 大陀螺 */
	void big_top()
	{
		/* 获取自己基址 */
		DWORD local_player = m_memory.read<DWORD>(m_client + dwLocalPlayer);
		if (local_player == 0) return;

		/* 获取自己存活状态 */
		if (get_life_state(local_player) == false) return;

		/* 获取当前武器索引 */
		short weapon_index = get_weapon_index(local_player);
		if (IsIDKnife(weapon_index) || IsIDOther(weapon_index)) return;

		/* 陀螺前的角度 */
		static D3DXVECTOR3 last_angle{ 0,0,0 };

		if (m_memory.read<bool>(m_client + dwForceForward) == false
			&& m_memory.read<bool>(m_client + dwForceBackward) == false
			&& m_memory.read<bool>(m_client + dwForceLeft) == false
			&& m_memory.read<bool>(m_client + dwForceRight) == false)
		{
			if (last_angle.x == 0.0f &&last_angle.y == 0.0f) last_angle = get_angle();

			D3DXVECTOR3 last_angle{ 89,0,0 };
			last_angle.x = rand() % 90;
			last_angle.y = rand() % 180;
			if (rand() % 2 == 0)
			{
				last_angle.x = -last_angle.x;
				last_angle.y = -last_angle.y;
			}
			set_angle(last_angle);
		}
		else
		{
			if (last_angle.x && last_angle.y)
			{
				set_angle(last_angle);
				last_angle = { 0,0,0 };
			}
		}
	}

	/* 虚表值设置 */
	void handle_convar()
	{
		/* 禁用后期处理 */
		set_convar_value(AY_OBFUSCATE("mat_postprocess_enable"), 0);

		/* 没有天空 */
		set_convar_value(AY_OBFUSCATE("r_3dsky"), 0);

		/* 忽略烟雾 */
		set_convar_value(AY_OBFUSCATE("r_drawparticles"), 0); $$$;

		/* 移除阴影 */
		set_convar_value(AY_OBFUSCATE("cl_csm_enabled"), 0);

		/* 视觉后坐力 */
		set_convar_value(AY_OBFUSCATE("view_recoil_tracking"), 1); $$$;

		/* 后坐力控制 */
		set_convar_value(AY_OBFUSCATE("cl_crosshair_recoil"), 1); $$$;
		set_convar_value(AY_OBFUSCATE("weapon_debug_spread_show"), 3); $$$;

		/* 显示投雷轨迹 */
		set_convar_value(AY_OBFUSCATE("cl_grenadepreview"), 1); $$$;

		set_convar_value(AY_OBFUSCATE("cl_forwardspeed"), 450); $$$;
		set_convar_value(AY_OBFUSCATE("cl_sidespeed"), 450); $$$;
		set_convar_value(AY_OBFUSCATE("cl_backspeed"), 450); $$$;
	}
};