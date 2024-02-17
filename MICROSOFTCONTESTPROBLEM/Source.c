/*
microsoft problem: October 3, 1986
Create a multitasking window consisting of 4 windows. the first window must show a series of increasing 
numbers, the second window show a series of increasing prime numbers, the third must show the Fibonacci 
series. all three window must clear itself when numbers reach its bottom or implement a scrollbar.
the forth wondow must display circle of random radii. The program must terminate with a press of escape key.

*/

#include <Windows.h>
#include <math.h>
#include <process.h>

typedef struct {
	HWND hwnd;
	int cxClient;
	int cyClient;
	int cyChar;
	BOOL bKill;
}PARAMS, *PPARAMS;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrivInstance, PSTR szCmdLine, int iCmdShow) {
	static TCHAR szAppName[] = TEXT("Multi2");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIconW(NULL, IDI_WINLOGO);
	wndclass.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	//register class
	if (!RegisterClass(&wndclass)) {
		MessageBox(NULL, TEXT("This program requires windows!!"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(
		szAppName,
		TEXT("MultiTasking Demo"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//check bottom function
int CheckBottom(HWND hwnd, int cyClient, int cyChar, int iLine) {
	if (iLine * cyChar + cyChar > cyClient) {
		InvalidateRect(hwnd, NULL, TRUE);
		UpdateWindow(hwnd);
		iLine = 0;
	}
	return iLine;
}

//window 1: Display increasing sequence of numbers

void Thread1(PVOID pvoid) {
	HDC hdc;
	int iNum = 0, iLine = 0;
	PPARAMS pParams;
	TCHAR szBuffer[16];

	pParams = (PPARAMS) pvoid;

	while (!pParams->bKill) {
		if (iNum < 0)
			iNum = 0;
		iLine = CheckBottom(pParams->hwnd, pParams->cyClient,pParams->cyChar, iLine);
		hdc = GetDC(pParams->hwnd);

		TextOut(hdc, 0, iLine * pParams->cyChar, szBuffer, wsprintf(szBuffer, TEXT("%d"), iNum++));
		ReleaseDC(pParams->hwnd, hdc);
		iLine++;
		Sleep(100);
	}
	_endthread();
}

LRESULT CALLBACK WndProc1(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static PARAMS params;

	switch (message) {
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread1, 0, &params);
		return 0;

	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//window 2: incresing sequence of prime numbers

void Thread2(PVOID pvoid) {
	HDC hdc;
	int iNum = 1, iLine = 0, i, iSqrt;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams = (PPARAMS) pvoid;

	while (!pparams->bKill) {
		do {
			if (++iNum < 0)
				iNum = 0;
			iSqrt = (int)sqrt(iNum);
			for (i = 2; i <= iSqrt; i++) {
				if (iNum % i == 0)
					break;
			}
		} while (i <= iSqrt);
		iLine = CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc = GetDC(pparams->hwnd);
		TextOut(hdc, 0, iLine * pparams->cyChar, szBuffer, wsprintf(szBuffer, TEXT("%d"), iNum));
		ReleaseDC(pparams->hwnd, hdc);
		iLine++;
		Sleep(100);
	}
	_endthread();
}

LRESULT CALLBACK WndProc2(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static PARAMS params;

	switch (message) {
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread2, 0, &params);
		return 0;

	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//window 3: display increasing sequence of fibonacci number

void Thread3(PVOID pvoid) {
	HDC hdc;
	int iNum1 = 0, iNum2 = 1, iNext, iLine = 0;
	PPARAMS pparams;
	TCHAR szBuffer[16];

	pparams = (PPARAMS)pvoid;

	while (!pparams->bKill) {
		iNext = iNum1 + iNum2;
		if (iNext < 0) {
			iNum1 = 0;
			iNum2 = 1;
			iNext = 1;
		}

		iLine = CheckBottom(pparams->hwnd, pparams->cyClient, pparams->cyChar, iLine);
		hdc = GetDC(pparams->hwnd);
		TextOut(hdc, 0, iLine * pparams->cyChar, szBuffer, wsprintf(szBuffer, TEXT("%d"), iNum1));
		ReleaseDC(pparams->hwnd, hdc);

		iNum1 = iNum2;
		iNum2 = iNext;
		iLine++;
		Sleep(100);
	}
	_endthread();
}

LRESULT CALLBACK WndProc3(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static PARAMS params;

	switch (message) {
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread3, 0, &params);
		return 0;

	case WM_SIZE:
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}


//window 4: display circle of random radii

void Thread4(PVOID pvoid) {
	HDC hdc;
	int iDiameter;
	PPARAMS pparams;

	pparams = (PPARAMS)pvoid;

	while (!pparams->bKill) {
		InvalidateRect(pparams->hwnd, NULL, TRUE);
		UpdateWindow(pparams->hwnd);
		iDiameter = rand() % (max(1, min(pparams->cxClient, pparams->cyClient)));
		hdc = GetDC(pparams->hwnd);
		Ellipse(hdc, (pparams->cxClient - iDiameter) / 2,
			(pparams->cyClient - iDiameter) / 2,
			(pparams->cxClient + iDiameter) / 2,
			(pparams->cyClient + iDiameter) / 2);

		ReleaseDC(pparams->hwnd, hdc);
		Sleep(100);
	}
	_endthread();
}

LRESULT CALLBACK WndProc4(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static PARAMS params;

	switch (message) {
	case WM_CREATE:
		params.hwnd = hwnd;
		params.cyChar = HIWORD(GetDialogBaseUnits());
		_beginthread(Thread4, 0, &params);
		return 0;

	case WM_SIZE:
		params.cxClient = LOWORD(lParam);
		params.cyClient = HIWORD(lParam);
		return 0;

	case WM_DESTROY:
		params.bKill = TRUE;
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

//main window to create child window
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HWND hwndChild[4];
	static TCHAR* szChildClass[] = { TEXT("child1"), TEXT("child2"), TEXT("child3"), TEXT("child4") };
	static WNDPROC ChildProc[] = { WndProc1, WndProc2, WndProc3, WndProc4 };
	HINSTANCE hInstance;
	int i, cxClient, cyClient;
	WNDCLASS wndclass;

	switch(message) {
	case WM_CREATE:
		hInstance = (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE);

		wndclass.style = CS_HREDRAW | CS_VREDRAW;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;

		for (i = 0; i < 4; i++) {
			wndclass.lpfnWndProc = ChildProc[i];
			wndclass.lpszClassName = szChildClass[i];
			RegisterClass(&wndclass);

			hwndChild[i] = CreateWindow(szChildClass[i], NULL, WS_CHILDWINDOW | WS_BORDER | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)i, hInstance, NULL);
		}
		
		return 0;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		for (i = 0; i < 4; i++) {
			MoveWindow(hwndChild[i], (i % 2) * cxClient / 2, (i > 1) * cyClient / 2, cxClient / 2, cyClient / 2, TRUE);
		}
		return 0;

	case WM_CHAR:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

