#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <TlHelp32.h>
#include <WinInet.h>
#pragma comment(lib, "Wininet.lib")

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>

enum weapon_id : uint16_t
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

namespace weapon_n
{
	static unsigned int get_waepon_skin(int index)
	{
		unsigned int result = 0;
		enum weapon_id i = (weapon_id)index;
		switch (i)
		{
		case WEAPON_NONE:
			break;
		case WEAPON_DEAGLE:
			result = 527;
			break;
		case WEAPON_ELITE:
			result = 396;
			break;
		case WEAPON_FIVESEVEN:
			result = 660;
			break;
		case WEAPON_GLOCK:
			result = 495;
			break;
		case WEAPON_AK47:
			result = 639;
			break;
		case WEAPON_AUG:
			result = 913;
			break;
		case WEAPON_AWP:
			result = 344;
			break;
		case WEAPON_FAMAS:
			result = 178;
			break;
		case WEAPON_G3SG1:
			result = 465;
			break;
		case WEAPON_GALILAR:
			result = 76;
			break;
		case WEAPON_M249:
			result = 452;
			break;
		case WEAPON_M4A1:
			result = 632;
			break;
		case WEAPON_MAC10:
			result = 947;
			break;
		case WEAPON_P90:
			result = 669;
			break;
		case WEAPON_MP5SD:
			result = 872;
			break;
		case WEAPON_UMP45:
			result = 615;
			break;
		case WEAPON_XM1014:
			result = 348;
			break;
		case WEAPON_BIZON:
			result = 884;
			break;
		case WEAPON_MAG7:
			result = 385;
			break;
		case WEAPON_NEGEV:
			result = 698;
			break;
		case WEAPON_SAWEDOFF:
			result = 638;
			break;
		case WEAPON_TEC9:
			result = 905;
			break;
		case WEAPON_TASER:
			break;
		case WEAPON_HKP2000:
			result = 104;
			break;
		case WEAPON_MP7:
			result = 423;
			break;
		case WEAPON_MP9:
			result = 262;
			break;
		case WEAPON_NOVA:
			result = 158;
			break;
		case WEAPON_P250:
			result = 358;
			break;
		case WEAPON_SHIELD:
			break;
		case WEAPON_SCAR20:
			result = 165;
			break;
		case WEAPON_SG556:
			result = 378;
			break;
		case WEAPON_SSG08:
			result = 319;
			break;
		case WEAPON_KNIFEGG:
			break;
		case WEAPON_KNIFE:
			break;
		case WEAPON_FLASHBANG:
			break;
		case WEAPON_HEGRENADE:
			break;
		case WEAPON_SMOKEGRENADE:
			break;
		case WEAPON_MOLOTOV:
			break;
		case WEAPON_DECOY:
			break;
		case WEAPON_INCGRENADE:
			break;
		case WEAPON_C4:
			break;
		case WEAPON_HEALTHSHOT:
			break;
		case WEAPON_KNIFE_T:
			break;
		case WEAPON_M4A1_SILENCER:
			result = 663;
			break;
		case WEAPON_USP_SILENCER:
			result = 454;
			break;
		case WEAPON_CZ75A:
			result = 268;
			break;
		case WEAPON_REVOLVER:
			result = 683;
			break;
		case WEAPON_TAGRENADE:
			break;
		case WEAPON_FISTS:
			break;
		case WEAPON_BREACHCHARGE:
			break;
		case WEAPON_TABLET:
			break;
		case WEAPON_MELEE:
			break;
		case WEAPON_AXE:
			break;
		case WEAPON_HAMMER:
			break;
		case WEAPON_SPANNER:
			break;
		case WEAPON_KNIFE_GHOST:
			break;
		case WEAPON_FIREBOMB:
			break;
		case WEAPON_DIVERSION:
			break;
		case WEAPON_FRAG_GRENADE:
			break;
		case WEAPON_SNOWBALL:
			break;
		case WEAPON_BUMPMINE:
			break;
		case WEAPON_BAYONET:
			result = 558;
			break;
		case WEAPON_KNIFE_FLIP:
			result = 559;
			break;
		case WEAPON_KNIFE_GUT:
			result = 560;
			break;
		case WEAPON_KNIFE_KARAMBIT:
			result = 561;
			break;
		case WEAPON_KNIFE_M9_BAYONET:
			result = 558;
			break;
		case WEAPON_KNIFE_TACTICAL:
			result = 620;
			break;
		case WEAPON_KNIFE_FALCHION:
			result = 621;
			break;
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
			break;
		case WEAPON_KNIFE_BUTTERFLY:
			break;
		case WEAPON_KNIFE_PUSH:
			break;
		case WEAPON_KNIFE_URSUS:
			break;
		case WEAPON_KNIFE_GYPSY_JACKKNIFE:
			break;
		case WEAPON_KNIFE_STILETTO:
			break;
		case WEAPON_KNIFE_WIDOWMAKER:
			result = 856;
			break;
		default:
			break;
		}
		return result;
	}

