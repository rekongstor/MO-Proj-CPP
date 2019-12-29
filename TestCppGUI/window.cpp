#include "stdafx.h"
#include "field.h"
#include "quadtree.h"
#include "robot.h"
#pragma comment (lib,"Gdiplus.lib")

Field field;
Robot robot;

set<char> Keys;


void Prepare(Graphics& graphics)
{
    Pen pen(Color(0, 0, 0), 2.f);
    SolidBrush br(Color(255, 255, 255));
    // рисуем границы наших регионов
    graphics.DrawRectangle(&pen, r1);
    graphics.FillRectangle(&br, r1);
    graphics.DrawRectangle(&pen, r2);
    graphics.FillRectangle(&br, r2);

    br.SetColor(Color(0, 0, 0));
    FontFamily fontFamily(L"Courier New");
    Font font(&fontFamily, reg_w / 25, FontStyleBold, UnitPixel);
    graphics.DrawString(Tip, sizeof(Tip)/2, &font, PointF(padding, reg_w + 2 * padding), &br);
}

void OnGenerate(HDC hdc)
{
    Graphics graphics(hdc);
    Prepare(graphics);
    Region region_1(r1); // первый регион рисования
    Region region_2(r2); // второй регион рисования

    // рисуем первый регион
    // поле с препятствиями
    graphics.SetClip(&region_1);
    field = Field();
    field.Draw(&graphics);
}

void OnSimulate(HDC hdc)
{
    Graphics graphics(hdc);
    Prepare(graphics);
    Region region_1(r1); // первый регион рисования
    Region region_2(r2); // второй регион рисования

    // рисуем первый регион
    // поле с препятствиями
    graphics.SetClip(&region_1);
    field.Draw(&graphics);

    // draw robot movement

    // draw quadtree
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

void Event(HWND hWnd, void(*callback)(HDC))
{
    PAINTSTRUCT  ps;
    HDC          hdc;

    InvalidateRect(hWnd, NULL, TRUE);
    hdc = BeginPaint(hWnd, &ps);
    callback(hdc);
    EndPaint(hWnd, &ps);
}

//void OnPaint2(HDC hdc)
//{
//    Graphics graphics(hdc);
//    int green = 0;
//    if (GetKeyState('A') & 0x8000)
//        green = 255;
//    SolidBrush br(Color(255- green, green, 0));
//    graphics.FillEllipse(&br, rand() % 500, rand() % 500, 100, 100);
//}
//VOID OnPaint(HDC hdc)
//{
//    Graphics graphics(hdc);
//    //Pen      pen(Color(255, 0, 0, 255));
//    SolidBrush  solidBrush(Color(255, 0, 0, 255));
//    //graphics.DrawLine(&pen, 0, 0, 200, 100);
//    //
//    //graphics.DrawEllipse(&pen, 0, 0, 200, 100);
//    //graphics.FillEllipse(&brush, 200, 100, 200, 100);
//
//    //FontFamily  fontFamily(L"Times New Roman");
//    //Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
//    //PointF      pointF(10.0f, 20.0f);
//
//    //graphics.DrawString(L"Hello World!", -1, &font, pointF, &brush);
//    Point point(60, 10);
//    // Assume that the variable "point" contains the location
//    // of the most recent click.
//    // To simulate a hit, assign (60, 10) to point.
//    // To simulate a miss, assign (0, 0) to point.
//    Rect rect(0,0,700,1000);
//// Create a path that consists of a single polygon.
//    Point polyPoints[] = { Point(10, 10), Point(150, 10),
//       Point(100, 75), Point(100, 150) };
//    GraphicsPath path;
//    path.AddPolygon(polyPoints, 4);
//    // Construct a region based on the path.
//    //Region region(&path);
//    Region region(rect);
//    // Draw the outline of the region.
//    Pen pen(Color(255, 0, 0, 0));
//    graphics.DrawPath(&pen, &path);
//    // Set the clipping region of the Graphics object.
//    graphics.SetClip(&region);
//    // Draw some clipped strings.
//    FontFamily fontFamily(L"Arial");
//    Font font(&fontFamily, 36, FontStyleBold, UnitPixel);
//    graphics.DrawString(L"A Clipping Region", 20, &font,
//        PointF(15, 25), &solidBrush);
//    graphics.DrawString(L"A Clipping Region", 20, &font,
//        PointF(15, 68), &solidBrush);
//    
//}
//
//VOID OnPaint2(HDC hdc)
//{
//    Graphics graphics(hdc);
//    //Pen      pen(Color(255, 0, 0, 255));
//    SolidBrush  solidBrush(Color(255, 0, 0, 255));
//    //graphics.DrawLine(&pen, 0, 0, 200, 100);
//    //
//    //graphics.DrawEllipse(&pen, 0, 0, 200, 100);
//    //graphics.FillEllipse(&brush, 200, 100, 200, 100);
//
//    //FontFamily  fontFamily(L"Times New Roman");
//    //Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
//    //PointF      pointF(10.0f, 20.0f);
//
//    //graphics.DrawString(L"Hello World!", -1, &font, pointF, &brush);
//    Point point(60, 10);
//    // Assume that the variable "point" contains the location
//    // of the most recent click.
//    // To simulate a hit, assign (60, 10) to point.
//    // To simulate a miss, assign (0, 0) to point.
//    Rect rect(0, 0, 700, 1000);
//    // Create a path that consists of a single polygon.
//    Point polyPoints[] = { Point(10, 10), Point(150, 10),
//       Point(100, 75), Point(100, 150) };
//    GraphicsPath path;
//    path.AddPolygon(polyPoints, 4);
//    // Construct a region based on the path.
//    //Region region(&path);
//    Region region(rect);
//    // Draw the outline of the region.
//    Pen pen(Color(255, 0, 0, 0));
//    graphics.DrawPath(&pen, &path);
//    // Set the clipping region of the Graphics object.
//    graphics.SetClip(&region);
//    // Draw some clipped strings.
//    FontFamily fontFamily(L"Arial");
//    Font font(&fontFamily, 36, FontStyleBold, UnitPixel);
//    graphics.DrawString(L"SAS", 20, &font,
//        PointF(15, 25), &solidBrush);
//    graphics.DrawString(L"SASAS", 20, &font,
//        PointF(15, 68), &solidBrush);
//}

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
