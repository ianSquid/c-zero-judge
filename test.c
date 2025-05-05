#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH        400
#define HEIGHT       600
#define MAX_ENEMIES  10
#define MAX_BULLETS  5
#define TARGET_FPS   60
#define FRAME_TIME   (1000.0 / TARGET_FPS)
#define NUM_STARS    100
#define MAX_EXPL     20
#define EXP_FRAMES   10

typedef enum { GS_MENU, GS_PLAYING, GS_PAUSED, GS_GAMEOVER } GameState;
typedef struct { int x, y; BOOL active; } Bullet;
typedef struct { int x, y, speed; BOOL active; } Enemy;
typedef struct { int x, y, speed; } Star;
typedef struct { int x, y, frame; BOOL active; } Explosion;

static int playerX, score, lives;
static Bullet bullets[MAX_BULLETS];
static Enemy enemies[MAX_ENEMIES];
static Star stars[NUM_STARS];
static Explosion expls[MAX_EXPL];
static GameState gs = GS_MENU;
static int enemySpawnCooldown = 0;

static HBITMAP hMemBmp;
static HDC hMemDC;
static LARGE_INTEGER freq, lastTime;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitGame(void);
void UpdateGame(void);
void DrawGame(HDC);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    WNDCLASS wc = {0};
    HWND hwnd;
    MSG msg;
    HDC hdc;
    LARGE_INTEGER now;
    double elapsed;

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = "PlaneGame";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    hwnd = CreateWindow("PlaneGame","射飛機遊戲",
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, nShow);

    hdc = GetDC(hwnd);
    hMemDC = CreateCompatibleDC(hdc);
    hMemBmp = CreateCompatibleBitmap(hdc, WIDTH, HEIGHT);
    SelectObject(hMemDC, hMemBmp);
    ReleaseDC(hwnd, hdc);

    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);
    InitGame();

    while (1) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        QueryPerformanceCounter(&now);
        elapsed = (now.QuadPart - lastTime.QuadPart) * 1000.0 / freq.QuadPart;
        if (elapsed < FRAME_TIME) { Sleep(1); continue; }
        lastTime = now;

        if (gs == GS_PLAYING) UpdateGame();
        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void InitGame(void) {
    int i;
    srand((unsigned)time(NULL));
    playerX = WIDTH/2 - 20;
    score   = 0;
    lives   = 3;
    gs      = GS_MENU;
    enemySpawnCooldown = 0;
    for (i = 0; i < MAX_BULLETS; i++) bullets[i].active = FALSE;
    for (i = 0; i < MAX_ENEMIES; i++) enemies[i].active = FALSE;
    for (i = 0; i < NUM_STARS; i++) {
        stars[i].x = rand() % WIDTH;
        stars[i].y = rand() % HEIGHT;
        stars[i].speed = 1 + rand() % 3;
    }
    for (i = 0; i < MAX_EXPL; i++) expls[i].active = FALSE;
}

void UpdateGame(void) {
    int i, b, e;
    for (i = 0; i < NUM_STARS; i++) {
        stars[i].y += stars[i].speed;
        if (stars[i].y >= HEIGHT) {
            stars[i].y = 0;
            stars[i].x = rand() % WIDTH;
        }
    }

    int target = 2 + score / 300;
    if (target > 3) target = 3;
    if (--enemySpawnCooldown <= 0) {
        for (i = 0; i < MAX_ENEMIES && target > 0; i++) {
            if (!enemies[i].active) {
                enemies[i].active = TRUE;
                enemies[i].x = rand() % (WIDTH - 40);
                enemies[i].y = -20;
                enemies[i].speed = 1 + score / 300;
                enemySpawnCooldown = 40;
                break;
            }
        }
    }

    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].y += enemies[i].speed;
            if (enemies[i].y > HEIGHT) {
                enemies[i].active = FALSE;
                lives--;
                if (lives <= 0) gs = GS_GAMEOVER;
            }
        }
    }

    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].y -= 12;
            if (bullets[i].y < -10) bullets[i].active = FALSE;
        }
    }

    for (b = 0; b < MAX_BULLETS; b++) {
        if (!bullets[b].active) continue;
        for (e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;
            if (bullets[b].x < enemies[e].x + 40 &&
                bullets[b].x + 5 > enemies[e].x &&
                bullets[b].y < enemies[e].y + 20 &&
                bullets[b].y + 10 > enemies[e].y) {
                bullets[b].active = FALSE;
                enemies[e].active = FALSE;
                score += 100;
                for (i = 0; i < MAX_EXPL; i++) {
                    if (!expls[i].active) {
                        expls[i].active = TRUE;
                        expls[i].x = enemies[e].x + 20;
                        expls[i].y = enemies[e].y + 10;
                        expls[i].frame = 0;
                        break;
                    }
                }
                break;
            }
        }
    }

    for (i = 0; i < MAX_EXPL; i++) {
        if (expls[i].active) {
            expls[i].frame++;
            if (expls[i].frame > EXP_FRAMES) expls[i].active = FALSE;
        }
    }
}

