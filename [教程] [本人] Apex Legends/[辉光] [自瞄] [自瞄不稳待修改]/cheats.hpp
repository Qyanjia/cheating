#pragma once
#pragma warning(disable : 4244)

#include "memory.hpp"
#include "vectors.hpp"

//进程ID
DWORD32 g_pid = 3132;

//进程基址
DWORD64 g_base = 0x00007FF6B0BD0000;

//线程循环
bool g_loop = true;

int m_max = 8000;

// 骨骼
typedef struct Bone
{
	uint8_t pad1[0xCC];
	float x;
	uint8_t pad2[0xC];
	float y;
	uint8_t pad3[0xC];
	float z;
}Bone;

// 预测
struct PredictCtx
{
	Vector StartPos;
	Vector TargetPos;
	Vector TargetVel;
	float BulletSpeed;
	float BulletGravity;

	Vector2D AimAngles;
};

// 实例
class Entity
{
private:
	DWORD64 m_point;	//基址
	DWORD32 m_pid;		//进程ID

public:
	Entity(DWORD32 d, DWORD64 p) : m_point(p), m_pid(d) {}
	~Entity() {}

	//获取位置
	Vector get_position()
	{
		return read<Vector>(m_pid, m_point + 0x14c);
	}

	//是否假人
	bool is_dummy()
	{
		return read<int>(m_pid, m_point + 0x430) == 97;
	}

	//是否玩家
	bool is_player()
	{
		return read<uint64_t>(m_pid, m_point + 0x561) == 125780153691248;
	}

	//是否存活
	bool is_alive()
	{
		return read<int>(m_pid, m_point + 0x770) == 0;
	}

	//团队ID
	int get_team_id()
	{
		return read<int>(m_pid, m_point + 0x430);
	}

	//血量
	int get_health()
	{
		return read<int>(m_pid, m_point + 0x420);
	}

	//获取角度
	Vector get_view_angle()
	{
		return read<Vector>(m_pid, m_point + 0x24A0);
	}

	//设置角度
	void set_view_angle(Vector v)
	{
		write<Vector>(m_pid, m_point + 0x24A0, v);
	}

	//启用辉光
	void enable_glow()
	{
		write<int>(m_pid, m_point + 0x262, 16256);
		write<int>(m_pid, m_point + 0x2c4, 1193322764);
		write<int>(m_pid, m_point + 0x350, 7);
		write<int>(m_pid, m_point + 0x360, 2);
	}

	//禁用辉光
	void disable_glow()
	{
		write<int>(m_pid, m_point + 0x262, 0);
		write<int>(m_pid, m_point + 0x2c4, 0);
		write<int>(m_pid, m_point + 0x350, 2);
		write<int>(m_pid, m_point + 0x360, 5);
	}

	//获取骨骼位置
	Vector get_bone_position(int id)
	{
		Vector position = get_position();
		uintptr_t boneArray = read<uintptr_t>(m_pid, m_point + 0xF18);

		uint32_t boneloc = (id * 0x30);
		Bone bo = read<Bone>(m_pid, boneArray + boneloc);

		Vector bone{};
		bone.x = bo.x + position.x;
		bone.y = bo.y + position.y;
		bone.z = bo.z + position.z;
		return bone;
	}

	//获取摄像机角度
	Vector get_cam_pos()
	{
		return read<Vector>(m_pid, m_point + 0x1E6C);
	}

	//获取相对速度
	Vector get_abs_velocity()
	{
		return read<Vector>(m_pid, m_point + 0x140);
	}

	//获取武器数据
	void get_weapon_data(float& speed, float& scale)
	{
		//获取武器数据
		uint64_t entitylist = g_base + 0x18ad3a8;
		uint64_t wephandle = read<uint64_t>(m_pid, m_point + 0x1A0c) & 0xffff;
		if (wephandle)
		{
			uint64_t wep_entity = read<uint64_t>(m_pid, entitylist + (wephandle << 5));
			if (wep_entity)
			{
				speed = read<float>(m_pid, wep_entity + 0x1E0C);
				scale = read<float>(m_pid, wep_entity + 0x1E14);
			}
		}
	}

	//获取后坐力
	Vector get_recoil()
	{
		return read<Vector>(m_pid, m_point + 0x23C8);
	}

	//
	Vector get_swap_angle()
	{
		return read<Vector>(m_pid, m_point + 0x24A0 - 0x10);
	}
};

//归一化
void normalize_angles(Vector& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

// 获取fov
double get_fov(const Vector& viewAngle, const Vector& aimAngle)
{
	Vector delta = aimAngle - viewAngle;
	normalize_angles(delta);

	return sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f));
}

