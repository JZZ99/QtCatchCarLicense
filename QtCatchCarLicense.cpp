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
	imshow("ԭͼ", src1);
}

void QtCatchCarLicense::GrayProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	//imshow("�Ҷ�ͼ", gray);
	QImage gray1 = QImage((const unsigned char*)(gray.data), gray.cols, gray.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(gray1));
}

void QtCatchCarLicense::GussProcess() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	//imshow("��˹�˲�", guss);
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

	//�� X�����ݶ�  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X����Sobel", absX);*/

		//��Y�����ݶ�  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y����Sobel", absY);*/

		//�ϲ��ݶ�(����)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);
	//imshow("Sobel����", sobel);
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

	//�� X�����ݶ�  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X����Sobel", absX);*/

		//��Y�����ݶ�  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y����Sobel", absY);*/

		//�ϲ��ݶ�(����)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);

	//threshold ������ͨ�������Ҷ�ͼ�е㣬��ͼ����Ϣ��ֵ������������ͼƬֻ�ж���ɫֵ��
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);
	//imshow("��ֵ������", tsd);
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

	//�� X�����ݶ�  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X����Sobel", absX);*/

		//��Y�����ݶ�  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y����Sobel", absY);*/

		//�ϲ��ݶ�(����)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);

	//threshold ������ͨ�������Ҷ�ͼ�е㣬��ͼ����Ϣ��ֵ������������ͼƬֻ�ж���ɫֵ��
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);

	//�����
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 17));
	//������̬ѧ����
	morphologyEx(tsd, closeImg, MORPH_CLOSE, element);
	//imshow("�����㴦��", closeImg);
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

	//�� X�����ݶ�  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X����Sobel", absX);*/

		//��Y�����ݶ�  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y����Sobel", absY);*/

		//�ϲ��ݶ�(����)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);

	//threshold ������ͨ�������Ҷ�ͼ�е㣬��ͼ����Ϣ��ֵ������������ͼƬֻ�ж���ɫֵ��
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);

	//�����
	Mat element = getStructuringElement(MORPH_RECT, Size(17, 17));
	//������̬ѧ����
	morphologyEx(tsd, closeImg, MORPH_CLOSE, element);

	//��ʴ����
	Mat ele = getStructuringElement(MORPH_RECT, Size(5, 5));//getStructuringElement����ֵ�����ں˾���
	erode(closeImg, closeImg, ele);//erode����ֱ�ӽ��и�ʴ����
/*	imshow("ȥ��С����", closeImg);*/

	//���Ͳ���
	ele = getStructuringElement(MORPH_RECT, Size(17, 5));//getStructuringElement����ֵ�����ں˾���
	dilate(closeImg, closeImg, ele);//dilate����ֱ�ӽ������Ͳ���
	//imshow("���ͺ���ͼ", closeImg);
	QImage closeImg2 = QImage((const unsigned char*)(closeImg.data), closeImg.cols, closeImg.rows, QImage::Format_Indexed8);
	ui.Img->setPixmap(QPixmap::fromImage(closeImg2));
}

void QtCatchCarLicense::getMaxArea() {
	cvtColor(src, gray, COLOR_BGR2GRAY);
	cvtColor(src, markedImg, CV_16S);
	//imshow("�Ҷ�ͼ", gray);
	GaussianBlur(gray, guss, Size(3, 3), 0);
	//imshow("��˹�˲�", guss);
	Mat sobelY;
	Mat sobelX;
	Mat absX;
	Mat absY;

	//�� X�����ݶ�  
	Sobel(guss, sobelX, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelX, absX);
	/*	imshow("X����Sobel", absX);*/

		//��Y�����ݶ�  
	Sobel(guss, sobelY, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(sobelY, absY);
	/*	imshow("Y����Sobel", absY);*/

		//�ϲ��ݶ�(����)  
	addWeighted(absX, 0.5, absY, 0.5, 0, sobel);
	//imshow("Sobel����", sobel);


	//threshold ������ͨ�������Ҷ�ͼ�е㣬��ͼ����Ϣ��ֵ������������ͼƬֻ�ж���ɫֵ��
	threshold(sobel, tsd, 180, 255, CV_THRESH_BINARY);
	//imshow("��ֵ������", tsd);

	//�����
	Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));
	//������̬ѧ����
	morphologyEx(tsd, closeImg, MORPH_CLOSE, element);
	//imshow("�����㴦��", closeImg);


	//��ʴ����
	Mat ele = getStructuringElement(MORPH_RECT, Size(5, 5));//getStructuringElement����ֵ�����ں˾���
	erode(closeImg, closeImg, ele);//erode����ֱ�ӽ��и�ʴ����
/*	imshow("ȥ��С����", closeImg);*/

	//���Ͳ���
	ele = getStructuringElement(MORPH_RECT, Size(17, 5));//getStructuringElement����ֵ�����ں˾���
	dilate(closeImg, closeImg, ele);//dilate����ֱ�ӽ������Ͳ���
	//imshow("���ͺ���ͼ", closeImg);

	//ȡ����
	vector< vector< Point> > contours;
	findContours(closeImg,
		contours, // a vector of contours
		CV_RETR_EXTERNAL, // ��ȡ�ⲿ����
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
	//�����������
	for (size_t i = 0; i < maxArea.size(); i++)
	{
		//circle(markedImg, maxArea[i], 1, Scalar(0,0,255));
	}

	//������С��Ӿ���
	minRect = minAreaRect(maxArea);
	minRect.points(P);
	for (int j = 0; j <= 3; j++)
	{
		//line(markedImg, P[j], P[(j + 1) % 4], Scalar(255), 2);
	}
	//imshow("���", markedImg);
	Point2f center = minRect.center;
	//��ȡROI
	result = markedImg(Rect((center.x - (minRect.size.width / 2) - 30),
		center.y - ((minRect.size.height / 2) + 10), minRect.size.width + 40, minRect.size.height + 20));
	//imshow("����", result);
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
	

//�Ҷȴ���
void QtCatchCarLicense::gray_onclick() {
	GrayProcess();
}

//��˹ģ������
void QtCatchCarLicense::guss_onclick() {
	GussProcess();
}

//Sobel����
void QtCatchCarLicense::sobel_onclick() {
	SobelProcess();
}

//��ֵ������
void QtCatchCarLicense::threshold_onclick() {
	ThresholdProcess();
}

//�����㴦��
void QtCatchCarLicense::close_onclick() {
	CloseProcess();
}

//��ʴ���ʹ���
void QtCatchCarLicense::clear_onclick() {
	ClearLittlePointsProcess();
}

//������С��Ӿ���
void QtCatchCarLicense::drawrect_onclick() {
	getMaxArea();
}

//�����ַ���
void QtCatchCarLicense::carplateshow_onclick() {
	showplate();
}
