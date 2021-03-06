
#include "pch.h"
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <fstream>

#include <iostream>
using namespace std;
const double EPSILON = 0.001f;

double ODEJMIJ = 0.1;
int ILOSC_WYNIKOW = 10;

inline void printInt(int n, FILE* file) {

	if (n > 0) { //wyłuskanie kolejnych cyfr z liczby n
		printInt(n / 10, file);
		_putc_nolock(n % 10 + 48, file);
	}

}
double rounddouble(double d, double fix=2.0)
{
	return round(d*pow(10.0, fix)) / pow(10.0, fix)*1.0;
}

//inline void printdouble(double n, FILE* file)
//{
//	short k = (short)(n * 100); //n*10^eps
//	char arr[16];
//	int p = 0;
//
//	//i = 0
//	arr[p++] = (k % 10) + 48;
//	k /= 10;
//	//i = 1
//	arr[p++] = (k % 10) + 48;
//	k /= 10;
//
//	arr[p++] = '.';
//	do{
//		arr[p++] = (k % 10) + 48;
//		k /= 10;
//	} while (k != 0);
//
//	while (p--)
//		_putc_nolock(arr[p], file);
//}

inline bool doubleEquals(double left, double right)
{
	return (fabs(left - right) < EPSILON);
}

inline bool doubleLess(double left, double right, bool orequal = false)
{
	if (fabs(left - right) < EPSILON) {
		return (orequal);
	}
	return (left < right);
}

inline bool doubleGreater(double left, double right, bool orequal = false)
{
	if (fabs(left - right) < EPSILON) {
		return (orequal);
	}
	return (left > right);
}

struct SubResult
{
	SubResult(double s, double c, double l) : startNumber(s), condition(c), looseCondition(l), next(NULL) {}
	double startNumber;
	double condition;
	double looseCondition;

	SubResult *next;
};

struct FullResult
{
	double result;

	SubResult *front;

	FullResult()
	{
		result = -DBL_MAX;
		front = NULL;
	}

	inline void push(SubResult *s)
	{
		s->next = front;
		front = s;
	}

	inline void print(FILE* file)
	{
		fprintf(file, "%.2lf", round(result * 100.0) / 100.0);
		_fputc_nolock('\n', file);
		while (front)
		{
			fprintf(file, "%.2lf", round(front->startNumber * 100.0) / 100.0);
			_fputc_nolock(' ', file);
			fprintf(file, "%.2lf", round(front->condition * 100.0) / 100.0);
			_fputc_nolock(' ', file);
			fprintf(file, "%.2lf", round(front->looseCondition * 100.0) / 100.0);
			_fputc_nolock('\n', file);
			front = front->next;
		}
		_fputc_nolock('\n', file);
	}
};

inline void InsertResult(FullResult **results, double startNumber, double condition, double looseCondition, double newResult)
{
	int ind = 0;
	for (int i = 0; i < ILOSC_WYNIKOW; ++i)
	{
		if (doubleEquals(results[i]->result, newResult))
		{
			SubResult *s = new SubResult(startNumber, condition, looseCondition);
			results[i]->push(s);
			return;
		}
		if (doubleLess(results[i]->result, results[ind]->result))
			ind = i;
	}
	if (doubleLess(results[ind]->result, newResult))
	{
		results[ind]->result = newResult;
		results[ind]->front = new SubResult(startNumber, condition, looseCondition);
	}
}

inline double GetDouble(FILE* f, char *number, char *c)
{
	int i = 0;
	*c = _getc_nolock(f);
	while (*c != '\n' && *c != EOF)
	{
		if (*c != ',')
			number[i++] = *c;
		else
			number[i++] = '.';
		*c = _getc_nolock(f);
	}
	number[i] = '\0';
	return round(strtod(number, 0) * 100.0) / 100.0;
}