	static bool IsIDPistol(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_DEAGLE:
			return true;
		case WEAPON_ELITE:
			return true;
		case WEAPON_FIVESEVEN:
			return true;
		case WEAPON_GLOCK:
			return true;
		case WEAPON_TEC9:
			return true;
		case WEAPON_HKP2000:
			return true;
		case WEAPON_USP_SILENCER:
			return true;
		case WEAPON_P250:
			return true;
		case WEAPON_CZ75A:
			return true;
		case WEAPON_REVOLVER:
			return true;
		default:
			return false;
		}
	}

	static bool IsIDShotgun(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_XM1014:
			return true;
		case WEAPON_MAG7:
			return true;
		case WEAPON_SAWEDOFF:
			return true;
		case WEAPON_NOVA:
			return true;
		default:
			return false;
		}
	}

	static bool IsIDAutomatic(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_MP5SD:
			return true;
		case WEAPON_AK47:
			return true;
		case WEAPON_AUG:
			return true;
		case WEAPON_FAMAS:
			return true;
		case WEAPON_GALILAR:
			return true;
		case WEAPON_M249:
			return true;
		case WEAPON_M4A1:
			return true;
		case WEAPON_M4A1_SILENCER:
			return true;
		case WEAPON_MAC10:
			return true;
		case WEAPON_P90:
			return true;
		case WEAPON_UMP45:
			return true;
		case WEAPON_BIZON:
			return true;
		case WEAPON_NEGEV:
			return true;
		case WEAPON_MP7:
			return true;
		case WEAPON_MP9:
			return true;
		case WEAPON_SG556:
			return true;
		default:
			return false;
		}
	}

	static bool IsIDSniper(int m_iWeaponId)
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

	static bool IsIDKnife(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
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

	static bool IsIDOther(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
			//case WEAPON_NONE:
			//	return true;
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

	static const char *GetWeaponName(int m_iWeaponId)
	{
		switch (m_iWeaponId)
		{
		case WEAPON_MP5SD:
			return "MP7"; break;
		case WEAPON_DEAGLE:
			return "DEAGLE"; break;
		case WEAPON_ELITE:
			return "DUALS"; break;
		case WEAPON_FIVESEVEN:
			return "FIVE SEVEN"; break;
		case WEAPON_GLOCK:
			return "GLOCK"; break;
		case WEAPON_AK47:
			return "AK47"; break;
		case WEAPON_AUG:
			return "AUG"; break;
		case WEAPON_AWP:
			return "AWP"; break;
		case WEAPON_FAMAS:
			return "FAMAS"; break;
		case WEAPON_G3SG1:
			return "G3SG1"; break;
		case WEAPON_GALILAR:
			return "GALIL"; break;
		case WEAPON_M249:
			return "M249"; break;
		case WEAPON_M4A1:
			return "M4A1"; break;
		case WEAPON_MAC10:
			return "MAC10"; break;
		case WEAPON_P90:
			return "P90"; break;
		case WEAPON_UMP45:
			return "UMP45"; break;
		case WEAPON_XM1014:
			return "XM1014"; break;
		case WEAPON_BIZON:
			return "BIZON"; break;
		case WEAPON_MAG7:
			return "MAG7"; break;
		case WEAPON_NEGEV:
			return "NEGEV"; break;
		case WEAPON_SAWEDOFF:
			return "SAWED OFF"; break;
		case WEAPON_TEC9:
			return "TEC9"; break;
		case WEAPON_TASER:
			return "TASER"; break;
		case WEAPON_HKP2000:
			return "P2000"; break;
		case WEAPON_MP7:
			return "MP7"; break;
		case WEAPON_MP9:
			return "MP9"; break;
		case WEAPON_NOVA:
			return "NOVA"; break;
		case WEAPON_P250:
			return "P250"; break;
		case WEAPON_SCAR20:
			return "SCAR20"; break;
		case WEAPON_SG556:
			return "SG553"; break;
		case WEAPON_SSG08:
			return "SCOUT"; break;
		case WEAPON_KNIFEGG:
			return "KNIFE"; break;
		case WEAPON_KNIFE_T:
			return "KNIFE"; break;
		case WEAPON_KNIFE_GUT:
			return "GUT KNIFE"; break;
		case WEAPON_KNIFE_FLIP:
			return "FLIP KNIFE"; break;
		case WEAPON_BAYONET:
			return "BAYONET"; break;
		case WEAPON_KNIFE_KARAMBIT:
			return "KARAMBIT"; break;
		case WEAPON_KNIFE_M9_BAYONET:
			return "M9 BAYONET"; break;
		case WEAPON_KNIFE_BUTTERFLY:
			return "BUTTERFLY"; break;
		case WEAPON_KNIFE_FALCHION:
			return "FALCHION"; break;
		case WEAPON_KNIFE_TACTICAL:
			return "HUNTSMAN"; break;
		case WEAPON_KNIFE_PUSH:
			return "BUTTPLUGS"; break;
		case WEAPON_FLASHBANG:
			return "FLASHBANG"; break;
		case WEAPON_HEGRENADE:
			return "HE GRENADE"; break;
		case WEAPON_SMOKEGRENADE:
			return "SMOKE"; break;
		case WEAPON_MOLOTOV:
			return "MOLOTOV"; break;
		case WEAPON_DECOY:
			return "DECOY"; break;
		case WEAPON_INCGRENADE:
			return "INC GRENADE"; break;
		case WEAPON_M4A1_SILENCER:
			return "M4A1-S"; break;
		case WEAPON_USP_SILENCER:
			return "USP-S"; break;
		case WEAPON_CZ75A:
			return "CZ75"; break;
		case WEAPON_C4:
			return "C4"; break;
		case WEAPON_REVOLVER:
			return "COWBOY GUN"; break;

		default:
			return "INVALID"; break;
		}
		return "INVALID";
	}
}

//错误处理
void error_handle(const char* title = nullptr)
{
	MessageBoxA(nullptr, title, nullptr, MB_OK);
	exit(0);
}

//获取进程ID
unsigned long get_process_id(const char* proc);

//获取指定模块基址
unsigned long get_module_addr(const char* mod, unsigned long pid);

//获取指定网页源码
std::string get_web_code(const char* web);

//获取网页csgo地址
void get_csgo_address_from_web();

//获取指定名称地址
unsigned long get_address(const std::string& buffer, const char* str, int offset = 0);

//开启换肤
void change_skin();
