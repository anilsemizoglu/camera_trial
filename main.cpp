#include <windows.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <tchar.h>
#include "utility.h"
#include "ini_reader.h"
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
void Threshold_Demo(int, void*);

int c = 0, cols = 0, rows = 0;
int tim = 0;
char buffer[512];

cv::Mat img,draw,img_tres;

const char* settings_file = "settings.txt";
SYSTEMTIME Image_LocaleTime; 

// settings structure for control 
// of the camera with a text file
struct settings_s{
	bool show_images;
	bool img_process;

	int ExposureTime;
	int Latitude;
	int Longitude;
	int Altitude;
} Settings;

// function to set the settings, IniGetBool etc. are from ini_reader.cpp
int CameraLoadSettings( const char* filename )
{
    FILE* file = fopen( filename, "r" );
        if ( file == NULL )
                printf( "Error opening settings file.\n" );
	Settings.show_images =  IniGetBool(  file,"show_images" , false );
	Settings.img_process =  IniGetBool(  file,"img_process" , false );

	Settings.ExposureTime = IniGetInt( file, "ExposureTime", 20 );
	Settings.Latitude =     IniGetFloat( file,"Latitude"	, 0.0f );
        Settings.Longitude =    IniGetFloat( file,"Longitude"	, 0.0f );
        Settings.Altitude =     IniGetFloat( file,"Altitude"	, 0.0f );
	return 1;
}

// image processing, data outputing and GUI is dealt with in here
void FrameCallBack(TProcessedDataProperty* Attributes, unsigned char* BytePtr){
	cols = Attributes->Column;
	rows = Attributes->Row;

	SYSTEMTIME localtime;
        GetLocalTime( &localtime );
        Image_LocalTime = localtime;

	cout << Image_LocalTime.wHour << ":" << Image_LocalTime.wMinute << ":" <<Image_LocalTime.wSecond << endl;

	img = Mat(rows,cols,CV_8U,BytePtr);
	if (img.empty()) 	
	{
	cout << "Error : Image cannot be loaded..!!" << endl;
	cv::waitKey();
	}


	cv::threshold( img, img_tres, 220, 255,3 );
	vector<vector<cv::Point> > contours; //create the vectors where the contours are drawn
	cv::findContours(img_tres, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE); //find the contours of the tresholded image output to contours v<v<Points>>

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

	int radius_r = 2;

	cv::drawContours(img, contours, c_idx, cv::Scalar(72,118,255),1.5); 	
	cv::circle	(img, cvPoint(mc[c_idx].x, mc[c_idx].y), radius_r, CV_RGB(255, 0, 0), -1, 8, 0);

	if(Settings.show_images)
	imshow("",img);
		}
	}




void CameraFaultCallBack(int ImageType)
{
	printf("Error: Camera fault.\n");
	BUFCCDUSB_StopFrameGrab();
	BUFCCDUSB_StopCameraEngine();
	BUFCCDUSB_UnInitDevice();
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

	printf( "Loading camera settings from file: %s\n", settings_file );
        CameraLoadSettings(settings_file);


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
	ret = BUFCCDUSB_StartFrameGrab(0x8888);
	cout << "Start Frame Grab: " << ret << endl;

	ret = BUFCCDUSB_SetExposureTime( 1, Settings.ExposureTime);


	for(;;){

	// get the frame, save it 
	// with the time stamp in FrameCallBack
	ret = BUFCCDUSB_InstallFrameHooker(1, FrameCallBack);

	// FrameCallBack needs this message loop 
	// to keep the camera engine active
	if (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	}

	stopCamera();

	cout << "success stopCamera: " << ret << endl;

	return 0;
}

