#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace zbar;

void get_qr_value(const Mat& src_) {
	Mat grey;
	cvtColor(src_, grey, CV_BGR2GRAY);

	int width = src_.cols;
	int height = src_.rows;

	uchar *raw = (uchar *)grey.data;

	Image image(width, height, "Y800", raw, width * height);

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	int n = scanner.scan(image);

	ofstream ofs("./data.txt");

	for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
		ofs << symbol->get_data();
	}
}


void get_qr_position(const Mat& src_, int *qrx, int *qry) {
	Mat grey;
	cvtColor(src_, grey, CV_BGR2GRAY);

	int width = src_.cols;
	int height = src_.rows;
	uchar *raw = (uchar *)grey.data;
	
	Image image(width, height, "Y800", raw, width * height);
	
	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	int n = scanner.scan(image);
	
	for (Image::SymbolIterator symbol = image.symbol_begin();symbol != image.symbol_end(); ++symbol) {
	    vector<Point> vp;
		int n = symbol->get_location_size();
		for (int i = 0; i<n; i++) {
			vp.push_back(Point(symbol->get_location_x(i), symbol->get_location_y(i)));
			*qrx += symbol->get_location_x(i) / n;
			*qry += symbol->get_location_y(i) / n;
		}
	}
}


void camear() {
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		printf("ÉJÉÅÉâì«Ç›çûÇ›é∏îs\n");
	}

	cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 600);

	while (1) {
		Mat frame;

		cap >> frame;

		int Key = waitKey(1);

		if (Key == 113) {
			break;
		}
		else if (Key == 115) {
			imwrite("img.jpg", frame);
		}

		int qr_x = 0, qr_y = 0;
		get_qr_position(frame, &qr_x, &qr_y);
		cv::circle(frame, Point(qr_x, qr_y), 100, cv::Scalar(0, 255, 0), 10, 1);

		imshow("window", frame);
	}

	destroyAllWindows();
}


int main(int argc) {
	FILE *fp;
	int length;//èc
	int side;//â°
	int agent[4];
	int val[12][12];
	char A;

	camear();

	Mat src = cv::imread("./img.jpg", 1);
	get_qr_value(src);

	if ((fp = fopen("./data.txt", "r")) == NULL) {
		return 1;
	}

	fscanf(fp, "%d %d", &length, &side);

	for (int i = 0; i < length; i++) {
		fscanf(fp, "%c", &A);
		for (int j = 0; j < side; j++) {
			fscanf(fp, "%d", &val[length][side]);
			printf(" %d", val[length][side]);
		}
	}

	fscanf(fp, "%c", &A);
	fscanf(fp, "%d %d", &agent[0], &agent[1]);
	printf(" %d %d", agent[0], agent[1]);
	fscanf(fp, "%c", &A);
	fscanf(fp, "%d %d", &agent[2], &agent[3]);
	printf(" %d %d", agent[2], agent[3]);
	fclose(fp);


	waitKey(0);
	return 0;
}


/*
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace cv;
using namespace std;
using namespace zbar;

void get_qr_value(const Mat& src_) {
	Mat grey;
	cvtColor(src_, grey, CV_BGR2GRAY);

	int width = src_.cols;
	int height = src_.rows;

	uchar *raw = (uchar *)grey.data;

	Image image(width, height, "Y800", raw, width * height);

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	int n = scanner.scan(image);

	ofstream ofs("./data.txt");

	for (Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol) {
		ofs << symbol->get_data();
	}
}

void camear() {
	VideoCapture cap(0);

	if (!cap.isOpened()) {
		printf("ÉJÉÅÉâì«Ç›çûÇ›é∏îs\n");
	}

	cap.set(CV_CAP_PROP_FRAME_WIDTH, 800);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 800);

	while (1) {
		Mat frame;

		cap >> frame; 

		imshow("window", frame);

		int Key = waitKey(1);

		if (Key == 113) {
			break;
		}
		else if(Key == 115) {
			imwrite("img.jpg", frame);
		}
	}

	destroyAllWindows();
}


int main(int argc) {
	FILE *fp;
	int length;//èc
	int side;//â°
	int agent[4];
	int val[12][12];
	char A;

	camear();

	Mat src = cv::imread("./img.jpg", 1);
	get_qr_value(src);

	if ((fp = fopen("./data.txt", "r")) == NULL) {
		return 1;
	}

	fscanf(fp, "%d %d", &length, &side);

	for (int i = 0; i < length; i++) {
		fscanf(fp, "%c", &A);
		for (int j = 0; j < side; j++) {
			fscanf(fp, "%d", &val[length][side]);
			printf(" %d", val[length][side]);
		}
	}

	fscanf(fp, "%c", &A);
	fscanf(fp, "%d %d", &agent[0], &agent[1]);
	printf(" %d %d", agent[0], agent[1]);
	fscanf(fp, "%c", &A);
	fscanf(fp, "%d %d", &agent[2], &agent[3]);
	printf(" %d %d", agent[2], agent[3]);
	fclose(fp);


	waitKey(0);
	return 0;
}
*/