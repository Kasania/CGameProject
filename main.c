#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <math.h>
#pragma comment(lib,"winmm.lib") 
//---------------------Define------------------------------------//

#define false 0
#define true 1
#define PI 3.14159265358979323846
#define Root2 1.41421356237309504880

#define Key_Status_NonPress 0
#define Key_Status_Pressed 1
#define Key_Status_PressedSustain 2
#define Key_Status_Released 3

#define Status_Def_inWorld 0
#define Status_Def_inMainMenu 1
#define Status_Def_inGameOver 2
#define Status_Def_inSetting 3
#define Status_Def_inGamePause 4
#define Status_Def_inPlayerPlaneSelect 5
#define Status_Def_inGameExit 6
#define Status_Def_inVictory 7

#define Menu_Def_Sel_GameStart 0
#define Menu_Def_Sel_GameResume 0
#define Menu_Def_Sel_GameSetting 1
#define Menu_Def_Sel_GameExit 2
#define Menu_Sel_cursorF22 0
#define Menu_Sel_cursorT50 1
#define Menu_Def_numofPlane 2

#define System_Def_Diff_Easy 0
#define System_Def_Diff_Normal 1
#define System_Def_Diff_Hard 2
#define System_Def_Diff_VHard 3

#define Player_boostSpeed 1.8
#define Player_SlowDownSpeed 0.4
#define Player_In_UP 0
#define Player_In_DW 1
#define Player_In_LF 2
#define Player_In_RH 3

#define Projectile_Def_BPlayer 0
#define Projectile_Def_BEnemyRED 1
#define Projectile_Def_BEnemyPUPPLE 2

//---------------------Structs------------------------------------//
typedef struct {
	HBITMAP BITMAP;
	HDC DC;
	int Width;
	int Height;
}sImage;

typedef struct {
	int isColide;
	int coliderX1;
	int coliderX2;
	int coliderY1;
	int coliderY2;

}BoxColider;

typedef struct {
	int movespeed;
	double dmg;
	int bulletSpeed;
	int AtackRate;
	double variance;
	BoxColider colider;
	sImage PlaneImg;
	sImage PlaneMask;
	sImage hitImg1;
	sImage hitImg2;
} Plane;

typedef struct {
	Plane *playerPlane;
	int xpos;
	int ypos;
	int life;
	int boost;
	int slowDown;
	int upgrade;
	int Bomb;
	int sight;
	int frame;
	int invincTime;
} Player;

typedef struct enemy {
	int xpos;
	int ypos;
	double currentHP;
	double MaxHP;
	int currentframe;
	int numOfFrame;
	int isdestroy;
	int MovementSpeed;

	int invincible;

	struct enemy *prev;
	struct enemy *next;
	BoxColider colider;
	sImage image;
	sImage Mask;
} enemy;

typedef struct projectile {
	double xpos;
	double ypos;
	double dmg;

	double speed;
	double speedRate;
	double angle;
	double angleRate;
	int type;

	BoxColider colider;
	sImage image;
	struct projectile *prev;
	struct projectile *next;
} projectile;

typedef struct {
	projectile *Head;
	int num;
} projectileHead;

//----------------------GlobalVar-----------------------------------//
HWND System_Handle_Window;
HDC System_DC_FrontDC;
HDC System_DC_BackDC;
HANDLE System_Handle_ConsoleOutput;
HBITMAP System_Bitmap_BackBuffer;

int System_Def_DisplayTime = 14;
int System_Status_CurrrentDisplayTime = 14;

unsigned int System_UpdatedFrames = 0;
const int System_Def_ScreenXSize = 1760;
const int System_Def_ScreenYSize = 972;
double System_Status_BackgroundYoffset = 1944;

int System_Status_PlayerScore;
int System_Status_PlayedFrame;
int System_Status_IsVictory = 0;
int System_Status_Difficult = System_Def_Diff_Hard;

MCI_OPEN_PARMS System_Snd_Open; 
MCI_PLAY_PARMS System_Snd_Play; 
MCI_STATUS_PARMS System_Snd_Status;
UINT wDeviceID = 0;
int System_Snd_BGMVolume = 1000;
int System_Snd_EffectVolume = 1000;

projectile Projectile_Def_PlayerBullet;
projectile Projectile_Def_enemyBulletRED;
projectile Projectile_Def_enemyBulletPup;
projectileHead Projectile_Def_ListHead;

sImage Projectile_Def_PlayerBulletMask;
sImage Projectile_Def_enemyBulletMask;
sImage Player_Img_Effect_Heart;
sImage Player_Img_Effect_Bomb;
sImage Player_Img_Effect_HeartMask;
sImage Player_Img_Effect_BombMask;
sImage Enemy_Img_BossHPBar;

sImage Menu_Img_WhiteScr;
sImage Menu_Img_Stage1Background;
sImage Menu_Img_Numbers[10];
sImage Menu_Img_Alphabet[26];
sImage Menu_Img_Start;
sImage Menu_Img_Exit;
sImage Menu_Img_Title;
sImage Menu_Img_Setting;
sImage Menu_Img_RHArr;
sImage Menu_Img_UpArr;
sImage Menu_Img_Resume;
sImage Menu_Img_Pause;
sImage Menu_Img_GameOverScr;
sImage Menu_Img_VictoryScr;
sImage Menu_Img_SettingScr;
sImage Menu_Img_Check;
sImage Menu_Img_CheckMask;
sImage Menu_Img_AbleKeysScr;

Player *player;
Plane *Player_Unit_F22;
Plane *Player_Unit_T50;
enemy Enemy_Unit_Boss1;
DWORD Snd_Player_Fireing;
DWORD Snd_Enemy_Boss1_EZNM;
DWORD Snd_Enemy_Boss1_HD;
DWORD Snd_Enemy_Boss1_VHD; 
int Status_CurrentState;

int Menu_Status_CursorPosition;
int Menu_Status_isSelected;

int Key_Status_KeySet[256] = { 0 };

int Key_Def_Attack = 'X';
int Key_Def_UseBomb = 'Z';
int Key_Def_Select = VK_RETURN;
int Key_Def_GoForward = VK_UP;
int Key_Def_GoBackWard = VK_DOWN;
int Key_Def_GoLeft = VK_LEFT;
int Key_Def_GoRight = VK_RIGHT;
int Key_Def_EnableBooster = VK_LSHIFT;
int Key_Def_EnableSlowDown = VK_LCONTROL;
//int Key_Def_SlowMode = VK_CAPITAL;
//for debug
//int debug_SlowModeEn = 0;

int Player_Status_isBombing = false;

int Level_Projectile_Xoffset;
int Level_Projectile_Yoffset;
int Level_Projectile_Xoffset2;
int Level_Projectile_Yoffset2;
int Level_Projectile_ShootTerm;
int Level_Projectile_BulletNum;
int Level_Projectile_BulletWays;
int Level_Projectile_SrcXPos;
int Level_Projectile_SrcYPos;
int Level_Projectile_Phase;
int Level_Status_DisplayFrame;
int Level_Projectile_DstX;
int Level_Projectile_DstY;

double Level_Status_BackgroundMoveSpeed;
double Level_Projectile_SrcAngle;
double Level_Projectile_SrcAngle2;
double Level_Projectile_SrcAngleRate;
double Level_projectile_SrcAngleRange;
double Level_Projectile_BulletSpeed;
double Level_Projectile_BulletSpeedRate;
double Level_Projectile_BulletAngleRate;
double Level_Projectile_DelaySpeed;

//-----------------------Funcdecl----------------------------------//
void init();
void load();
void LoadsImage(sImage* target, LPCWSTR loc);
void dispose();

double calcDir(int srcX, int dstX, int srcY, int dstY, int isXValue);
void createPlanes();
void createEnemy(int diff);
void userInit();
sImage getAlphabetImg(char in);

void render();
void resetScr();
void drawString(char* string, int xpos, int ypos);
void drawKey(int keyval, int xpos, int ypos);

void renderMainMenu();
void renderPauseScr();
void renderCharacterSelect();
void renderSettingScr();
void renderGameOver();
void renderVictoryScr();

void renderWorld();
void renderBackGround();
void renderPlayer();
void renderEnemy();
void renderProjectiles();
void renderUI();

void update();
void updateKeys();
void updateSound();

void updateMainMenu();
void updateGamePause();
void updateCharacterSelect();
void updateSettingScr();
void ChangeKeySetting(int key);
void updateGameOver();
void updateVictory();

void updateWorld();
void updatePlayer();
void updateLevel();
void updatePlayerMove();
void updatePlayerAttack();
void updateEnemy();

void updateProjectiles();
void addProjectiles(int startX, int startY, double angle, double bulletSpeed, double angleRate, double Speedrate, int type);
void removeProjectiles(projectile* target);
void removeAllEnemyProjectiles();
void removeAllProjectiles();
void detectColide(projectile* target);

void EnemyMvHoriozonSwing();
void EnemyMvVerticalSwing();
void EnemyMvChasePlayer();

double EnemySTWasher(int srcX, int srcY, int Bullettype, int num, double srcAngle, double SrcAngleRate, double BulletSpeed, double BulletSpeedRate, double BulletAngleRate);
void EnemySTnWay(int srcX, int srcY, int Bullettype, int num, double srcAngle, double AngleRange, double BulletSpeed, double BulletSpeedRate, double BulletAngleRate);
void EnemySTTrackingPlayer(int srcX, int srcY, int Bullettype, int num, double AngleRange, double BulletSpeed, double BulletSpeedRate, double BulletAngleRate);
double EnemySTDelay(int srcX, int srcY, int Bullettype, int num, double srcAngle, double AngleRange, double BulletSpeed, double BulletSpeedRate, double delaySpeed, double BulletAngleRate);

void UpdateBoss1();

DWORD LoadSound(HWND hWnd, LPCTSTR path);

//--------------------------------------------------------//
int main() {
	init();
	clock_t currentTime, oldTime;
	currentTime = clock();
	oldTime = currentTime;
	while (Status_CurrentState != Status_Def_inGameExit) {
		currentTime = clock();
		if (currentTime - oldTime >= System_Status_CurrrentDisplayTime) {
			update();
			render();
			System_UpdatedFrames++;
			oldTime = currentTime;
		}
	}
	resetScr();
	dispose();
	return 0;
}

//-------------------------------------------------------//

void init() {
	srand((unsigned int)time(NULL));

	System_Status_CurrrentDisplayTime = System_Def_DisplayTime;

	System_Handle_ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	System_Handle_Window = FindWindow(L"ConsoleWindowClass", NULL);
	System_DC_FrontDC = GetDC(System_Handle_Window);
	System_DC_BackDC = CreateCompatibleDC(System_DC_FrontDC);
	System_Bitmap_BackBuffer = CreateCompatibleBitmap(System_DC_FrontDC, System_Def_ScreenXSize, System_Def_ScreenXSize);

	CONSOLE_CURSOR_INFO ConsoleCursor;
	ConsoleCursor.bVisible = 0;
	ConsoleCursor.dwSize = 1;
	SetConsoleCursorInfo(System_Handle_ConsoleOutput, &ConsoleCursor);

	COORD buff = { System_Def_ScreenXSize, System_Def_ScreenYSize };
	SetConsoleScreenBufferSize(System_Handle_ConsoleOutput, buff);
	system("echo off");
	system("mode con:lines=54 cols=220");//1760, 972
	SetConsoleTextAttribute(System_Handle_ConsoleOutput, 0 | (15 * 16));
	SetConsoleTitle(L"SBAL");
	//system("title Shooting");
	system("cls");
	Projectile_Def_ListHead.Head = NULL;
	Projectile_Def_ListHead.num = 0;

	Status_CurrentState = Status_Def_inMainMenu;
	Menu_Status_CursorPosition = Menu_Def_Sel_GameStart;
	player = (Player*)malloc(sizeof(Player));

	createPlanes();
	load();
}

