#include "stdafx.h"
#include "field.h"
#include "quadtree.h"
#include "robot.h"
#include "push.h"
#pragma comment (lib,"Gdiplus.lib")

Field* field;
Robot* robot;

set<char> Keys;
Mipmap mipmap[threads];
bool KeyDown(char key);
bool want_stop = true;

void Prepare(Graphics& graphics)
{
    Pen pen(Color(0, 0, 0), 2.f);
    SolidBrush br(Color(255, 255, 255));
    // рисуем границы наших регионов
    graphics.DrawRectangle(&pen, r1);
    graphics.FillRectangle(&br, r1);
    graphics.DrawRectangle(&pen, r2);
    graphics.FillRectangle(&br, r2);

    if (want_stop)
        br.SetColor(Color(0, 0, 0));
    else
        br.SetColor(Color(255, 0, 0));
    FontFamily fontFamily(L"Courier New");
    Font font(&fontFamily, reg_w / 25, FontStyleBold, UnitPixel);
    graphics.DrawString(Tip, sizeof(Tip)/2, &font, PointF(padding, reg_w + 2 * padding), &br);
}

void OnGenerate(HWND hWnd)
{
    PAINTSTRUCT  ps;
    HDC          hdc;
    InvalidateRect(hWnd, NULL, TRUE);
    hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    Prepare(graphics);
    Region region_1(r1); // первый регион рисования
    Region region_2(r2); // второй регион рисования

    // рисуем первый регион
    // поле с препятствиями
    graphics.SetClip(&region_1);
    delete field;
    delete robot;
    field = new Field();
    robot = new Robot();
    field->Draw(&graphics);

    EndPaint(hWnd, &ps);
}

void OnSimulate(HWND hWnd)
{
    want_stop = false;
    PAINTSTRUCT  ps;
    HDC          hdc;
    delete robot;
    robot = new Robot();

    vector<thread> thds;
    array<Container, threads> thr_cont;
	for (int i = 0; i < threads; ++i)
	{
		thr_cont[i].mip = &mipmap[i];
		mipmap[i].Clear();
	}
    Container* best = &thr_cont[0];
    bool fin = false;
    
    int tries = max_tries;
    while ((robot->fin_dist2 > finish_dist2) && --tries > 0 && !want_stop)
    {
        for (auto& t : thr_cont)
        {
#ifdef threading
            thds.push_back(thread(
                [&](void)
            {
#endif
                Simulate(t);
#ifdef threading
            }));
#endif
        }

        for (auto& t : thds)
            t.join();
        thds.clear();

        best = &thr_cont[0];

        for (auto& b : thr_cont)
        {
            if (b.best->fin_dist2 < finish_dist2)
            {
                fin = true;
                best = &b;
            }
        }

        for (auto& p : thr_cont)
        {
            if (fin)
            {
                if ((best->best->fin_dist2 > p.best->fin_dist2) && (best->best->life_time > p.best->life_time))
                    best = &p;
            }
            else
            {
                if (best->best->fin_dist2 > p.best->fin_dist2)
                    best = &p;
            }
        }
        if (best)
        {
            delete robot;
            robot = new Robot(*best->best);
        }
        for (auto& b : thr_cont)
            delete b.best;

        InvalidateRect(hWnd, NULL, TRUE);
        hdc = BeginPaint(hWnd, &ps);
        Graphics graphics(hdc);
        Prepare(graphics);
        Region region_1(r1); // первый регион рисования
        Region region_2(r2); // второй регион рисования

        // рисуем первый регион
        // поле с препятствиями
        graphics.SetClip(&region_1);
        field->Draw(&graphics);
        // рисуем лучшего
        robot->Simulate(&graphics);
        // рисуем его квадродерево
        graphics.SetClip(&region_2);
        robot->DrawQT(&graphics);

        EndPaint(hWnd, &ps);
    }
    want_stop = true;

    InvalidateRect(hWnd, NULL, TRUE);
    hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    Prepare(graphics);
    Region region_1(r1); // первый регион рисования
    Region region_2(r2); // второй регион рисования

    // рисуем первый регион
    // поле с препятствиями
    graphics.SetClip(&region_1);
    field->Draw(&graphics);
    // рисуем лучшего
    robot->Simulate(&graphics);
    // рисуем его квадродерево
    graphics.SetClip(&region_2);
    robot->DrawQT(&graphics);
	best->mip->Draw(&graphics);
    EndPaint(hWnd, &ps);


}

bool KeyDown(char key)
{
    if (Keys.find(key) == Keys.end()) // если клавиша не была нажата
    {
        if (GetKeyState(key) & 0x8000) // если клавиша нажата сейчас
        {
            Keys.insert(key); // запоминаем нажатие
            return true;
        }
    }
    else// если клавиша была нажата
        if (!(GetKeyState(key) & 0x8000)) // если клавиша не нажата сейчас
            Keys.erase(key); // забываем нажатие
    return false;
}

void Event(HWND hWnd, void(*callback)(HWND))
{

    PAINTSTRUCT  ps;
    HDC          hdc;
    InvalidateRect(hWnd, NULL, TRUE);
    hdc = BeginPaint(hWnd, &ps);
    callback(hWnd);
    EndPaint(hWnd, &ps);
}


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
    HWND                hWnd;
    MSG                 msg;
    WNDCLASS            wndClass;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR           gdiplusToken;


    // Initialize GDI+.
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = TEXT("Curvy robot");

    RegisterClass(&wndClass);

    hWnd = CreateWindow(
        TEXT("Curvy robot"),   // window class name
        TEXT("Curvy robot"),  // window caption
        WS_OVERLAPPED | WS_SYSMENU,      // window style
        CW_USEDEFAULT,            // initial x position
        CW_USEDEFAULT,            // initial y position
        reg_w * 2 + 4 * padding,  // initial x size
        reg_w + 8 * padding,      // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    switch (message)
    {
    case WM_ACTIVATE:
        Event(hWnd, OnGenerate);
        return 0;
    case WM_KEYDOWN:
    case WM_KEYUP:
        if (KeyDown('G'))
            Event(hWnd, OnGenerate);
        if (KeyDown('S'))
            Event(hWnd, OnSimulate);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
