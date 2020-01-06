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
map<wstring, Field_p> field_list;
bool KeyDown(char key);
bool want_stop = true;
HWND hWndEdit, hList;
HWND hBtnSave, hBtnLoad, hBtnSim, hBtnGen, hBtnDel;
const int btnSim = 4221, btnGen = 4222, btnSave = 4223, btnLoad = 4224, btnDel = 4225, listbox = 4226;
WCHAR buff[1024];
wstring selected_item = L"";

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

    //if (want_stop)
    //    br.SetColor(Color(0, 0, 0));
    //else
    //    br.SetColor(Color(255, 0, 0));
    //FontFamily fontFamily(L"Courier New");
    //Font font(&fontFamily, reg_w / 25, FontStyleBold, UnitPixel);
    //graphics.DrawString(Tip, sizeof(Tip)/2, &font, PointF(padding, reg_w + 2 * padding), &br);
}


void OnPaint(HWND hWnd)
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
    graphics.SetClip(&region_1);
    field->Draw(&graphics);
    if (robot->fin_dist2 < finish_dist2)
        robot->Simulate(&graphics);
    graphics.SetClip(&region_2);
    robot->DrawQT(&graphics);
    graphics.SetClip(&region_3);
    mipmap.Draw(&graphics);

    EndPaint(hWnd, &ps);
}

void UpdateList()
{
    SendMessage(hList, LB_RESETCONTENT, 0, 0);
    for (auto& f : field_list)
        SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)(LPSTR)(f.first.c_str()));
    selected_item = L"";
}

void OnStart()
{
    field = make_shared<Field>();
    robot = make_shared<Robot>();
}
void LoadList()
{
    wifstream in("SavedFields.txt", ios::in);
    if (in.is_open())
    {
        while (!in.eof())
        {
            wstring name;
            int size;
            in >> name >> size;
            if (name.size() > 0)
            {
                field_list[name] = make_shared<Field>();
                field_list[name]->Clear(); // мы загрузили поле, но оно заполнилось чем-то
                float x, y, r;
                for (int i = 0; i < size; ++i)
                {
                    in >> x >> y >> r;
                    field_list[name]->AddZone(xy(x, y), r);
                }
            }
        }
    }
    UpdateList();
}
void OnGenerate(HWND hWnd)
{
    field = make_shared<Field>();
    robot = make_shared<Robot>();
    OnPaint(hWnd);
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
    graphics.Clear(Color::White);
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

/*
name point1.x point1.y radius1 point2.x point2.y radius2 ...
name point1.x point1.y radius1 point2.x point2.y radius2 ...
*/
void SaveFile()
{
    // записывам map в наш файл
    wofstream out("SavedFields.txt", ios::out);
    if (out.is_open())
    {
        wstring s = L"";
        for (auto& f : field_list)
        {
            s = f.first + L" " + to_wstring(f.second->obstacles.size()) + L" ";
            for (auto& o : f.second->obstacles)
                s = s + to_wstring(o->point.x) + L" " + to_wstring(o->point.y) + L" " + to_wstring(o->radius) + L" ";
            out << s << endl;
        }
        //fprintf_s(out, "[%s] %s", name, s);
        out.close();
    }
}

void OnSave()
{
    // заносим в map наше поле, если в textbox есть что-то
    GetWindowText(hWndEdit, buff, 1024);
    wstring name = buff; // получаем текст из бокса
    if (name.size() > 0)
    {
        // занести в map наше текущее поле
        field_list[name] = field;
    }
    SaveFile();
    UpdateList();
}

void OnLoad(HWND hWnd)
{
    if (selected_item.length() > 0)
    {
        field.reset();
        field = make_shared<Field>(*field_list[selected_item]);
        robot = make_shared<Robot>();
    }
    OnPaint(hWnd);
}

void OnDelete()
{
    if (selected_item.length() > 0)
    {
        field_list.erase(selected_item);
        OnSave();
    }
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
        reg_w * 4 + padding * 4,  // initial x size
        reg_w * 2 + padding * 4,  // initial y size
        NULL,                     // parent window handle
        NULL,                     // window menu handle
        hInstance,                // program instance handle
        NULL);                    // creation parameters

    HWND hWndLabel = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("static"), TEXT("Save as:"), WS_CHILD | WS_VISIBLE | BS_CENTER | ES_CENTER, 
        reg_w * 3 + padding * 3, padding, 70, 25, 
        hWnd, NULL, NULL, NULL);
    hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), TEXT(""), WS_CHILD | WS_VISIBLE, 
        reg_w * 3 + padding * 3 + 70, padding, 80, 25, 
        hWnd, NULL, NULL, NULL);
    hBtnSave = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("button"), TEXT("Save"), WS_CHILD | WS_VISIBLE,
        reg_w * 3 + padding * 3 + 150, padding, 100, 25, 
        hWnd, (HMENU)btnSave, NULL, NULL);

    hList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("listbox"), NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD | LBS_NOTIFY,
        reg_w * 3 + padding * 3, padding + 35, 250, 400, 
        hWnd, (HMENU)listbox, hInstance, NULL);

    hBtnLoad = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("button"), TEXT("Load"), WS_CHILD | WS_VISIBLE,
        reg_w * 3 + padding * 3, padding + 435, 120, 25, 
        hWnd, (HMENU)btnLoad, NULL, NULL);
    hBtnDel = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("button"), TEXT("Delete"), WS_CHILD | WS_VISIBLE,
        reg_w * 3 + padding * 3 + 130, padding + 435, 120, 25,
        hWnd, (HMENU)btnDel, NULL, NULL);
    hBtnGen = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("button"), TEXT("Generate"), WS_CHILD | WS_VISIBLE,
        reg_w * 3 + padding * 3, padding + 465, 120, 50, 
        hWnd, (HMENU)btnGen, NULL, NULL);
    hBtnSim = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("button"), TEXT("Simulate"), WS_CHILD | WS_VISIBLE,
        reg_w * 3 + padding * 3 + 130, padding + 465, 120, 50, 
        hWnd, (HMENU)btnSim, NULL, NULL);
    LoadList();
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

void SelectItem()
{
    SendMessage(hList, LB_GETTEXT, SendMessage(hList, LB_GETCURSEL, 0, 0), (LPARAM)buff);
    selected_item = buff;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HDC          hdc;
    switch (message)
    {
    case WM_CREATE:
        OnStart();
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case listbox:
            if (HIWORD(wParam) == LBN_SELCHANGE)
                SelectItem();
            return 0;
        case btnGen:
            OnGenerate(hWnd);
            return 0;
        case btnSim:
            OnSimulate(hWnd);
            return 0;
        case btnLoad:
            OnLoad(hWnd);
            return 0;
        case btnDel:
            OnDelete();
            return 0;
        case btnSave:
            OnSave();
            return 0;
        }
    case WM_PAINT:
        OnPaint(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