void load() {
	LoadsImage(&Menu_Img_WhiteScr, L".\\res\\img\\Menu\\WhiteBack.bmp");
	LoadsImage(&Menu_Img_Stage1Background, L".\\res\\img\\Stage\\st1.bmp");

	LoadsImage(&Menu_Img_Start, L".\\res\\img\\Menu\\GameStart.bmp");
	LoadsImage(&Menu_Img_Exit, L".\\res\\img\\Menu\\Exit.bmp");
	LoadsImage(&Menu_Img_Title, L".\\res\\img\\Menu\\Title.bmp");
	LoadsImage(&Menu_Img_Resume, L".\\res\\img\\Menu\\Resume.bmp");
	LoadsImage(&Menu_Img_Pause, L".\\res\\img\\Menu\\Pause.bmp");
	LoadsImage(&Menu_Img_Setting, L".\\res\\img\\Menu\\GameSetting.bmp");
	LoadsImage(&Menu_Img_RHArr, L".\\res\\img\\Menu\\RightArrow.bmp");
	LoadsImage(&Menu_Img_UpArr, L".\\res\\img\\Menu\\UpArrow.bmp");
	LoadsImage(&Menu_Img_GameOverScr, L".\\res\\img\\Menu\\GameOverScr.bmp");
	LoadsImage(&Menu_Img_VictoryScr, L".\\res\\img\\Menu\\VictoryScr.bmp");

	LoadsImage(&Menu_Img_SettingScr, L".\\res\\img\\Menu\\Settings\\SettingScr.bmp");
	LoadsImage(&Menu_Img_Check, L".\\res\\img\\Menu\\Settings\\Check.bmp");
	LoadsImage(&Menu_Img_CheckMask, L".\\res\\img\\Menu\\Settings\\CheckMask.bmp");
	LoadsImage(&Menu_Img_AbleKeysScr, L".\\res\\img\\Menu\\Settings\\AbleKeys.bmp");

	LoadsImage(&Projectile_Def_enemyBulletRED.image, L".\\res\\img\\Effect\\EnemyBulletRED.bmp");
	LoadsImage(&Projectile_Def_enemyBulletPup.image, L".\\res\\img\\Effect\\EnemyBulletPUPPLE.bmp");
	LoadsImage(&Projectile_Def_enemyBulletMask, L".\\res\\img\\Effect\\EnemyBulletMask.bmp");

	LoadsImage(&Projectile_Def_PlayerBullet.image, L".\\res\\img\\Effect\\userBullet.bmp");
	LoadsImage(&Projectile_Def_PlayerBulletMask, L".\\res\\img\\Effect\\userBulletMask.bmp");

	LoadsImage(&Player_Img_Effect_Heart, L".\\res\\img\\Effect\\Heart.bmp");
	LoadsImage(&Player_Img_Effect_Bomb, L".\\res\\img\\Effect\\Bomb.bmp");
	LoadsImage(&Player_Img_Effect_HeartMask, L".\\res\\img\\Effect\\HeartMask.bmp");
	LoadsImage(&Player_Img_Effect_BombMask, L".\\res\\img\\Effect\\BombMask.bmp");
	LoadsImage(&Enemy_Img_BossHPBar, L".\\res\\img\\Effect\\BOSSHPBar.bmp");

	LoadsImage(&Player_Unit_T50->PlaneImg, L".\\res\\img\\Player\\T50PAK.bmp");
	LoadsImage(&Player_Unit_F22->PlaneImg, L".\\res\\img\\Player\\F22.bmp");
	LoadsImage(&Player_Unit_T50->PlaneMask, L".\\res\\img\\Player\\T50PAKMask.bmp");
	LoadsImage(&Player_Unit_F22->PlaneMask, L".\\res\\img\\Player\\F22Mask.bmp");
	LoadsImage(&Player_Unit_T50->hitImg1, L".\\res\\img\\Player\\T50PAKhit.bmp");
	LoadsImage(&Player_Unit_T50->hitImg2, L".\\res\\img\\Player\\T50PAKhit2.bmp");
	LoadsImage(&Player_Unit_F22->hitImg1, L".\\res\\img\\Player\\F22hit.bmp");
	LoadsImage(&Player_Unit_F22->hitImg2, L".\\res\\img\\Player\\F22hit2.bmp");
	LoadsImage(&Enemy_Unit_Boss1.image, L".\\res\\img\\Enemy\\BOSS1.bmp");
	LoadsImage(&Enemy_Unit_Boss1.Mask, L".\\res\\img\\Enemy\\BOSS1Mask.bmp");

	for (int i = 0; i < 10; i++) {
		wchar_t index[34] = L".\\res\\img\\Menu\\number\\number";
		wsprintf(&index[28], L"%d%s", i, L".bmp");
		LoadsImage(&Menu_Img_Numbers[i], index);
	}

	for (int i = 0; i < 26; i++) {
		wchar_t index[30] = L".\\res\\img\\Menu\\Alphabet\\A";
		wsprintf(&index[24], L"%c%s", (i + 65), L".bmp");
		LoadsImage(&Menu_Img_Alphabet[i], index);
	}
	Snd_Player_Fireing = LoadSound(System_Handle_Window, L".\\res\\sound\\Effect\\PlayerFire.wav");
	Snd_Enemy_Boss1_EZNM = LoadSound(System_Handle_Window, L".\\res\\sound\\BGM\\Boss1-1.mp3");
	Snd_Enemy_Boss1_HD = LoadSound(System_Handle_Window, L".\\res\\sound\\BGM\\Boss1-3.mp3");
	Snd_Enemy_Boss1_VHD = LoadSound(System_Handle_Window, L".\\res\\sound\\BGM\\Boss1-4.mp3");

}

void LoadsImage(sImage* target, LPCWSTR loc) {
	BITMAP tmpBtm;
	target->BITMAP = (HBITMAP)LoadImage(NULL, loc, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	GetObject(target->BITMAP, sizeof(BITMAP), &tmpBtm);
	target->Width = tmpBtm.bmWidth;
	target->Height = tmpBtm.bmHeight;
	target->DC = CreateCompatibleDC(System_DC_FrontDC);
	SelectObject(target->DC, target->BITMAP);
}

void dispose() {
	DeleteObject(Menu_Img_WhiteScr.BITMAP);
	DeleteObject(Projectile_Def_enemyBulletRED.image.BITMAP);
	DeleteObject(Projectile_Def_enemyBulletPup.image.BITMAP);
	DeleteObject(Projectile_Def_PlayerBullet.image.BITMAP);
	DeleteObject(Menu_Img_Start.BITMAP);
	DeleteObject(Menu_Img_Setting.BITMAP);
	DeleteObject(Menu_Img_Exit.BITMAP);
	DeleteObject(Menu_Img_Title.BITMAP);
	DeleteObject(Menu_Img_Pause.BITMAP);
	DeleteObject(Menu_Img_Resume.BITMAP);
	DeleteObject(Menu_Img_RHArr.BITMAP);
	DeleteObject(Menu_Img_UpArr.BITMAP);
	DeleteObject(Menu_Img_Stage1Background.BITMAP);
	DeleteObject(Menu_Img_GameOverScr.BITMAP);
	DeleteObject(Menu_Img_SettingScr.BITMAP);
	DeleteObject(Menu_Img_Check.BITMAP);
	DeleteObject(Menu_Img_CheckMask.BITMAP);
	DeleteObject(Menu_Img_AbleKeysScr.BITMAP);

	DeleteObject(Player_Unit_F22->PlaneImg.BITMAP);
	DeleteObject(Player_Unit_T50->PlaneImg.BITMAP);
	DeleteObject(Player_Unit_F22->PlaneMask.BITMAP);
	DeleteObject(Player_Unit_T50->PlaneMask.BITMAP);
	DeleteObject(Player_Unit_F22->hitImg1.BITMAP);
	DeleteObject(Player_Unit_F22->hitImg2.BITMAP);
	DeleteObject(Player_Unit_T50->hitImg1.BITMAP);
	DeleteObject(Player_Unit_T50->hitImg2.BITMAP);
	DeleteObject(Enemy_Unit_Boss1.image.BITMAP);
	DeleteObject(Enemy_Unit_Boss1.Mask.BITMAP);
	DeleteObject(Player_Img_Effect_Heart.BITMAP);
	DeleteObject(Player_Img_Effect_HeartMask.BITMAP);
	DeleteObject(Player_Img_Effect_Bomb.BITMAP);
	DeleteObject(Player_Img_Effect_BombMask.BITMAP);
	DeleteObject(Enemy_Img_BossHPBar.BITMAP);
	DeleteObject(System_Bitmap_BackBuffer);

	DeleteDC(Menu_Img_WhiteScr.DC);
	DeleteDC(Projectile_Def_enemyBulletRED.image.DC);
	DeleteDC(Projectile_Def_enemyBulletPup.image.DC);
	DeleteDC(Projectile_Def_PlayerBullet.image.DC);
	DeleteDC(Menu_Img_Start.DC);
	DeleteDC(Menu_Img_Setting.DC);
	DeleteDC(Menu_Img_Exit.DC);
	DeleteDC(Menu_Img_Title.DC);
	DeleteDC(Menu_Img_Pause.DC);
	DeleteDC(Menu_Img_Resume.DC);
	DeleteDC(Menu_Img_RHArr.DC);
	DeleteDC(Menu_Img_UpArr.DC);
	DeleteDC(Menu_Img_Stage1Background.DC);
	DeleteDC(Menu_Img_GameOverScr.DC);
	DeleteDC(Menu_Img_SettingScr.DC);
	DeleteDC(Menu_Img_Check.DC);
	DeleteDC(Menu_Img_CheckMask.DC);
	DeleteDC(Menu_Img_AbleKeysScr.DC);
	DeleteDC(Player_Unit_F22->PlaneImg.DC);
	DeleteDC(Player_Unit_T50->PlaneImg.DC);
	DeleteDC(Player_Unit_F22->PlaneMask.DC);
	DeleteDC(Player_Unit_T50->PlaneMask.DC);
	DeleteDC(Player_Unit_F22->hitImg1.DC);
	DeleteDC(Player_Unit_F22->hitImg2.DC);
	DeleteDC(Player_Unit_T50->hitImg1.DC);
	DeleteDC(Player_Unit_T50->hitImg2.DC);
	DeleteDC(Enemy_Unit_Boss1.image.DC);
	DeleteDC(Enemy_Unit_Boss1.Mask.DC);
	DeleteDC(Player_Img_Effect_Heart.DC);
	DeleteDC(Player_Img_Effect_Bomb.DC);
	DeleteDC(Player_Img_Effect_HeartMask.DC);
	DeleteDC(Player_Img_Effect_BombMask.DC);
	DeleteDC(Enemy_Img_BossHPBar.DC);

	for (int i = 0; i < 10; i++) {
		DeleteObject(Menu_Img_Numbers[i].BITMAP);
		DeleteDC(Menu_Img_Numbers[i].DC);
	}
	for (int i = 0; i < 26; i++) {
		DeleteObject(Menu_Img_Alphabet[i].BITMAP);
		DeleteDC(Menu_Img_Alphabet[i].DC);
	}

	DeleteDC(System_DC_BackDC);
	ReleaseDC(System_Handle_Window, System_DC_FrontDC);

	mciSendCommand(4, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
	mciSendCommand(3, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
	mciSendCommand(2, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
	mciSendCommand(1, MCI_CLOSE, 0, (DWORD)(LPVOID)NULL);
	system("exit");
}

double calcDir(int srcX, int dstX, int srcY, int dstY, int isXValue) {
	if (isXValue == true)
		return ((double)(dstX - srcX) / (abs(srcY - dstY) + abs(srcX - dstX)));
	else
		return ((double)(dstY - srcY) / (abs(srcY - dstY) + abs(srcX - dstX)));
}

void userInit() {
	player->upgrade = 3;
	player->life = 7;
	player->Bomb = 3;
	player->xpos = (System_Def_ScreenXSize - player->playerPlane->PlaneImg.Width) / 2;
	player->ypos = System_Def_ScreenYSize - player->playerPlane->PlaneImg.Height - 50;
	player->boost = 0;
	player->slowDown = 0;
	player->invincTime = 0;
	player->sight = Player_In_DW;
	player->frame = 0;
}

void levelInit() {
	Level_Projectile_Xoffset = 0;
	Level_Projectile_Yoffset = 0;
	Level_Projectile_Xoffset2 = 0;
	Level_Projectile_Yoffset2 = 0;
	Level_Projectile_ShootTerm = 0;
	Level_Projectile_BulletNum = 0;
	Level_Projectile_SrcXPos = 0;
	Level_Projectile_SrcYPos = 0;
	Level_Projectile_SrcAngle = 0;
	Level_Projectile_SrcAngle2 = 0;
	Level_Projectile_SrcAngleRate = 0;
	Level_Projectile_BulletSpeed = 0;
	Level_Projectile_BulletSpeedRate = 0;
	Level_Projectile_DelaySpeed = 0;
	Level_Projectile_Phase = 0;
	Level_Status_DisplayFrame = 0;
	Level_projectile_SrcAngleRange = 0;
	Level_Projectile_BulletWays = 0;
	Level_Projectile_BulletAngleRate = 0;
	Level_Projectile_DstX = 0;
	Level_Projectile_DstY = 0;
	Level_Status_BackgroundMoveSpeed = 3;
	Player_Status_isBombing = false;
}

DWORD LoadSound(HWND hWnd, LPCTSTR path)
{
	DWORD Result;

	System_Snd_Open.lpstrDeviceType = L"MPEGVideo";

	System_Snd_Open.lpstrElementName = path;

	Result = mciSendCommand(wDeviceID, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&System_Snd_Open);

	if (Result)
		return Result;

	wDeviceID = System_Snd_Open.wDeviceID;

	System_Snd_Play.dwCallback = (DWORD)hWnd;

	if (Result)
		return Result;

	return 0;
}

void createPlanes() {
	Player_Unit_F22 = (Plane*)malloc(sizeof(Plane));
	Player_Unit_F22->dmg = 13;
	Player_Unit_F22->movespeed = 6;
	Player_Unit_F22->bulletSpeed = 20;
	Player_Unit_F22->AtackRate = 2;
	Player_Unit_F22->colider.coliderX1 = 32;
	Player_Unit_F22->colider.coliderX2 = 34;
	Player_Unit_F22->colider.coliderY1 = 13;
	Player_Unit_F22->colider.coliderY2 = 18;
	Player_Unit_F22->colider.isColide = false;
	Player_Unit_F22->variance = 0.02;

	Player_Unit_T50 = (Plane*)malloc(sizeof(Plane));
	Player_Unit_T50->dmg = 16;
	Player_Unit_T50->movespeed = 8;
	Player_Unit_T50->bulletSpeed = 26;
	Player_Unit_T50->AtackRate = 3;
	Player_Unit_T50->colider.coliderX1 = 32;
	Player_Unit_T50->colider.coliderX2 = 35;
	Player_Unit_T50->colider.coliderY1 = 14;
	Player_Unit_T50->colider.coliderY2 = 17;
	Player_Unit_T50->colider.isColide = false;
	Player_Unit_T50->variance = 0.0075;
}

void createEnemy(int diff) {
	if (diff == System_Def_Diff_Easy) {
		Enemy_Unit_Boss1.MaxHP = 300000.0;
		Enemy_Unit_Boss1.currentHP = Enemy_Unit_Boss1.MaxHP;

	}
	else if (diff == System_Def_Diff_Normal) {
		Enemy_Unit_Boss1.MaxHP = 500000.0;
		Enemy_Unit_Boss1.currentHP = Enemy_Unit_Boss1.MaxHP;

	}
	else if (diff == System_Def_Diff_Hard) {
		Enemy_Unit_Boss1.MaxHP = 1600000.0;
		Enemy_Unit_Boss1.currentHP = 0.1;
	}
	else {
		Enemy_Unit_Boss1.MaxHP = 1600000.0;
		Enemy_Unit_Boss1.currentHP = Enemy_Unit_Boss1.MaxHP;

	}
	Enemy_Unit_Boss1.currentframe = 0;
	Enemy_Unit_Boss1.numOfFrame = 4;
	Enemy_Unit_Boss1.colider.coliderX1 = 0;
	Enemy_Unit_Boss1.colider.coliderX2 = (Enemy_Unit_Boss1.image.Width / Enemy_Unit_Boss1.numOfFrame);
	Enemy_Unit_Boss1.colider.coliderY1 = 0;
	Enemy_Unit_Boss1.colider.coliderY2 = Enemy_Unit_Boss1.image.Height - 20;
	Enemy_Unit_Boss1.MovementSpeed = 6;

	Enemy_Unit_Boss1.xpos = (System_Def_ScreenXSize - Enemy_Unit_Boss1.image.Width / Enemy_Unit_Boss1.numOfFrame) / 2;
	Enemy_Unit_Boss1.ypos = -420;
	Enemy_Unit_Boss1.invincible = true;

	Enemy_Unit_Boss1.isdestroy = false;

}

sImage getAlphabetImg(char in) {
	return Menu_Img_Alphabet[in - 65];
}

//---------------------------------------Render--------------------------------------------//

void render() {
	switch (Status_CurrentState) {
	case Status_Def_inMainMenu: renderMainMenu(); break;
	case Status_Def_inGamePause: renderPauseScr(); break;
	case Status_Def_inWorld: renderWorld(); break;
	case Status_Def_inPlayerPlaneSelect: renderCharacterSelect(); break;
	case Status_Def_inSetting: renderSettingScr(); break;
	case Status_Def_inGameOver: renderGameOver(); break;
	case Status_Def_inVictory: renderVictoryScr(); break;
	}
}

void resetScr() {
	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenXSize, System_DC_BackDC, 0, 0, WHITENESS);
}

void drawString(char* string, int xpos, int ypos) {
	int count = 0;

	while (string[count] != '\0') {
		BitBlt(System_DC_BackDC, xpos + (getAlphabetImg(string[count]).Width * count), ypos, 
			getAlphabetImg(string[count]).Width, getAlphabetImg(string[count]).Height, getAlphabetImg(string[count]).DC, 0, 0, SRCCOPY);
		count++;
	}
}

void drawKey(int keyval, int xpos, int ypos) {
	if ('A' <= keyval && keyval <= 'Z') {
		BitBlt(System_DC_BackDC, xpos, ypos, getAlphabetImg(keyval).Width, getAlphabetImg(keyval).Height, getAlphabetImg(keyval).DC, 0, 0, SRCCOPY);
	}
	else if (keyval == VK_LCONTROL) {
		drawString("LCTRL", xpos, ypos);
	}
	else if (keyval == VK_LSHIFT) {
		drawString("LSHIFT", xpos, ypos);
	}
	else if (keyval == VK_LMENU) {
		drawString("LALT", xpos, ypos);
	}
	else if (keyval == VK_SPACE) {
		drawString("SPACE", xpos, ypos);
	}
	else if (keyval == VK_UP) {
		drawString("UP", xpos, ypos);
	}
	else if (keyval == VK_DOWN) {
		drawString("DOWN", xpos, ypos);
	}
	else if (keyval == VK_LEFT) {
		drawString("LEFT", xpos, ypos);
	}
	else if (keyval == VK_RIGHT) {
		drawString("RIGHT", xpos, ypos);
	}
}
//----------------------//
void renderMainMenu() {
	SelectObject(System_DC_BackDC, System_Bitmap_BackBuffer);
	BitBlt(System_DC_BackDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_WhiteScr.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Title.Width) / 2, 200, 
		Menu_Img_Title.Width, Menu_Img_Title.Height, Menu_Img_Title.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Start.Width) / 2, 600, 
		Menu_Img_Start.Width, Menu_Img_Start.Height, Menu_Img_Start.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Setting.Width) / 2, 700, 
		Menu_Img_Setting.Width, Menu_Img_Setting.Height, Menu_Img_Setting.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Exit.Width) / 2, 800, 
		Menu_Img_Exit.Width, Menu_Img_Exit.Height, Menu_Img_Exit.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, ((System_Def_ScreenXSize - Menu_Img_Start.Width) / 2 - Menu_Img_RHArr.Width - 20), 
		600 + Menu_Status_CursorPosition * 100, Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, System_DC_BackDC, 0, 0, SRCCOPY);
}

