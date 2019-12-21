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
	Mat OriginalImg, BinaryImg, ResultImg;  //�������ͼƬ
	int	ImageWidth, ImageHeight;            //����ͼ��ĳ��ο�
	int i, j;
	unsigned char pixelB, pixelG, pixelR;  //��ͨ��ֵ
	unsigned char DifMax = 50;             //������ɫ���ֵ���ֵ
	unsigned char B = 138, G = 63, R = 23; //��ͨ������ֵ�趨���������ɫ����
	Mat element;                           //��̬ѧ�����Ĵ�С
	Mat PlantRGBImg;                       //���Ʋ�ɫ��Ƭ

	double length, area, rectArea;     //���������ܳ�����������������
	double rectDegree = 0.0;           //���ζ�=���������/�������
	double long2Short = 1.0;           //��̬��=����/�̱�

	CvRect rect;       //������
	CvBox2D box, boxTemp;       //��Ӿ���
	double axisLong = 0.0, axisShort = 0.0;//���εĳ��ߺͶ̱�
	double axisLongTemp = 0.0, axisShortTemp = 0.0;//���εĳ��ߺͶ̱�
	float  angle = 0;
	float  angleTemp = 0;
	double widthtemp;       //�м����
	bool   TestPlantFlag = 0;  //�����Ƿ���ɹ���־λ  
	char Nametemp[256];


public slots:
	void processing();
};

#endif