#include <string>

using namespace std;

class track
{
public:
	track()
	{
		user = "null";
		assigned = false;
	};

	track(float a)
	{
		user = " ";
		y = a;
		assigned = false;
	};


	void Assign(string s)
	{
		assigned = true;
		user = s;
	}

	void Free(void)
	{
		assigned = false;
		user = " ";
	}

	bool isNull(void)
	{
		if(user == "null")
			return true;
		else
			return false;
	}

	float y;
	string user;
	bool assigned;
};