void renderPauseScr() {
	SelectObject(System_DC_BackDC, System_Bitmap_BackBuffer);

	BitBlt(System_DC_BackDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_WhiteScr.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Pause.Width) / 2, 200, 
		Menu_Img_Pause.Width, Menu_Img_Pause.Height, Menu_Img_Pause.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Resume.Width) / 2, 600, 
		Menu_Img_Resume.Width, Menu_Img_Resume.Height, Menu_Img_Resume.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Menu_Img_Exit.Width) / 2, 700, 
		Menu_Img_Exit.Width, Menu_Img_Exit.Height, Menu_Img_Exit.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, ((System_Def_ScreenXSize - Menu_Img_Resume.Width) / 2 - Menu_Img_RHArr.Width - 20), 
		600 + Menu_Status_CursorPosition * 100, 60, 40, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, System_DC_BackDC, 0, 0, SRCCOPY);
}

void renderCharacterSelect() {
	SelectObject(System_DC_BackDC, System_Bitmap_BackBuffer);

	BitBlt(System_DC_BackDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_WhiteScr.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Player_Unit_F22->PlaneImg.Width) / (Menu_Def_numofPlane + 1), 400,
		Player_Unit_F22->PlaneImg.Width, Player_Unit_F22->PlaneImg.Height, Player_Unit_F22->PlaneMask.DC, 0, 0, SRCPAINT);
	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Player_Unit_F22->PlaneImg.Width) / (Menu_Def_numofPlane + 1), 400,
		Player_Unit_F22->PlaneImg.Width, Player_Unit_F22->PlaneImg.Height, Player_Unit_F22->PlaneImg.DC, 0, 0, SRCAND);

	BitBlt(System_DC_BackDC, ((System_Def_ScreenXSize - Player_Unit_T50->PlaneImg.Width) / (Menu_Def_numofPlane + 1)) * 2, 400, 
		Player_Unit_T50->PlaneImg.Width, Player_Unit_T50->PlaneImg.Height, Player_Unit_T50->PlaneMask.DC, 0, 0, SRCPAINT);
	BitBlt(System_DC_BackDC, ((System_Def_ScreenXSize - Player_Unit_T50->PlaneImg.Width) / (Menu_Def_numofPlane + 1)) * 2, 400, 
		Player_Unit_T50->PlaneImg.Width, Player_Unit_T50->PlaneImg.Height, Player_Unit_T50->PlaneImg.DC,0,0,SRCAND);
	
	BitBlt(System_DC_BackDC, ((System_Def_ScreenXSize - Player_Unit_T50->PlaneImg.Width) / (Menu_Def_numofPlane + 1)) * (Menu_Status_CursorPosition + 1) + (Player_Unit_F22->PlaneImg.Width - Menu_Img_UpArr.Width) / 2,
		(400 + Player_Unit_F22->PlaneImg.Height + 20), Menu_Img_UpArr.Width, Menu_Img_UpArr.Height, Menu_Img_UpArr.DC, 0, 0, SRCCOPY);

	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, System_DC_BackDC, 0, 0, SRCCOPY);
}

void renderSettingScr() {
	SelectObject(System_DC_BackDC, System_Bitmap_BackBuffer);

	BitBlt(System_DC_BackDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_SettingScr.DC, 0, 0, SRCCOPY);

	//Draw Arr
	if (Menu_Status_CursorPosition < 8) {
		if (Menu_Status_CursorPosition % 2 == 0) {
			//233 197(+108)
			BitBlt(System_DC_BackDC, 233, 197 + (Menu_Status_CursorPosition / 2 * 108), 
				Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
		}
		else {
			//819 197(+108)
			BitBlt(System_DC_BackDC, 819, 197 + (Menu_Status_CursorPosition / 2 * 108), 
				Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
		}
	}
	else if (Menu_Status_CursorPosition == 8) {
		//164 804
		BitBlt(System_DC_BackDC, 164, 804, Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
	}
	else if (Menu_Status_CursorPosition == 9) {
		BitBlt(System_DC_BackDC, 419, 804, Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
	}
	else if (Menu_Status_CursorPosition == 10) {
		BitBlt(System_DC_BackDC, 742, 804, Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
	}
	else if (Menu_Status_CursorPosition == 11) {
		BitBlt(System_DC_BackDC, 979, 804, Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
	}
	else {
		//1446
		BitBlt(System_DC_BackDC, 1386, 804, Menu_Img_RHArr.Width, Menu_Img_RHArr.Height, Menu_Img_RHArr.DC, 0, 0, SRCCOPY);
	}
	//draw Difficulty
	if (System_Status_Difficult == System_Def_Diff_Easy) {
		BitBlt(System_DC_BackDC, 272, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_CheckMask.DC,
			0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, 272, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_Check.DC,
			0, 0, SRCAND);
	}
	else if (System_Status_Difficult == System_Def_Diff_Normal) {
		BitBlt(System_DC_BackDC, 537, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_CheckMask.DC,
			0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, 537, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_Check.DC,
			0, 0, SRCAND);
	}
	else if (System_Status_Difficult == System_Def_Diff_Hard) {
		BitBlt(System_DC_BackDC, 850, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_CheckMask.DC,
			0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, 850, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_Check.DC,
			0, 0, SRCAND);
	}
	else {
		BitBlt(System_DC_BackDC, 1117, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_CheckMask.DC,
			0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, 1117, 774, Menu_Img_Check.Width, Menu_Img_Check.Height, Menu_Img_Check.DC,
			0, 0, SRCAND);
	}

	//Draw Keybind

	drawKey(Key_Def_Attack, 586, 197);
	drawKey(Key_Def_UseBomb, 586, 305);
	drawKey(Key_Def_EnableBooster, 586, 413);
	drawKey(Key_Def_EnableSlowDown, 586, 521);

	drawKey(Key_Def_GoForward, 1172, 197);
	drawKey(Key_Def_GoBackWard, 1172, 305);
	drawKey(Key_Def_GoLeft, 1172, 413);
	drawKey(Key_Def_GoRight, 1172, 521);

	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, System_DC_BackDC, 0, 0, SRCCOPY);

}

void renderGameOver() {
	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_GameOverScr.DC, 0, 0, SRCCOPY);
}

void renderVictoryScr() {
	SelectObject(System_DC_BackDC, System_Bitmap_BackBuffer);
	BitBlt(System_DC_BackDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_VictoryScr.DC, 0, 0, SRCCOPY);
	int score = System_Status_PlayerScore;
	int div = 100000;
	for (int i = 0; div != 0; i++) {
		int quat = score / div;
		score -= quat*div;
		div /= 10;
		BitBlt(System_DC_BackDC, 825 + Menu_Img_Numbers[quat].Width*i, 554, 
			Menu_Img_Numbers[quat].Width, Menu_Img_Numbers[quat].Height, Menu_Img_Numbers[quat].DC, 0, 0, SRCCOPY);
	}
	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, System_DC_BackDC, 0, 0, SRCCOPY);
}

//----------------------//
void renderWorld() {
	SelectObject(System_DC_BackDC, System_Bitmap_BackBuffer);
	renderBackGround();
	renderEnemy();
	renderPlayer();
	renderProjectiles();
	renderUI();
	BitBlt(System_DC_FrontDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, System_DC_BackDC, 0, 0, SRCCOPY);
}

void renderBackGround() {
	BitBlt(System_DC_BackDC, 0, 0, System_Def_ScreenXSize, System_Def_ScreenYSize, Menu_Img_Stage1Background.DC, 0, (int)System_Status_BackgroundYoffset, SRCCOPY);
}

void renderEnemy() {
	if (!Enemy_Unit_Boss1.isdestroy) {
		BitBlt(System_DC_BackDC, Enemy_Unit_Boss1.xpos, Enemy_Unit_Boss1.ypos,
			(Enemy_Unit_Boss1.image.Width / Enemy_Unit_Boss1.numOfFrame), Enemy_Unit_Boss1.image.Height, Enemy_Unit_Boss1.Mask.DC,
			(Enemy_Unit_Boss1.currentframe*(Enemy_Unit_Boss1.image.Width / Enemy_Unit_Boss1.numOfFrame)), 0, SRCPAINT);
		BitBlt(System_DC_BackDC, Enemy_Unit_Boss1.xpos, Enemy_Unit_Boss1.ypos, 
			(Enemy_Unit_Boss1.image.Width / Enemy_Unit_Boss1.numOfFrame),Enemy_Unit_Boss1.image.Height, Enemy_Unit_Boss1.image.DC,
			(Enemy_Unit_Boss1.currentframe*(Enemy_Unit_Boss1.image.Width / Enemy_Unit_Boss1.numOfFrame)), 0, SRCAND);
	}

}

void renderPlayer() {

	if (player->invincTime == 0) {
		BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->PlaneImg.Width, player->playerPlane->PlaneMask.Height,
			player->playerPlane->PlaneMask.DC, 0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->PlaneImg.Width, player->playerPlane->PlaneImg.Height, 
			player->playerPlane->PlaneImg.DC, 0, 0, SRCAND);
	}
	else {
		if (System_UpdatedFrames % 21 < 7) {
			BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->PlaneImg.Width, player->playerPlane->PlaneMask.Height,
				player->playerPlane->PlaneMask.DC, 0, 0, SRCPAINT);
			BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->hitImg1.Width, player->playerPlane->hitImg1.Height, 
				player->playerPlane->hitImg1.DC, 0, 0, SRCAND);
		}
		else if (System_UpdatedFrames % 21 < 14) {
			BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->PlaneImg.Width, player->playerPlane->PlaneMask.Height,
				player->playerPlane->PlaneMask.DC, 0, 0, SRCPAINT);
			BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->hitImg2.Width, player->playerPlane->hitImg2.Height, 
				player->playerPlane->hitImg2.DC,	0, 0, SRCAND);
		}
		else {
			BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->PlaneImg.Width, player->playerPlane->PlaneMask.Height,
				player->playerPlane->PlaneMask.DC, 0, 0, SRCPAINT);
			BitBlt(System_DC_BackDC, player->xpos, player->ypos, player->playerPlane->PlaneImg.Width, player->playerPlane->PlaneImg.Height, 
				player->playerPlane->PlaneImg.DC, 0, 0, SRCAND);
		}
	}
}

