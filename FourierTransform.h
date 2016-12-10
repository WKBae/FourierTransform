#pragma once
#include <vector>
#include <complex>

using std::vector;
typedef std::complex<double> complexd;

typedef struct _ftData {
	double x;
	double time;
} ftData;
class FourierTransform {
public:
	FourierTransform();

	void feed(double x, float dt);

	complexd calculate(double freq);

	ftData operator[](int n) const;
	int count() const;
	double getCurrentTime() const;
	double magnitude(complexd number) const;
	double angle(complexd number) const;
private:
	double currentTime;
	vector<ftData> data;
};