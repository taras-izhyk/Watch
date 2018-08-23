#include "source.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

void ImageProcessing(Mat &frame, Mat &watch_image)
{
	int morph_elem = 1;
	int morph_size = 2;
	int morph_operator = 1;

	int threshold_value = 100;
	int threshold_type = 1;
	int const max_BINARY_value = 255;

	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

	cvtColor(frame, watch_image, COLOR_BGR2GRAY); // Convert the image to Gray

	threshold(watch_image, watch_image, 60, max_BINARY_value, 1);
	
	blur(watch_image, watch_image, Size(5, 5));

	threshold(watch_image, watch_image, 170, max_BINARY_value, 0);

	morphologyEx(watch_image, watch_image, morph_operator, element);
}

void ImageProcessingForPicture(Mat &frame, Mat &watch_image)
{
	int morph_elem = 1;
	int morph_size = 2;
	int morph_operator = 1;

	int threshold_value = 100;
	int threshold_type = 1;
	int const max_BINARY_value = 255;

	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

	cvtColor(frame, watch_image, COLOR_BGR2GRAY); // Convert the image to Gray

	threshold(watch_image, watch_image, 100, max_BINARY_value, 1);

	blur(watch_image, watch_image, Size(3, 3));

	threshold(watch_image, watch_image, 170, max_BINARY_value, 0);

	morphologyEx(watch_image, watch_image, morph_operator, element);
}

void DeleteUnnecessaryLines(vector<Vec4i> &linesP, int weight, int hight, int a)
{
	Vec4i l;
	Point midlle;
	for (int i = 0; i < linesP.size(); ++i)
	{
		l = linesP[i];
		Point pt11(l[0], l[1]);
		Point pt12(l[2], l[3]);
		midlle.x = (pt11.x + pt12.x) / 2;
		midlle.y = (pt11.y + pt12.y) / 2;
		if ((midlle.x < weight / 4) | (midlle.x >(weight - weight / 4)) | (midlle.y < hight / 4) | (midlle.y >(hight - hight / 4)))
		{
			linesP.erase(linesP.begin() + i);
			--i;
		}
	}
	if (a == 0)  // it right for real time video
	{
		for (int i = 0; i < linesP.size(); ++i)
		{
			l = linesP[i];
			Point pt11(l[0], l[1]);
			Point pt12(l[2], l[3]);
			int len = norm(pt11 - pt12);
			if (len < hight / 6)
			{
				linesP.erase(linesP.begin() + i);
				--i;
			}
		}
	}
}

void SortLinesByLength(vector<Vec4i> &linesP)
{
	int len1, len2;
	int count = linesP.size();

	for (int i = 0; i < (count - 1); ++i)
	{
		Vec4i l, t, temp;
		for (int j = i + 1; j < count; ++j)
		{
			l = linesP[i];
			Point pt11(l[0], l[1]);
			Point pt12(l[2], l[3]);
			len1 = norm(pt11 - pt12);

			t = linesP[j];
			Point pt21(t[0], t[1]);
			Point pt22(t[2], t[3]);
			len2 = norm(pt21 - pt22);

			if (len1 < len2)
			{
				temp = linesP[j];
				linesP[j] = linesP[i];
				linesP[i] = temp;
			}
		}
	}

}