void renderProjectiles() {
	if (Projectile_Def_ListHead.Head != NULL) {
		projectile *target = Projectile_Def_ListHead.Head;
		while (target != NULL) {
			if (target->type == Projectile_Def_BPlayer) {
				BitBlt(System_DC_BackDC, (int)(target->xpos + 0.5), (int)(target->ypos + 0.5), target->image.Width, target->image.Height,
					Projectile_Def_PlayerBulletMask.DC, 0, 0, SRCPAINT);
				BitBlt(System_DC_BackDC, (int)(target->xpos + 0.5), (int)(target->ypos + 0.5), target->image.Width, target->image.Height,
					target->image.DC,0,0,SRCAND);
			}
			else {
				BitBlt(System_DC_BackDC, (int)(target->xpos + 0.5), (int)(target->ypos + 0.5), target->image.Width, target->image.Height,
					Projectile_Def_enemyBulletMask.DC, 0, 0, SRCPAINT);
				BitBlt(System_DC_BackDC, (int)(target->xpos + 0.5), (int)(target->ypos + 0.5), target->image.Width, target->image.Height,
					target->image.DC, 0, 0, SRCAND);
			}

			target = target->next;
		}
	}
}

void renderUI() {
	BitBlt(System_DC_BackDC, (System_Def_ScreenXSize - Enemy_Img_BossHPBar.Width) / 2, 10, 
		(int)(Enemy_Img_BossHPBar.Width*(Enemy_Unit_Boss1.currentHP / Enemy_Unit_Boss1.MaxHP)), Enemy_Img_BossHPBar.Height, 
		Enemy_Img_BossHPBar.DC, 0, 0, SRCCOPY);

	for (int i = 0; i < player->life; i++) {
		BitBlt(System_DC_BackDC, 0 + i*(Player_Img_Effect_Heart.Width + 10), Enemy_Img_BossHPBar.Height + 20,
			Player_Img_Effect_Heart.Width, Player_Img_Effect_Heart.Height, Player_Img_Effect_HeartMask.DC,
			0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, 0 + i*(Player_Img_Effect_Heart.Width + 10), Enemy_Img_BossHPBar.Height + 20, 
			Player_Img_Effect_Heart.Width, Player_Img_Effect_Heart.Height, Player_Img_Effect_Heart.DC,
			0, 0, SRCAND);
	}
	for (int i = 0; i < player->Bomb; i++) {
		BitBlt(System_DC_BackDC, 0 + i*(Player_Img_Effect_Bomb.Width + 10),
			Player_Img_Effect_Heart.Height + Enemy_Img_BossHPBar.Height + 20, Player_Img_Effect_Bomb.Width,
			Player_Img_Effect_Bomb.Height, Player_Img_Effect_BombMask.DC, 0, 0, SRCPAINT);
		BitBlt(System_DC_BackDC, 0 + i*(Player_Img_Effect_Bomb.Width + 10), 
			Player_Img_Effect_Heart.Height + Enemy_Img_BossHPBar.Height + 20, Player_Img_Effect_Bomb.Width, 
			Player_Img_Effect_Bomb.Height, Player_Img_Effect_Bomb.DC, 0, 0, SRCAND);
	}

}

//----------------------------------------Update------------------------------------------//

void update() {
	updateKeys();
	switch (Status_CurrentState) {
	case Status_Def_inMainMenu: updateMainMenu(); break;
	case Status_Def_inSetting: updateSettingScr(); break;
	case Status_Def_inWorld: updateWorld(); break;
	case Status_Def_inGamePause: updateGamePause(); break;
	case Status_Def_inPlayerPlaneSelect: updateCharacterSelect(); break;
	case Status_Def_inGameOver: updateGameOver(); break;
	case Status_Def_inVictory: updateVictory(); break;
	}
	updateSound();
}

void updateKeys() {
	for (int i = 0; i < 256; i++) {
		if (GetAsyncKeyState(i) & 0x8000) {
			if (Key_Status_KeySet[i] == Key_Status_NonPress) Key_Status_KeySet[i] = Key_Status_Pressed;
			else if (Key_Status_KeySet[i] == Key_Status_Pressed) Key_Status_KeySet[i] = Key_Status_PressedSustain;
		}
		else if (!GetAsyncKeyState(i)) {
			if (Key_Status_KeySet[i] == Key_Status_Pressed || Key_Status_KeySet[i] == Key_Status_PressedSustain) Key_Status_KeySet[i] = Key_Status_Released;
			else if (Key_Status_KeySet[i] == Key_Status_Released) Key_Status_KeySet[i] = Key_Status_NonPress;
		}
	}
}

