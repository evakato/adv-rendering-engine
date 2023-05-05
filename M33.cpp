
#include "M33.h"

V3& M33::operator[](int i) {

	return rows[i];

}

V3 M33::operator*(V3 v) {

	M33 &m = *this;
	V3 ret;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;

}


V3 M33::GetColumn(int i){

	return V3(rows[0][i], rows[1][i], rows[2][i]);

}

void M33::SetColumn(int i, V3 c) {

	rows[0][i] = c[0];
	rows[1][i] = c[1];
	rows[2][i] = c[2];

}

void M33::SetRotY(float theta) {

	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33 &m = *this;
	m[0] = V3(cost, 0.0f, sint);
	m[1] = V3(0.0f, 1.0f, 0.0f);
	m[2] = V3(-sint, 0.0f, cost);

}


// ^ stands for cross product
// * for dot product
M33 M33::Inverted() {

	M33 ret;
	V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	V3 _a = b ^ c; _a = _a / (a * _a);
	V3 _b = c ^ a; _b = _b / (b * _b);
	V3 _c = a ^ b; _c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}

M33 M33::operator*(M33 m1) {

	M33 ret;
	M33 &m0 = *this;
	ret.SetColumn(0, m0*m1.GetColumn(0));
	ret.SetColumn(1, m0*m1.GetColumn(1));
	ret.SetColumn(2, m0*m1.GetColumn(2));
	return ret;

}


M33 M33::Transposed() {

	M33 ret;
	ret.SetColumn(0, rows[0]);
	ret.SetColumn(1, rows[1]);
	ret.SetColumn(2, rows[2]);
	return ret;

}
