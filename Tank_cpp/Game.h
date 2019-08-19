#pragma once
#include "Tank.h"

class CMap;
class CTank;

class CGame
{
public:
	bool g_isRunning = false;
	int g_levelEneTank = 200;
	int g_levelEneBul = 70;

	const char* menuPlay[3] = { "开始游戏","读取游戏","退出游戏" };
	const char* menuWhoMap[3] = { "系统默认","玩家提供","返回上页" };
	const char* menuWhenMap[3] = { "新建地图","已有地图","返回上页" };
	const char* menuLevel[3] = { "简单","一般","困难" };
public:
	void GameInit(CMap& map);				//初始化工作
	//char* ShowGameFile();					//显示存档
	void GameOver(CTank* penemytank);		//善后工作
	//int SelectMenu(int size, int* pindex);  //选择菜单项
	//void SaveGame(PTANK ptank, PTANK penemytank);				//存档
	//void LoadGame(PTANK ptank, PTANK penemytank, char* str);	//读档

	void DrawLogo();						//打印图标
	void DrawGameHelp();					//打印帮助
	void DrawGameInfo(CTank ptank, CTank* penemytank);			//打印游戏信息
	void DrawMenu(const char** menu, int size, int index);		//打印各种菜单
};


