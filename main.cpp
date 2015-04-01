#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <tchar.h>
#include "utility.h"
#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
//#include "SDL.h"

#include "BUF_USBCCDCamera_SDK.h"

using namespace cv;
using namespace std;


#define CCD_FRAME_RAW 0
#define FRAME_TYPE	CCD_FRAME_RAW
#define WIDTH    1280
#define HEIGHT   960

void* pixel_data = NULL;

int c = 0, cols = 0, rows = 0;
int time = 0;
	cv::Mat img;
/*
void FrameCallBackHelper(unsigned char* BytePtr){
cout << "here 0\n";
	img = cv::Mat(rows, cols, CV_8UC1, &BytePtr);	

cout << "here 1\n";
//detection starts
	if (img.empty()) 	
	{
cout << "here 2\n";
		cout << "Error : Image cannot be loaded..!!" << endl;
		cin >> cols;
	}
	cv::imshow("",img);
cout << "here 3\n";
  	cv::threshold( img, img, 150, 255,3 );
cout << "here 4\n";
	vector<vector<cv::Point> > contours; //create the vectors where the contours are drawn
cin >> cols;
cout << "here 5\n";
	//cv::findContours(img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE); //find the contours of the tresholded image output to contours v<v<Points>>
cout << "here 6\n";
		cin >> cols;
//find the largest contour
	int c_idx = -1,largestContour = 0;
	for( int i = 0; i< contours.size(); i++ )
	{
	if(contours[i].size() > largestContour){
	largestContour = contours[i].size();
	c_idx = i;
		}
	}

	if(c_idx != -1){
//find the center of contour
	vector<cv::Moments> mu(contours.size());
	mu[c_idx] = moments(contours[c_idx], false);
	vector<cv::Point2f> mc(contours.size());
	mc[c_idx] = cv::Point2f(mu[c_idx].m10 / mu[c_idx].m00, mu[c_idx].m01 / mu[c_idx].m00);
	

}
}
*/

void FrameCallBack(TProcessedDataProperty* Attributes, unsigned char* BytePtr){
	cols = Attributes->Column;
	rows = Attributes->Row;


	memcpy(pixel_data,BytePtr, WIDTH*HEIGHT);
c++;

	char buffer[512];
	sprintf(buffer, "imgs\\comon_%d.raw", time);

	FILE* file = fopen(buffer, "wb");
	if (file == NULL)
	{
		printf("Error: Could not open file.\n");
	}
	else
	{
		fwrite(BytePtr, rows*cols, 1, file);
		fclose(file);
	}



}



void CameraFaultCallBack(int ImageType)
{
	printf("Error: Camera fault.\n");
	BUFCCDUSB_StopFrameGrab();
	BUFCCDUSB_StopCameraEngine();
	BUFCCDUSB_UnInitDevice();
	//fault = true;
}
void stopCamera(){
	BUFCCDUSB_StopFrameGrab();
	BUFCCDUSB_StopCameraEngine();
	BUFCCDUSB_UnInitDevice();
}


int _tmain(int argc, _TCHAR* argv[])
{
	MSG msg;
	
	int ret;
	int d;
	//allocate static framebuffer for the camera image

	pixel_data = new unsigned char[WIDTH][HEIGHT];
memset( pixel_data,'e', WIDTH*HEIGHT);

	//init - addevice - startcameraengine - workmode - framehooker - USBHooker - framegrab


	ret = BUFCCDUSB_InitDevice();
	if (ret != 1) { cout << "no camera\n"; return 0; }

	ret = BUFCCDUSB_AddDeviceToWorkingSet(1);
	cout << "Add Device to Working set: " << ret << endl;
	ret = BUFCCDUSB_InstallUSBDeviceHooker(CameraFaultCallBack);
	cout << "USB install: " << ret << endl;



	ret = BUFCCDUSB_StartCameraEngine(NULL, 8);
	cout << "Camera Engine: " << ret << endl;
	ret = BUFCCDUSB_SetFrameTime(1, 400000);
	//cout << "Set Frame Time: " << ret << endl;
	ret = BUFCCDUSB_StartFrameGrab(0x8888);
	cout << "Start Frame Grab: " << ret << endl;



for(;;){

	time = get_date();
	ret = BUFCCDUSB_InstallFrameHooker(1, FrameCallBack);
	if (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}










	//imshow("",img);


	/*
	if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
	if( msg.message == WM_QUIT )
	{
	goto exit;
	}
	else if ( msg.message == WM_TIMER )
	{
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	}
	}
	*/
	stopCamera();

	cout << "success stopCamera: " << ret << endl;

	return 0;
}

