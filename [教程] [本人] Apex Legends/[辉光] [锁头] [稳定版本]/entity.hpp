#pragma once

#include "memory.hpp"
#include "offsets.hpp"

// 矩阵信息
struct matrix_info
{
	union
	{
		float d1[4][4];
		float d2[16];
	}u;
};

// 矩阵信息
struct mat3x4
{
	union
	{
		float d1[3][4];
		float d2[12];
	}u;
};

// 客户端信息
struct client_info
{
	uint64_t pEntity;				// 实例指针
	int64_t SerialNumber;		// 唯一码
	uint64_t pPrev;					// 上一个
	uint64_t pNext;					// 下一个
};

// 客户端类信息
struct client_class_info
{
	uint64_t pCreateFn;
	uint64_t pCreateEventFn;
	uint64_t pNetworkName;	// 名称
	uint64_t pRecvTable;
	uint64_t pNext;
	uint32_t ClassID;
	uint32_t ClassSize;
};

inline float rad2deg(float rad)
{
	return rad * 180.0f / 3.1415927f;
}

inline float deg2rad(float deg)
{
	return deg * 3.1415927f / 180.0f;
}

struct Vec3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;

	static inline float distance(Vec3 lhs, Vec3 rhs)
	{
		Vec3 delta = Vec3{ rhs.x - lhs.x, rhs.y - lhs.y, rhs.z - lhs.z };
		return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
	}

	inline float operator[](int index) const
	{
		if (index == 0) return x;
		else if (index == 1) return y;
		else return z;
	}

	inline float& operator[](int index)
	{
		if (index == 0) return x;
		else if (index == 1) return y;
		else return z;
	}

	inline Vec3 operator+ (Vec3 v) const
	{
		return Vec3{ x + v.x, y + v.y, z + v.z };
	}

	inline Vec3 operator- (Vec3 v) const
	{
		return Vec3{ x - v.x, y - v.y, z - v.z };
	}

	inline Vec3 operator* (float scale) const
	{
		return Vec3{ x * scale, y * scale, z * scale };
	}

	inline Vec3 operator+=(Vec3 v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}

	inline Vec3 operator-=(Vec3 v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}

	inline Vec3 to_angles() const
	{
		float yaw, pitch;
		if (y == 0.0f && x == 0.0f)
		{
			yaw = 0.0f;
			pitch = z > 0.0f ? 270.0f : 90.0f;
		}
		else
		{
			yaw = rad2deg(atan2(y, x));
			const float tmp = sqrt(x * x + y * y);
			pitch = rad2deg(atan2(-z, tmp));
		}
		return Vec3{ pitch, yaw, 0.0f };
	}

	inline Vec3 norm_angles() const
	{
		const float pitch = x < -90.0f ? -90.0f : (x > 90.0f ? 90.0f : x);
		float yaw = y;
		while (yaw <= -180.0f) yaw += 360.0f;
		while (yaw > 180.0f) yaw -= 360.0f;
		return Vec3{ pitch, yaw, 0.0f };
	}

	inline float length() const
	{
		float f = x * x + y * y + z * z;
		return sqrt(f);
	}

	inline float distance(Vec3& v) const
	{
		float a = abs(x - v.x);
		float b = abs(y - v.y);
		float c = abs(z - v.z);
		return sqrt(a * a + b * b + c * c);
	}

	inline bool visable() const
	{
		const float max_f = 0.00001f;
		const float min_f = -0.00001f;
		bool state1 = x > min_f && x < max_f;
		bool state2 = y > min_f && y < max_f;
		bool state3 = z > min_f && z < max_f;
		return !(state1 && state2 && state3);
	}
};

// 骨骼
typedef struct _Bone
{
	uint8_t pad1[0xC];
	float x;
	uint8_t pad2[0xC];
	float y;
	uint8_t pad3[0xC];
	float z;
}Bone;

// 武器索引
enum class Weapon_Index : uint32_t
{
	R301 = 0,
	SENTINEL = 1,
	MELEE_SURVIVAL = 17,
	ALTERNATOR = 59,
	RE45,
	DEVOTION,
	LONGBOW,
	EVA8_AUTO,
	FLATLINE,
	G7_SCOUT,
	HEMLOK,
	KRABER,
	MASTIFF,
	MOZAMBIQUE,
	PROWLER,
	PEACEKEEPER,
	R99,
	P2020,
	SPITFIRE,
	TRIPLE_TAKE,
	WINGMAN,
	HAVOC,
	LSTAR,
	CHARGE_RIFLE,
	VOLT,
};

