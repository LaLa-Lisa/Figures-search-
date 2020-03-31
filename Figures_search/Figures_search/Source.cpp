#include <Magick++.h>
#include <fstream>
#include <string>
#include <windows.h>
#include <Commdlg.h>
#include <vector>
#include "LMatrix.h"
#include "LNeironNet.h"
LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM); 
OPENFILENAME ofn;
std::string fileName(MAX_PATH, '\0');

using namespace Magick;


class quadImg {
public:
	//высота и длинна исходной картинки 
	int Width = 10;
	int Height = 10;
	//координаты начала маленькой картинки на большой
	int x0, y0;


	quadImg(Image inImg) {
		Img = inImg;
		Width = Img.columns();
		Height = Img.rows();
		Img.sample("50x50!");
		Fig = LMatrix(1, 2500);
		int iter = 0;
		for (int i = 0; i < Img.rows(); i++) {
			for (int j = 0; j < Img.columns(); j++)
			{
				if (isWhite(i, j)) Fig.data[0][iter] = 0;
				else Fig.data[0][iter] = 1;
				iter++;
			}
		}
		int L[3] = { 2500, 50, 4 };
		//создаётся объект нейронной сети для определения типа фигуры 
		LNeironNet Net(3, L);
		Net.read_coeff("weits.txt");
		LMatrix ans = Net.work(Fig);
		if (ans.data[0][0] > 0.7) type = 1;
		if (ans.data[0][1] > 0.7) type = 2;
		if (ans.data[0][2] > 0.7) type = 3;
		if (ans.data[0][3] > 0.7) type = 4;
	}
	void initCoords(int inX, int inY) {
		x0 = inX;
		y0 = inY;
	}
	//тип: 0 - не определён, 1 - круг, 2 - треугольник, 3 - квадрат, 4 - звезда
	int type = 0;
private:
	//проверка белый ли пиксел
	bool isWhite(int row, int column) {
		ColorRGB px = Img.pixelColor(column, row);
		if (px.red() == 1 && px.green() == 1 && px.blue() == 1) return true;
		else return false;
	}
	//cтрока для нейронной сети
	LMatrix Fig;

	Image Img;
};



class myImage {
public:

	//тут будет храниться перекрашенное изображение
	Image typeImage;
	int SIZEX = 0;
	int SIZEY = 0;
	COLORREF *PixelBuffer;
	//0 - неопределено, 1 - круг, 2 - треугольник, 3 - квадрат, 4 - звезда
	int countFig[5] = { 0 };

	myImage(std::string mainImgFile) {
		mainImage.read(mainImgFile);
		imgWidth = mainImage.columns();
		imgHeight = mainImage.rows();
		typeImage = mainImage;
	}


	//алгоритм поиска фигур
	void figuresSearch() {
		//заполняем фигуру по краям белыми пикселями
		for (int i = 0; i < imgWidth; i++) {
			setColorWhite(0, i);
			setColorWhite(imgHeight - 1, i);
		}
		for (int i = 0; i < imgHeight; i++) {
			setColorWhite(i, 0);
			setColorWhite(i, imgWidth - 1);
		}

		//к этому моменту картика имеет рамку белого цвета
		int x0 = 1;
		int y0 = 1;
		//ищем черные точки 
		while (x0 != imgWidth - 1 && y0 != imgHeight - 1) {
			x0++;
			if (x0 == imgWidth - 1) {
				x0 = 1;
				y0++;
			}
			if (!isWhite(y0, x0)) CutFigure(x0, y0 - 1);
		}

	}
	void get_rez() {
		int R[5] = { 0 };
		for (int i = 0; i < figures.size(); i++)
		{
			R[figures[i].type]++;
		}
		for (int i = 0; i<5; i++)
			countFig[i] = R[i];
	}

