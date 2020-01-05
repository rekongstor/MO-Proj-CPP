#include "stdafx.h"
#include "field.h"
#include "quadtree.h"
#include "robot.h"
#include "push.h"
#include <iostream>
#include <fstream>
#pragma comment (lib,"Gdiplus.lib")



#ifndef true_random
volatile Rnd rnd;
#endif // !true_random
Field_p field;
Robot_p robot;

set<char> Keys;
Mipmap mipmap;
array<Robot_p, gen_size> bots[threads];
bool KeyDown(char key);
bool want_stop = true;
HWND hWndEdit, hList;
int btnSim = 4221, btnGen = 4222, btnSave = 4223;

void Prepare(Graphics& graphics)
{
    Pen pen(Color(0, 0, 0), 2.f);
    SolidBrush br(Color(255, 255, 255));
    // рисуем границы наших регионов
    graphics.DrawRectangle(&pen, r2);
    graphics.FillRectangle(&br, r2);
    graphics.DrawRectangle(&pen, r1);
    graphics.FillRectangle(&br, r1);
    br.SetColor(Color::Gray);
    graphics.DrawRectangle(&pen, r3);
    graphics.FillRectangle(&br, r3);

    if (want_stop)
        br.SetColor(Color(0, 0, 0));
    else
        br.SetColor(Color(255, 0, 0));
    FontFamily fontFamily(L"Courier New");
    Font font(&fontFamily, reg_w / 25, FontStyleBold, UnitPixel);
    graphics.DrawString(Tip, sizeof(Tip)/2, &font, PointF(padding, reg_w + 2 * padding), &br);
}

void OnStart(HWND hWnd)
{
    PAINTSTRUCT  ps;
    HDC          hdc;
    InvalidateRect(hWnd, NULL, TRUE);
    hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    Prepare(graphics);
    Region region_1(r1); // первый регион рисования
    Region region_2(r2); // второй регион рисования
    Region region_3(r3); // второй регион рисования


    EndPaint(hWnd, &ps);
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
    Region region_3(r3); // второй регион рисования

    // рисуем первый регион
    // поле с препятствиями
    graphics.SetClip(&region_1);
    field = make_shared<Field>();
    robot = make_shared<Robot>();
    field->Draw(&graphics);

    EndPaint(hWnd, &ps);
}

void OnSimulate(HWND hWnd)
{
    want_stop = false;
    PAINTSTRUCT  ps;
    HDC          hdc;
    robot = Robot_p(new Robot);

    vector<thread> thds;
    array<Container, threads> thr_cont;
	for (int i = 0; i < threads; ++i)
	{
		thr_cont[i].mip = &mipmap;

        // для всех
		thr_cont[i].bots = &bots[i];
	}
    mipmap.Clear();
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
            robot = make_shared<Robot>(*best->best);
            robot->q.Split(robot->coord.x, robot->coord.y);
        }
#ifdef alwaysdraw
        InvalidateRect(hWnd, NULL, TRUE);
        hdc = BeginPaint(hWnd, &ps);
        Graphics graphics(hdc);
        graphics.Clear(Color::Wheat);
        Prepare(graphics);
        Region region_1(r1); // первый регион рисования
        Region region_2(r2); // второй регион рисования
        Region region_3(r3); // третий регион рисования
        // рисуем первый регион
        // поле с препятствиями
        graphics.SetClip(&region_1);
        field->Draw(&graphics);
        // рисуем лучшего
        robot->Simulate(&graphics);
        // рисуем его квадродерево
        graphics.SetClip(&region_2);
        robot->DrawQT(&graphics);
        // обновляем зоны отталкивания, но если не дошло (чтобы не запороть)
        if (!fin)
            for (auto& bs : thr_cont)
                for (auto& b : *bs.bots)
                    bs.mip->Put(b->coord, b->c.normal);
        graphics.SetClip(&region_3);
        best->mip->Draw(&graphics);
        EndPaint(hWnd, &ps);
#endif
    }
    want_stop = true;

    InvalidateRect(hWnd, NULL, TRUE);
    hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    graphics.Clear(Color::Wheat);
    Prepare(graphics);
    Region region_1(r1); // первый регион рисования
    Region region_2(r2); // второй регион рисования
    Region region_3(r3); // третий регион рисования
    // рисуем первый регион
    // поле с препятствиями
    graphics.SetClip(&region_1);
    field->Draw(&graphics);
    // рисуем лучшего
    robot->Simulate(&graphics);
    // рисуем его квадродерево
    graphics.SetClip(&region_2);
    robot->DrawQT(&graphics);
    graphics.SetClip(&region_3);
    mipmap.Draw(&graphics);
    EndPaint(hWnd, &ps);


}

map<wstring, Field_p> field_list;
/*
name point1.x point1.y radius1 point2.x point2.y radius2 ...
name point1.x point1.y radius1 point2.x point2.y radius2 ...
*/
WCHAR buff[1024];
void OnDownload()
{
    // заносим в map наше поле, если в textbox есть что-то
    GetWindowText(hWndEdit, buff, 1024);
    wstring name = buff; // получаем текст из бокса
    if (name.size() > 0)
    {
        // занести в map наше текущее поле
        field_list[name] = field;
    }
    // записывам map в наш файл
    wofstream out("SavedFields.txt", ios::out);
    wstring s = L"";
    if (out.is_open())
    {
        GetWindowText(hWndEdit, buff, 1024);
        wstring name = buff; // L"name ";//здесь нужно запрашивать имя
        for (auto& f : field_list)
        {
            wstring tmpl = to_wstring(o->point.x) + L" " + to_wstring(o->point.y) + L" " + to_wstring(o->radius) + L" ";
            s = s + tmpl;
        }
        //fprintf_s(out, "[%s] %s", name, s);
        out << L"[" << name << s << L"]" << L"\n" << std::endl;
    }
    out.close();
}

void OnLoad()
{

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
    wndClass.hIcon = (HICON)LoadImage(NULL, TEXT("robot.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
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
        reg_w * 3 + 6 * padding,  // initial x size
        reg_w + 8 * padding,      // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    HWND hWndLabel = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("static"), TEXT("Save field with name: "),
        WS_CHILD | WS_VISIBLE, padding - 1, 2, 150,
        padding - 4, hWnd, NULL, NULL, NULL);
    hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), TEXT(""),
        WS_CHILD | WS_VISIBLE, padding + 150, 2, 100,
        padding - 4, hWnd, NULL, NULL, NULL);

    hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD,
        reg_w + padding - 100, reg_w + 2 * padding, 100, 80, hWnd, (HMENU)502, hInstance, NULL);

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
    case WM_CREATE:
        OnGenerate(hWnd);
        return 0;
    case WM_KEYDOWN:
    case WM_KEYUP:
        if (KeyDown('G'))
            OnGenerate(hWnd);
        if (KeyDown('S'))
            OnSimulate(hWnd);
        if (KeyDown('D'))
            OnDownload();
        if (KeyDown('L'))
            OnLoad();

        //if (GetKeyState(VK_SHIFT) & 0x8000)
        //{
        //    if (KeyDown('1')) // размножить на разные клавиши
        //        Event(hWnd, OnSimulate); // сюда вызов нужной функции
        //}
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
