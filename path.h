


class path
{
public:
	path()
	{
		name = " ";
		startX = 0;
		startY = 0;
		endX = 0;
		endY = 0;
		M1orM2 = 1;
	};
	path(string s, float a, float b, float c, float d)
	{
		name = s;
		if(a<c)
		{
			startX = a;
			endX = c;
		}
		else
		{
			startX = a;
			endX = c;
		}
		if(b<d)
		{
			startY = b;
			endY = d;
		}
		else
		{
			startY = d;
			endY = b;
		}
		if(b == d)
			M1orM2 = 2;
		else if(a == c)
			M1orM2 = 1;
	};
	string name;
	float startX;
	float startY;
	float endX;
	float endY;
	int M1orM2;
};