#include <iostream>
using namespace std;

int
main(int argc, char *argv[])
{
	constexpr int a {5}, b {6}, c {7};
	int x[] = {1, 7, 1, 3, 2, 0, 0, 0, 2, 1, 4};
	int y[] = {1, 9, 8, 7, 6, 5, 4, 3, 2, 1, 1};

	constexpr int j {sizeof x / sizeof(int)};
	for (int _a = 0; _a < 10; _a++)
	{
		x[a] = _a;
		for (int _b = 0; _b < 10; _b++)
		{
			x[b] = _b;
			for (int _c = 0; _c < 10; _c++)
			{
				x[c]= _c;
				int sum {0};
				for (int i = 0; i < j; i++)
				{
					sum += x[i] * y[i];
				}
				
				if (sum % 10 == 0) {
					for (int i = 0; i < j; i++)
						cout << x[i];
					cout << " sum = " << sum;
					cout << "\tsum % 10 = " << sum % 10 << endl;
				}
			}
		}
	}
}