void FindMinuteAndHourArrows(Mat &frame, vector<Vec4i> &linesP, const int weight, const int hight, int &minute, int &hour)
{
	Vec4i l = linesP[0];
	Point pt11(l[0], l[1]);
	Point pt12(l[2], l[3]);
	Point midlle1, midlle2;
	midlle1.x = (pt11.x + pt12.x) / 2;
	midlle1.y = (pt11.y + pt12.y) / 2;

	line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, LINE_AA);

	int angle = atan2(pt11.x - pt12.x, pt11.y - pt12.y) * 180 / CV_PI;

	bool flag = 0;
	if (linesP.size() == 1)
	{
		int X = weight / 2;
		int Y = hight / 2;
		if ((midlle1.y <= Y) && (midlle1.x < X))
		{
			angle = abs(angle) + 180;
			minute = angle / 6;
			
		}
		else if ((midlle1.y >= Y) && (midlle1.x < X))
		{
			angle = abs(angle) + 180;
			minute = angle / 6;
			hour = angle / 30;
		}
		else  if ((midlle1.y >= Y) && (midlle1.x >= X))
		{
			angle = abs(angle);
			minute = angle / 6;
			hour = angle / 30;
		}
		else  if ((midlle1.y <= Y) && (midlle1.x >= X))
		{
			angle = abs(angle);
			minute = angle / 6;
			hour = angle / 30;
		}
	}
	else {
		for (int i = 1; i < linesP.size(); i++)
		{
			Vec4i temp;
			Vec4i t = linesP[i];

			Point pt21(t[0], t[1]);
			Point pt22(t[2], t[3]);

			float A11 = (pt12.y - pt11.y);
			float A12 = (pt11.x - pt12.x);
			float b1 = -(pt11.y *pt12.x - pt11.x *pt12.y);
			float A21 = pt22.y - pt21.y;
			float A22 = pt21.x - pt22.x;
			float b2 = -(pt21.y*pt22.x - pt21.x*pt22.y);
			float d = A11 * A22 - A21 * A12;
			float d1 = b1 * A22 - b2 * A12;
			float d2 = A11 * b2 - A21 * b1;

			if (abs(d) <= 0.1)
			{
				;     // Прямі не перетинаються!
			}
			else
			{
				int X = d1 / d;
				int Y = d2 / d;

				if ((X < (weight / 8) * 3) | (X > (weight - (weight / 8) * 3)) | (Y < (hight / 6) * 2) | (Y > (hight - (hight / 6) * 2)))
				{
					;
				}
				else
				{
					circle(frame, Point(X, Y), 32, Scalar(255, 0, 0), 2, 8);

					line(frame, Point(t[0], t[1]), Point(t[2], t[3]), Scalar(0, 255, 0), 3, LINE_AA);

					if ((midlle1.y <= Y) && (midlle1.x < X))
					{
						angle = abs(angle) + 180;
						minute = angle / 6;
					}
					else if ((midlle1.y >= Y) && (midlle1.x < X))
					{
						angle = abs(angle) + 180;
						minute = angle / 6;
					}
					else  if ((midlle1.y >= Y) && (midlle1.x >= X))
					{
						angle = abs(angle);
						minute = angle / 6;
					}
					else  if ((midlle1.y <= Y) && (midlle1.x >= X))
					{
						angle = abs(angle);
						minute = angle / 6;
					}

					midlle2.x = (pt21.x + pt22.x) / 2;
					midlle2.y = (pt21.y + pt22.y) / 2;

					int angle2 = atan2(pt21.x - pt22.x, pt21.y - pt22.y) * 180 / CV_PI + 1;

					if ((midlle2.y <= Y) && (midlle2.x < X))
					{
						angle2 = abs(angle2) + 180;
						hour = angle2 / 30;
					}
					else if ((midlle2.y >= Y) && (midlle2.x < X))
					{
						angle2 = abs(angle2) + 180;
						hour = angle2 / 30;
					}
					else  if ((midlle2.y >= Y) && (midlle2.x >= X))
					{
						angle2 = abs(angle2);
						hour = angle2 / 30;
					}
					else  if ((midlle2.y <= Y) && (midlle2.x >= X))
					{
						angle2 = abs(angle2);
						hour = angle2 / 30;
					}

					flag = 1;
				}
			}
			if (flag == 1) {
				break;
			}
		}
	}
}

int ArrangeMinuteAndHour(vector<int> &arr_of_values)
{
	int result = 0, count_max = 0;
	int count1 = 0, count2 = 0, value, max1, max2;
	bool flag = 0;

	sort(arr_of_values.begin(), arr_of_values.end());

	for (int i = 0; i < arr_of_values.size() - 1; ++i)
	{
		value = arr_of_values[i];
		if (flag == 0)
		{
			if (value == arr_of_values[i + 1])
			{
				count1++;
				max1 = value;
			}
			else
			{
				flag = 1;
			}
		}
		else {
			if (value == arr_of_values[i + 1])
			{
				count2++;
				max2 = value;
			}
			else
			{
				if (count1 > count2)
				{
					if (count_max > count1)
						;
					else {
						count_max = count1;
						result = max1;
					}
				}
				else
				{
					if (count_max > count2)
						;
					else {
						count_max = count2;
						result = max2;
					}
				}
			}
			flag = 0;
		}
	}

	if (count1 > count2)
	{
		if (count_max > count1)
			;
		else {
			count_max = count1;
			result = max1;
		}
	}
	else
	{
		if (count_max > count2)
			;
		else {
			count_max = count2;
			result = max2;
		}
	}
	return result;
}

void OutPutDigitalWatch(Mat &frame, int &hour, int &minute)
{
	if (hour == 0) {
		hour = 12;
	}

	string text;
	stringstream strhour, strminute;

	strhour << hour;
	strminute << minute;

	if (minute < 10)
	{
		text = strhour.str() + ":0" + strminute.str();
	}
	else
	{
		text = strhour.str() + ":" + strminute.str();
	}
	
	putText(frame, text, Point(5, 60), FONT_HERSHEY_PLAIN, 5.0, CV_RGB(255, 0, 0), 5);
}

void DetectionByContures()
{

}