int main(int argc, char *argv[])
{
	//for (int repeats = 0; repeats < 10; repeats++)
	//{
		/*clock_t start, end;
		start = clock();*/
	if (argc < 5)
	{
		puts("Podaj nazwe pliku, liczbe, ilosc wynikow jako argumenty wywolania");
		return -1;
	}

	const char* path = argv[1];
	ODEJMIJ = rounddouble(strtod(argv[2], NULL));
	double y = rounddouble(strtod(argv[3], NULL));
	ILOSC_WYNIKOW = atoi(argv[4]);

	register FILE *file = fopen(path, "r");
	register FILE *outputFile = fopen("output.txt", "w");
	if (file == NULL)
	{
		puts("Nie mozna otworzyc pliku");
		return -1;
	}
	register FullResult **topResults = new FullResult*[ILOSC_WYNIKOW];
	register double max = 0.0;
	register double min = DBL_MAX;
	register double maxCondition = 0.0;
	register char c = 0;
	register char *number = new char[10];
	register double found;
	register double value;
	register int eof;
	register double result;

	register double startNumber;
	register double condition;
	register double looseCondition;

	register int i;

	for (i = 0; i < ILOSC_WYNIKOW; ++i)
		topResults[i] = new FullResult();

	//min max
	while (c != EOF)
	{
		double value = GetDouble(file, number, &c);

		if (doubleGreater(value, max))
			max = value;
		if (doubleLess(value, min))
			min = value;
	}

	maxCondition = max - min;

	//for (startNumber = min; startNumber <= max; startNumber += 0.3)
	{//if in input file there are ,
		stringstream*ss;
		ss = new stringstream;
		fstream fstr;
		string tekst;
		fstr.open(path, ios_base::app | ios_base::in | ios_base::out);
		fstr.seekg(ios_base::beg);
		char tab[32];
		fstr.getline(tab, 32, '\n');
		*ss << tab;
		*ss >> tekst;
		if (tekst.empty()) { puts("Nie mozna otworzyc pliku lub jest on pusty");return 1; }
		if (tekst.find(",") != string::npos) { tekst[tekst.find(",")] = '.'; }
		delete ss;
		ss = new stringstream;
		*ss << tekst;
		*ss >> startNumber;
		fstr.close();
		delete ss;
	}
	bool check = false;
	bool done = false;
	{
		for (condition = y; condition < maxCondition; condition += y)
			//condition = 0.9;
		{
			for (looseCondition = y; looseCondition < maxCondition; looseCondition += y)
				//looseCondition = 0.3;
			{
				//return to beginning of file
				fseek(file, 0, SEEK_SET);
				c = 0;
				found = startNumber;
				value = 0.0;
				eof = 0;
				result = 0.0;
				while (c != EOF)
				{
					done = false;
					while (done != true)
					{
						if (check == false)
						{
							if (c == EOF)
							{
								eof = 1;
								done = true;
								continue;
							}
							value = GetDouble(file, number, &c);
							if (value>=found && abs(value - found) >=condition)
							{
								result += rounddouble(abs(found-value))-ODEJMIJ;
								found = value;
								check = !check;
								done = true;
							}
							else if (value < found && (abs(value - found)) > looseCondition)
							{
								result -= rounddouble(abs(found - value)) + ODEJMIJ;
								found = value;
								check = !check;
								done = true;
							}
							if (c == EOF)
							{
								eof = 1;
								done = true;
							}
						}
						else
						{
							if (c == EOF)
							{
								eof = 1;
								done = true;
								continue;
							}
							value = GetDouble(file, number, &c);
							if (value < found && abs(value - found) >= condition)
							{
								result += rounddouble(abs(found - value)) - ODEJMIJ;
								found = value;
								check = !check;
								done = true;
							}
							else if (value >= found && (abs(value - found)) > looseCondition)
							{
								result -= rounddouble(abs(found - value)) + ODEJMIJ;
								found = value;
								check = !check;
								done = true;
							}
							if (c == EOF)
							{
								eof = 1;
								done = true;
							}
						}
					}
					if (eof) { break; }
					/*value *= 2;
					done = false;
					while (done != true)
					{
						if (check == false)
						{
							if (c == EOF)
							{
								eof = 1;
								done = true;
								continue;
							}
							value = GetDouble(file, number, &c);
							if (value >= found && abs(value - found) >= condition)
							{
								result += rounddouble(abs(found - value)) - ODEJMIJ;
								cout << "Value: " << value << "; Found: " << found << "; Result: " << rounddouble(result) << "; value-found: " << (value - found) << "; Condition: " << condition << ";\n";
								found = value;
								check = !check;
								done = true;
							}
							else if (value < found && (abs(value - found)) > looseCondition)
							{
								result -= rounddouble(abs(found - value)) - ODEJMIJ;
								cout << "Value: " << value << "; Found: " << found << "; Result: " << rounddouble(result) << "; found-value: " << (found - value) << "; Loose: " << looseCondition << ";\n";
								found = value;
								check = !check;
								done = true;
							}
							if (c == EOF)
							{
								eof = 1;
								done = true;
							}
						}
						else
						{
							if (c == EOF)
							{
								eof = 1;
								done = true;
								continue;
							}
							value = GetDouble(file, number, &c);
							if (value < found && abs(value - found) > looseCondition)
							{
								result -= rounddouble(abs(found - value)) - ODEJMIJ;
								cout << "Value: " << value << "; Found: " << found << "; Result: " << rounddouble(result) << "; found-value: " << (found - value) << "; Loose: " << looseCondition << ";\n";
								found = value;
								check = !check;
								done = true;
							}
							else if (value >= found && (abs(value - found)) >= condition)
							{
								result += rounddouble(abs(found - value)) - ODEJMIJ;
								cout << "Value: " << value << "; Found: " << found << "; Result: " << rounddouble(result) << "; value-found: " << (value - found) << "; Condition: " << condition << ";\n";
								found = value;
								check = !check;
								done = true;
							}
							if (c == EOF)
							{
								eof = 1;
								done = true;
							}
						}
					}
					value /= 2;
					if (eof) { break; }*/
				}
				//fprintf(outputFile, "start:%.2lf\ncond:%.2lf\nloosecond:%.2lf\nresult:%.2lf\n", startNumber, condition, looseCondition, result);
				InsertResult(topResults, startNumber, condition, looseCondition, result);
			}
		}
	}

	sort(topResults, topResults + ILOSC_WYNIKOW,
		[](const FullResult *a, const FullResult *b)
	{
		return doubleGreater(a->result, b->result);
	});

	for (i = 0; i < ILOSC_WYNIKOW; ++i)
	{
		fputs("Wynik", outputFile);
		printInt(i + 1, outputFile);
		fputs(": ", outputFile);
		topResults[i]->print(outputFile);
	}

	fclose(outputFile);
	fclose(file);
	return 0;
}
