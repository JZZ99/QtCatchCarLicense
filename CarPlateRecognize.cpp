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
	OriginalImg = imread("car.bmp", IMREAD_COLOR);   //读原始图片
	//imshow("原图", OriginalImg);                   //显示图片

	double  BeginTime = (double)getTickCount();     //识别开始锚点

	ImageWidth = OriginalImg.rows;                 //获取图片的宽
	ImageHeight = OriginalImg.cols;                 //获取图像的长
//	cout << "width:" << ImageWidth << "\t height:" << ImageHeight << endl;  //打印图片的长和宽

	BinaryImg = OriginalImg.clone();               //克隆原始图像
	for (i = 0; i < ImageWidth; i++)               //通过颜色分量将图片进行二值化处理
	{
		for (j = 0; j < ImageHeight; j++)
		{
			pixelB = BinaryImg.at<Vec3b>(i, j)[0]; //获取图片各个通道的值
			pixelG = BinaryImg.at<Vec3b>(i, j)[1];
			pixelR = BinaryImg.at<Vec3b>(i, j)[2];

			if (abs(pixelB - B) < DifMax && abs(pixelG - G) < DifMax && abs(pixelR - R) < DifMax)
			{                                           //将各个通道的值和各个通道阈值进行比较
				BinaryImg.at<Vec3b>(i, j)[0] = 255;     //符合颜色阈值范围内的设置成白色
				BinaryImg.at<Vec3b>(i, j)[1] = 255;
				BinaryImg.at<Vec3b>(i, j)[2] = 255;
			}
			else
			{
				BinaryImg.at<Vec3b>(i, j)[0] = 0;        //不符合颜色阈值范围内的设置为黑色
				BinaryImg.at<Vec3b>(i, j)[1] = 0;
				BinaryImg.at<Vec3b>(i, j)[2] = 0;
			}
		}
	}
	//	imshow("二值化处理后图片", BinaryImg);        //显示二值化处理之后的图像

	element = getStructuringElement(MORPH_RECT, Size(3, 3)); //设置形态学处理窗的大小
	dilate(BinaryImg, BinaryImg, element);     //进行多次膨胀操作
	dilate(BinaryImg, BinaryImg, element);
	dilate(BinaryImg, BinaryImg, element);

	erode(BinaryImg, BinaryImg, element);      //进行多次腐蚀操作
	erode(BinaryImg, BinaryImg, element);
	erode(BinaryImg, BinaryImg, element);
	//	imshow("形态学处理后", BinaryImg);           //显示形态学处理之后的图像

	cvtColor(BinaryImg, BinaryImg, CV_BGR2GRAY); //将形态学处理之后的图像转化为灰度图像
	threshold(BinaryImg, BinaryImg, 100, 255, THRESH_BINARY); //灰度图像二值化




	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* seq = 0;     //创建一个序列,CvSeq本身就是一个可以增长的序列，不是固定的序列
	CvSeq* tempSeq = cvCreateSeq(CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), storage);

	IplImage* BwImg;                //定义IplImage指针
	BwImg = &(IplImage(BinaryImg)); //用MAT强制转换为IplImage指针类型
	int cnt = cvFindContours(BwImg, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//返回轮廓的数目 
	//获取二值图像中轮廓的个数
//	cout << "number of contours   " << cnt << endl;  //打印轮廓个数
	for (tempSeq = seq; tempSeq != NULL; tempSeq = tempSeq->h_next)
	{
		length = cvArcLength(tempSeq);       //获取轮廓周长
		area = cvContourArea(tempSeq);       //获取轮廓面积
		if (area > 1000 && area < 50000)     //矩形区域面积大小判断
		{
			rect = cvBoundingRect(tempSeq, 1);//计算矩形边界
			CvPoint2D32f pt[4];
			boxTemp = cvMinAreaRect2(tempSeq, 0);  //获取轮廓的矩形
			cvBoxPoints(boxTemp, pt);              //获取矩形四个顶点坐标
			angleTemp = boxTemp.angle;                 //得到车牌倾斜角度

			axisLongTemp = sqrt(pow(pt[1].x - pt[0].x, 2) + pow(pt[1].y - pt[0].y, 2));  //计算长轴（勾股定理）
			axisShortTemp = sqrt(pow(pt[2].x - pt[1].x, 2) + pow(pt[2].y - pt[1].y, 2)); //计算短轴（勾股定理）

			if (axisShortTemp > axisLongTemp)   //短轴大于长轴，交换数据
			{
				widthtemp = axisLongTemp;
				axisLongTemp = axisShortTemp;
				axisShortTemp = widthtemp;
			}
			else
				angleTemp += 90;
			//	cout << "旋转角度：" << angle << endl;
			rectArea = axisLongTemp * axisShortTemp;  //计算矩形的面积
			rectDegree = area / rectArea;     //计算矩形度（比值越接近1说明越接近矩形）

			long2Short = axisLongTemp / axisShortTemp; //计算长宽比
			if (long2Short > 2.2 && long2Short < 3.8 && rectDegree > 0.63 && rectDegree < 1.37 && rectArea > 2000 && rectArea < 50000)
			{
				Mat CutPlant = OriginalImg.clone();
				TestPlantFlag = true;             //检测车牌区域成功
				IplImage* ReImg;                  //定义提取车牌框图像
				ReImg = &(IplImage(CutPlant)); //强制类型转换
				for (int i = 0; i < 4; ++i)       //划线框出车牌区域
					cvLine(ReImg, cvPointFrom32f(pt[i]), cvPointFrom32f(pt[((i + 1) % 4) ? (i + 1) : 0]), CV_RGB(255, 0, 0));
				//cvShowImage("提取车牌结果图", ReImg);    //显示最终结果图

				box = boxTemp;
				angle = angleTemp;
				axisLong = axisLongTemp;
				axisShort = axisShortTemp;
			}
		}
	}
	if (!TestPlantFlag)
	{
		cout << "车牌区域提取失败！！！" << endl;
	}

	Mat RotractImg(ImageWidth, ImageHeight, CV_8UC1, Scalar(0, 0, 0));//倾斜矫正图片
	Point2f center = box.center;     //获取车牌中心坐标
	Mat M2 = getRotationMatrix2D(center, angle, 1);//计算旋转加缩放的变换矩阵 
	warpAffine(OriginalImg, RotractImg, M2, OriginalImg.size(), 1, 0, Scalar(0));  //进行倾斜矫正
	//imshow("倾斜矫正后图片", RotractImg);

	//	cout << "矩形中心:" << box.center.x << "," << box.center.y << endl;
	Rect recttemp(center.x - axisLong / 2, center.y - axisShort / 2, axisLong, axisShort);//定义车牌所在矩形区域
	PlantRGBImg = RotractImg(recttemp);  //截取车牌彩色照片
//	imshow("截取车牌照", PlantRGBImg);

	cv::resize(PlantRGBImg, PlantRGBImg, Size(500, 500 * axisShort / axisLong), (0, 0), (0, 0), INTER_AREA);
	//imshow("截取车牌变换尺寸后", PlantRGBImg);

	Mat PlantBinImg;   //车牌二值图像
	cvtColor(PlantRGBImg, PlantBinImg, CV_BGR2GRAY);
	threshold(PlantBinImg, PlantBinImg, 100, 255, CV_THRESH_BINARY);//二值化处理
	//imshow("车牌二值图像", PlantBinImg);

	Mat PlantBinImgTemp = PlantBinImg.clone();
	element = getStructuringElement(MORPH_RECT, Size(5, 6)); //设置形态学处理窗的大小

	erode(PlantBinImgTemp, PlantBinImgTemp, element);
	//	imshow("腐蚀后图像", PlantBinImgTemp);
	element = getStructuringElement(MORPH_RECT, Size(10, 3));
	dilate(PlantBinImgTemp, PlantBinImgTemp, element);
	//	imshow("膨胀后图像", PlantBinImgTemp);

	Mat PlantBinImgNot = PlantBinImgTemp < 100;    //字符颜色取反
	//imshow("车牌字符颜色反转", ~PlantBinImgTemp);



	IplImage* BinImg;                //定义IplImage指针
	BinImg = &(IplImage(PlantBinImgTemp)); //将MAT强制转换为IplImage指针类型
	cnt = cvFindContours(BinImg, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//寻找车牌二值图像中的轮廓

	Mat NewCopyImg(PlantBinImgTemp.rows, PlantBinImgTemp.cols, CV_8UC1, CvScalar(255, 255, 255));  //提取出来的字符复制到该图片
	IplImage* NewBinImg;
	for (tempSeq = seq; tempSeq != NULL; tempSeq = tempSeq->h_next)
	{
		area = cvContourArea(tempSeq);       //获取轮廓面积
		rect = cvBoundingRect(tempSeq, 1);   //用矩形框选出字符

		if (area > 800 && rect.width >= 8 && rect.width < 100 && rect.height >= 8 && rect.height < 110)
		{
			BinImg = &(IplImage(PlantBinImgNot));      //形态学处理后提取字符的图像
			NewBinImg = &(IplImage(NewCopyImg));        //提取字符后复制生成的新图像

			cvSetImageROI(BinImg, rect);                //提取矩形的字符区域
			cvSetImageROI(NewBinImg, rect);             //提取新图像的字符复制区域
			cvCopy(BinImg, NewBinImg);                  //复制提取出来的字符到新图像指定区域

			//imshow("提取字符图", NewCopyImg);           //显示提取字符之后的新图像
		}
	}
	Mat CharacterCutImg = NewCopyImg.clone();
	element = getStructuringElement(MORPH_RECT, Size(4, 6)); //设置形态学处理窗的大小
	erode(CharacterCutImg, CharacterCutImg, element);        //对字符图进行水平膨胀处理
//	imshow("字符分割准备图", CharacterCutImg);

	Mat CharacterCutNot = CharacterCutImg < 100;            //对字符分割图取反
	IplImage* CharacterImg;                //定义IplImage指针
	CharacterImg = &(IplImage(CharacterCutNot)); //将MAT强制转换为IplImage指针类型
	cnt = cvFindContours(CharacterImg, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);//寻找车牌二值图像中的轮廓
	int RectCnt = 0;   //提取的字符个数
	CvRect CharacterRect[10];
	for (tempSeq = seq; tempSeq != NULL; tempSeq = tempSeq->h_next)
	{
		area = cvContourArea(tempSeq);       //获取轮廓面积
		rect = cvBoundingRect(tempSeq, 1);   //用矩形框选出字符

		CvPoint PotLT, PotHW;            //提取矩形框的左上角坐标，长宽数据
		PotLT.x = rect.x;
		PotLT.y = rect.y;
		PotHW.x = rect.x + rect.width;
		PotHW.y = rect.y + rect.height;

		CharacterRect[RectCnt] = rect;   //记录每个字符矩形
		RectCnt++;       //字符个数统计
		cout << "Character:" << RectCnt << endl;
		cvDrawRect(&(IplImage(CharacterCutImg)), PotLT, PotHW, CvScalar(0, 0, 255));//画矩形，框出提取到的字符
	}
	if (RectCnt < 7)
	{
		cout << "字符提取失败！！！" << endl;
		//	return -2;
	}
	//imshow("字符提取成功图", CharacterCutImg);
	QImage result2 = QImage((const unsigned char*)(CharacterCutImg.data), CharacterCutImg.cols, CharacterCutImg.rows, QImage::Format_Indexed8);
	ui.character->setPixmap(QPixmap::fromImage(result2));

	int p = 0, q = 0;     //将记录的字符矩形进行升序排序
	CvRect Temprect;  //目的是将提取到的字符按照顺序进行保存
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
	for (p = RectCnt - 1; p >= RectCnt - 7; p--)  //将提取到的字符进行依次保存
	{
		if (p >= RectCnt - 6)
		{
			if (CharacterRect[p].width < 60 && p >= RectCnt - 6) //如果字符宽度小于60
			{
				CharacterRect[p].x = CharacterRect[p].x - (60 - CharacterRect[p].width) / 2;
				CharacterRect[p].width = 60;  //将宽度拓宽到60
			//	cout << "更改了第" << p + 1 << "个字符" << endl;//修正字符之后进行输出指示说明
			}
		}
		else if (RectCnt > 7)
		{
			FirstTemp = CharacterRect[RectCnt - 7].x - CharacterRect[0].x + CharacterRect[RectCnt - 7].width;
			CharacterRect[RectCnt - 7].x = CharacterRect[0].x;
			CharacterRect[RectCnt - 7].width = FirstTemp;
			//	cout << "第一个字符宽度" << CharacterRect[RectCnt - 7].width << endl;
		}
		char file_img[100];
		Mat roiImg = ~PlantBinImg(CharacterRect[p]);   //依次保存各个字符到目标文件夹
		sprintf_s(file_img, "%d.jpg", p + 1);
		imwrite(file_img, roiImg);
		//	string PhotoName = format("%d.jpg", p+1);
		//	imshow(PhotoName, roiImg);
		if (p == RectCnt - 7) break;
	}

	Ptr<ANN_MLP> model = StatModel::load<ANN_MLP>("MLPModel.xml");  //神经网络提取特征的文件
	Mat TestCharacter, dsttemp;
	const int imageRows = 8;
	const int imageCols = 16;
	double maxVal = 0;
	Point maxLoc;

	//cout << "识别的车牌为：" << endl;
	for (p = RectCnt - 7 + 1; p < RectCnt; p++)
	{
		string PhotoName = format("%d.jpg", p + 1); 
		TestCharacter = ~imread(PhotoName, 0);
		//	imshow("待识别字符", TestCharacter);

			//将测试图像转化为1*128的向量
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
