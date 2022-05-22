// pwir-differential-equation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <omp.h>

using namespace std;

#define f(x,y) x*y

class Equation {
	int step;
	float x, y, slope, yn;
public:
	void setValues(int tstep, float tx, float ty, float tslope, float tyn)
	{
		step = tstep;
		x = tx;
		y = ty;
		slope = tslope;
		yn = tyn;
	}
	void getValues()
	{
		cout << " " << step + 1 << "\t" << x << "\t" << y << "\t" << slope << "\t" << yn << "\n";
	}
	int getStep() { return step; }
	float getX() { return x; }
	float getYN() { return yn; }
};

void ShowIntroInformation(HANDLE hConsole);
void SetInitialValueX0(HANDLE hConsole, float& x0);
void SetInitialValueY0(HANDLE hConsole, float& y0);
void SetEndValue(HANDLE hConsole, float& xn, float x0);
void SetStepsValue(HANDLE hConsole, int& n);
void SetDelayValue(HANDLE hConsole, int& delay);
void SetDisplayOption(HANDLE hConsole, int& display);
void SetValuesForEquation(Equation values[], int n);
void SetRepeatOption(HANDLE hConsole, int& repeat);
float GetStepSizeValue(float xn, float x0, int n);
void RunEulerMethod(HANDLE hConsole, Equation values[], float x0, float y0, float xn, float h, int n, int delay, int display);
void RunEulerMethodParallel(HANDLE hConsole, Equation values[], float x0, float y0, float xn, float h, int n, int delay, int display);
void RunEulerMethodParallelOpenMP(HANDLE hConsole, Equation values[], float x0, float y0, float xn, float h, int n, int delay, int display);

int main()
{
	setlocale(LC_CTYPE, "Polish");

	HANDLE hConsole;
	float x0, y0, xn, h;
	int n, delay, repeat, display;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	ShowIntroInformation(hConsole);
	while (true) 
	{
		SetInitialValueX0(hConsole, x0);
		SetInitialValueY0(hConsole, y0);
		SetEndValue(hConsole, xn, x0);
		SetStepsValue(hConsole, n);
		SetDelayValue(hConsole, delay);
		SetDisplayOption(hConsole, display);
		auto values = new Equation[n];
		h = GetStepSizeValue(xn, x0, n);
		RunEulerMethod(hConsole, values, x0, y0, xn, h, n, delay, display);
		RunEulerMethodParallel(hConsole, values, x0, y0, xn, h, n, delay, display);
		RunEulerMethodParallelOpenMP(hConsole, values, x0, y0, xn, h, n, delay, display);

		delete[] values;
		SetRepeatOption(hConsole, repeat);
		if (repeat == 0) break;
	}
}

void ShowIntroInformation(HANDLE hConsole)
{
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i < 70; i++) cout << '*';
	SetConsoleTextAttribute(hConsole, 3);
	cout << "\n\n  PROGRAMOWANIE WSPӣBIE�NE I ROZPROSZONE 21/22L\n  Rozwi�zanie r�wnania r�niczkowego zwyczajnego\n  Autor programu: ";
	SetConsoleTextAttribute(hConsole, 15);
	cout << "Kamil Hojka -- 97632\n\n";
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i < 70; i++) cout << '*';
	cout << "\n";
	SetConsoleTextAttribute(hConsole, 15);
}

void SetInitialValueX0(HANDLE hConsole, float& x0)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Wprowad� warto�� pocz�tkow� dla x0: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> x0;
		if (!cin.good())
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� musi by� liczb�\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

void SetInitialValueY0(HANDLE hConsole, float& y0)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Wprowad� warto�� pocz�tkow� dla y0: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> y0;
		if (!cin.good())
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� musi by� liczb�\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

void SetEndValue(HANDLE hConsole, float& xn, float x0)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Wprowad� warto�� ko�cow� dla xn: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> xn;
		if (!cin.good())
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� musi by� liczb�\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else if (xn <= x0)
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� ko�cowa powinna by� wi�ksza ni� warto�� pocz�tkowa\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

void SetStepsValue(HANDLE hConsole, int& n)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Wprowad� liczb� krok�w dla n: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> n;
		if (!cin.good())
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� musi by� liczb� ca�kowit�\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else if (n <= 0)
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Liczba krok�w powinna by� liczb� dodatni�\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

void SetDelayValue(HANDLE hConsole, int& delay)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Podaj op�nienie? [ms]: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> delay;
		if (!cin.good())
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� op�nienia musi by� liczb� naturaln� {0, 1, 2, ...}\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

void SetDisplayOption(HANDLE hConsole, int& display)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Czy wy�wietli� wyniki dla kolejnych krok�w? [1/0]: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> display;
		if (!cin.good() || (display != 0 && display != 1))
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� musi by� liczb� 0 lub 1\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

void SetValuesForEquation(Equation values[], int n)
{
	for (int i = 0; i < n; i++)
	{
		values[i].setValues(i, -1, -1, -1, -1);
	}
}

void SetRepeatOption(HANDLE hConsole, int& repeat)
{
	while (true) {
		SetConsoleTextAttribute(hConsole, 14);
		cout << "\n -> Czy powt�rzy� program? [1/0]: ";
		SetConsoleTextAttribute(hConsole, 15);
		cin >> repeat;
		if (!cin.good() || (repeat != 0 && repeat != 1))
		{
			SetConsoleTextAttribute(hConsole, 4);
			cout << "    ! Warto�� musi by� liczb� 0 lub 1\n";
			SetConsoleTextAttribute(hConsole, 15);
			cin.clear();
			cin.ignore();
		}
		else break;
	}
}

