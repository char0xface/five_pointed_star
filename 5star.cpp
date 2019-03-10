//wingw64
//gcc.exe  *.cpp -o 5star.exe   -s -Os -mwindows  -std=c++11 -lws2_32 -lgdiplus -lsetupapi -lgdi32 -luser32 -lkernel32 -lcomctl32 -lcomdlg32 -lstdc++   -static

#define _WIN32_WINNT  0x0600
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <process.h>
#include <unistd.h>

//#define line(dc,x0,y0,x1,y1)  do{ MoveToEx((dc),(x0),(y0),NULL); LineTo((dc),(x1),(y1));}while(0)
#define DP(A,...) do{printf(A,##__VA_ARGS__);}while(0)
#define min(a,b) ((a)>(b)?(b):(a))

class   FIVESTAR
{
private:
    HWND hwnd;
    int radius;
    int color_red,color_green,color_blue;
    int  direction;
    int speed;
public:

    FIVESTAR( int  dir,int diameter,int red=0,int green=0,int blue=0,int sp=1)
    {
        speed=sp;
        radius=diameter/2;
        direction=dir;
        color_red=red;
        color_green=green;
        color_blue=blue;
    }

    unsigned int draw()
    {
        POINT pt0= {radius,radius};
        float step=0;
        float twopi=2*3.1415926;
        float delta=twopi/5.0;
        int order[]= {0,2,4,1,3,0};
        POINT pt[6];
        COLORREF  rgb=0;
        DWORD t0=GetTickCount();
        int t=0;
        srand(GetTickCount());
        while(true)
        {
            Sleep(3);
            if(GetTickCount()-t0>15)
            {

                HDC hdc=GetDC(hwnd);
                HDC hmem=CreateCompatibleDC(hdc);
                HBITMAP hbmp=CreateCompatibleBitmap(hdc,radius*3,radius*3);
                SelectObject(hmem,hbmp);

                if(t%500==0)
                {
                    if(color_red!=0 || color_green!=0 || color_blue!=0) rgb=RGB(color_red,color_green,color_blue);
                    else    rgb=RGB((rand()%255)+1,(rand()%255)+1,(rand()%255)+1);
                }
                t++;

                step+=0.012*speed*direction;
                if(step>twopi)step-=twopi;
                for(unsigned int i=0; i<sizeof(order)/sizeof(*order); i++)
                {
                    float dt= delta*order[i]+step;
                    pt[i].x=pt0.x+radius*cos(dt);
                    pt[i].y=pt0.y+radius*sin(dt);
                }
                //HPEN  hpen=CreatePen(PS_DASH,0,rgb);
                HPEN  hpen=CreatePen(PS_SOLID,2,rgb);
                SelectObject(hmem,hpen);
                Polyline(hmem,pt,sizeof(order)/sizeof(*order));
                BitBlt(hdc,0,0,radius*3,radius*3,hmem,0,0,SRCCOPY	);

                DeleteObject(hpen);
                DeleteObject(hbmp);
                DeleteDC(hmem);
                ReleaseDC(hwnd,hdc);
                t0=GetTickCount();
            }
        }
        return 0;
    }

    static unsigned int __stdcall mythread(void* p)
    {
        return   ((FIVESTAR* )p)->draw();;
    }
    void show(HWND hw)
    {
        hwnd=hw;
        HANDLE h = (HANDLE)_beginthreadex(NULL, 0,FIVESTAR:: mythread, this, 0, NULL);
        CloseHandle(h);
    }
};

class XWIN
{
private:
    HWND hwnd;
    int radius;
    int color_red,color_green,color_blue;
public:
    HWND get_hwnd()
    {
        return hwnd;
    }
    XWIN()
    { }
    ~XWIN()
    {}
    void show()
    {
        ShowWindow (hwnd, SW_SHOW) ;
        UpdateWindow (hwnd) ;
    }

