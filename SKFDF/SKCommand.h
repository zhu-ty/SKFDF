//SKCommand
//Command handler ver2.0.
//Warning! Only in Windows.
//Author:ShadowK
//email:zhu.shadowk@gmail.com
//2015.11.19
//Use Ctrl+M,Ctrl+O to fold the code.

#include<string>
#include<iostream>
#include<vector>
#include<windows.h>
#include<conio.h>
using namespace std;

class SKCommand
{
public:
	static bool compare(const string& x, const string& y)
	{
		string::const_iterator p = x.begin();
		string::const_iterator q = y.begin();
		while (p != x.end() && q != y.end() && toupper(*p) == toupper(*q))
		{
			++p;
			++q;
		}
		if (p == x.end())
		{
			return (q == y.end());
		}
		if (q == y.end())
		{
			return false;
		}
		return false;
	}
	static vector<string> split(string str, string pattern)
	{
		int pos;
		std::vector<std::string> result;
		if (str[str.length() - 1] == '\n')
			str.pop_back();
		str += pattern;
		int size = str.size();

		for (int i = 0; i < size; i++)
		{
			pos = str.find(pattern, i);
			if (pos < size)
			{
				std::string s = str.substr(i, pos - i);
				result.push_back(s);
				i = pos + pattern.size() - 1;
			}
		}
		return result;
	}
	static double get_double(string mes = "")
	{
		fflush(stdin);
		printf(mes.c_str());
		double x;
		scanf("%lf", &x);
		fflush(stdin);
		return x;
	}
	static int get_int(string mes = "")
	{
		fflush(stdin);
		printf(mes.c_str());
		int x;
		scanf("%d", &x);
		fflush(stdin);
		return x;
	}
	static void wait_till_end(string mes = "")
	{
		fflush(stdin);
		printf(mes.c_str());
		int c = _getch();
		fflush(stdin);
		return;
	}
	static void wait_seconds(double seconds)
	{
		fflush(stdin);
		printf("\nWaiting...Input any key to continue...\n");
		int ms10_1 =(int)(seconds * 100);
		for (int i = 0; i < 10; i++)
		{
			printf("%d ",10-i);
			Sleep(ms10_1);
			if (_kbhit() != 0)
			{
				_getch();
				break;
			}
		}
		fflush(stdin);
		printf("\n");
		return;
	}
};