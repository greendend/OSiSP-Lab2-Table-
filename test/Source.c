#include <Windows.h>
#include <time.h>
#include <stdio.h>
#define ROWS 6
#define COLS 6

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void MyRedrawWindow(HWND hWnd);
void DrawTable(HWND hWnd);
char* GetRandomString();
char* DevideString(char* str, SIZE size, HDC hdc);
void CreateMatrix();
void DestroyMatrix();
int GetSecondArgument(char* str);

static char*** matrix;
int cols = ROWS;
int rows = COLS;
int maxrowsheight[ROWS] = { 0 };

char* tempstr;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	if (lpCmdLine)
	{
		cols = atoi(lpCmdLine); //Функция atoi преобразует строку string в целое значение типа int
		rows = GetSecondArgument(lpCmdLine); 
	}

	//cols = COLS;
	//rows = ROWS;

	tempstr = lpCmdLine;
	WNDCLASSEX wcex; HWND hWnd; MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "HelloWorldClass";
	wcex.hIconSm = wcex.hIcon;
	RegisterClassEx(&wcex);

	hWnd = CreateWindow("HelloWorldClass", "Task 2",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0,
		CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
	{
		MyRedrawWindow(hWnd); //msg на изменение размера окна
		break;
	}
	case WM_CREATE:
	{
		srand(time(NULL));
		CreateMatrix();
		break;
	}

	case WM_PAINT:
	{
		DrawTable(hWnd); //перерисовка
		break;
	}
	case WM_DESTROY:
	{
		DestroyMatrix();	//выход
		PostQuitMessage(0);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = 450;  //ширина
		mmi->ptMinTrackSize.y = 250;
		//	mmi->ptMaxTrackSize.x = 360;
		//	mmi->ptMaxTrackSize.y = 240;
		return 0;
	}
	default:
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return 0;
}

void DestroyMatrix()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			free(matrix[i][j]);
		}
		free(matrix[i]);
	}
	free(matrix);
}

void CreateMatrix()
{
	//двумерный массив строк
	matrix = (char***)calloc(rows, sizeof(char**));
	if (matrix == NULL)
		exit(1);

	FILE* file;
	char* s; // считанные строки
	file = fopen("source.txt", "r");
	char* real_tail;
	int i = 0;

	for (int i = 0; i < rows; i++)
	{
		matrix[i] = (char**)calloc(cols, sizeof(char*));
		if (matrix[i] != NULL)
		{
			for (int j = 0; j < cols; j++)
			{
				char result_string[256];
				if (fgets(result_string, sizeof(result_string), file))
				{
					result_string[strlen(result_string) - 1] = '\0';
					matrix[i][j] = (char*)calloc(256, sizeof(char));
					strcpy(matrix[i][j], result_string);
					//matrix[i][j] = result_string;
				}
				if (matrix[i][j] == NULL)
				{
					matrix[i][j] = "smirnoff";
				}
				//matrix[i][j] = GetRandomString();
			}
		}
		else
		{
			free(matrix);
			exit(1);
		}
	}
	fclose(file);
}

