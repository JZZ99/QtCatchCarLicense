#pragma once

#ifndef QtCatchCarLicense_hpp
#define QtCatchCarLicense_hpp

#include <QtWidgets/QMainWindow>
#include "ui_QtCatchCarLicense.h"
#include "CarPlateRecognize.h" 
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <io.h>
#include <opencv2/ml.hpp>

using namespace std;
using namespace cv;
using namespace ml;


class QtCatchCarLicense : public QMainWindow
{
	Q_OBJECT

public:
	QtCatchCarLicense(QWidget *parent = Q_NULLPTR);


	void OriginalShow();//显示原图
	void GrayProcess();//显示灰度图
	void GussProcess();//高斯模糊降噪
	void SobelProcess();//Sobel轮廓提取
	void ThresholdProcess();//二值化
	void CloseProcess();//闭运算
	void ClearLittlePointsProcess();//膨胀腐蚀操作
	void getMaxArea();//找最大轮廓
	void showplate();//车牌字符化
	

private:
	Ui::QtCatchCarLicenseClass ui;
	Mat src;//原图
	Mat gray;//灰度图
	Mat gray1;
	Mat guss;//高斯滤波后的图像
	Mat sobel;//sobel计算后的图像
	Mat tsd;//二值化图像
	Mat closeImg;//闭运算后图像
	Mat markedImg;//标记
	Mat plateImg;//车牌区域
	Mat result;
	vector<Point> maxArea;//最大区域
	RotatedRect minRect;//最小外接矩形
	Point2f P[4];//矩形的顶点


    CarPlateRecognize* plate;//新窗口


public slots:
	void about_triggered();
	void openImg_triggered();
	void gray_onclick();
	void guss_onclick();
	void sobel_onclick();
	void threshold_onclick();
	void close_onclick();
	void clear_onclick();
	void drawrect_onclick();
	void carplateshow_onclick();
};


#endif