

#define lambda 1
#define basePolyLong 31
#define side (12 + 7)
#define baseCellLong basePolyLong + side
#define lineGap1 1
#define lineGap2 2
#define lineGap3 3
#define lineGap4 4



enum color{IDLE, POLY, METAL1, METAL2, ACTIVE, NACTIVE, PACTIVE, PSELECT, NSELECT, VIA, M1ToM2};

class material
{
public:
	material()
	{
		name = " ";
		Color = IDLE;
		startX = 0;
		startY = 0;
		W = 0;
		H = 0;
	}
	material(string s, color clr, float a, float b, float c, float d)
	{
		name = s;
		Color = clr;
		startX = a * lambda;
		startY = b * lambda;
		W = c * lambda;
		H = d * lambda;
	}
	string name;
	color Color;
	float startX;
	float startY;
	float W;
	float H;

	material *amplify(float f)
	{
		material *temp = new material;
		temp->Color = this->Color;
		temp->name = this->name;
		temp->startX = this->startX * f;
		temp->startY = this->startY * f;
		temp->W = this->W * f;
		temp->H = this->H * f;
		return temp;
	}
};