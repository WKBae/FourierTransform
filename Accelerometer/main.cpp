#include <fstream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "../FourierTransform.h"

using namespace cv;

#define ANGLE_LINE_GRAPH

int main() {
	FourierTransform ft;

	double x, y, z;
	double dt;

	std::ifstream inFile("accelerometer.txt");
	while(inFile) {
		inFile >> x >> y >> z >> dt;
		// The file gives dt in milliseconds scale, convert it to seconds
		dt /= 1000;
		if(ft.getCurrentTime() + dt > 5) break;
		ft.feed(x, dt);
	}
	inFile.close();

	int height = 400;

	/* Draw imput data */
	const float timeWidth = 1000;
	float totalTime = ft.getCurrentTime();
	const float inputScale = 10;
	Mat inputImg = Mat::zeros(height, timeWidth, CV_8UC3);
	double lastTime = 0;
	for(int i = 0; i < ft.count(); i++) {
		rectangle(inputImg,
			Point(lastTime / totalTime * timeWidth, height / 2),
			Point(ft[i].time / totalTime * timeWidth, height / 2 - ft[i].x * inputScale),
			Scalar(255, 0, 0),
			-1);
		lastTime = ft[i].time;
	}
	/* End draw input data */

	const float freqStart = -50, freqEnd = 50;
	const float freqDiff = 1.0f / 8;// 1.0f / 8.0f;// 0.125f;
	const int width = 6;
	const float count = freqEnd - freqStart;
	/* Setup magnitude image */
	const float magScale = 0.5f;
	Mat magImg = Mat::zeros(height, width * count, CV_8UC3);

	/* Setup angle image */
	const float angleScale = height / 2 / 3.141592;
	Mat angleImg = Mat::zeros(height, width * count, CV_8UC3);
#ifdef ANGLE_LINE_GRAPH
	// Store last point of angle to draw line graph connected to previous data
	Point lastAnglePoint(0, height / 2);
#endif
	for(float freq = freqStart; freq <= freqEnd; freq += freqDiff) {
		// Calculate Fourier transformation at 'freq' frequency
		complexd res = ft.calculate(freq);
		
		// Draw magnitude data
		rectangle(magImg,
			Point((freq - freqStart) * width, height),
			Point((freq - freqStart + freqDiff) * width, height - ft.magnitude(res) * magScale),
			Scalar(0, 255, 0),
			-1);

		// Draw angle data
#ifndef ANGLE_LINE_GRAPH
		rectangle(angleImg,
			Point((freq - freqStart) * width, height/2),
			Point((freq - freqStart + df) * width, height/2 - ft.angle(res) * angleScale),
			Scalar(0, 0, 255),
			-1);
#else
		Point newAnglePoint((freq - freqStart) * width, height / 2 - ft.angle(res) * angleScale);
		line(angleImg,
			lastAnglePoint,
			newAnglePoint,
			Scalar(0, 0, 255));
		lastAnglePoint = newAnglePoint;
#endif
	}
#ifdef ANGLE_LINE_GRAPH
	// Draw last line in line graph
	line(angleImg,
		lastAnglePoint,
		Point(width * count, height/2),
		Scalar(0, 0, 255));
#endif

	// Display graph images
	imshow("Input", inputImg);
	imshow("Magnitude", magImg);
	imshow("Angle", angleImg);
	waitKey(0);
	return 0;
}