float GetStepSizeValue(float xn, float x0, int n)
{
	return (xn - x0) / n;
}

void EulerMethod(Equation values[], float x0, float y0, float h, int n, int delay)
{
	float xi, yi, slope, yn;
	for (int i = 0; i < n; i++) {
		this_thread::sleep_for(std::chrono::milliseconds(delay));
		if (i != 0) {
			xi = values[i - 1].getX();
			yi = values[i - 1].getYN();
			xi += h;
		}
		else {
			xi = x0;
			yi = y0;
		}
		slope = f(xi, yi);
		yn = yi + h * slope;
		values[i].setValues(i, xi, yi, slope, yn);
	}
}

void RunEulerMethod(HANDLE hConsole, Equation values[], float x0, float y0, float xn, float h, int n, int delay, int display)
{
	cout << "\n\n";
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i < 70; i++) cout << '*';
	SetConsoleTextAttribute(hConsole, 3);
	cout << "\n ---> Sekwencyjne rozwi�zanie r�wnania r�niczkowego - Metoda Eulera\n";
	if (display)
	{
		cout << "\n Wielko�� op�nienia:\t\t" << delay << " ms";
		cout << "\n Wielko�� rozmiaru kroku h:\t" << h;
		cout << "\n\n krok\tx0\ty0\tnachyl\tyn\n\n";
	}
	SetConsoleTextAttribute(hConsole, 15);
	SetValuesForEquation(values, n);
	auto begin = chrono::high_resolution_clock::now();
	EulerMethod(values, x0, y0, h, n, delay);
	auto end = chrono::high_resolution_clock::now();
	auto elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count(); 
	if (display)
	{
		for (int i = 0; i < n; i++) 
		{
			values[i].getValues();
		}
	}
	SetConsoleTextAttribute(hConsole, 14);
	cout << "\n Szukana warto�� dla y(" << xn << ") = " << values[n-1].getYN();
	SetConsoleTextAttribute(hConsole, 15);
	cout << "\n\n Zmierzony czas: " << elapsed << " ms\n";
}

void EulerMethodParallel(Equation values[], float x0, float y0, int i, float h, int delay)
{
	float xi, yi, slope, yn;
	this_thread::sleep_for(std::chrono::milliseconds(delay));
	if (i != 0) {
		while (true) {
			xi = values[i - 1].getX();
			yi = values[i - 1].getYN();
			if (xi != -1 && yi != -1) break;
		}
		xi += h;
	}
	else {
		xi = x0;
		yi = y0;
	}
	slope = f(xi, yi);
	yn = yi + h * slope;
	values[i].setValues(i, xi, yi, slope, yn);
}

void RunEulerMethodParallel(HANDLE hConsole, Equation values[], float x0, float y0, float xn, float h, int n, int delay, int display)
{
	cout << "\n\n";
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i < 70; i++) cout << '*';
	SetConsoleTextAttribute(hConsole, 3);
	cout << "\n ---> R�wnoleg�e rozwi�zanie r�wnania r�niczkowego - Metoda Eulera\n";
	if (display)
	{
		cout << "\n Wielko�� op�nienia:\t\t" << delay << " ms";
		cout << "\n Wielko�� rozmiaru kroku h:\t" << h;
		cout << "\n\n krok\tx0\ty0\tnachyl\tyn\n\n";
	}
	SetConsoleTextAttribute(hConsole, 15);
	SetValuesForEquation(values, n);
	auto begin = chrono::high_resolution_clock::now();
	vector<thread> threads(n);
	for (int i = 0; i < threads.size(); i++)
	{
		threads[i] = thread(EulerMethodParallel, values, x0, y0, i, h, delay);
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
	auto end = chrono::high_resolution_clock::now();
	auto elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	if (display)
	{
		for (int i = 0; i < n; i++)
		{
			values[i].getValues();
		}
	}
	SetConsoleTextAttribute(hConsole, 14);
	cout << "\n Szukana warto�� dla y(" << xn << ") = " << values[n - 1].getYN();
	SetConsoleTextAttribute(hConsole, 15);
	cout << "\n\n Zmierzony czas: " << elapsed << " ms\n";
}

void RunEulerMethodParallelOpenMP(HANDLE hConsole, Equation values[], float x0, float y0, float xn, float h, int n, int delay, int display)
{
	cout << "\n\n";
	SetConsoleTextAttribute(hConsole, 11);
	for (int i = 0; i < 70; i++) cout << '*';
	SetConsoleTextAttribute(hConsole, 3);
	cout << "\n ---> R�wnoleg�e rozwi�zanie r�wnania r�niczkowego OpenMP - Metoda Eulera\n";
	if (display)
	{
		cout << "\n Wielko�� op�nienia:\t\t" << delay << " ms";
		cout << "\n Wielko�� rozmiaru kroku h:\t" << h;
		cout << "\n\n krok\tx0\ty0\tnachyl\tyn\n\n";
	}
	SetConsoleTextAttribute(hConsole, 15);
	SetValuesForEquation(values, n);
	auto begin = chrono::high_resolution_clock::now();
#pragma omp parallel for schedule(static, 1)
	for (int i = 0; i < n; i++)
	{
		EulerMethodParallel(values, x0, y0, i, h, delay);
	}
	auto end = chrono::high_resolution_clock::now();
	auto elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	if (display)
	{
		for (int i = 0; i < n; i++)
		{
			values[i].getValues();
		}
	}
	SetConsoleTextAttribute(hConsole, 14);
	cout << "\n Szukana warto�� dla y(" << xn << ") = " << values[n - 1].getYN();
	SetConsoleTextAttribute(hConsole, 15);
	cout << "\n\n Zmierzony czas: " << elapsed << " ms\n";
}