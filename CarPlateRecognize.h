#pragma once
#ifndef CarPlateRecognize_hpp
#define CarPlateRceognize_hpp

#include <QWidget>
#include "ui_CarPlateRecognize.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <io.h>
#include <opencv2/ml.hpp>

using namespace std;
using namespace cv;
using namespace ml;

class CarPlateRecognize : public QWidget
{
	Q_OBJECT

public:
	CarPlateRecognize(QWidget *parent = Q_NULLPTR);
	~CarPlateRecognize();

	void GetCarCharacter();

private:
	Ui::CarPlateRecognize ui;
	Mat in_Image, out_Image;
	Mat OriginalImg, BinaryImg, ResultImg;  //定义过程图片
	int	ImageWidth, ImageHeight;            //输入图像的长何宽
	int i, j;
	unsigned char pixelB, pixelG, pixelR;  //各通道值
	unsigned char DifMax = 50;             //基于颜色区分的阈值
	unsigned char B = 138, G = 63, R = 23; //各通道的阈值设定，针对与蓝色车牌
	Mat element;                           //形态学处理窗的大小
	Mat PlantRGBImg;                       //车牌彩色照片

	double length, area, rectArea;     //定义轮廓周长、面积、外界矩形面积
	double rectDegree = 0.0;           //矩形度=外界矩形面积/轮廓面积
	double long2Short = 1.0;           //体态比=长边/短边

	CvRect rect;       //外界矩形
	CvBox2D box, boxTemp;       //外接矩形
	double axisLong = 0.0, axisShort = 0.0;//矩形的长边和短边
	double axisLongTemp = 0.0, axisShortTemp = 0.0;//矩形的长边和短边
	float  angle = 0;
	float  angleTemp = 0;
	double widthtemp;       //中间变量
	bool   TestPlantFlag = 0;  //车牌是否检测成功标志位  
	char Nametemp[256];


public slots:
	void processing();
};

#endif