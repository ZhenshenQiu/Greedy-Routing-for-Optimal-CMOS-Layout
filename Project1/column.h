#include <string>
#include <vector>
using namespace std;

class column
{
public:
	column()
	{
		x = 0;
		y = 0;
		name = " ";
		poly = false;
		broughtIn = false;
	}
	column(string s, bool tf, float a, float b)
	{
		x = a;
		y = b;
		name = s;
		poly = tf;
		broughtIn = false;
	}
	//~column();

	float x;
	float y;
	string name;
	bool poly;
	bool broughtIn;
	vector<vector<float> > MergeSpace;

private:

};

//column::column()
//{
//}
//
//column::~column()
//{
//}