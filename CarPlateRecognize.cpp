#include "CarPlateRecognize.h"
#include "QtCatchCarLicense.h" 
#include "ui_QtCatchCarLicense.h"

CarPlateRecognize::CarPlateRecognize(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

CarPlateRecognize::~CarPlateRecognize()
{
}

void CarPlateRecognize::processing() {
	GetCarCharacter();
}

void CarPlateRecognize::GetCarCharacter () {
	OriginalImg = imread("car.bmp", IMREAD_COLOR);   //��ԭʼͼƬ
	//imshow("ԭͼ", OriginalImg);                   //��ʾͼƬ

	double  BeginTime = (double)getTickCount();     //ʶ��ʼê��

	ImageWidth = OriginalImg.rows;                 //��ȡͼƬ�Ŀ�
	ImageHeight = OriginalImg.cols;                 //��ȡͼ��ĳ�
//	cout << "width:" << ImageWidth << "\t height:" << ImageHeight << endl;  //��ӡͼƬ�ĳ��Ϳ�

	BinaryImg = OriginalImg.clone();               //��¡ԭʼͼ��
	for (i = 0; i < ImageWidth; i++)               //ͨ����ɫ������ͼƬ���ж�ֵ������
	{
		for (j = 0; j < ImageHeight; j++)
		{
			pixelB = BinaryImg.at<Vec3b>(i, j)[0]; //��ȡͼƬ����ͨ����ֵ
			pixelG = BinaryImg.at<Vec3b>(i, j)[1];
			pixelR = BinaryImg.at<Vec3b>(i, j)[2];

			if (abs(pixelB - B) < DifMax && abs(pixelG - G) < DifMax && abs(pixelR - R) < DifMax)
			{                                           //������ͨ����ֵ�͸���ͨ����ֵ���бȽ�
				BinaryImg.at<Vec3b>(i, j)[0] = 255;     //������ɫ��ֵ��Χ�ڵ����óɰ�ɫ
				BinaryImg.at<Vec3b>(i, j)[1] = 255;
				BinaryImg.at<Vec3b>(i, j)[2] = 255;
			}
			else
			{
				BinaryImg.at<Vec3b>(i, j)[0] = 0;        //��������ɫ��ֵ��Χ�ڵ�����Ϊ��ɫ
				BinaryImg.at<Vec3b>(i, j)[1] = 0;
				BinaryImg.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}
	//	imshow("��ֵ�������ͼƬ", BinaryImg);        //��ʾ��ֵ������֮���ͼ��

	element = getStructuringElement(MORPH_RECT, Size(3, 3)); //������̬ѧ�����Ĵ�С
	dilate(BinaryImg, BinaryImg, element);     //���ж�����Ͳ���
	dilate(BinaryImg, BinaryImg, element);
	dilate(BinaryImg, BinaryImg, element);

	erode(BinaryImg, BinaryImg, element);      //���ж�θ�ʴ����
	erode(BinaryImg, BinaryImg, element);
	erode(BinaryImg, BinaryImg, element);
	//	imshow("��̬ѧ�����", BinaryImg);           //��ʾ��̬ѧ����֮���ͼ��

	cvtColor(BinaryImg, BinaryImg, CV_BGR2GRAY); //����̬ѧ����֮���ͼ��ת��Ϊ�Ҷ�ͼ��
	threshold(BinaryImg, BinaryImg, 100, 255, THRESH_BINARY); //�Ҷ�ͼ���ֵ��




	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* seq = 0;     //����һ������,CvSeq�������һ���������������У����ǹ̶�������
	CvSeq* tempSeq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);

	IplImage* BwImg;                //����IplImageָ��
	BwImg = &(IplImage(BinaryImg)); //��MATǿ��ת��ΪIplImageָ������
	int cnt = cvFindContours(BwImg, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//������������Ŀ 
	//��ȡ��ֵͼ���������ĸ���
//	cout << "number of contours   " << cnt << endl;  //��ӡ��������
	for (tempSeq = seq; tempSeq != NULL; tempSeq = tempSeq->h_next)
	{
		length = cvArcLength(tempSeq);       //��ȡ�����ܳ�
		area = cvContourArea(tempSeq);       //��ȡ�������
		if (area > 1000 && area < 50000)     //�������������С�ж�
		{
			rect = cvBoundingRect(tempSeq, 1);//������α߽�
			CvPoint2D32f pt[4];
			boxTemp = cvMinAreaRect2(tempSeq, 0);  //��ȡ�����ľ���
			cvBoxPoints(boxTemp, pt);              //��ȡ�����ĸ���������
			angleTemp = boxTemp.angle;                 //�õ�������б�Ƕ�

			axisLongTemp = sqrt(pow(pt[1].x - pt[0].x, 2) + pow(pt[1].y - pt[0].y, 2));  //���㳤�ᣨ���ɶ���
			axisShortTemp = sqrt(pow(pt[2].x - pt[1].x, 2) + pow(pt[2].y - pt[1].y, 2)); //������ᣨ���ɶ���

			if (axisShortTemp > axisLongTemp)   //������ڳ��ᣬ��������
			{
				widthtemp = axisLongTemp;
				axisLongTemp = axisShortTemp;
				axisShortTemp = widthtemp;
			}
			else
				angleTemp += 90;
			//	cout << "��ת�Ƕȣ�" << angle << endl;
			rectArea = axisLongTemp * axisShortTemp;  //������ε����
			rectDegree = area / rectArea;     //������ζȣ���ֵԽ�ӽ�1˵��Խ�ӽ����Σ�

			long2Short = axisLongTemp / axisShortTemp; //���㳤���
			if (long2Short > 2.2 && long2Short < 3.8 && rectDegree > 0.63 && rectDegree < 1.37 && rectArea > 2000 && rectArea < 50000)
			{
				Mat CutPlant = OriginalImg.clone();
				TestPlantFlag = true;             //��⳵������ɹ�
				IplImage* ReImg;                  //������ȡ���ƿ�ͼ��
				ReImg = &(IplImage(CutPlant)); //ǿ������ת��
				for (int i = 0; i < 4; ++i)       //���߿����������
					cvLine(ReImg, cvPointFrom32f(pt[i]), cvPointFrom32f(pt[((i + 1) % 4) ? (i + 1) : 0]), CV_RGB(255, 0, 0));
				//cvShowImage("��ȡ���ƽ��ͼ", ReImg);    //��ʾ���ս��ͼ

				box = boxTemp;
				angle = angleTemp;
				axisLong = axisLongTemp;
				axisShort = axisShortTemp;
			}
		}
	}
	if (!TestPlantFlag)
	{
		cout << "����������ȡʧ�ܣ�����" << endl;
	}

	Mat RotractImg(ImageWidth, ImageHeight, CV_8UC1, Scalar(0, 0, 0));//��б����ͼƬ
	Point2f center = box.center;     //��ȡ������������
	Mat M2 = getRotationMatrix2D(center, angle, 1);//������ת�����ŵı任���� 
	warpAffine(OriginalImg, RotractImg, M2, OriginalImg.size(), 1, 0, Scalar(0));  //������б����
	//imshow("��б������ͼƬ", RotractImg);

	//	cout << "��������:" << box.center.x << "," << box.center.y << endl;
	Rect recttemp(center.x - axisLong / 2, center.y - axisShort / 2, axisLong, axisShort);//���峵�����ھ�������
	PlantRGBImg = RotractImg(recttemp);  //��ȡ���Ʋ�ɫ��Ƭ
//	imshow("��ȡ������", PlantRGBImg);

	cv::resize(PlantRGBImg, PlantRGBImg, Size(500, 500 * axisShort / axisLong), (0, 0), (0, 0), INTER_AREA);
	//imshow("��ȡ���Ʊ任�ߴ��", PlantRGBImg);

	Mat PlantBinImg;   //���ƶ�ֵͼ��
	cvtColor(PlantRGBImg, PlantBinImg, CV_BGR2GRAY);
	threshold(PlantBinImg, PlantBinImg, 100, 255, CV_THRESH_BINARY);//��ֵ������
	//imshow("���ƶ�ֵͼ��", PlantBinImg);

	Mat PlantBinImgTemp = PlantBinImg.clone();
	element = getStructuringElement(MORPH_RECT, Size(5, 6)); //������̬ѧ�����Ĵ�С

	erode(PlantBinImgTemp, PlantBinImgTemp, element);
	//	imshow("��ʴ��ͼ��", PlantBinImgTemp);
	element = getStructuringElement(MORPH_RECT, Size(10, 3));
	dilate(PlantBinImgTemp, PlantBinImgTemp, element);
	//	imshow("���ͺ�ͼ��", PlantBinImgTemp);

	Mat PlantBinImgNot = PlantBinImgTemp < 100;    //�ַ���ɫȡ��
	//imshow("�����ַ���ɫ��ת", ~PlantBinImgTemp);



	IplImage* BinImg;                //����IplImageָ��
	BinImg = &(IplImage(PlantBinImgTemp)); //��MATǿ��ת��ΪIplImageָ������
	cnt = cvFindContours(BinImg, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//Ѱ�ҳ��ƶ�ֵͼ���е�����

	Mat NewCopyImg(PlantBinImgTemp.rows, PlantBinImgTemp.cols, CV_8UC1, CvScalar(255, 255, 255));  //��ȡ�������ַ����Ƶ���ͼƬ
	IplImage* NewBinImg;
	for (tempSeq = seq; tempSeq != NULL; tempSeq = tempSeq->h_next)
	{
		area = cvContourArea(tempSeq);       //��ȡ�������
		rect = cvBoundingRect(tempSeq, 1);   //�þ��ο�ѡ���ַ�

		if (area > 800 && rect.width >= 8 && rect.width < 100 && rect.height >= 8 && rect.height < 110)
		{
			BinImg = &(IplImage(PlantBinImgNot));      //��̬ѧ�������ȡ�ַ���ͼ��
			NewBinImg = &(IplImage(NewCopyImg));        //��ȡ�ַ��������ɵ���ͼ��

			cvSetImageROI(BinImg, rect);                //��ȡ���ε��ַ�����
			cvSetImageROI(NewBinImg, rect);             //��ȡ��ͼ����ַ���������
			cvCopy(BinImg, NewBinImg);                  //������ȡ�������ַ�����ͼ��ָ������

			//imshow("��ȡ�ַ�ͼ", NewCopyImg);           //��ʾ��ȡ�ַ�֮�����ͼ��
		}
	}
	Mat CharacterCutImg = NewCopyImg.clone();
	element = getStructuringElement(MORPH_RECT, Size(4, 6)); //������̬ѧ�����Ĵ�С
	erode(CharacterCutImg, CharacterCutImg, element);        //���ַ�ͼ����ˮƽ���ʹ���
//	imshow("�ַ��ָ�׼��ͼ", CharacterCutImg);

	Mat CharacterCutNot = CharacterCutImg < 100;            //���ַ��ָ�ͼȡ��
	IplImage* CharacterImg;                //����IplImageָ��
	CharacterImg = &(IplImage(CharacterCutNot)); //��MATǿ��ת��ΪIplImageָ������
	cnt = cvFindContours(CharacterImg, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//Ѱ�ҳ��ƶ�ֵͼ���е�����
	int RectCnt = 0;   //��ȡ���ַ�����
	CvRect CharacterRect[10];
	for (tempSeq = seq; tempSeq != NULL; tempSeq = tempSeq->h_next)
	{
		area = cvContourArea(tempSeq);       //��ȡ�������
		rect = cvBoundingRect(tempSeq, 1);   //�þ��ο�ѡ���ַ�

		CvPoint PotLT, PotHW;            //��ȡ���ο�����Ͻ����꣬��������
		PotLT.x = rect.x;
		PotLT.y = rect.y;
		PotHW.x = rect.x + rect.width;
		PotHW.y = rect.y + rect.height;

		CharacterRect[RectCnt] = rect;   //��¼ÿ���ַ�����
		RectCnt++;       //�ַ�����ͳ��
		cout << "Character:" << RectCnt << endl;
		cvDrawRect(&(IplImage(CharacterCutImg)), PotLT, PotHW, CvScalar(0, 0, 255));//�����Σ������ȡ�����ַ�
	}
	if (RectCnt < 7)
	{
		cout << "�ַ���ȡʧ�ܣ�����" << endl;
		//	return -2;
	}
	//imshow("�ַ���ȡ�ɹ�ͼ", CharacterCutImg);
	QImage result2 = QImage((const unsigned char*)(CharacterCutImg.data), CharacterCutImg.cols, CharacterCutImg.rows, QImage::Format_Indexed8);
	ui.character->setPixmap(QPixmap::fromImage(result2));

	int p = 0, q = 0;     //����¼���ַ����ν�����������
	CvRect Temprect;  //Ŀ���ǽ���ȡ�����ַ�����˳����б���
	for (p = 0; p < RectCnt; p++)
	{
		for (q = 1; q < RectCnt - p; q++)
		{
			if (CharacterRect[q - 1].x > CharacterRect[q].x)
			{
				Temprect = CharacterRect[q];
				CharacterRect[q] = CharacterRect[q - 1];
				CharacterRect[q - 1] = Temprect;
			}
		}
	}
	int FirstTemp = 0;
	for (p = RectCnt - 1; p >= RectCnt - 7; p--)  //����ȡ�����ַ��������α���
	{
		if (p >= RectCnt - 6)
		{
			if (CharacterRect[p].width < 60 && p >= RectCnt - 6) //����ַ����С��60
			{
				CharacterRect[p].x = CharacterRect[p].x - (60 - CharacterRect[p].width) / 2;
				CharacterRect[p].width = 60;  //������ؿ�60
			//	cout << "�����˵�" << p + 1 << "���ַ�" << endl;//�����ַ�֮��������ָʾ˵��
			}
		}
		else if (RectCnt > 7)
		{
			FirstTemp = CharacterRect[RectCnt - 7].x - CharacterRect[0].x + CharacterRect[RectCnt - 7].width;
			CharacterRect[RectCnt - 7].x = CharacterRect[0].x;
			CharacterRect[RectCnt - 7].width = FirstTemp;
			//	cout << "��һ���ַ����" << CharacterRect[RectCnt - 7].width << endl;
		}
		char file_img[100];
		Mat roiImg = ~PlantBinImg(CharacterRect[p]);   //���α�������ַ���Ŀ���ļ���
		sprintf_s(file_img, "%d.jpg", p + 1);
		imwrite(file_img, roiImg);
		//	string PhotoName = format("%d.jpg", p+1);
		//	imshow(PhotoName, roiImg);
		if (p == RectCnt - 7) break;
	}

	Ptr<ANN_MLP> model = StatModel::load<ANN_MLP>("MLPModel.xml");  //��������ȡ�������ļ�
	Mat TestCharacter, dsttemp;
	const int imageRows = 8;
	const int imageCols = 16;
	double maxVal = 0;
	Point maxLoc;

	//cout << "ʶ��ĳ���Ϊ��" << endl;
	for (p = RectCnt - 7 + 1; p < RectCnt; p++)
	{
		string PhotoName = format("%d.jpg", p + 1); 
		TestCharacter = ~imread(PhotoName, 0);
		//	imshow("��ʶ���ַ�", TestCharacter);

			//������ͼ��ת��Ϊ1*128������
		cv::resize(TestCharacter, TestCharacter, Size(imageRows, imageCols), (0, 0), (0, 0), INTER_AREA);
		threshold(TestCharacter, TestCharacter, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

		Mat_<float> testMat(1, imageRows * imageCols);
		for (int i = 0; i < imageRows * imageCols; i++)
		{
			testMat.at<float>(0, i) = (float)TestCharacter.at<uchar>(i / 8, i % 8);
		}
		model->predict(testMat, dsttemp);
		minMaxLoc(dsttemp, NULL, &maxVal, NULL, &maxLoc);

		if (maxLoc.x <= 9)//0-9
		{
			sprintf_s(Nametemp, "%d", maxLoc.x);
		}
		else//A-Z
		{
			sprintf_s(Nametemp, "%c", maxLoc.x + 55);
		}
		//cout << Nametemp << "\t";	
		ui.character1->setText(ui.character1->text()+(QString)Nametemp);
	}
}
