#include "Bullet.h"
#include "Tank.h"
#include "Map.h"
#include "Game.h"
#pragma comment(lib,"winmm.lib")

// 获取私有
COORD CBullet::GetCore()
{
	return m_core;
}
int CBullet::GetDir()
{
	return m_dir;
}
int CBullet::GetState()
{
	return m_state;
}
int CBullet::GetWho()
{
	return m_who;
}
// 修改私有
void CBullet::SetCore(COORD core)
{
	m_core = core;
}
void CBullet::SetDir(int dir)
{
	m_dir = dir;
}
void CBullet::SetState(int state)
{
	m_state = state;
}
void CBullet::SetWho(int who)
{
	m_who = who;
}
// 功能函数
CBullet::CBullet()
{
	m_core = { 0,0 };
	m_dir = UP;
	m_state = 不存在;
	m_who = -1;
}
void CBullet::SetBullet(CTank tank)
{
	m_core = tank.GetCore();
	m_dir = tank.GetDir();
	m_who = tank.GetWho();
}
void CBullet::MoveBullet()
{
	// 不存在状态时则不移动
	if (m_state == 不存在) return;

	switch (m_dir)
	{
	case UP:
		m_core.Y--;
		break;
	case DOWN:
		m_core.Y++;
		break;
	case LEFT:
		m_core.X--;
		break;
	case RIGHT:
		m_core.X++;
		break;
	default:
		break;
	}
}
void CBullet::CleanBullet(COORD oldBulCore)
{
	GotoxyAndPrint(oldBulCore.X, oldBulCore.Y, "  ");
}
void CBullet::IsBulMeetOther(CTank& tank, vector<CTank>& myTank, vector<CTank>& enemyTank, CMap& map, CGame& game)
{
	//tank 参数：为了获取奖励值

	//遇边界
	if (m_core.X <= 0 || m_core.X >= MAP_X_WALL / 2 || m_core.Y <= 0 || m_core.Y >= MAP_Y - 1)
	{
		m_state = 不存在;
	}
	//遇石块
	if (map.GetArrMap(m_core.X, m_core.Y) == 石块)
	{
		m_state = 不存在;
	}
	//遇土块（遇草丛、河流则穿过
	if (map.GetArrMap(m_core.X, m_core.Y) == 土块)
	{
		m_state = 不存在;
		map.SetArrMap(m_core.X, m_core.Y, 空地);
	}
	// 若是我方子弹
	if (m_who != 敌方坦克)
	{
		//遇泉水
		if (map.GetArrMap(m_core.X, m_core.Y) == 泉水)
			m_state = 不存在;
		// 遇我方其他
		for (vector<CTank>::iterator it = myTank.begin(); it != myTank.end(); it++)
		{
			// 死坦克不判断
			//if (it->m_isAlive == false) continue;
			// 发出本子弹的坦克不判断（对比body而非core，因为子弹已经向前走了一步；C版本未判断但是正常也许某些实现不一样
			if (m_core.X == it->GetBody(0).X && m_core.Y == it->GetBody(0).Y) continue;
			if (
				(m_core.X == it->GetCore().X) && (m_core.Y == it->GetCore().Y) ||
				(m_core.X == it->GetBody(0).X) && (m_core.Y == it->GetBody(0).Y) ||
				(m_core.X == it->GetBody(1).X) && (m_core.Y == it->GetBody(1).Y) ||
				(m_core.X == it->GetBody(2).X) && (m_core.Y == it->GetBody(2).Y) ||
				(m_core.X == it->GetBody(3).X) && (m_core.Y == it->GetBody(3).Y) ||
				(m_core.X == it->GetBody(4).X) && (m_core.Y == it->GetBody(4).Y)
				)
			{
				m_state = 不存在;
			}
		}
		// 遇敌方坦克及子弹
		for (vector<CTank>::iterator it = enemyTank.begin(); it != enemyTank.end(); it++)
		{
			// 遇坦克
			if (
				(m_core.X == it->GetCore().X) && (m_core.Y == it->GetCore().Y) ||
				(m_core.X == it->GetBody(0).X) && (m_core.Y == it->GetBody(0).Y) ||
				(m_core.X == it->GetBody(1).X) && (m_core.Y == it->GetBody(1).Y) ||
				(m_core.X == it->GetBody(2).X) && (m_core.Y == it->GetBody(2).Y) ||
				(m_core.X == it->GetBody(3).X) && (m_core.Y == it->GetBody(3).Y) ||
				(m_core.X == it->GetBody(4).X) && (m_core.Y == it->GetBody(4).Y)
				)
			{
				PlaySound(TEXT("C:\\Users\\ry1yn\\source\\repos\\15PB\\Tank_cpp\\conf\\duang.wav"), NULL, SND_FILENAME | SND_ASYNC);//播放音效
				m_state = 不存在;
				int blood = it->GetBlood() - 1;
				it->SetBlood(blood);
				//减血后为0则死亡
				if (it->GetBlood() <= 0)
				{
					it->SetIsAlive(false);//死亡
					int killCount = tank.GetKillCount() + 1;
					tank.SetKillCount(killCount);//杀敌数+1
				}
				// 每打死三个奖励一条生命值
				if (tank.GetKillCount() == 3)
				{
					int blood = tank.GetBlood() + 1;
					tank.SetBlood(blood);
					tank.SetKillCount(0);
				}
			}
			// 遇子弹
			if ((m_core.X == it->m_bullet.GetCore().X) && (m_core.Y == it->m_bullet.GetCore().Y))
			{
				// 设置子弹状态
				m_state = 不存在;
				it->m_bullet.SetState(不存在);
				// 抹除子弹
				GotoxyAndPrint(m_core.X, m_core.Y, " ");
				GotoxyAndPrint(it->m_bullet.GetCore().X, it->m_bullet.GetCore().Y, " ");
				// 破坏相等的条件(设其坐标为界面某空白处，若设负数，因线程时间不一样，可能会出现在(0,0）
				m_core = { MAP_X_WALL / 2 + 1, MAP_Y / 2 + 1 };
				COORD coor = { MAP_X_WALL / 2 + 1, MAP_Y / 2 + 2 };
				it->m_bullet.SetCore(coor);
			}
		}
	}
	// 若是敌军子弹
	else if (m_who == 敌方坦克)
	{
		//遇泉水
		if (map.GetArrMap(m_core.X, m_core.Y) == 泉水)
		{
			m_state = 不存在;
			game.SetIsOver(true);
			//myTank[0].m_blood = 0;//泉水打到，我方坦克当场去世
			//myTank[1].m_blood = 0;//泉水打到，我方坦克当场去世
		}
		//遇到我方坦克及子弹
		for (vector<CTank>::iterator it = myTank.begin(); it != myTank.end(); it++)
		{
			// 遇到坦克
			if (
				(m_core.X == it->GetCore().X) && (m_core.Y == it->GetCore().Y) ||
				(m_core.X == it->GetBody(0).X) && (m_core.Y == it->GetBody(0).Y) ||
				(m_core.X == it->GetBody(1).X) && (m_core.Y == it->GetBody(1).Y) ||
				(m_core.X == it->GetBody(2).X) && (m_core.Y == it->GetBody(2).Y) ||
				(m_core.X == it->GetBody(3).X) && (m_core.Y == it->GetBody(3).Y) ||
				(m_core.X == it->GetBody(4).X) && (m_core.Y == it->GetBody(4).Y)
				)
			{
				PlaySound(TEXT("conf/duang.wav"), NULL, SND_FILENAME | SND_ASYNC);//播放音效
				m_state = 不存在;
				// 坦克在隐藏状态被打到不掉血
				if (!it->GetIsHidden())
				{
					if (tank.GetPower() == 1)
					{
						int blood = it->GetBlood() - 1;
						it->SetBlood(blood);
					}

					else if (tank.GetPower() == 2)
					{
						int blood = it->GetBlood() - 2;
						it->SetBlood(blood);
					}

				}
				//如果减血后为0
				if (it->GetBlood() <= 0)
				{
					it->SetIsAlive(false);//声明为死亡
				}

			}

			// 遇子弹
			if ((m_core.X == it->m_bullet.GetCore().X) && (m_core.Y == it->m_bullet.GetCore().Y))
			{
				// 设置子弹状态
				m_state = 不存在;
				it->m_bullet.SetState(不存在);
				// 抹除子弹
				GotoxyAndPrint(m_core.X, m_core.Y, " ");// 抹除其子弹
				GotoxyAndPrint(it->m_bullet.GetCore().X, it->m_bullet.GetCore().Y, " ");// 抹除其子弹
				// 破坏相等的条件(设其坐标为界面某空白处，若设负数，因线程时间不一样，可能会出现在(0,0）
				m_core = { MAP_X_WALL / 2 + 1, MAP_Y / 2 - 1 };
				COORD coor = { MAP_X_WALL / 2 + 1, MAP_Y / 2 - 2 };
				it->m_bullet.SetCore(coor);
			}

		}

		//遇其他敌方坦克
		for (vector<CTank>::iterator it = enemyTank.begin(); it != enemyTank.end(); it++)
		{
			// 死坦克不判断
			if (it->GetIsAlive() == false) continue;
			//发出本子弹的坦克不判断
			if (m_core.X == it->GetBody(0).X && m_core.Y == it->GetBody(0).Y) continue;

			if (
				(m_core.X == it->GetCore().X) && (m_core.Y == it->GetCore().Y) ||
				(m_core.X == it->GetBody(0).X) && (m_core.Y == it->GetBody(0).Y) ||
				(m_core.X == it->GetBody(1).X) && (m_core.Y == it->GetBody(1).Y) ||
				(m_core.X == it->GetBody(2).X) && (m_core.Y == it->GetBody(2).Y) ||
				(m_core.X == it->GetBody(3).X) && (m_core.Y == it->GetBody(3).Y) ||
				(m_core.X == it->GetBody(4).X) && (m_core.Y == it->GetBody(4).Y)
				)
			{
				m_state = 不存在;
			}

		}
	}
}
void CBullet::DrawBullet(CMap map)
{
	// 不存在状态时则不画
	if (m_state == 不存在) return;
	//遇边界，更改为其颜色
	if (m_core.X <= 0 || m_core.X >= MAP_X_WALL / 2 || m_core.Y <= 0 || m_core.Y >= MAP_Y - 1)
	{
		GotoxyAndPrint(m_core.X, m_core.Y, "■", 默认颜色);
	}
	//遇石块，更改为其颜色
	else if (map.GetArrMap(m_core.X, m_core.Y) == 石块)
	{
		GotoxyAndPrint(m_core.X, m_core.Y, "■", 石块颜色);
	}
	//遇土块，画为空格（遇草丛、河流则不变
	else if (map.GetArrMap(m_core.X, m_core.Y) == 土块)
	{
		GotoxyAndPrint(m_core.X, m_core.Y, "  ", 默认颜色);
	}
	//碰到泉水，更改为其颜色和形状
	else if (map.GetArrMap(m_core.X, m_core.Y) == 泉水)
	{
		GotoxyAndPrint(m_core.X, m_core.Y, "★", 泉水颜色);
	}
	//一般运动状态
	else
	{
		if (m_who != 敌方坦克)
			GotoxyAndPrint(m_core.X, m_core.Y, "■", 我坦颜色);
		else
			GotoxyAndPrint(m_core.X, m_core.Y, "■", 敌坦颜色);
	}
}