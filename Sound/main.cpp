#include <iostream>
#include <Windows.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "../FourierTransform.h"

#pragma comment(lib, "winmm.lib")

using namespace cv;

#define ANGLE_LINE_GRAPH

void doRecord(short int* buffer, int count);

int main() {
	FourierTransform ft;

	const int bufferCount = 44100 * 5;
	short int data[bufferCount];

	doRecord(data, bufferCount);

	float maxData = 0;
	for(int i = 0; i < bufferCount; i++) {
		ft.feed(data[i], 1.0 / 44100);
		maxData = max(maxData, abs(data[i]) * 1.0f);
	}

	int height = 400;

	/* Draw imput data */
	const float timeWidth = 1000;
	float totalTime = ft.getCurrentTime();
	//const float inputScale = 0.01f;
	float inputScale = height / maxData;
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
	const float freqDiff = 1.0f / 4;// 1.0f / 8.0f;// 0.125f;
	const int width = 6;
	const float count = freqEnd - freqStart;
	/* Setup magnitude image */
	const float magScale = 5e-6f;
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
			Point((freq - freqStart) * width, height / 2),
			Point((freq - freqStart + df) * width, height / 2 - ft.angle(res) * angleScale),
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
		Point(width * count, height / 2),
		Scalar(0, 0, 255));
#endif

	// Display graph images
	imshow("Input", inputImg);
	imshow("Magnitude", magImg);
	imshow("Angle", angleImg);
	waitKey(0);
	return 0;
}

void doRecord(short int* buffer, int count) {
	const int sampleRate = 44100;

	HWAVEIN waveIn;
	//MMRESULT result;
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.nSamplesPerSec = sampleRate;
	format.nAvgBytesPerSec = sampleRate * 2;
	format.nBlockAlign = 2;
	format.wBitsPerSample = 16;
	format.cbSize = 0;

	waveInOpen(&waveIn, WAVE_MAPPER, &format, 0L, 0L, WAVE_FORMAT_DIRECT);
	WAVEHDR waveInHdr;
	waveInHdr.lpData = (LPSTR) buffer;
	waveInHdr.dwBufferLength = sizeof(short int) * count;
	waveInHdr.dwBytesRecorded = 0;
	waveInHdr.dwUser = 0L;
	waveInHdr.dwFlags = 0L;
	waveInHdr.dwLoops = 0L;
	waveInPrepareHeader(waveIn, &waveInHdr, sizeof(WAVEHDR));

	waveInAddBuffer(waveIn, &waveInHdr, sizeof(WAVEHDR));
	waveInStart(waveIn);
	
	std::cout << "Recording..." << std::endl;
	Sleep(count * 1000 / sampleRate);
	std::cout << "Record done." << std::endl;
	waveInClose(waveIn);
}