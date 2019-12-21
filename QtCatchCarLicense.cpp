#include "QtCatchCarLicense.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QScrollArea>

using namespace cv;
QtCatchCarLicense::QtCatchCarLicense(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
}


void QtCatchCarLicense::about_triggered()
{
	QMessageBox msg;
	msg.setText("OpenCv Project\nDesigned by ld & ljw");
	msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msg.setDefaultButton(QMessageBox::Cancel);
	msg.exec();
}

void QtCatchCarLicense::openImg_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("open image file"),
		"./", tr("Image files(*.bmp *.jpg *.pbm *.pgm *.png *.ppm *.xbm *.xpm);;All files (*.*)"));
	if (fileName.isEmpty())
	{
		QMessageBox mesg;
		mesg.warning(this, "warnning", "failed to open image!");
		return;
	}
	else{
		string str = fileName.toStdString();
		src = imread(str);
		cvtColor(src, src, CV_BGR2RGB);
		//cv::resize(src, src, Size(300, 200));
		QImage image = QImage((const unsigned char*)(src.data), src.cols, src.rows, QImage::Format_RGB888);
		ui.Img->setPixmap(QPixmap::fromImage(image));
	}		
}

void QtCatchCarLicense::OriginalShow() {
	Mat src1 = src.clone();
	imshow("原图", src1);
}

void QtCatchCarLicense::GrayProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	//imshow("灰度图", gray);
	QImage gray1 = QImage((const unsigned char*)(gray.data), gray.cols, gray.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(gray1));
}

void QtCatchCarLicense::GussProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	//imshow("高斯滤波", guss);
	QImage guss1 = QImage((const unsigned char*)(guss.data), guss.cols, guss.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(guss1));
}

void QtCatchCarLicense::SobelProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	Mat sobelY;
	Mat sobelX;
	Mat absX;
	Mat absY;

	//求 X方向梯度  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X方向Sobel", absX);*/

		//求Y方向梯度  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y方向Sobel", absY);*/

		//合并梯度(近似)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);
	//imshow("Sobel处理", sobel);
	QImage sobel1 = QImage((const unsigned char*)(sobel.data), sobel.cols, sobel.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(sobel1));
}

void QtCatchCarLicense::ThresholdProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	Mat sobelY;
	Mat sobelX;
	Mat absX;
	Mat absY;

	//求 X方向梯度  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X方向Sobel", absX);*/

		//求Y方向梯度  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y方向Sobel", absY);*/

		//合并梯度(近似)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);

	//threshold 方法是通过遍历灰度图中点，将图像信息二值化，处理过后的图片只有二种色值。
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);
	//imshow("二值化处理", tsd);
	QImage tsd1 = QImage((const unsigned char*)(tsd.data), tsd.cols, tsd.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(tsd1));
}

void QtCatchCarLicense::CloseProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	Mat sobelY;
	Mat sobelX;
	Mat absX;
	Mat absY;

	//求 X方向梯度  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X方向Sobel", absX);*/

		//求Y方向梯度  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y方向Sobel", absY);*/

		//合并梯度(近似)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);

	//threshold 方法是通过遍历灰度图中点，将图像信息二值化，处理过后的图片只有二种色值。
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);

	//定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 17));
	//进行形态学操作
	morphologyEx(tsd, closeImg, MORPH_CLOSE, element);
	//imshow("闭运算处理", closeImg);
	QImage closeImg1 = QImage((const unsigned char*)(closeImg.data), closeImg .cols, closeImg.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(closeImg1));
}

void QtCatchCarLicense::ClearLittlePointsProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	Mat sobelY;
	Mat sobelX;
	Mat absX;
	Mat absY;

	//求 X方向梯度  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X方向Sobel", absX);*/

		//求Y方向梯度  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y方向Sobel", absY);*/

		//合并梯度(近似)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);

	//threshold 方法是通过遍历灰度图中点，将图像信息二值化，处理过后的图片只有二种色值。
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);

	//定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 17));
	//进行形态学操作
	morphologyEx(tsd, closeImg, MORPH_CLOSE, element);

	//腐蚀操作
	Mat ele = getStructuringElement(MORPH_RECT, Size(5, 5));//getStructuringElement返回值定义内核矩阵
	erode(closeImg, closeImg, ele);//erode函数直接进行腐蚀操作
