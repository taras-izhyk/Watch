//#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

#include "source.h"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

const string ImageFolder = "C:\\Users\\Administrator\\source\\repos\\Watch\\Watch\\WatchIm\\";

int main()
{	
	Mat input_frame, watch_image;
	vector<Vec4i> linesP;
	int minute = 0;
	int hour = 0;
	
	int method = 1;

	cout << endl << "  Enter your choice - ";
	cin >> method;


	if (method == 2)
	{
		int arrange = 0;
		vector<int> arr_of_hours(11);
		vector<int> arr_of_minutes(11);
		bool flag_hour = 0;

		VideoCapture camera(0);
		if (!camera.isOpened())
		{
			return -1;
		}

		namedWindow("Video", WINDOW_AUTOSIZE);

		camera >> input_frame;

		while (1)
		{
			camera >> input_frame;
			
			//imwrite("watchphoto.jpg", frame);

			Mat canny_image;
			
			const int weight = input_frame.cols;
			const int hight = input_frame.rows;
			
			ImageProcessing(input_frame, watch_image);
			
			Canny(watch_image, canny_image, 50, 200, 3);

			HoughLinesP(canny_image, linesP, 1, CV_PI / 180, 40, 30, 3); // runs the actual detection

			if (linesP.size() == 0)
			{
				imshow("Video", input_frame);
				continue;
			}

			DeleteUnnecessaryLines(linesP, weight, hight, 0);

			if (linesP.size() == 0)
			{
				imshow("Video", input_frame);
				continue;
			}

			SortLinesByLength(linesP);

			FindMinuteAndHourArrows(input_frame, linesP, weight, hight, minute, hour);
			
			if (arrange <= 5) {
				arr_of_hours[arrange] = hour;
				arr_of_minutes[arrange] = minute;
			}
			else {
				arr_of_hours[arrange] = hour;
				arr_of_minutes[arrange] = minute;
				flag_hour = 1;
			}

			if (arrange % 5 == 0 && flag_hour == 1)
			{
				hour = ArrangeMinuteAndHour(arr_of_hours);
				minute = ArrangeMinuteAndHour(arr_of_minutes);
			}

			if (arrange < 11) {
				arrange++;
			}
			else {
				arrange = 0;
			}

		//flip(frame, frame, 1);

		OutPutDigitalWatch(input_frame, hour, minute);

		imshow("Video", input_frame);

		if (waitKey(300) == 27)
		{
			break;
		}

		}
	}
	else  if(method == 1)
	{
		for (int i = 1; i <= 8; ++i)   // for presentation
		{

			stringstream ss;
			ss << i;
			std::string str;
			ss >> str;
			
			Mat watch = imread(ImageFolder + "watch" + str + ".jpg"); // Loads image form disk
			if (watch.empty())
			{
				return -1;
			}

			const int weight = watch.cols;
			const int hight = watch.rows;

			imshow("Watch Image", watch);
			waitKey(2000);

			ImageProcessingForPicture(watch, watch_image);

			imshow("Watch Image", watch_image);
			waitKey(1000);

			Mat canny_image;

			Canny(watch_image, canny_image, 50, 200, 3);
			
			imshow("Watch Image", canny_image);
			waitKey(1000);

			HoughLinesP(canny_image, linesP, 1, CV_PI / 180, 40, 30, 3); // runs the actual detection

			if (linesP.size() == 0)
			{
				cout << endl << "  Sorry, lines not found.  " << endl;
			}
			else {
				DeleteUnnecessaryLines(linesP, weight, hight, 1);

				if (linesP.size() == 0)
				{
					cout << endl << "  Sorry, lines not found.  " << endl;
				}
				else
				{
					SortLinesByLength(linesP);

					FindMinuteAndHourArrows(watch, linesP, weight, hight, minute, hour);
					
					imshow("Watch Image", watch);
					waitKey(2000);

					OutPutDigitalWatch(watch, hour, minute);

					imshow("Watch Image", watch);
					
					if (waitKey(3000) == 27)
					{
						break;
					}

				}
			}
		}
	}
	else
	{
		DetectionByContures();
	}

	system("pause");

	return 0;
}