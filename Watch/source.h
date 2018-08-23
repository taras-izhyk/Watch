#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\imgproc.hpp>

using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

void ImageProcessing(Mat& frame, Mat& watch_image);

void ImageProcessingForPicture(Mat &frame, Mat &watch_image);

void DeleteUnnecessaryLines(vector<Vec4i> &linesP, const int weight, const int hight, int a);

void SortLinesByLength(vector<Vec4i> &linesP);

void FindMinuteAndHourArrows(Mat &frame, vector<Vec4i> &linesP, const int weight, const int hight, int &minute, int &hour);

int ArrangeMinuteAndHour(vector<int> &arr_of_values);

void OutPutDigitalWatch(Mat &frame, int &hour, int &minute);

void DetectionByContures();