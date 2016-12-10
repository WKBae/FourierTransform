#include "FourierTransform.h"
#include <cmath>

FourierTransform::FourierTransform() : currentTime(0), data() {
	
}

void FourierTransform::feed(double x, float dt) {
	currentTime += dt;
	data.push_back({x, currentTime});
}

ftData FourierTransform::operator[](int n) const {
	return data[n];
}
int FourierTransform::count() const {
	return data.size();
}


complexd FourierTransform::calculate(double freq) {
	double real = 0, imaginary = 0;
	for(auto iter = data.begin(); iter != data.end(); ++iter) {
		double wt = freq * iter->time;
		real += iter->x * cos(wt);
		imaginary -= iter->x * sin(wt);
	}
	return complexd(real, imaginary);
}

double FourierTransform::getCurrentTime() const {
	return currentTime;
}


//#define SQ(x) ((x)*(x))
double FourierTransform::magnitude(complexd number) const {
	return std::abs(number); // sqrt(SQ(number.real()) + SQ(number.imag()));
}

double FourierTransform::angle(complexd number) const {
	return std::arg(number); // atan2(number.imag(), number.real());
}
