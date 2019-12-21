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


	void OriginalShow();//��ʾԭͼ
	void GrayProcess();//��ʾ�Ҷ�ͼ
	void GussProcess();//��˹ģ������
	void SobelProcess();//Sobel������ȡ
	void ThresholdProcess();//��ֵ��
	void CloseProcess();//������
	void ClearLittlePointsProcess();//���͸�ʴ����
	void getMaxArea();//���������
	void showplate();//�����ַ���
	

private:
	Ui::QtCatchCarLicenseClass ui;
	Mat src;//ԭͼ
	Mat gray;//�Ҷ�ͼ
	Mat gray1;
	Mat guss;//��˹�˲����ͼ��
	Mat sobel;//sobel������ͼ��
	Mat tsd;//��ֵ��ͼ��
	Mat closeImg;//�������ͼ��
	Mat markedImg;//���
	Mat plateImg;//��������
	Mat result;
	vector<Point> maxArea;//�������
	RotatedRect minRect;//��С��Ӿ���
	Point2f P[4];//���εĶ���


    CarPlateRecognize* plate;//�´���


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