	//перекрашивает исходную картику, так что бы фигуры были помечены каждая под свой тип
	void recolorImg() {
		for (int i = 0; i < figures.size(); i++)
		{
			ColorRGB p;
			switch (figures[i].type) {
			case(0):
				p = ColorRGB(1, 1, 0);
				break;
			case(1):
				p = ColorRGB(1, 0, 1);
				break;
			case(2):
				p = ColorRGB(0, 1, 1);
				break;
			case(3):
				p = ColorRGB(1, 0, 0);
				break;
			case(4):
				p = ColorRGB(0, 0, 1);
				break;
			}
			for (int j = 0; j < figures[i].Height; j++) {
				for (int k = 0; k < figures[i].Width; k++) {
					ColorRGB px = typeImage.pixelColor(figures[i].x0 + k, figures[i].y0 + j);
					if (!(px.red() == 1 && px.green() == 1 && px.blue() == 1)) typeImage.pixelColor(figures[i].x0 + k, figures[i].y0 + j, p);
				}
			}
		}
		//typeImage.write("recolor.png");
		typeImage.sample("650x410");
		SIZEX = typeImage.columns();
		SIZEY = typeImage.rows();
		PixelBuffer = new COLORREF[SIZEX * SIZEY];
		for (int i = 0; i < SIZEY; i++) {
			for (int j = 0; j < SIZEX; j++) {
				ColorRGB px = typeImage.pixelColor(j, i);
				PixelBuffer[j + (i*SIZEX)] = RGB(0xff, 0xff, 0xff);
				if ((px.red() == 1 && px.green() == 1 && px.blue() == 0)) PixelBuffer[j + (i*SIZEX)] = RGB(0xff, 0xff, 0x00);
				if ((px.red() == 1 && px.green() == 0 && px.blue() == 1)) PixelBuffer[j + (i*SIZEX)] = RGB(0xff, 0x00, 0xff);
				if ((px.red() == 0 && px.green() == 1 && px.blue() == 1)) PixelBuffer[j + (i*SIZEX)] = RGB(0x00, 0xff, 0xff);
				if ((px.red() == 1 && px.green() == 0 && px.blue() == 0)) PixelBuffer[j + (i*SIZEX)] = RGB(0xff, 0x00, 0x00);
				if ((px.red() == 0 && px.green() == 0 && px.blue() == 1)) PixelBuffer[j + (i*SIZEX)] = RGB(0x00, 0x00, 0xff);
				//if ((px.red() == 0 && px.green() == 0 && px.blue() == 0)) PixelBuffer[j + (i*SIZEX)] = RGB(0x00, 0x00, 0x00);
			}
		}
	}

private:
	Magick::Image mainImage;
	std::vector<quadImg> figures;
	int imgWidth;
	int imgHeight;



	//определение цвета пикселя
	bool isBlack(int row, int column) {
		ColorRGB px = mainImage.pixelColor(column, row);
		if (px.red() == 0 && px.green() == 0 && px.blue() == 0) return true;
		else return false;
	}
	bool isWhite(int row, int column) {
		ColorRGB px = mainImage.pixelColor(column, row);
		if (px.red() == 1 && px.green() == 1 && px.blue() == 1) return true;
		else return false;
	}

	//задание цвета пикселя
	void setColorWhite(int row, int column) {
		ColorRGB p(1, 1, 1);
		mainImage.pixelColor(column, row, p);
	}

	//вырезает прямоугольник с фигурой 
	Image CutImg(Image img, int x_len, int y_len, int x0, int y0) {
		//по порядку - длина, ширина, cтолбец, строка
		img.crop(Geometry(x_len, y_len, x0, y0));
		img.repage();
		return img;
	}

	//вырезаем фигуру 
	void CutFigure(int x0, int y0) {
		int l1 = x0 - 1;
		int r1 = x0 + 1;
		//сторона квадрата
		int k = 0;
		bool m = true;

		//вырезаем фигуру
		while (m) {
			k++;
			while (m) {
				m = false;
				for (int i = 0; i <= k; i++)
					if (isBlack(y0 + i, l1)) {
						l1--;
						m = true;
						break;
					}
			}
			m = true;
			while (m) {
				m = false;
				for (int i = 0; i <= k; i++)
					if (isBlack(y0 + i, r1)) {
						r1++;
						m = true;
						break;
					}
			}
			m = false;
			for (int i = l1; i <= r1; i++)
				if (isBlack(y0 + k, i)) m = true;
		}
		//создаём новый обьект фигуры и вставляем его в вектор
		quadImg im(CutImg(mainImage, r1 - l1 + 1, k, l1, y0));
		im.initCoords(l1, y0);
		figures.push_back(im);

		//заполняем место, где была фигура белыми пикселями 
		for (int i = y0; i <= y0 + k; i++)
			for (int j = l1; j <= r1; j++)
				setColorWhite(i, j);

	}
};