void DrawGame(HDC memDC) {
    int i;
    RECT r = {0,0,WIDTH,HEIGHT};
    char buf[64];

    FillRect(memDC, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetBkMode(memDC, TRANSPARENT);
    SetTextColor(memDC, RGB(255, 255, 255));
    SelectObject(memDC, GetStockObject(SYSTEM_FONT));

    for (i = 0; i < NUM_STARS; i++)
        SetPixel(memDC, stars[i].x, stars[i].y, RGB(200, 200, 200));

    if (gs == GS_MENU) {
        TextOut(memDC, 130, 240, "射飛機遊戲", 12);
        TextOut(memDC, 110, 280, "← → 移動   空白鍵射擊", 24);
        TextOut(memDC, 120, 320, "P 暫停 / Enter 開始", 22);
        return;
    }

    HBRUSH brE = CreateSolidBrush(RGB(255, 50, 50));
    SelectObject(memDC, brE);
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active)
            Ellipse(memDC, enemies[i].x, enemies[i].y,
                             enemies[i].x+40, enemies[i].y+20);
    }
    DeleteObject(brE);

    HBRUSH brB = CreateSolidBrush(RGB(50, 255, 255));
    SelectObject(memDC, brB);
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active)
            Rectangle(memDC, bullets[i].x, bullets[i].y,
                                bullets[i].x+5, bullets[i].y+10);
    }
    DeleteObject(brB);

    HPEN pPl = CreatePen(PS_SOLID,1,RGB(255,255,50));
    HBRUSH bPl = CreateSolidBrush(RGB(255,255,50));
    SelectObject(memDC, pPl);
    SelectObject(memDC, bPl);
    POINT tri[3] = {
        {playerX+20, HEIGHT-30},
        {playerX,    HEIGHT-10},
        {playerX+40, HEIGHT-10}
    };
    Polygon(memDC, tri, 3);
    DeleteObject(pPl);
    DeleteObject(bPl);

    for (i = 0; i < MAX_EXPL; i++) {
        if (expls[i].active) {
            int r = expls[i].frame * 3;
            HPEN pEx = CreatePen(PS_SOLID, 1, RGB(255,200,50));
            SelectObject(memDC, pEx);
            Ellipse(memDC,
                expls[i].x - r, expls[i].y - r,
                expls[i].x + r, expls[i].y + r);
            DeleteObject(pEx);
        }
    }

    wsprintf(buf, "Score: %d", score);
    TextOut(memDC, 20, 10, buf, lstrlen(buf));
    wsprintf(buf, "Lives: %d", lives);
    TextOut(memDC, WIDTH - 100, 10, buf, lstrlen(buf));

    if (gs == GS_PAUSED)
        TextOut(memDC, 160, 300, "Paused", 6);
    if (gs == GS_GAMEOVER) {
        TextOut(memDC, 140, 260, "Game Over!", 11);
        TextOut(memDC, 120, 290, "按 Enter 鍵再玩一次", 20);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int i;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (msg) {
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_RETURN:
            if (gs == GS_MENU || gs == GS_GAMEOVER) {
                InitGame();
                gs = GS_PLAYING;
            }
            break;
        case 'P':
            if (gs == GS_PLAYING) gs = GS_PAUSED;
            else if (gs == GS_PAUSED) gs = GS_PLAYING;
            break;
        case VK_LEFT:
            if (gs == GS_PLAYING && playerX > 0) playerX -= 10;
            break;
        case VK_RIGHT:
            if (gs == GS_PLAYING && playerX < WIDTH-40) playerX += 10;
            break;
        case VK_SPACE:
            if (gs == GS_PLAYING) {
                for (i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].active = TRUE;
                        bullets[i].x = playerX + 18;
                        bullets[i].y = HEIGHT - 40;
                        break;
                    }
                }
            }
            break;
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        DrawGame(hMemDC);
        BitBlt(hdc, 0, 0, WIDTH, HEIGHT, hMemDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        break;
    case WM_DESTROY:
        DeleteObject(hMemBmp);
        DeleteDC(hMemDC);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

