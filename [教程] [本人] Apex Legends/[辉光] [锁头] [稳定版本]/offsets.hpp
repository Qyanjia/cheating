#pragma once
#include <Windows.h>

/*
APEX偏移
*/

namespace apex_offsets
{
	static DWORD64 TimeDateStamp = 0x5f6d432f;					// 时间戳
	static DWORD64 CheckSum = 0x1fc1a53;								// 校验和

	static DWORD64 ClientState = 0x1261f80;								// 客户端状态
	static DWORD64 SignonState = 0x1262018;							// 登陆状态
	static DWORD64 LevelName = 0x1262130;							// 游戏版本

	static DWORD64 ViewRender = 0x40d5d98;							// 渲染视图
	static DWORD64 ViewMatrix = 0x1b3bd0;								// 矩阵视图
	static DWORD64 GlobalVars = 0x1261c80;							// 全局变量

	static DWORD64 LocalPlayer = 0x1c5bcc8;							// 本地玩家
	static DWORD64 cl_entitylist = 0x18ad3a8;							// 玩家列表

	static DWORD64 m_ModelName = 0x0030;							// 模型名称
	static DWORD64 m_fFlags = 0x0098;										// 标识
	static DWORD64 m_vecAbsOrigin = 0x014c;							// abs原点
	static DWORD64 m_iTeamNum = 0x0430;								// 团队标识		97的时候是电脑人
	static DWORD64 m_iName = 0x0561;									// 名称标识		125780153691248的时候是玩家
	static DWORD64 m_vecVelocity = 0x0460;								// 速度
	static DWORD64 m_bConstrainBetweenEndpoints = 0x0f18;//骨骼
	static DWORD64 m_localOrigin = 0x0158;								// 本地原点
	static DWORD64 m_localAngles = 0x0164;							// 本地角度
	static DWORD64 m_vecPunchWeapon_Angle = 0x23c8;		// 后坐力

	static DWORD64 m_shieldHealth = 0x0170;							// 护盾值
	static DWORD64 m_shieldHealthMax = 0x0174;					// 最大护盾值
	static DWORD64 m_iHealth = 0x0420;									// 血量
	static DWORD64 m_iMaxHealth = 0x0550;							// 最大血量
	static DWORD64 m_lifeState = 0x0770;									// 存活状态
	static DWORD64 m_latestPrimaryWeapons = 0x1a0c;			// 武器
	static DWORD64 m_iObserverMode = 0x32bc;						// 观察模式
	static DWORD64 m_helmetType = 0x4274;							// 头盔类型
	static DWORD64 m_armorType = 0x4278;								// 护甲类型
	static DWORD64 m_bleedoutState = 0x2610;						// 流血状态
	static DWORD64 m_bleedoutStartTime = 0x2614;					// 流血时间

	static DWORD64 m_highlightFunctionBits = 0x2A8;
	static DWORD64 m_highlight_t1 = m_highlightFunctionBits - 0x46;
	static DWORD64 m_highlight_t2 = m_highlightFunctionBits + 0x1C;
	static DWORD64 m_highlight_enable = m_highlightFunctionBits + 0xA8;
	static DWORD64 m_highlight_wall = m_highlight_enable + 0x10;

	static DWORD64 m_highlightParams = 0x01b8;
	static DWORD64 m_highlightServerFadeStartTimes = 0x0300;
	static DWORD64 m_highlightServerContextID = 0x0348;
	static DWORD64 m_highlightTeamBits = 0x0354;

	static DWORD64 m_weaponOwner = 0x1600;						// 武器
	static DWORD64 m_weaponNameIndex = 0x17b0;				// 武器类型
	static DWORD64 m_flProjectileSpeed = 0x1e0c;					//
	static DWORD64 m_flProjectileScale = 0x1e14;						//
};