int  WINAPI  WinMain(HINSTANCE  hInstance,
	HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;   
	WNDCLASS w; 
	memset(&w, 0, sizeof(WNDCLASS)); 
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = WndProc;
	w.hInstance = hInstance;
	w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	w.lpszClassName = "Class";
	RegisterClass(&w); 
	hwnd = CreateWindow("Class", "Поиск фигур",
		WS_OVERLAPPEDWINDOW&(~WS_MAXIMIZEBOX)&(~WS_THICKFRAME)&(~WS_MINIMIZEBOX),
		100, 100, 1000, 500,
		NULL, NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow); 
	UpdateWindow(hwnd);         
							
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam) {

	InitializeMagick("");
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = &fileName[0];
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter =
		"Image files (*.jpg;*.png;*.bmp)\0*.jpg;*.png;*.bmp\0"
		"All files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;;



	HDC hdc;
	HINSTANCE hInst;
	PAINTSTRUCT ps;
	static HWND hBtn; 
	static HWND hEdt1;
	static HWND hStatKrug; 
	static HWND hStatTreug;
	static HWND hStatKvadr;
	static HWND hStatZvezd;


	switch (Message) {
	case WM_CREATE: 
		hInst = ((LPCREATESTRUCT)lparam)->hInstance; 

		
		hBtn = CreateWindow("button", "Рассчитать",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			50, 100, 120, 30, hwnd, 0, hInst, NULL);
		ShowWindow(hBtn, SW_SHOWNORMAL);
		// Создаем и показываем поле текста для результата
		hStatKrug = CreateWindow("static", "0", WS_CHILD | WS_VISIBLE,
			170, 180, 120, 20, hwnd, 0, hInst, NULL);
		ShowWindow(hStatKrug, SW_SHOWNORMAL);
		//треугольников
		hStatTreug = CreateWindow("static", "0", WS_CHILD | WS_VISIBLE,
			170, 220, 120, 20, hwnd, 0, hInst, NULL);
		ShowWindow(hStatTreug, SW_SHOWNORMAL);
		//квадратов
		hStatKvadr = CreateWindow("static", "0", WS_CHILD | WS_VISIBLE,
			170, 260, 120, 20, hwnd, 0, hInst, NULL);
		ShowWindow(hStatKvadr, SW_SHOWNORMAL);
		//звезд
		hStatZvezd = CreateWindow("static", "0", WS_CHILD | WS_VISIBLE,
			170, 300, 120, 20, hwnd, 0, hInst, NULL);
		ShowWindow(hStatZvezd, SW_SHOWNORMAL);
		break;
	case WM_COMMAND:  
		if (lparam == (LPARAM)hBtn)    
		{

			GetOpenFileName(&ofn);
			std::string mu = fileName;
			CloseHandle(ofn.hInstance); //закрываем файл
			myImage img(mu);
			img.figuresSearch();
			img.get_rez();
			SetWindowText(hStatKrug, std::to_string(img.countFig[1]).c_str());
			SetWindowText(hStatTreug, std::to_string(img.countFig[2]).c_str());
			SetWindowText(hStatKvadr, std::to_string(img.countFig[3]).c_str());
			SetWindowText(hStatZvezd, std::to_string(img.countFig[4]).c_str());

			img.recolorImg();
			HDC hdc = GetDC(hwnd);
			//закрашиваем белым 
			for (int i = 0; i < 410; i++) {
				for (int j = 0; j < 650; j++) {
					SetPixel(hdc, j + 310, i + 20, RGB(0xff, 0xff, 0xff));
				}
			}
			//рисуется картинка 
			for (int i = 0; i<img.SIZEY; i++) {
				for (int j = 0; j < img.SIZEX; j++) {
					SetPixel(hdc, j + 310, i + 20, img.PixelBuffer[j + (i*img.SIZEX)]); // рисуем каждый
				}
			}
		}
		break;
	case WM_PAINT: 
		hdc = BeginPaint(hwnd, &ps); 
		TextOut(hdc, 50, 180, "Кругов:", 7);
		TextOut(hdc, 50, 220, "Треугольников:", 14);
		TextOut(hdc, 50, 260, "Квадратов:", 10);
		TextOut(hdc, 50, 300, "Звезд:", 7);
		EndPaint(hwnd, &ps); 
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: 
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}