void updateSound() {
	static int BGMPlayed = 0;
	switch (Status_CurrentState) {
	case Status_Def_inMainMenu:
		Snd_Enemy_Boss1_EZNM = mciSendCommand(2, MCI_STOP, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
		Snd_Enemy_Boss1_EZNM = mciSendCommand(2, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)&System_Snd_Play);
		Snd_Enemy_Boss1_HD = mciSendCommand(3, MCI_STOP, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
		Snd_Enemy_Boss1_HD = mciSendCommand(3, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)&System_Snd_Play);
		Snd_Enemy_Boss1_HD = mciSendCommand(4, MCI_STOP, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
		Snd_Enemy_Boss1_HD = mciSendCommand(4, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)&System_Snd_Play);
		BGMPlayed = false;

		break;
	case Status_Def_inSetting: break;
	case Status_Def_inWorld:
		if (!BGMPlayed) {
			if (System_Status_Difficult == System_Def_Diff_Easy || System_Status_Difficult == System_Def_Diff_Normal) {
				Snd_Enemy_Boss1_EZNM = mciSendCommand(2, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			if (System_Status_Difficult == System_Def_Diff_Hard) {
				Snd_Enemy_Boss1_HD = mciSendCommand(3, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			if (System_Status_Difficult == System_Def_Diff_VHard) {
				Snd_Enemy_Boss1_VHD = mciSendCommand(4, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
		
			BGMPlayed = true;
		}
		if (Key_Status_KeySet[Key_Def_Attack] && ((System_UpdatedFrames % (player->playerPlane->AtackRate * 2)) == 0)) {
				Snd_Player_Fireing = mciSendCommand(1, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)&System_Snd_Play);
				Snd_Player_Fireing = mciSendCommand(1, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
		}
		break;
	case Status_Def_inGamePause:
		if (BGMPlayed) {
			if (System_Status_Difficult == System_Def_Diff_Easy || System_Status_Difficult == System_Def_Diff_Normal) {
				Snd_Enemy_Boss1_EZNM = mciSendCommand(2, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			if (System_Status_Difficult == System_Def_Diff_Hard) {
				Snd_Enemy_Boss1_HD = mciSendCommand(3, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			if (System_Status_Difficult == System_Def_Diff_VHard) {
				Snd_Enemy_Boss1_VHD = mciSendCommand(4, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			BGMPlayed = false;
		}
		break;
	case Status_Def_inPlayerPlaneSelect: break;
	case Status_Def_inGameOver: 
		if (BGMPlayed) {
			if (System_Status_Difficult == System_Def_Diff_Easy || System_Status_Difficult == System_Def_Diff_Normal) {
				Snd_Enemy_Boss1_EZNM = mciSendCommand(2, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			if (System_Status_Difficult == System_Def_Diff_Hard) {
				Snd_Enemy_Boss1_HD = mciSendCommand(3, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			if (System_Status_Difficult == System_Def_Diff_VHard) {
				Snd_Enemy_Boss1_VHD = mciSendCommand(4, MCI_PAUSE, MCI_NOTIFY, (DWORD)(LPVOID)&System_Snd_Play);
			}
			BGMPlayed = false;
		}
		break;
	case Status_Def_inVictory: break;
	}

	if (Key_Status_KeySet[VK_ADD] == Key_Status_Pressed){
		if (System_Snd_BGMVolume == 500) {
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-1.mp3 volume to 1000", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-3.mp3 volume to 1000", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-4.mp3 volume to 1000", 0, 0, 0);
			System_Snd_BGMVolume = 1000;
		}
		else if (System_Snd_BGMVolume == 0) {
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-1.mp3 volume to 500", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-3.mp3 volume to 500", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-4.mp3 volume to 500", 0, 0, 0);
			System_Snd_BGMVolume = 500;
		}

	}
	if (Key_Status_KeySet[VK_SUBTRACT] == Key_Status_Pressed) {
		if (System_Snd_BGMVolume == 500) {
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-1.mp3 volume to 0", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-3.mp3 volume to 0", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-4.mp3 volume to 0", 0, 0, 0);
			System_Snd_BGMVolume = 0;
		}
		else if (System_Snd_BGMVolume == 1000) {
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-1.mp3 volume to 500", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-3.mp3 volume to 500", 0, 0, 0);
			mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-4.mp3 volume to 500", 0, 0, 0);
			System_Snd_BGMVolume = 500;
		}

	}

	if (Key_Status_KeySet[VK_MULTIPLY] == Key_Status_Pressed) {
		if (System_Snd_EffectVolume == 500) {
			mciSendString(L"setaudio .\\res\\sound\\Effect\\PlayerFire.wav volume to 1000", 0, 0, 0);
			System_Snd_EffectVolume = 1000;
		}
		else if (System_Snd_EffectVolume == 0) {
			mciSendString(L"setaudio .\\res\\sound\\Effect\\PlayerFire.wav volume to 500", 0, 0, 0);
			System_Snd_EffectVolume = 500;
		}
	}
	if (Key_Status_KeySet[VK_DIVIDE] == Key_Status_Pressed) {
		if (System_Snd_EffectVolume == 500) {
			mciSendString(L"setaudio .\\res\\sound\\Effect\\PlayerFire.wav volume to 0", 0, 0, 0);
			System_Snd_EffectVolume = 0;
		}
		else if (System_Snd_EffectVolume == 1000) {
			mciSendString(L"setaudio .\\res\\sound\\Effect\\PlayerFire.wav volume to 500", 0, 0, 0);
			System_Snd_EffectVolume = 500;
		}
	}


}
//----------Menu------------//

void updateMainMenu() {
	if (Key_Status_KeySet[Key_Def_Select] == Key_Status_Pressed) {

		if (Menu_Status_CursorPosition == Menu_Def_Sel_GameStart) {
			Status_CurrentState = Status_Def_inPlayerPlaneSelect;
		}
		else if (Menu_Status_CursorPosition == Menu_Def_Sel_GameSetting) {
			Status_CurrentState = Status_Def_inSetting;
		}
		else if (Menu_Status_CursorPosition == Menu_Def_Sel_GameExit) {
			Status_CurrentState = Status_Def_inGameExit;
		}
		Menu_Status_isSelected = 0;
		Menu_Status_CursorPosition = 0;

	}
	if (Key_Status_KeySet[VK_UP] == Key_Status_Pressed) {
		Menu_Status_CursorPosition--;
	}
	if (Key_Status_KeySet[VK_DOWN] == Key_Status_Pressed) {
		Menu_Status_CursorPosition++;
	}
	if (Menu_Status_CursorPosition < Menu_Def_Sel_GameStart) Menu_Status_CursorPosition = Menu_Def_Sel_GameExit;
	else if (Menu_Status_CursorPosition > Menu_Def_Sel_GameExit) Menu_Status_CursorPosition = Menu_Def_Sel_GameStart;

}

void updateGamePause() {
	if (Menu_Status_isSelected == 1) {
		if (Menu_Status_CursorPosition == Menu_Def_Sel_GameResume) {
			Status_CurrentState = Status_Def_inWorld;

		}
		else if (Menu_Status_CursorPosition == Menu_Def_Sel_GameSetting) {
			removeAllProjectiles();
			Status_CurrentState = Status_Def_inMainMenu;

		}
		Menu_Status_isSelected = 0;
		Menu_Status_CursorPosition = 0;
	}

	if (Key_Status_KeySet[Key_Def_Select] == Key_Status_Pressed) {
		Menu_Status_isSelected = 1;
	}
	if (Key_Status_KeySet[VK_UP] == Key_Status_Pressed) {
		Menu_Status_CursorPosition--;
	}
	if (Key_Status_KeySet[VK_DOWN] == Key_Status_Pressed) {
		Menu_Status_CursorPosition++;
	}
	if (Key_Status_KeySet[VK_ESCAPE] == Key_Status_Pressed) {
		Menu_Status_isSelected = 1;
		Menu_Status_CursorPosition = Menu_Def_Sel_GameResume;
	}
	if (Menu_Status_CursorPosition < Menu_Def_Sel_GameResume) Menu_Status_CursorPosition = Menu_Def_Sel_GameSetting;//purposeful
	else if (Menu_Status_CursorPosition > Menu_Def_Sel_GameSetting) Menu_Status_CursorPosition = Menu_Def_Sel_GameResume;

}

void updateCharacterSelect() {

	if (Key_Status_KeySet[VK_LEFT] == Key_Status_Pressed) {
		Menu_Status_CursorPosition--;
	}
	if (Key_Status_KeySet[VK_RIGHT] == Key_Status_Pressed) {
		Menu_Status_CursorPosition++;
	}

	if (Menu_Status_CursorPosition < 0) Menu_Status_CursorPosition = Menu_Def_numofPlane - 1;
	if (Menu_Status_CursorPosition >= Menu_Def_numofPlane) Menu_Status_CursorPosition = 0;
	if (Key_Status_KeySet[Key_Def_Select] == Key_Status_Pressed) {
		switch (Menu_Status_CursorPosition) {
		case Menu_Sel_cursorF22: player->playerPlane = Player_Unit_F22; break;
		case Menu_Sel_cursorT50: player->playerPlane = Player_Unit_T50; break;
		}
		userInit();
		levelInit();
		createEnemy(System_Status_Difficult);
		System_Status_PlayerScore = 1000000;
		System_Status_PlayedFrame = 0;
		System_Status_CurrrentDisplayTime = System_Def_DisplayTime;
		Status_CurrentState = Status_Def_inWorld;
		Menu_Status_CursorPosition = 0;
	}

}

void updateSettingScr() {

	if (Key_Status_KeySet[VK_UP] == Key_Status_Pressed) {
		if (Menu_Status_CursorPosition < 2);
		else if (Menu_Status_CursorPosition == 8 || Menu_Status_CursorPosition == 9) Menu_Status_CursorPosition = 6;
		else if (Menu_Status_CursorPosition >= 10 && Menu_Status_CursorPosition <= 12) Menu_Status_CursorPosition = 7;
		else Menu_Status_CursorPosition -= 2;
	}
	if (Key_Status_KeySet[VK_DOWN] == Key_Status_Pressed) {
		if (8 <= Menu_Status_CursorPosition);
		else if (Menu_Status_CursorPosition == 7) Menu_Status_CursorPosition = 10;
		else Menu_Status_CursorPosition += 2;
	}
	if (Key_Status_KeySet[VK_LEFT] == Key_Status_Pressed) {
		if (Menu_Status_CursorPosition == 0);
		else if (Menu_Status_CursorPosition == 8);
		else Menu_Status_CursorPosition--;
	}
	if (Key_Status_KeySet[VK_RIGHT] == Key_Status_Pressed) {
		if (Menu_Status_CursorPosition == 7);
		else if (Menu_Status_CursorPosition == 12);
		else Menu_Status_CursorPosition++;
	}

	if (Key_Status_KeySet[VK_RETURN] == Key_Status_Pressed) {
		if (Menu_Status_CursorPosition < 8) ChangeKeySetting(Menu_Status_CursorPosition);
		else if (Menu_Status_CursorPosition<12) System_Status_Difficult = Menu_Status_CursorPosition - 8;
		else {
			Status_CurrentState = Status_Def_inMainMenu;
			Menu_Status_CursorPosition = 1;
		}
	}

	if (Key_Status_KeySet[VK_ESCAPE] == Key_Status_Pressed) {
		Status_CurrentState = Status_Def_inMainMenu;
		Menu_Status_CursorPosition = 1;
	}

}

void ChangeKeySetting(int key) {

	//return;

	int *target;
	if (key == 0) target = &Key_Def_Attack;
	else if (key == 1) target = &Key_Def_GoForward;
	else if (key == 2) target = &Key_Def_UseBomb;
	else if (key == 3) target = &Key_Def_GoBackWard;
	else if (key == 4) target = &Key_Def_EnableBooster;
	else if (key == 5) target = &Key_Def_GoLeft;
	else if (key == 6) target = &Key_Def_EnableSlowDown;
	else if (key == 7) target = &Key_Def_GoRight;

	BitBlt(System_DC_FrontDC, 450, 100, Menu_Img_AbleKeysScr.Width, Menu_Img_AbleKeysScr.Height, Menu_Img_AbleKeysScr.DC, 0, 0, SRCCOPY);

	while (true) {
		if (GetAsyncKeyState(0x20) & 0x8000 && !Key_Status_KeySet[0x20]) {//space
			*target = 0x20;
			return;
		}
		for (int i = 0x25; i <= 0x28; i++) {
			if (GetAsyncKeyState(i) & 0x8000 && !Key_Status_KeySet[i]) {//arrowkey
				*target = i;
				return;
			}
		}
		for (int i = 0x41; i <= 0x5A; i++) {
			if (GetAsyncKeyState(i) & 0x8000 && !Key_Status_KeySet[i]) {//planealphabet
				*target = i;
				return;
			}
		}
		for (int i = 0xA0; i <= 0xA5; i++) {
			if (GetAsyncKeyState(i) & 0x8000 && !Key_Status_KeySet[i]) {//CtrlAltShift
				*target = i;
				return;
			}
		}
	}
}

void updateGameOver() {
	removeAllProjectiles();
	if (Key_Status_KeySet[Key_Def_Select] == Key_Status_Pressed) {
		Status_CurrentState = Status_Def_inMainMenu;
		//system("\"c:\\Program Files\\Internet Explorer\\iexplore.exe\" playheroes.com");
	}
}

void updateVictory() {
	if (System_Status_IsVictory == 0) {
		removeAllProjectiles();
		System_Status_PlayerScore = (int)(System_Status_PlayerScore - (double)System_Status_PlayedFrame * 40 / ((System_Status_Difficult + 1)*(System_Status_Difficult + 1)));
		System_Status_IsVictory = 1;
		if (System_Status_PlayerScore < 0) System_Status_PlayerScore = 0;
	}
	if (Key_Status_KeySet[Key_Def_Select] == Key_Status_Pressed) {
		Status_CurrentState = Status_Def_inMainMenu;
		System_Status_IsVictory = 0;
	}
}

//----------------------//

void updateWorld() {
	updatePlayer();
	updateProjectiles();
	updateEnemy();
	updateLevel();
}

void updatePlayer() {
	System_Status_PlayedFrame++;
	updatePlayerMove();
	if (player->invincTime != 0) player->invincTime--;
	if (player->life <= 0) Status_CurrentState = Status_Def_inGameOver;
}

void updatePlayerMove() {
	int xcod = 0;
	int ycod = 0;
	if (Key_Status_KeySet[Key_Def_EnableBooster] == Key_Status_Pressed) {
		player->boost = true;
	}
	if (Key_Status_KeySet[Key_Def_EnableBooster] == Key_Status_Released) {
		player->boost = false;
	}
	if (Key_Status_KeySet[Key_Def_EnableSlowDown] == Key_Status_Pressed) {
		player->slowDown = true;
	}
	if (Key_Status_KeySet[Key_Def_EnableSlowDown] == Key_Status_Released) {
		player->slowDown = false;
	}

	if (Key_Status_KeySet[Key_Def_GoForward]) {
		player->sight = Player_In_UP;
		ycod--;
	}
	if (Key_Status_KeySet[Key_Def_GoBackWard]) {
		player->sight = Player_In_DW;
		ycod++;
	}
	if (Key_Status_KeySet[Key_Def_GoLeft]) {
		player->sight = Player_In_LF;
		xcod--;
	}
	if (Key_Status_KeySet[Key_Def_GoRight]) {
		player->sight = Player_In_RH;
		xcod++;
	}
	if (player->xpos < 0 && xcod < 0) xcod = 0;
	if (player->ypos < 0 && ycod < 0) ycod = 0;
	if (player->xpos > System_Def_ScreenXSize - player->playerPlane->PlaneImg.Width && xcod>0) xcod = 0;
	if (player->ypos > System_Def_ScreenYSize - player->playerPlane->PlaneImg.Height && ycod>0) ycod = 0;

	if ((xcod != 0) && (ycod != 0)) {
		player->frame++;
		double offset = player->playerPlane->movespeed / Root2;

		if (player->slowDown) {
			player->xpos += (int)(xcod * offset * Player_SlowDownSpeed);
			player->ypos += (int)(ycod * offset * Player_SlowDownSpeed);
		}
		else if (player->boost) {
			player->xpos += (int)(xcod * offset * Player_boostSpeed);
			player->ypos += (int)(ycod * offset * Player_boostSpeed);
		}
		else {
			player->xpos += (int)(xcod * offset);
			player->ypos += (int)(ycod * offset);
		}

	}
	else if ((xcod != 0) || (ycod != 0)) {
		player->frame++;
		if (player->slowDown) {
			player->xpos += (int)(xcod * player->playerPlane->movespeed * Player_SlowDownSpeed);
			player->ypos += (int)(ycod * player->playerPlane->movespeed * Player_SlowDownSpeed);
		}
		else if (player->boost) {
			player->xpos += (int)(xcod * (player->playerPlane->movespeed * Player_boostSpeed));
			player->ypos += (int)(ycod * (player->playerPlane->movespeed * Player_boostSpeed));
		}
		else {
			player->xpos += (int)(xcod * player->playerPlane->movespeed);
			player->ypos += (int)(ycod * player->playerPlane->movespeed);
		}
	}

	if (Key_Status_KeySet[Key_Def_Attack] && ((System_UpdatedFrames % player->playerPlane->AtackRate) == 0)) {
		updatePlayerAttack();
	}

	if (Key_Status_KeySet[Key_Def_UseBomb] == Key_Status_Pressed && player->Bomb>0 && !Player_Status_isBombing) {
		
		player->Bomb--;
		System_Status_PlayerScore -= 10000;
		Player_Status_isBombing = true;
	}

	if (Player_Status_isBombing) {
		int srcx = player->xpos + (player->playerPlane->PlaneImg.Width - Projectile_Def_PlayerBullet.image.Width) / 2;
		int srcy = player->ypos + player->playerPlane->PlaneImg.Height/2;
		static double speed = 5;
		static double speedrate = 0.1;
		static double bulletang = 0.005;
		static double srcAng = 0;
		static int frame = 0;
		if (frame<50) {
			speed = EnemySTDelay(srcx, srcy, Projectile_Def_BPlayer, 36, srcAng, 1, speed, speedrate, 3, bulletang);
			bulletang += 0.0005;
			speedrate += 0.05;
			removeAllEnemyProjectiles();
			frame++;
		}
		else if(frame>=50){
			speed = 5;
			bulletang = 0.005;
			speedrate = 0.2;
			frame = 0;
			Player_Status_isBombing = false;
		}
	}

	if (Key_Status_KeySet[Key_Def_Select]) {
		//select someting
	}
	if (Key_Status_KeySet[VK_ESCAPE] == Key_Status_Pressed) {
		Status_CurrentState = Status_Def_inGamePause;

	}
	//for test
	//if (Key_Status_KeySet['1'] == Key_Status_Pressed) player->life++;
	//if (Key_Status_KeySet['2'] == Key_Status_Pressed) player->Bomb++;

	//if (Key_Status_KeySet[Key_Def_SlowMode] == Key_Status_Pressed) {
	//	if (debug_SlowModeEn == 0) {
	//		System_Status_CurrrentDisplayTime = System_Def_DisplayTime * 4;
	//		debug_SlowModeEn = 1;
	//		//mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-3.mp3 volume to 0", 0, 0, 0); //Sound Controll

	//	}
	//	else {
	//		System_Status_CurrrentDisplayTime = System_Def_DisplayTime;
	//		debug_SlowModeEn = 0;
	//		//mciSendString(L"setaudio .\\res\\sound\\BGM\\Boss1-3.mp3  volume to 1000", 0, 0, 0); //Sound Controll
	//	}
	//}
	//if (debug_SlowModeEn == 1) System_Status_PlayerScore -= 65;

	//for test
	if (player->frame / 10 >= 4) player->frame = 0;
}

void updatePlayerAttack() {
	int startx = player->xpos + (player->playerPlane->PlaneImg.Width - Projectile_Def_PlayerBullet.image.Width) / 2;
	int starty = player->ypos - Projectile_Def_PlayerBullet.image.Height / 2;
	double offset = 1;
	if (player->slowDown) offset = 0.03;

	for (int i = 1; i <= player->upgrade; i++) {
		addProjectiles(startx - Projectile_Def_PlayerBullet.image.Width * i, starty, 0.5 + player->playerPlane->variance*i * offset, 
			((player->playerPlane->bulletSpeed)), 0, 0, Projectile_Def_BPlayer);
		addProjectiles(startx + Projectile_Def_PlayerBullet.image.Width * i, starty, 0.5 - player->playerPlane->variance*i * offset, 
			((player->playerPlane->bulletSpeed)), 0, 0, Projectile_Def_BPlayer);
	}

	addProjectiles(startx, starty, 0.5, ((player->playerPlane->bulletSpeed)), 0, 0, Projectile_Def_BPlayer);
}

void updateLevel() {

	if (System_Status_BackgroundYoffset <= 0) System_Status_BackgroundYoffset = 1944;
	else System_Status_BackgroundYoffset -= Level_Status_BackgroundMoveSpeed;

	updateEnemy();

	if(Enemy_Unit_Boss1.isdestroy) Status_CurrentState = Status_Def_inVictory;
}

void updateEnemy() {

	if (!Enemy_Unit_Boss1.isdestroy) {
		UpdateBoss1();
	}

}

void updateProjectiles() {
	if (Projectile_Def_ListHead.Head != NULL) {
		projectile *temp = Projectile_Def_ListHead.Head;
		projectile *deleted = temp;;
		while (temp != NULL) {
			int isdestroy = false;
			double rad = temp->angle * PI * 2;
			temp->xpos += (temp->speed * sin(rad));
			temp->ypos += (temp->speed * cos(rad));
			temp->angle += temp->angleRate;
			temp->angle -= floor(temp->angle);
			temp->speed += temp->speedRate;

			if (temp->xpos + temp->image.Width < 0) {

				isdestroy = true;
			}
			if (temp->ypos + temp->image.Height < 0) {
				isdestroy = true;
			}
			if (temp->xpos > System_Def_ScreenXSize) {
				isdestroy = true;
			}
			if (temp->ypos > System_Def_ScreenYSize) {
				isdestroy = true;
			}
			if (temp->colider.isColide == true) {
				isdestroy = true;
			}
			detectColide(temp);
			if (temp->next == temp) temp->next = NULL;
			if (isdestroy) {
				deleted = temp;
				temp = temp->next;
				removeProjectiles(deleted);
			}
			else {
				temp = temp->next;
			}

		}
		temp = NULL;
		deleted = NULL;
	}
}

void addProjectiles(int startX, int startY, double angle, double bulletSpeed, double angleRate, double Speedrate, int type) {
	projectile *newBullet = (projectile*)malloc(sizeof(projectile));
	if (type == Projectile_Def_BPlayer) {
		newBullet->image = Projectile_Def_PlayerBullet.image;

		newBullet->colider.coliderX1 = 0;
		newBullet->colider.coliderX2 = newBullet->image.Width;
		newBullet->colider.coliderY1 = 0;
		newBullet->colider.coliderY2 = newBullet->image.Height;
		newBullet->dmg = player->playerPlane->dmg;

	}
	else if (type == Projectile_Def_BEnemyRED) {
		newBullet->image = Projectile_Def_enemyBulletRED.image;
		newBullet->colider.coliderX1 = 2;
		newBullet->colider.coliderX2 = 13;
		newBullet->colider.coliderY1 = 2;
		newBullet->colider.coliderY2 = 13;
		newBullet->dmg = 1;
	}
	else if (type == Projectile_Def_BEnemyPUPPLE) {
		newBullet->image = Projectile_Def_enemyBulletPup.image;
		newBullet->colider.coliderX1 = 2;
		newBullet->colider.coliderX2 = 13;
		newBullet->colider.coliderY1 = 2;
		newBullet->colider.coliderY2 = 13;
		newBullet->dmg = 1;
	}

	//D -> R -> U -> L
	//0  0.25  0.5  0.75
	newBullet->xpos = startX;
	newBullet->ypos = startY;
	newBullet->angle = angle;
	newBullet->speed = bulletSpeed;
	newBullet->speedRate = Speedrate;
	newBullet->angleRate = angleRate;

	newBullet->type = type;
	newBullet->colider.isColide = false;

	if (Projectile_Def_ListHead.Head == NULL) {
		newBullet->next = NULL;
		newBullet->prev = NULL;
		Projectile_Def_ListHead.Head = newBullet;

	}
	else {
		Projectile_Def_ListHead.Head->prev = newBullet;
		newBullet->next = Projectile_Def_ListHead.Head;
		newBullet->prev = NULL;
		Projectile_Def_ListHead.Head = newBullet;
	}

	detectColide(newBullet);
	++Projectile_Def_ListHead.num;

}

void removeProjectiles(projectile* target) {
	if (target->prev == NULL && target->next == NULL) {
		Projectile_Def_ListHead.Head = NULL;
		free(target);
		target = NULL;
		--Projectile_Def_ListHead.num;

	}
	else if (target->prev == NULL) {
		target->next->prev = NULL;
		Projectile_Def_ListHead.Head = target->next;
		free(target);
		target = NULL;
		--Projectile_Def_ListHead.num;

	}
	else if (target->next == NULL) {
		target->prev->next = NULL;
		free(target);
		target = NULL;
		--Projectile_Def_ListHead.num;

	}
	else {
		target->next->prev = target->prev;
		target->prev->next = target->next;
		free(target);
		target = NULL;
		--Projectile_Def_ListHead.num;

	}

}

void removeAllEnemyProjectiles() {
	if (Projectile_Def_ListHead.Head != NULL) {
		projectile *temp = Projectile_Def_ListHead.Head;
		projectile *temp2 = temp->next;
		while (temp != NULL) {
			temp2 = temp->next;
			if (temp->type != Projectile_Def_BPlayer)
				removeProjectiles(temp);
			temp = temp2;
		}
	}
}

void removeAllProjectiles() {
	if (Projectile_Def_ListHead.Head != NULL) {
		projectile *temp = Projectile_Def_ListHead.Head;
		while (Projectile_Def_ListHead.Head != NULL) {
			temp = Projectile_Def_ListHead.Head;
			Projectile_Def_ListHead.Head = temp->next;
			free(temp);
			temp = NULL;
		}
	}
	Projectile_Def_ListHead.num = 0;

}

void detectColide(projectile* target) {
	if (target->type == Projectile_Def_BPlayer) {
		if (!Enemy_Unit_Boss1.isdestroy && !Enemy_Unit_Boss1.invincible) {
			int ECX1 = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.colider.coliderX1;
			int ECY1 = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.colider.coliderY1;

			//HEAD
			if (ECY1 + 222 >= target->ypos + target->colider.coliderY1 && ECY1 + 0 <= target->ypos + target->colider.coliderY2) {
				if (ECX1 + 226 <= (target->xpos + target->colider.coliderX2) && ECX1 + 300 >= target->xpos + target->colider.coliderX1) {
					target->colider.isColide = true;
					Enemy_Unit_Boss1.currentHP -= target->dmg * 3;
				}
			}

			//ENGINES
			if (ECY1 + 278 >= target->ypos + target->colider.coliderY1 && ECY1 + 55 <= target->ypos + target->colider.coliderY2) {
				if (ECX1 + 163 <= (target->xpos + target->colider.coliderX2) && ECX1 + 221 >= target->xpos + target->colider.coliderX1) {
					target->colider.isColide = true;
					Enemy_Unit_Boss1.currentHP -= target->dmg;

				}
				else if (ECX1 + 300 <= (target->xpos + target->colider.coliderX2) && ECX1 + 365 >= target->xpos + target->colider.coliderX1) {
					target->colider.isColide = true;
					Enemy_Unit_Boss1.currentHP -= target->dmg;

				}
			}
			
			//WINGS
			if (ECY1 + 230 >= target->ypos + target->colider.coliderY1 && ECY1 + 74 <= target->ypos + target->colider.coliderY2) {
				if (ECX1 + 0 <= (target->xpos + target->colider.coliderX2) && ECX1 + 162 >= target->xpos + target->colider.coliderX1) {
					target->colider.isColide = true;
					Enemy_Unit_Boss1.currentHP -= target->dmg * 0.5;
				}
				else if (ECX1 + 365 <= (target->xpos + target->colider.coliderX2) && ECX1 + 527 >= target->xpos + target->colider.coliderX1) {
					target->colider.isColide = true;
					Enemy_Unit_Boss1.currentHP -= target->dmg * 0.5;

				}
			}

		}
	}
	else {
		if (!player->invincTime) {
			int playercolX1 = player->xpos + player->playerPlane->colider.coliderX1;
			int playercolX2 = player->xpos + player->playerPlane->colider.coliderX2;
			int playercolY1 = player->ypos + player->playerPlane->colider.coliderY1;
			int playercolY2 = player->ypos + player->playerPlane->colider.coliderY2;

			int projcolX1 = (int)target->xpos + target->colider.coliderX1;
			int projcolX2 = (int)target->xpos + target->colider.coliderX2;
			int projcolY1 = (int)target->ypos + target->colider.coliderY1;
			int projcolY2 = (int)target->ypos + target->colider.coliderY2;
			if (playercolX1 <= projcolX2 && playercolX2 >= projcolX1) {
				if (playercolY2 >= projcolY1 && playercolY1 <= projcolY2) {
					target->colider.isColide = true;
					player->life--;
					player->invincTime = 1500 / System_Def_DisplayTime;
					System_Status_PlayerScore -= 50000;
				}
			}
		}
	}
}

void EnemyMvHoriozonSwing() {
	if (Enemy_Unit_Boss1.xpos <= 0 || Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.colider.coliderX2 >= System_Def_ScreenXSize)
		Enemy_Unit_Boss1.MovementSpeed *= -1;
	Enemy_Unit_Boss1.xpos += Enemy_Unit_Boss1.MovementSpeed;
}

void EnemyMvVerticalSwing() {

	if (Enemy_Unit_Boss1.ypos <= 0 || Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.colider.coliderY2 >= System_Def_ScreenXSize)
		Enemy_Unit_Boss1.MovementSpeed *= -1;
	Enemy_Unit_Boss1.ypos += Enemy_Unit_Boss1.MovementSpeed;

}

void EnemyMvChasePlayer() {

	return;
	//unused
	
	int srcX = (Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.colider.coliderX2 / 2);
	int srcY = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.colider.coliderY2;
	int dstX = player->xpos + (player->playerPlane->colider.coliderX1 + player->playerPlane->colider.coliderX2) / 2;
	int dstY = player->ypos + (player->playerPlane->colider.coliderY1 + player->playerPlane->colider.coliderY2) / 2;

	if (abs(srcX - dstX) <= abs(Enemy_Unit_Boss1.MovementSpeed) && abs(srcY - dstY) <= abs(Enemy_Unit_Boss1.MovementSpeed)) return;
	int moveX = (int)(calcDir(srcX, dstX, srcY, dstY, true) * abs(Enemy_Unit_Boss1.MovementSpeed) + 0.5);
	int moveY = (int)(calcDir(srcX, dstX, srcY, dstY, false) * abs(Enemy_Unit_Boss1.MovementSpeed) + 0.5);
	Enemy_Unit_Boss1.xpos += moveX;
	Enemy_Unit_Boss1.ypos += moveY;
}

double EnemySTWasher(int srcX, int srcY, int Bullettype, int num, double srcAngle, double SrcAngleRate,
	double BulletSpeed, double BulletSpeedRate, double BulletAngleRate) {

	for (int i = 1; i <= num; i++) {
		addProjectiles(srcX, srcY, srcAngle + ((double)1 / num)*i, BulletSpeed, BulletAngleRate, BulletSpeedRate, Bullettype);
	}
	return srcAngle + SrcAngleRate;

}

void EnemySTnWay(int srcX, int srcY, int Bullettype, int num, double srcAngle, double AngleRange, double BulletSpeed, double BulletSpeedRate, double BulletAngleRate) {
	if (num == 0) return;

	if (num == 1) {
		addProjectiles(srcX, srcY, srcAngle, BulletSpeed, BulletAngleRate, BulletSpeedRate,  Bullettype);

	}

	for (int i = 0; i < num; i++) {
		addProjectiles(srcX, srcY, srcAngle + AngleRange*((double)i / (num - 1) - 0.5), BulletSpeed, BulletAngleRate, BulletSpeedRate, Bullettype);
	}

}

void EnemySTTrackingPlayer(int srcX, int srcY, int Bullettype, int num, double AngleRange, double BulletSpeed, double BulletSpeedRate, double BulletAngleRate) {

	double angle = -(((atan2((player->ypos + player->playerPlane->colider.coliderY1 - srcY), (player->xpos + player->playerPlane->colider.coliderX1 - srcX)) / PI) / 2) - 0.25);

	if (num == 1) {
		addProjectiles(srcX, srcY, angle, BulletSpeed, BulletAngleRate, BulletSpeedRate, Bullettype);
	}

	for (int i = 0; i < num; i++) {
		addProjectiles(srcX, srcY, angle + AngleRange*((double)i / (num - 1) - 0.5), BulletSpeed, BulletAngleRate, BulletSpeedRate, Bullettype);
	}

}

double EnemySTDelay(int srcX, int srcY, int Bullettype, int num, double srcAngle, double AngleRange, double BulletSpeed, double BulletSpeedRate, 
	double delaySpeed, double BulletAngleRate) {

	for (int i = 0; i < num; i++)
		addProjectiles(srcX, srcY, srcAngle + AngleRange*((double)i / (num) - 0.5), BulletSpeed, BulletAngleRate, BulletSpeedRate, Bullettype);

	return BulletSpeed + delaySpeed;

}

void UpdateBoss1() {

	if (System_Status_Difficult == System_Def_Diff_Easy) {
		if (Enemy_Unit_Boss1.currentHP < 0) Enemy_Unit_Boss1.isdestroy = true;
		else {
			Enemy_Unit_Boss1.invincible = false;
			Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
			Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
			if (Enemy_Unit_Boss1.ypos < 50) Enemy_Unit_Boss1.ypos += 2;

			if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP) {
				if (Level_Projectile_Phase == 0) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_ShootTerm = 10;
					Level_Projectile_Phase = 1;
				}
				if (Enemy_Unit_Boss1.currentHP > Enemy_Unit_Boss1.MaxHP * 0.9) {


					if (Level_Projectile_Phase == 1) {
						Level_Projectile_ShootTerm = 10;
						Level_Projectile_Phase = 2;
					}

					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm == 0) {
						EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 3, 0.2, 3, 0.075, 0);
					}
				}
				else if (Enemy_Unit_Boss1.currentHP > Enemy_Unit_Boss1.MaxHP * 0.8) {
					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm == 0) {
						EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 7, 0.5, 3, 0.075, 0);
					}
				}
				else if (Enemy_Unit_Boss1.currentHP > Enemy_Unit_Boss1.MaxHP * 0.3) {
					if (Level_Projectile_Phase == 2) {
						Level_Projectile_Xoffset = 400;
						Level_Projectile_Yoffset = 100;
						Level_Projectile_BulletNum= 20;
						Level_Projectile_ShootTerm = 100;
						Level_Projectile_Phase = 3;
					}
					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {

						addProjectiles(Level_Projectile_Xoffset, 0, 0, 10, 0, 0, Projectile_Def_BEnemyRED);

						if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.7) {
							addProjectiles(System_Def_ScreenXSize - Level_Projectile_Xoffset, 0, 0, 10, 0, 0, Projectile_Def_BEnemyRED);
						}
						if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.6) {
							addProjectiles(0, Level_Projectile_Yoffset, 0.25, 10, 0, 0, Projectile_Def_BEnemyRED);
						}
						if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.5) {
							addProjectiles(System_Def_ScreenXSize, System_Def_ScreenYSize - Level_Projectile_Yoffset, 0.75, 10, 0, 0, Projectile_Def_BEnemyRED);
						}
						Level_Projectile_Xoffset += 2;
						Level_Projectile_Yoffset += 2;
						if (Level_Projectile_Xoffset > System_Def_ScreenXSize) Level_Projectile_Xoffset = 0;
						if (Level_Projectile_Yoffset > System_Def_ScreenYSize) Level_Projectile_Yoffset = 0;

					}
				}
				else if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.3) {

					if (Level_Projectile_Phase == 3) {
						Level_Projectile_ShootTerm = 60;
						Level_Projectile_SrcAngle = 0.3;
						Level_Projectile_SrcAngleRate = 0.025;
						Level_Projectile_Phase = 4;
					}
					if (Level_Projectile_SrcAngle > 0.75 || Level_Projectile_SrcAngle < 0.25) {
						Level_Projectile_SrcAngleRate *= -1;
						Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
					}
					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm == 0) {
						EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 120, Level_Projectile_SrcAngle, 0.85, 0, 0.05, 0);
						Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;

					}

				}
			}
		}
	}
	else if (System_Status_Difficult == System_Def_Diff_Normal) {
		if (Enemy_Unit_Boss1.currentHP < 0) {
			Enemy_Unit_Boss1.isdestroy = true;
		}
		else {
			Enemy_Unit_Boss1.invincible = false;
			if (Enemy_Unit_Boss1.ypos < 50) Enemy_Unit_Boss1.ypos += 2;
			Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
			Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
			if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP) {
				if (Level_Projectile_Phase == 0) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height-200;
					Level_Projectile_ShootTerm = 20;
					Level_Projectile_Phase = 1;
				}
				if (Enemy_Unit_Boss1.currentHP > Enemy_Unit_Boss1.MaxHP * 0.9) {
					if (Level_Projectile_Phase == 1) {
						Level_Projectile_ShootTerm = 20;
						Level_Projectile_Phase = 2;
					}

					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm == 0) {
						EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 3, 0.1, 6, 0, 0);
					}
				}
				else if (Enemy_Unit_Boss1.currentHP > Enemy_Unit_Boss1.MaxHP * 0.8) {
					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm == 0) {
						EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 7, 0.2, 6, 0, 0);
					}
				}
				else if (Enemy_Unit_Boss1.currentHP > Enemy_Unit_Boss1.MaxHP * 0.3) {
					if (Level_Projectile_Phase == 2) {
						Level_Projectile_Xoffset = 400;
						Level_Projectile_Yoffset = 100;
						Level_Projectile_BulletNum = 25;
						Level_Projectile_ShootTerm = 75;
						Level_Projectile_Phase = 3;
					}
					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {

						addProjectiles(Level_Projectile_Xoffset, 0, 0, 8, 0, 0, Projectile_Def_BEnemyRED);

						if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.7) {
							addProjectiles(1760 - Level_Projectile_Xoffset, 0, 0, 8, 0, 0, Projectile_Def_BEnemyRED);
						}
						if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.6) {
							addProjectiles(0, Level_Projectile_Yoffset, 0.25, 8, 0, 0, Projectile_Def_BEnemyPUPPLE);
						}
						if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.5) {
							addProjectiles(1760, System_Def_ScreenYSize - Level_Projectile_Yoffset, 0.75, 8, 0, 0, Projectile_Def_BEnemyPUPPLE);
						}
						if (Level_Projectile_Xoffset > System_Def_ScreenXSize) Level_Projectile_Xoffset = 0;
						if (Level_Projectile_Yoffset > System_Def_ScreenYSize) Level_Projectile_Yoffset = 0;

					}
						Level_Projectile_Xoffset += 4;
						Level_Projectile_Yoffset += 4;
				}
				else if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.3) {
					if (Level_Projectile_Phase == 3) {
						Level_Projectile_ShootTerm = 40;
						Level_Projectile_SrcAngle = 0.3;
						Level_Projectile_SrcAngleRate = 0.025;
						Level_Projectile_BulletSpeedRate = 0.05;
						Level_Projectile_Phase = 4;
					}
					if (Level_Projectile_SrcAngle > 0.75 || Level_Projectile_SrcAngle < 0.25) {
						Level_Projectile_SrcAngleRate *= -1;
						Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
					}
					if (System_Status_PlayedFrame % Level_Projectile_ShootTerm == 0) {
						EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 
							120, Level_Projectile_SrcAngle, 0.90, 0, Level_Projectile_BulletSpeedRate, 0);
						Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
					}
					if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.15) {
						if (Level_Projectile_Phase == 4) {
							Level_Projectile_Xoffset = 400;
							Level_Projectile_Phase = 5;
						}

						if (System_Status_PlayedFrame % (Level_Projectile_ShootTerm * 2) < (Level_Projectile_ShootTerm / 2)) {
							addProjectiles(Level_Projectile_Xoffset, 20, 0, 10, 0, 0, Projectile_Def_BEnemyPUPPLE);
						}
						Level_Projectile_Xoffset += 5;
						if (Level_Projectile_Xoffset > System_Def_ScreenXSize) Level_Projectile_Xoffset = 0;

					}

					if (Enemy_Unit_Boss1.currentHP < Enemy_Unit_Boss1.MaxHP * 0.10) {
						if (Level_Projectile_Phase == 5) {
							Level_Projectile_BulletSpeedRate = 0.065;
							Level_Projectile_ShootTerm = 35;
							Level_Projectile_Phase = 6;
						}
					}


				}
			}
		}
		
	}
	else if (System_Status_Difficult == System_Def_Diff_Hard) {
		if (Enemy_Unit_Boss1.currentHP < 0) {
			Enemy_Unit_Boss1.isdestroy = true;
		}
		else {
			if (Level_Projectile_Phase == 0) {
				Level_Status_DisplayFrame = 0;
				Level_Projectile_Phase = 1;
			}
			++Level_Status_DisplayFrame;
			//Frame per beat : 25 // 170BPM

			//road start
			if (Level_Status_DisplayFrame < 150) {
				if (Level_Projectile_Phase == 1) {
					Level_Projectile_Xoffset = 0;
					Level_Projectile_BulletSpeed = 15;
					Level_Status_DisplayFrame = 0;
					Level_Status_BackgroundMoveSpeed = 3;
					Level_Projectile_Phase = 2;
				}

				if (Level_Status_DisplayFrame % 30 == 0)
					++Level_Status_BackgroundMoveSpeed;

				addProjectiles(Level_Projectile_Xoffset, 0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyPUPPLE);
				addProjectiles(System_Def_ScreenXSize - Level_Projectile_Xoffset, 0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyPUPPLE);
				Level_Projectile_Xoffset += 5;
			}

			//roadPattern
			else if (Level_Status_DisplayFrame < 3050) {
				Level_Status_BackgroundMoveSpeed = 9;

				int num = (int)((double)Level_Projectile_Xoffset / 13 + 0.5);
				static int randI = 0;
				if (Level_Status_DisplayFrame % (num * 2) < num) {
					addProjectiles(Level_Status_DisplayFrame % num * 13, 0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyRED);
				}
				else if (Level_Status_DisplayFrame % (num * 2) >= num) {
					addProjectiles(System_Def_ScreenXSize - Level_Projectile_Xoffset + Level_Status_DisplayFrame % num * 13,
						0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyRED);
				}
				if (Level_Status_DisplayFrame % (int)(num * 1.5) < 1) {
					for (int i = 0; i < 10; i++) {
						addProjectiles(Level_Projectile_Xoffset + (i + randI) * 13,
							0, 0, 0, 0, 0.4, Projectile_Def_BEnemyRED);

					}
				}
				else {
					randI = rand() % 10 + 1;
				}


				addProjectiles(Level_Projectile_Xoffset, 0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyPUPPLE);
				addProjectiles(System_Def_ScreenXSize - Level_Projectile_Xoffset, 0, 0, Level_Projectile_BulletSpeed,
					0, 0, Projectile_Def_BEnemyPUPPLE);
			}

			//road end
			else if (Level_Status_DisplayFrame < 3200) {
				Enemy_Unit_Boss1.currentHP += Enemy_Unit_Boss1.MaxHP / 150;
				addProjectiles(Level_Projectile_Xoffset, 0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyPUPPLE);
				addProjectiles(System_Def_ScreenXSize - Level_Projectile_Xoffset, 0, 0, Level_Projectile_BulletSpeed, 0, 0, Projectile_Def_BEnemyPUPPLE);
				Level_Projectile_Xoffset -= 5;
				Enemy_Unit_Boss1.ypos += 3;
				Enemy_Unit_Boss1.invincible = false;
			}

			//tracking 7,9way
			else if (Level_Status_DisplayFrame < 4900) {
				if (Level_Projectile_Phase = 2) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_ShootTerm = 200;
					Level_Status_BackgroundMoveSpeed = 3;
					Level_Projectile_BulletWays = 7;
					Level_Projectile_Phase = 3;
				}
				if (Level_Status_DisplayFrame % (Level_Projectile_ShootTerm / 4) == 0)
					EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyPUPPLE,
						Level_Projectile_BulletWays, 0.25, 3, 0.075, 0);
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm == 0)
					EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays + 2, 0.25, 6, 0.075, 0);
			}

			//4way wired + 3way tracking
			else if (Level_Status_DisplayFrame < 8000) {
				if (Level_Projectile_Phase == 3) {

					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_Xoffset = 200;
					Level_Projectile_Yoffset = -50;
					Level_Projectile_BulletWays = 4;
					Level_Projectile_SrcAngle = 0.5;
					Level_Projectile_BulletSpeed = 13;
					Level_Projectile_BulletSpeedRate = 0.01;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_SrcAngleRate = 0.0005;
					Level_Projectile_ShootTerm = 100;
					Level_Projectile_BulletNum = 60;
					Level_Projectile_Phase = 4;
				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {

					EnemySTnWay(Level_Projectile_SrcXPos - Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyPUPPLE, Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);

					EnemySTnWay(Level_Projectile_SrcXPos + Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyPUPPLE, Level_Projectile_BulletWays, -Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);
				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm == 0)
					EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 3, 0.1, 6, 0.05, 0);
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;

			}

			//5way wired				
			else if (Level_Status_DisplayFrame < 11100) {

				if (Level_Projectile_Phase == 4) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height;
					Level_Projectile_Xoffset = 200;
					Level_Projectile_Yoffset = 150;
					Level_Projectile_SrcAngle = 0.5;
					Level_Projectile_SrcAngleRate = 0.0003;
					Level_Projectile_BulletSpeed = 13;
					Level_Projectile_BulletSpeedRate = 0.01;
					Level_Projectile_BulletWays = 5;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_ShootTerm = 200;
					Level_Projectile_BulletNum = 180;
					Level_Projectile_Phase = 5;
				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos - Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyRED, Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);
					EnemySTnWay(Level_Projectile_SrcXPos + Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyRED, Level_Projectile_BulletWays, -Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;

			}

			//Delay hexagon
			else if (Level_Status_DisplayFrame < 12800) {
				if (Level_Projectile_Phase == 5) {
					Level_Projectile_SrcAngleRate = 0.001;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 6;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_BulletSpeed = 7;
					Level_Projectile_BulletSpeedRate = -0.2;
					Level_Projectile_BulletAngleRate = 0.005;
					Level_Projectile_ShootTerm = 50;
					Level_Projectile_BulletNum = 20;
					Level_Projectile_Phase = 6;
				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;

			}

			//reverse DelayHexagon
			else if (Level_Status_DisplayFrame < 14200) {
				if (Level_Projectile_Phase == 6) {
					Level_Projectile_SrcAngleRate = -0.001;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 6;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_BulletSpeed = 7;
					Level_Projectile_BulletSpeedRate = -0.2;
					Level_Projectile_BulletAngleRate = -0.005;
					Level_Projectile_ShootTerm = 50;
					Level_Projectile_BulletNum = 20;
					Level_Projectile_Phase = 7;
				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyPUPPLE,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;

			}

			//Pattern clear
			else if (Level_Status_DisplayFrame < 14300) {}

			// increase bounse pattern
			else if (Level_Status_DisplayFrame < 15900) {
				if (Level_Projectile_Phase == 7) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_SrcAngleRate = 0.0005;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 2;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_BulletSpeed = 10;
					Level_Projectile_BulletSpeedRate = -0.5;
					Level_Projectile_BulletAngleRate = 0.0005;
					Level_Projectile_ShootTerm = 50;
					Level_Projectile_BulletNum = 10;
					Level_Projectile_Phase = 8;
				}

				if (Level_Status_DisplayFrame % 200 == 0) {
					Level_Projectile_BulletWays += 2;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Status_BackgroundMoveSpeed += 2;
				}

				if (Level_Status_DisplayFrame % 7 == 0) {
					++Enemy_Unit_Boss1.ypos;
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
			}

			//6way washer
			else if (Level_Status_DisplayFrame < 22500) {
				if (Level_Projectile_Phase == 8) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 8;
					Level_Projectile_SrcAngle2 = 0.5 / Level_Projectile_BulletWays;
					Level_Projectile_SrcAngleRate = 0.00575;
					Level_Projectile_BulletSpeed = 9;
					Level_Projectile_BulletSpeedRate = -0.3;
					Level_Projectile_BulletAngleRate = 0.00025;
					Level_Projectile_ShootTerm = 5;
					Level_Projectile_Phase = 9;

				}
				if (Level_Status_DisplayFrame < 16000) {
					Enemy_Unit_Boss1.ypos -= 10;
				}

				if (Level_Projectile_SrcYPos < System_Def_ScreenYSize / 2 - 100) ++Level_Projectile_SrcYPos;

				if (Level_Status_DisplayFrame % 200 == 0) {
					Level_Projectile_SrcAngleRate = -Level_Projectile_SrcAngleRate;
				}


				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm == 0) {
					Level_Projectile_SrcAngle = EnemySTWasher(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyPUPPLE,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_Projectile_SrcAngleRate,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
					
					if(Level_Status_DisplayFrame >= 16000)
					if (Level_Status_DisplayFrame % 50 == 0) {
						Level_Projectile_SrcAngle2 = Level_Projectile_SrcAngle;
						EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
							Level_Projectile_BulletWays * 2, 1 - 1.0 / Level_Projectile_BulletWays,
							Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate * 2, Level_Projectile_BulletAngleRate);
					}
				}

			}

			//tracking 7,8way
			else if (Level_Status_DisplayFrame < 24250) {
				if (Level_Projectile_Phase == 9) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_ShootTerm = 50;
					Level_Projectile_BulletWays = 7;
					Enemy_Unit_Boss1.ypos = -400;
					Level_Projectile_Phase = 10;
				}
				if (Level_Status_DisplayFrame < 22650) {
					Enemy_Unit_Boss1.ypos += 3;
					Level_Status_BackgroundMoveSpeed -= 16.0 / 150.0;
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm == 0)
					EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays + 2, 0.25, 5, 0.075, 0);
				if (Level_Status_DisplayFrame % (Level_Projectile_ShootTerm * 2) == Level_Projectile_ShootTerm)
					EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyPUPPLE,
						Level_Projectile_BulletWays, 0.25, 3, 0.075, 0);
			}

			//5way wired
			else if (Level_Status_DisplayFrame < 25750) {
				if (Level_Projectile_Phase == 10) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_Xoffset = 200;
					Level_Projectile_Yoffset = -50;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_SrcAngleRate = 0.0003;
					Level_Projectile_BulletSpeed = 13;
					Level_Projectile_BulletSpeedRate = 0.01;
					Level_Projectile_BulletWays = 5;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_ShootTerm = 100;
					Level_Projectile_BulletNum = 80;
					Level_Projectile_Phase = 11;
				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos - Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyRED, Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);
					EnemySTnWay(Level_Projectile_SrcXPos + Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyRED, Level_Projectile_BulletWays, -Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;

			}

			//4way wired + 3way tracking
			else if (Level_Status_DisplayFrame < 27250) {
				if (Level_Projectile_Phase == 11) {
					Level_Projectile_SrcAngleRate = 0.0005;
					Level_Projectile_BulletWays = 4;

					Level_Projectile_ShootTerm = 100;
					Level_Projectile_BulletNum = 60;
					Level_Projectile_Phase = 12;
				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {

					EnemySTnWay(Level_Projectile_SrcXPos - Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyPUPPLE, Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);

					EnemySTnWay(Level_Projectile_SrcXPos + Level_Projectile_Xoffset, Level_Projectile_SrcYPos - Level_Projectile_Yoffset,
						Projectile_Def_BEnemyPUPPLE, Level_Projectile_BulletWays, -Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, 0);
				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm == 0)
					EnemySTTrackingPlayer(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED, 3, 0.1, 6, 0.05, 0);
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
			}

			//Delay hexagon
			else if (Level_Status_DisplayFrame < 28750) {
				if (Level_Projectile_Phase == 12) {
					Level_Projectile_SrcAngleRate = 0.001;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 6;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_BulletSpeed = 7;
					Level_Projectile_BulletSpeedRate = -0.2;
					Level_Projectile_BulletAngleRate = 0.001;
					Level_Projectile_ShootTerm = 50;
					Level_Projectile_BulletNum = 20;
					Level_Projectile_Phase = 13;
				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
			}

			//Pattern clear
			else if (Level_Status_DisplayFrame < 28850) {}

			// increase bounse pattern
			else if (Level_Status_DisplayFrame < 30500) {
				if (Level_Projectile_Phase == 13) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_SrcAngleRate = 0.0005;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 2;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Projectile_BulletSpeed = 10;
					Level_Projectile_BulletSpeedRate = -0.5;
					Level_Projectile_BulletAngleRate = 0.005;
					Level_Projectile_ShootTerm = 50;
					Level_Projectile_BulletNum = 10;
					Level_Projectile_Phase = 14;
				}

				if (Level_Status_DisplayFrame % 200 == 0) {
					Level_Projectile_BulletWays += 2;
					Level_projectile_SrcAngleRange = 1 - 1.0 / Level_Projectile_BulletWays;
					Level_Status_BackgroundMoveSpeed += 2;
				}

				if (Level_Status_DisplayFrame % 7 == 0) {
					++Enemy_Unit_Boss1.ypos;
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					EnemySTnWay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
						Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
				}
				Level_Projectile_SrcAngle += Level_Projectile_SrcAngleRate;
			}

			// BeBrave
			else if (Level_Status_DisplayFrame < 33800) {
				if (Level_Projectile_Phase == 14) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_SrcAngle = 0;
					Level_Projectile_BulletWays = 8;
					Level_Projectile_SrcAngleRate = 0.00575;
					Level_Projectile_BulletSpeed = 3;
					Level_Projectile_BulletSpeedRate = 0.2;
					Level_Projectile_BulletAngleRate = 0.005;
					Level_Projectile_ShootTerm = 100;
					Level_Projectile_BulletNum = 30;
					Level_Projectile_Phase = 15;

				}
				if (Level_Status_DisplayFrame < 30540) {
					Enemy_Unit_Boss1.ypos -= 10;
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
				}
				else if (Level_Status_DisplayFrame < 30600) {
					Enemy_Unit_Boss1.ypos += 2;
				}

				if (Level_Projectile_SrcYPos < System_Def_ScreenYSize / 2 - 100) Level_Projectile_SrcYPos += 3;

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm == 0) {
					Level_Projectile_SrcAngle += 0.01;

				}

				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {
					Level_Projectile_BulletSpeed =  EnemySTDelay(Level_Projectile_SrcXPos, Level_Projectile_SrcYPos, Projectile_Def_BEnemyRED,
						Level_Projectile_BulletWays, Level_Projectile_SrcAngle, 1, Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate,
						0.125, Level_Projectile_BulletAngleRate);
					Level_Projectile_BulletAngleRate += 0.0005;
					Level_Projectile_BulletSpeedRate += 0.05;
				}
				else {
					Level_Projectile_BulletSpeed = 4;
					Level_Projectile_BulletSpeedRate = 0.2;
					Level_Projectile_BulletAngleRate = 0.005;
				}
			}
			
			//MovingRoad
			else if (Level_Status_DisplayFrame < 36875) {
				if (Level_Projectile_Phase == 15) {
					Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
					Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;
					Level_Projectile_Xoffset = 175;
					Level_Projectile_Yoffset = -50;
					Level_Projectile_SrcAngle = 0.75;
					Level_Projectile_SrcAngle2 = 0.25;
					Level_projectile_SrcAngleRange = 0.5;
					Level_Projectile_BulletWays = 12;
					Level_Projectile_SrcAngleRate = 0.0;
					Level_Projectile_BulletSpeed = 5;
					Level_Projectile_BulletSpeedRate = 0.2;
					Level_Projectile_BulletAngleRate = 0;
					Level_Projectile_ShootTerm = 3;
					Level_Projectile_BulletNum = 1;
					Level_Projectile_Phase = 16;

				}
				if (Level_Status_DisplayFrame % Level_Projectile_ShootTerm < Level_Projectile_BulletNum) {

				EnemySTnWay(Level_Projectile_SrcXPos - Level_Projectile_Xoffset, Level_Projectile_SrcYPos + Level_Projectile_Yoffset,
					Projectile_Def_BEnemyRED, Level_Projectile_BulletWays, Level_Projectile_SrcAngle, Level_projectile_SrcAngleRange,
					Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);

				EnemySTnWay(Level_Projectile_SrcXPos + Level_Projectile_Xoffset, Level_Projectile_SrcYPos + Level_Projectile_Yoffset,
					Projectile_Def_BEnemyRED, Level_Projectile_BulletWays, Level_Projectile_SrcAngle2, Level_projectile_SrcAngleRange,
					Level_Projectile_BulletSpeed, Level_Projectile_BulletSpeedRate, Level_Projectile_BulletAngleRate);
				}

				EnemyMvHoriozonSwing();
				Level_Projectile_SrcXPos = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;
				Level_Projectile_SrcYPos = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height - 200;

			}
			
			//Pattern clear
			else if (Level_Status_DisplayFrame < 37000) {
				if (Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5 < System_Def_ScreenXSize / 2)
					Enemy_Unit_Boss1.xpos++;
				else if (Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5 > System_Def_ScreenXSize / 2)
					Enemy_Unit_Boss1.xpos--;
				Enemy_Unit_Boss1.ypos += 2;
			}
			
			//run away
			else {
				if (Enemy_Unit_Boss1.ypos > -450)
					Enemy_Unit_Boss1.ypos -= 10;
				else
					if(Level_Status_BackgroundMoveSpeed > 0 && Level_Status_DisplayFrame % 10 == 0)
					Level_Status_BackgroundMoveSpeed--;
			}
			
			//patt end
		}



	}
		//VHard
	else {
		if (Enemy_Unit_Boss1.currentHP < 0) Enemy_Unit_Boss1.isdestroy = true;
		if (Enemy_Unit_Boss1.ypos < 50) Enemy_Unit_Boss1.ypos += 1;
		static int srcx = 0;
		static int srcy = 0;
		static int term = 10;
		srcy = Enemy_Unit_Boss1.ypos + Enemy_Unit_Boss1.image.Height;
		srcx = Enemy_Unit_Boss1.xpos + Enemy_Unit_Boss1.image.Width / 2 / Enemy_Unit_Boss1.numOfFrame - 5;

		if (Enemy_Unit_Boss1.ypos >= 50) {

			if (Level_Projectile_Phase == 0) {
				Level_Projectile_Xoffset = 0;
				Level_Projectile_Yoffset = 0;
				Level_Projectile_Xoffset2 = System_Def_ScreenXSize;
				Level_Projectile_Yoffset2 = System_Def_ScreenYSize;
				Level_Projectile_SrcAngle = 0;
				Level_Projectile_BulletSpeed = 1000;
				Level_Projectile_ShootTerm =100;
				Level_Projectile_BulletNum = 0;
				Level_Projectile_Phase = 1;
			}
			if (true) {

				//beBrave
				static double speed = 0.000;
				static double speedrate = 0;
				static double bulletang = 0.005;
				static double srcAng = 0;
				if (Level_Projectile_Xoffset == 0) {
					Level_Projectile_Phase = 0;
					Level_Projectile_Xoffset = 1;
				}

				if (true) {
					if (Level_Projectile_Phase == 0) {
						speed = EnemySTDelay(srcx, srcy, Projectile_Def_BEnemyRED, 5, srcAng, 1, speed, speedrate, 0.05, bulletang);
					}
					else {
						speed = EnemySTDelay(srcx, srcy, Projectile_Def_BEnemyPUPPLE, 6, srcAng, 1, speed, speedrate, 0.05, bulletang);
					}

					bulletang += 0.0005;
					speedrate += 0;
				}
				else {
					speed = 0;
					bulletang = 0.005;
					speedrate = 0;

				}
				srcAng += 0.00025;

			}

			
		}

	}

	if (System_UpdatedFrames % 5 == 0) Enemy_Unit_Boss1.currentframe++;

	if (Enemy_Unit_Boss1.currentframe >= Enemy_Unit_Boss1.numOfFrame) {
		Enemy_Unit_Boss1.currentframe = 0;
	}

}