/*	imshow("去除小区域", closeImg);*/

	//膨胀操作
	ele = getStructuringElement(MORPH_RECT, Size(17, 5));//getStructuringElement返回值定义内核矩阵
	dilate(closeImg, closeImg, ele);//dilate函数直接进行膨胀操作
	//imshow("膨胀后结果图", closeImg);
	QImage closeImg2 = QImage((const unsigned char*)(closeImg.data), closeImg.cols, closeImg.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(closeImg2));
}

void QtCatchCarLicense::getMaxArea() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	cvtColor(src, markedImg, CV_16S);
	//imshow("灰度图", gray);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	//imshow("高斯滤波", guss);
	Mat sobelY;
	Mat sobelX;
	Mat absX;
	Mat absY;

	//求 X方向梯度  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X方向Sobel", absX);*/

		//求Y方向梯度  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y方向Sobel", absY);*/

		//合并梯度(近似)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);
	//imshow("Sobel处理", sobel);


	//threshold 方法是通过遍历灰度图中点，将图像信息二值化，处理过后的图片只有二种色值。
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);
	//imshow("二值化处理", tsd);

	//定义核
	Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));
	//进行形态学操作
	morphologyEx(tsd, closeImg, MORPH_CLOSE, element);
	//imshow("闭运算处理", closeImg);


	//腐蚀操作
	Mat ele = getStructuringElement(MORPH_RECT, Size(5, 5));//getStructuringElement返回值定义内核矩阵
	erode(closeImg, closeImg, ele);//erode函数直接进行腐蚀操作
/*	imshow("去除小区域", closeImg);*/

	//膨胀操作
	ele = getStructuringElement(MORPH_RECT, Size(17, 5));//getStructuringElement返回值定义内核矩阵
	dilate(closeImg, closeImg, ele);//dilate函数直接进行膨胀操作
	//imshow("膨胀后结果图", closeImg);

	//取轮廓
	vector< vector< Point> > contours;
	findContours(closeImg,
		contours, // a vector of contours
		CV_RETR_EXTERNAL, // 提取外部轮廓
		CV_CHAIN_APPROX_NONE); // all pixels of each contours
	int max = 0;
	for (size_t i = 0; i < contours.size(); i++)
	{
		vector<Point> p;
		p = contours[i];
		if (p.size() > max)
		{
			max = p.size();
			maxArea = p;
		}
	}
	//绘制最大区域
	for (size_t i = 0; i < maxArea.size(); i++)
	{
		//circle(markedImg, maxArea[i], 1, Scalar(0,0,255));
	}

	//绘制最小外接矩形
	minRect = minAreaRect(maxArea);
	minRect.points(P);
	for (int j = 0; j <= 3; j++)
	{
		//line(markedImg, P[j], P[(j + 1) % 4], Scalar(255), 2);
	}
	//imshow("标记", markedImg);
	Point2f center = minRect.center;
	//提取ROI
	result = markedImg(Rect((center.x - (minRect.size.width / 2) - 30),
		center.y - ((minRect.size.height / 2) + 10), minRect.size.width + 40, minRect.size.height + 20));
	//imshow("车牌", result);
	/*cvtColor(result, gray1, COLOR_BGR2GRAY);
	imshow("huidu", gray1);*/
	Mat img1 = result.clone();
	cv::resize(img1, img1, Size(120, 40));
	//imshow("1",img1);
	QImage result1 = QImage((const unsigned char*)(img1.data), img1.cols, img1.rows, QImage::Format_RGB888);
	ui.Result->setPixmap(QPixmap::fromImage(result1));
	
}

void QtCatchCarLicense::showplate() {
	plate = new CarPlateRecognize;
	plate->show();
}
	

//灰度处理
void QtCatchCarLicense::gray_onclick() {
	GrayProcess();
}

//高斯模糊降噪
void QtCatchCarLicense::guss_onclick() {
	GussProcess();
}

//Sobel处理
void QtCatchCarLicense::sobel_onclick() {
	SobelProcess();
}

//二值化处理
void QtCatchCarLicense::threshold_onclick() {
	ThresholdProcess();
}

//闭运算处理
void QtCatchCarLicense::close_onclick() {
	CloseProcess();
}

//腐蚀膨胀处理
void QtCatchCarLicense::clear_onclick() {
	ClearLittlePointsProcess();
}

//绘制最小外接矩形
void QtCatchCarLicense::drawrect_onclick() {
	getMaxArea();
}

//车牌字符化
void QtCatchCarLicense::carplateshow_onclick() {
	showplate();
}