// 类别
enum class ItemID : uint32_t
{
	// 武器
	KRABER_HEIRLOOM = 1,
	MASTIFF = 2,
	MASTIFF_GOLD = 3,
	LSTAR = 7,
	LSTAR_GOLD = 8,
	HAVOC = 12,
	HAVOC_GOLD = 13,
	DEVOTION = 17,
	DEVOTION_GOLD = 18,
	TRIPLE_TAKE = 22,
	TRIPLE_TAKE_GOLD = 23,
	FLATLINE = 27,
	FLATLINE_GOLD = 28,
	HEMLOK = 32,
	HEMLOK_GOLD = 33,
	G7_SCOUT = 37,
	G7_SCOUT_GOLD = 38,
	ALTERNATOR = 42,
	ALTERNATOR_GOLD = 43,
	R99 = 47,
	R99_GOLD = 48,
	PROWLER_HEIRLOOM = 52,
	VOLT = 57,
	VOLT_GOLD = 58,
	LONGBOW = 62,
	LONGBOW_GOLD = 63,
	CHARGE_RIFLE = 67,
	CHARGE_RIFLE_GOLD = 68,
	SPITFIRE = 72,
	SPITFIRE_GOLD = 73,
	R301 = 77,
	R301_GOLD = 78,
	EVA8_AUTO = 82,
	EVA8_AUTO_GOLD = 83,
	PEACEKEEPER_HEIRLOOM = 87,
	MOZAMBIQUE = 88,
	MOZAMBIQUE_GOLD = 89,
	WINGMAN = 93,
	WINGMAN_GOLD = 94,
	P2020 = 98,
	P2020_GOLD = 99,
	RE45 = 103,
	RE45_GOLD = 104,
	SENTINEL = 108,
	SENTINEL_GOLD = 109,

	// 弹药
	LIGHT_ROUNDS = 113,
	ENERGY_AMMO,
	SHOTGUN_SHELLS,
	HEAVY_ROUNDS,
	SNIPER_AMMO,

	// 治疗
	ULTIMATE_ACCELERANT,
	PHOENIX_KIT,
	MED_KIT,
	SYRINGE,
	SHIELD_BATTERY,
	SHIELD_CELL,

	// 护甲
	HELMET_LV1,
	HELMET_LV2,
	HELMET_LV3,
	HELMET_LV4,
	BODY_ARMOR_LV1,
	BODY_ARMOR_LV2,
	BODY_ARMOR_LV3,
	BODY_ARMOR_LV4,
	EVO_SHIELD_LV0,
	EVO_SHIELD_LV1,
	EVO_SHIELD_LV2,
	EVO_SHIELD_LV3,
	EVO_SHIELD_LV4,
	KNOCKDOWN_SHIELD_LV1,
	KNOCKDOWN_SHIELD_LV2,
	KNOCKDOWN_SHIELD_LV3,
	KNOCKDOWN_SHIELD_LV4,
	BACKPACK_LV1,
	BACKPACK_LV2,
	BACKPACK_LV3,
	BACKPACK_LV4,

	// 手榴弹
	THERMITE_GRENADE,
	FRAG_GRENADE,
	ARC_STAR,

	// 倍镜
	HCOG_CLASSIC,
	HCOG_BRUISER,
	HOLO,
	VARIABLE_HOLO,
	DIGITAL_THREAT,
	HCOG_RANGER,
	VARIABLE_AOG,
	SNIPER,
	VARIABLE_SNIPER,
	DIGITAL_SNIPER_THREAT,

	// 配件
	BARREL_STABILIZER_LV1,
	BARREL_STABILIZER_LV2,
	BARREL_STABILIZER_LV3,
	BARREL_STABILIZER_LV4,
	LIGHT_MAGAZINE_LV1,
	LIGHT_MAGAZINE_LV2,
	LIGHT_MAGAZINE_LV3,
	HEAVY_MAGAZINE_LV1,
	HEAVY_MAGAZINE_LV2,
	HEAVY_MAGAZINE_LV3,
	ENERGY_MAGAZINE_LV1,
	ENERGY_MAGAZINE_LV2,
	ENERGY_MAGAZINE_LV3,
	SNIPER_MAGAZINE_LV1,
	SNIPER_MAGAZINE_LV2,
	SNIPER_MAGAZINE_LV3,
	SHOTGUN_BOLT_LV1,
	SHOTGUN_BOLT_LV2,
	SHOTGUN_BOLT_LV3,
	STANDARD_STOCK_LV1,
	STANDARD_STOCK_LV2,
	STANDARD_STOCK_LV3,
	SNIPER_STOCK_LV1,
	SNIPER_STOCK_LV2,
	SNIPER_STOCK_LV3,

	// 配件
	TURBOCHARGER,
	SKULLPIERCER_RIFLING,
	HAMMERPOINT_ROUNDS,
	DOUBLE_TAP_TRIGGER,
	HOPUP_187,
	QUICKDRAW_HOLSTER,
	VAULT_KEY,
	MOBILE_RESPAWN_BEACON,
	ITEM_191,
	TREASURE_PACK,
};

class entity
{
public:
	rm_driver* m_driver_point;		// 读写指针
	DWORD64 m_base;					// 基址

public:
	entity() : m_driver_point(nullptr), m_base(0) {}
	entity(rm_driver* r, DWORD64 d) : m_driver_point(r), m_base(d) {}
	~entity() {}

	entity& operator=(entity& e)
	{
		m_driver_point = e.m_driver_point;
		m_base = e.m_base;
		return *this;
	}