void DrawTable(HWND hWnd)
{
	PAINTSTRUCT ps;
	const HDC hdcWin = BeginPaint(hWnd, &ps);

	HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 100, 0));
	HPEN hOldPen = (HPEN)SelectObject(hdcWin, hPen);
	HBRUSH hBrush = GetStockObject(NULL_BRUSH);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcWin, hBrush);

	static int fontheight = 20;
	HFONT hFont = CreateFont(fontheight, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Amatic SC"));
	HFONT hOld = (HFONT)SelectObject(hdcWin, hFont);

	SetTextJustification(hdcWin, 0, 0);
	SetBkMode(hdcWin, TRANSPARENT);

	RECT rect;
	GetClientRect(hWnd, &rect);
	int colswidth = abs(rect.right - rect.left) / cols; //нп-лв
	int windowheight = abs(rect.top - rect.bottom);
	SIZE size;
	int tableheight = 0;
	tableheight = 0;

	while (!(((tableheight) >= windowheight) /*&& ( (tableheight - 100) <= windowheight)*/))
	{
		tableheight = 0;
		hFont = CreateFont(fontheight, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Amatic SC"));
		hOld = (HFONT)SelectObject(hdcWin, hFont);

		for (int i = 0; i < rows; i++)
		{
			maxrowsheight[i] = 0;
			for (int j = 0; j < cols; j++)
			{
				RECT cellrect;
				cellrect.left = 0;
				cellrect.right = colswidth;
				cellrect.bottom = 0;
				cellrect.top = 0;

				SIZE textsize;
				textsize.cx = colswidth;
				char* temp = DevideString(matrix[i][j], textsize, hdcWin);
				DrawText(hdcWin, temp, lstrlen(temp), &cellrect, DT_CALCRECT);
				int maxcolheight = abs(cellrect.top - cellrect.bottom);

				if (maxrowsheight[i] < maxcolheight)
					maxrowsheight[i] = maxcolheight;
				free(temp);
			}
			tableheight += maxrowsheight[i];
		}

		if (tableheight != windowheight)
		{
			if (tableheight > windowheight)
				fontheight -= 1;
			if (tableheight < windowheight)
				fontheight += 1;
		}

		SelectObject(hdcWin, hOld);
		DeleteObject(hFont);
	}

	hFont = CreateFont(fontheight, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Amatic SC"));
	hOld = (HFONT)SelectObject(hdcWin, hFont);

	//------------
	int rowsheight;
	int currrowy = 0;
	for (int i = 0; i < rows; i++)
	{
		rowsheight = maxrowsheight[i];
		for (int j = 0; j < cols; j++)
		{
			if (lstrlen(matrix[i][j]) <= 0)
				continue;

			RECT outrect;
			outrect.top = currrowy;
			outrect.bottom = currrowy + rowsheight;
			outrect.left = j * colswidth;
			outrect.right = (j + 1) * colswidth;

			SIZE textsize;
			textsize.cy = 100;
			textsize.cx = colswidth;

			char* temp = DevideString(matrix[i][j], textsize, hdcWin);
			//------------------------------------------------------------------------------------------
			DrawText(hdcWin, temp, lstrlen(temp), &outrect, DT_LEFT);
			Rectangle(hdcWin, j * colswidth, currrowy, (j + 1) * colswidth, currrowy + rowsheight);
			//------------------------------------------------------------------------------------------
			free(temp);
		}
		currrowy += maxrowsheight[i];
	}


	SelectObject(hdcWin, hOldPen);
	DeleteObject(hPen);

	SelectObject(hdcWin, hOldBrush);
	DeleteObject(hBrush);

	SelectObject(hdcWin, hOld);
	DeleteObject(hFont);

	EndPaint(hWnd, &ps);
}

void MyRedrawWindow(HWND hWnd)
{
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}

char* DevideString(char* str, SIZE size, HDC hdc)
{
	if (size.cx <= 0 || str == NULL || strlen(str) == 0)
		return NULL;

	int length = strlen(str);
	int resultlength = length + 1;
	char* result = (char*)calloc(resultlength, sizeof(char));
	int j = 0, templength = 0, buflength = 0;
	SIZE textsize;

	for (int i = 0; i < length; i++)
	{
		if ((j + 2) >= resultlength)
		{
			resultlength += 20;
			result = (char*)realloc(result, resultlength + 1);
		}

		GetTextExtentPoint32(hdc, result, lstrlen(result), &textsize);
		buflength = textsize.cx;
		if ((buflength - templength) >= size.cx)
		{
			result[j++] = '\n';
			result[j] = '\0';
			templength = buflength;
		}

		if (str[i] == '\n')
		{
			templength = buflength;
		}

		result[j] = str[i];
		result[j + 1] = '\0';
		j++;
	}
	result[j] = '\0';

	return result;
}

int GetSecondArgument(char* str)
{
	int cur = 0;
	char c;
	int length = strlen(str);

	while (str[cur] != ' ')
	{
		cur++;
	}

	char* result = (char*)calloc(length - cur, sizeof(char));

	int j = 0;
	for (int i = cur + 1; i < length; i++)
	{
		result[j++] = str[i];
		result[j] = '\0';
	}

	return atoi(result);
}

char* GetRandomString()
{
	int temp = rand();
	int n = (temp % 20) + 10;
	char* s = (char*)calloc(n + 4, sizeof(char));
	int i;
	for (i = 0; i < n - 1; i++)
	{
		s[i] = 'a' + rand() % 26;
	}
	s[i] = '\0';
	return s;
}