// 计算角度
Vector calc_angle(const Vector& src, const Vector& dst)
{
	Vector angle;
	Vector delta = Vector((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

	double hyp = sqrt(delta.x*delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / M_PI);
	angle.y = atan(delta.y / delta.x) * (180.0f / M_PI);
	angle.z = 0;
	if (delta.x >= 0.0) angle.y += 180.0f;

	return angle;
}

// 计算Fov
double calculate_fov(Entity& from, Entity& target)
{
	Vector ViewAngles = from.get_view_angle();
	Vector LocalCamera = from.get_cam_pos();
	Vector EntityPosition = target.get_position();
	Vector Angle = calc_angle(LocalCamera, EntityPosition);
	return get_fov(ViewAngles, Angle);
}

// 获取自瞄角度
void get_aimbot_angle(Vector self_location, Vector player_location, Vector& aim_angle)
{
	float x = self_location[0] - player_location[0];
	float y = self_location[1] - player_location[1];
	float z = self_location[2] - player_location[2];

	const float pi = 3.1415f;
	aim_angle[0] = (float)atan(z / sqrt(x * x + y * y)) / pi * 180.f;
	aim_angle[1] = (float)atan(y / x);

	if (x >= 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f - 180.0f;
	else if (x < 0.0f && y >= 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x < 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.0f;
	else if (x >= 0.0f && y < 0.0f) aim_angle[1] = aim_angle[1] / pi * 180.f + 180.0f;
}

//
bool optimal_pitch(const PredictCtx& Ctx, const Vector2D& Dir2D, float* OutPitch)
{
	float Vel = Ctx.BulletSpeed, Grav = Ctx.BulletGravity, DirX = Dir2D.x, DirY = Dir2D.y;
	float Root = Vel * Vel * Vel * Vel - Grav * (Grav * DirX * DirX + 2.f * DirY * Vel * Vel);
	if (Root >= 0.f) { *OutPitch = atanf((Vel * Vel - sqrt(Root)) / (Grav * DirX)); return true; }
	return false;
}

//
bool solve_trajectory(PredictCtx& Ctx, const Vector& ExtrPos, float* TravelTime)
{
	Vector Dir = ExtrPos - Ctx.StartPos;
	Vector2D Dir2D = { sqrtf(Dir.x * Dir.x + Dir.y * Dir.y), Dir.z };

	float CurPitch;
	if (!optimal_pitch(Ctx, Dir2D, &CurPitch))
	{
		return false;
	}

	*TravelTime = Dir2D.x / (cosf(CurPitch) * Ctx.BulletSpeed);
	Ctx.AimAngles.y = atan2f(Dir.y, Dir.x);
	Ctx.AimAngles.x = CurPitch;
	return true;
}

//
Vector extrapolate_pos(const PredictCtx& Ctx, float Time)
{
	return Ctx.TargetPos + (Ctx.TargetVel * Time);
}

// 子弹预测
bool bullet_predict(PredictCtx& Ctx)
{
	float MAX_TIME = 1.f, TIME_STEP = (1.f / 256.f);
	for (float CurrentTime = 0.f; CurrentTime <= MAX_TIME; CurrentTime += TIME_STEP)
	{
		float TravelTime;
		Vector ExtrPos = extrapolate_pos(Ctx, CurrentTime);
		if (!solve_trajectory(Ctx, ExtrPos, &TravelTime))
		{
			return false;
		}

		if (TravelTime < CurrentTime)
		{
			Ctx.AimAngles = { -RAD2DEG(Ctx.AimAngles.x), RAD2DEG(Ctx.AimAngles.y) };
			return true;
		}
	}
	return false;
}

// 计算最好的自瞄角度
bool calculate_best_bone_aim(Entity& from, Entity& target, float& max_fov, Vector& v)
{
	// 计算距离
	Vector EntityPosition = target.get_position();
	Vector LocalPlayerPosition = from.get_position();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);

	// 设置自瞄位置
	int bone = 2;
	if (dist < 500) bone = 5;

	Vector LocalCamera = from.get_cam_pos();
	Vector TargetBonePosition = target.get_bone_position(bone);
	Vector CalculatedAngles{ 0,0,0, };

	//获取武器数据
	float BulletSpeed = 0, BulletGrav = 0;
	from.get_weapon_data(BulletSpeed, BulletGrav);

	//more accurate prediction
	if (BulletSpeed > 1.f)
	{
		PredictCtx Ctx;
		Ctx.StartPos = LocalCamera;
		Ctx.TargetPos = TargetBonePosition;
		Ctx.BulletSpeed = BulletSpeed - (BulletSpeed*0.08);
		Ctx.BulletGravity = BulletGrav + (BulletGrav*0.05);
		Ctx.TargetVel = target.get_abs_velocity();

		//子弹预测
		if (bullet_predict(Ctx))
			CalculatedAngles = Vector{ Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f };
	}

	if (CalculatedAngles == Vector(0, 0, 0))
		CalculatedAngles = calc_angle(LocalCamera, TargetBonePosition);
	Vector ViewAngles = from.get_view_angle();
	Vector SwayAngles = from.get_swap_angle();

	//remove sway and recoil
	CalculatedAngles -= SwayAngles - ViewAngles;

	normalize_angles(CalculatedAngles);
	Vector Delta = CalculatedAngles - ViewAngles;
	double fov = get_fov(SwayAngles, CalculatedAngles);
	if (fov > max_fov) return false;
	else max_fov = fov;

	normalize_angles(Delta);

	v = ViewAngles + Delta / 1.0f;
	return true;
}

// 设置线程关闭
void stop_thread()
{
	g_loop = false;
}

//---------------------------------------------------------------------------------------------

/* 初始化 */
bool initialize()
{
	// 打开句柄
	HANDLE hDriver = open_device();
	if (hDriver == INVALID_HANDLE_VALUE) return false;

	// 获取进程id
	g_pid = get_process_id(L"r5apex.exe");
	if (g_pid == 0) return false;

	return true;
}

/* 人物辉光 */
void glow_players(bool state)
{
	// 获取自己基址
	DWORD64 local_player_ptr = read<DWORD64>(g_pid, g_base + 0x1c5bcc8);
	if (local_player_ptr == 0) return;
	Entity local_player(g_pid, local_player_ptr);

	// 自己死亡
	if (local_player.get_health() <= 0) return;

	// 玩家列表
	uint64_t entitylist = g_base + 0x18ad3a8;

	// 遍历玩家
	for (int i = 0; i < m_max; i++)
	{
		// 获取玩家
		uint64_t centity = read<uint64_t>(g_pid, entitylist + ((uint64_t)i << 5));
		if (centity == 0 || centity == local_player_ptr) continue;

		// 玩家实例
		Entity e(g_pid, centity);
		if ((e.is_dummy() || e.is_player()) && e.get_health() > 0 && local_player.get_team_id() != e.get_team_id())
		{
			// 启用辉光
			if (state) e.enable_glow();
			else e.disable_glow();
		}
	}
}
void _cdecl glow_thread(void*)
{
	while (g_loop)
	{
		glow_players(true);

		Sleep(30);
	}

	glow_players(false);
}

/* 人物自瞄 */
void aim_players()
{
	// 获取自己基址
	DWORD64 local_player_ptr = read<DWORD64>(g_pid, g_base + 0x1c5bcc8);
	if (local_player_ptr == 0) return;
	Entity local_player(g_pid, local_player_ptr);

	// 存活状态
	if (local_player.get_health() <= 0) return;

	// 获取自己位置
	Vector LocalPlayerPosition = local_player.get_position();

	// 玩家列表
	uint64_t entitylist = g_base + 0x18ad3a8;

	double min_fov = 30.0;
	Vector v;
	bool shot = false;

	// 遍历玩家
	for (int i = 0; i < m_max; i++)
	{
		// 获取玩家
		uint64_t centity = read<uint64_t>(g_pid, entitylist + ((uint64_t)i << 5));
		if (centity == 0 || centity == local_player_ptr) continue;

		// 玩家实例
		Entity e(g_pid, centity);
		if ((e.is_player() || e.is_dummy()) && e.get_health() > 0 && local_player.get_team_id() != e.get_team_id())
		{
			// 获取距离
			float distance = LocalPlayerPosition.DistTo(e.get_position());
			if (distance > 2000.0f) continue;

			// 计算fov
			double f = calculate_fov(local_player, e);
			if (f < min_fov)
			{
				min_fov = f;
				shot = true;

				get_aimbot_angle(local_player.get_bone_position(8), e.get_bone_position(8), v);
			}
		}
	}

	// 自瞄
	if (shot)
	{
		v -= local_player.get_recoil();
		local_player.set_view_angle(v);
	}
}
void _cdecl aim_thread(void*)
{
	while (g_loop)
	{
		//bool state = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_SHIFT) & 0x8000);
		//bool state = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_CONTROL) & 0x8000);
		//bool state = GetAsyncKeyState(VK_LBUTTON) & 0x8000;
		bool state = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) || (GetAsyncKeyState(VK_XBUTTON1) & 0x8000);
		if (state) aim_players();

		Sleep(5);
	}
}