    DWORD run()
    {
        MSG msg;
        while (GetMessage (&msg, NULL, 0, 0))
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }
        return msg.message;
    }


    static LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_KEYDOWN:
            if(wParam==VK_ESCAPE)  PostQuitMessage(0);
            break;
        case WM_LBUTTONDOWN:
            SendMessage(hwnd,WM_NCLBUTTONDOWN,HTCAPTION,lParam);
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps ;
            BeginPaint (hwnd, &ps) ;
            EndPaint (hwnd, &ps) ;
            return 0 ;
        }
        case WM_DESTROY:
            PostQuitMessage (0) ;
            return 0 ;
        }
        return DefWindowProc (hwnd, message, wParam, lParam) ;
    }

    XWIN(const TCHAR *title_name,const TCHAR *class_name,DWORD ex_style=0,DWORD style=WS_OVERLAPPEDWINDOW,int x=CW_USEDEFAULT,int y=CW_USEDEFAULT,int w=CW_USEDEFAULT,int h=CW_USEDEFAULT)
    {
        radius= min(w,h)/2;
        WNDCLASSEX  wcex;
        wcex.cbSize=sizeof(wcex);
        wcex.style=CS_HREDRAW | CS_VREDRAW ;
        wcex.style|= CS_DBLCLKS;
        wcex.lpfnWndProc   =XWIN::WndProc ;
        wcex.cbClsExtra    = 0 ;
        wcex.cbWndExtra    = 0 ;
        wcex.hInstance     = GetModuleHandle(NULL) ;
        wcex.hIcon         = NULL;
        wcex.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
        wcex.hbrBackground = (HBRUSH) GetStockObject (NULL_BRUSH) ;
        wcex.lpszMenuName  = NULL ;
        wcex.lpszClassName = class_name ;
        wcex.hIconSm=LoadIcon (NULL, IDI_QUESTION) ;
        RegisterClassEx (&wcex);
        hwnd = CreateWindowEx ( ex_style,class_name,title_name,style,   x,y,w,h,   NULL,NULL,GetModuleHandle(NULL),NULL) ;
    }

};

int main(int cnt,char **list)
{
    int diameter=100;
    int dir=1;
    int xr=0,xg=0,xb=0;
    int speed=1;
    opterr=0;
    int ch;
    bool quit=false;
    while (!quit && (ch = getopt(cnt, list, "nd:r:g:b:s:")) != EOF)
    {
        switch (ch)
        {
        case 'n':
            dir=-1;
            break;
        case 'd':
        {
            int d=atoi(optarg);
            if(d>=18 && d<=2000)   diameter=d;
        }
        break;
        case 'r':
            xr=atoi(optarg);
            break;
        case 'g':
            xg=atoi(optarg);
            break;
        case 'b':
            xb=atoi(optarg);
            break;
        case 's':
        {
            int sp=atoi(optarg);
            if(sp>0 && sp<=20) speed=sp;
        }
        break;
        case '?':
        {
            char tip[100];
            wsprintf(tip,"%s [-n] [-s speed:1~20] [-r red:1~255] [-g greed:1~255] [-b blue:1~255] [-d diameter:18~2000]\n\n-n\tanticlockwise\n-r\tred color\n-g\tgreen color\n-b\tblue color\n-d\tthe diameter of the star",list[0]);
            MessageBox(NULL,tip,"usage",MB_OK|MB_ICONINFORMATION);
            quit=true;
        }
        break;
        }
    }

    RECT rcdesk;
    SystemParametersInfo(SPI_GETWORKAREA,0,&rcdesk,0);
    srand(GetTickCount()+rand());
    POINT px= {GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)};
    XWIN win(TEXT("five pointed star"),TEXT("5star"),WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST,( WS_CLIPCHILDREN | WS_POPUPWINDOW ) & ~WS_BORDER,px.x-diameter,rcdesk.bottom-diameter,diameter,diameter);
    SetLayeredWindowAttributes( win.get_hwnd(), RGB( 0, 0, 0 ), 255, LWA_COLORKEY );//transparent background color RGB( 0, 0, 0 )
    // SetLayeredWindowAttributes( win.get_hwnd(), RGB( 0, 0, 0 ),255, LWA_ALPHA);//not transparent
    FIVESTAR fivestar(dir,diameter,xr,xg,xb,speed);
    fivestar.show(win.get_hwnd());
    win.show();
    return  win.run();

}