	/* 有效性判断 */
	bool empty()
	{
		if (m_driver_point == nullptr) return true;
		if (m_base == 0) return true;
		return false;
	}

	/* 设置基址 */
	void update(rm_driver* p, DWORD64 b)
	{
		m_driver_point = p;
		m_base = b;
	}

	/* 获取标识 */
	DWORD32 get_flags()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_fFlags);
	}

	/* 获取距离 */
	Vec3 get_vec_velocity()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_vecVelocity);
	}

	/* 获取原点 */
	Vec3 get_origin()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_localOrigin);
	}

	/* 获取abs原点 */
	Vec3 get_abs_origin()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_vecAbsOrigin);
	}

	/* 摄像机位置 */
	Vec3 get_cam_pos()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_vecCamPos);
	}

	/* 获取交换角度 */
	Vec3 get_sway_angle()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_vecSwayAngle);
	}

	/* 获取角度 */
	Vec3 get_angle()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_vecViewAngle); //apex_offsets::m_localAngles;
	}

	/* 设置角度 */
	void set_angle(Vec3 v)
	{
		return m_driver_point->write<Vec3>(m_base + apex_offsets::m_vecViewAngle, v); //apex_offsets::m_localAngles;
	}

	/* 获取当前血量 */
	DWORD32 get_current_health()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_iHealth);
	}

	/* 获取最大血量 */
	DWORD32 get_max_health()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_iMaxHealth);
	}

	/* 获取团队ID */
	DWORD32 get_team_id()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_iTeamNum);
	}

	/* 获取当前护盾值 */
	DWORD32 get_current_shields()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_shieldHealth);
	}

	/* 获取最大护盾值 */
	DWORD32 get_max_shields()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_shieldHealthMax);
	}

	/* 获取指定骨骼 */
	Vec3 get_bone_position(int id)
	{
		mat3x4 bones[128]{};
		m_driver_point->read_array(m_base + apex_offsets::m_bConstrainBetweenEndpoints, bones, sizeof(mat3x4) * 128);

		const auto& t = bones[id];
		return get_abs_origin() + Vec3{ t.u.d2[3],t.u.d2[7], t.u.d2[11] };
	}

	/* 获取指定骨骼 */
	Vec3 get_bone_position1(int id)
	{
		Vec3 origin = get_abs_origin();
		Bone b = m_driver_point->read<Bone>(m_base + apex_offsets::m_bConstrainBetweenEndpoints + id * 0x30);
		Vec3 res;
		res.x = origin.x + b.x;
		res.y = origin.y + b.y;
		res.z = origin.z + b.z;
		return res;
	}

	/* 辉光人物 */
	void glow_player(bool state)
	{
		if (state)
		{
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_t1, 16256);
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_t2, 1193322764);
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_enable, 7);
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_wall, 2);
		}
		else
		{
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_t1, 0);
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_t2, 0);
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_enable, 2);
			m_driver_point->write<DWORD32>(m_base + apex_offsets::m_highlight_wall, 5);
		}
	}

	/* 获取后坐力 */
	Vec3 get_recoil_angle()
	{
		return m_driver_point->read<Vec3>(m_base + apex_offsets::m_vecPunchWeapon_Angle);
	}

	/* 是否是玩家 */
	bool is_player()
	{
		return m_driver_point->read<DWORD64>(m_base + apex_offsets::m_iName) == 125780153691248;
	}

	/* 是否是电脑玩家 */
	bool is_npc()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_iTeamNum) == 97;
	}

	/* 是否存活 */
	bool is_life()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_lifeState) == 0;
	}

	/* 是否站着 */
	bool is_ground()
	{
		return (get_flags() & 0x1) != 0;
	}

	/* 是否蹲着 */
	bool is_duck()
	{
		return (get_flags() & 0x2) != 0;
	}

	/* 流血趴下状态 */
	bool is_bleed_out()
	{
		return m_driver_point->read<DWORD32>(m_base + apex_offsets::m_bleedoutState) != 0;
	}

	/* 流血时间 */
	float get_bleed_out_time()
	{
		return m_driver_point->read<float>(m_base + apex_offsets::m_bleedoutStartTime);
	}

	//////////////////////////////////////////////////////////////////////////
	/* 辉光物体 */
	void glow_item(bool state)
	{
		if (state)
			m_driver_point->write<DWORD>(m_base + apex_offsets::m_highlightFunctionBits, 1363184265);
		else
			m_driver_point->write<DWORD>(m_base + apex_offsets::m_highlightFunctionBits, 1411417991);
	}

	/* 获取类别索引 */
	ItemID get_item_index()
	{
		return m_driver_point->read<ItemID>(m_base + apex_offsets::m_customScriptInt);
	}

	//////////////////////////////////////////////////////////////////////////
	/* 获取武器名称索引 */
	Weapon_Index get_weapon_name_index()
	{
		return m_driver_point->read<Weapon_Index>(m_base + apex_offsets::m_weaponNameIndex);
	}
};