#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

struct Colour {
	int Lab_L = 0;
	int Lab_a = 0;
	int Lab_b = 0;
	int RGB_R = 0;
	int RGB_G = 0;
	int RGB_B = 0;
};

void showOriginalImage(Mat image) {
	namedWindow("Original Image", WINDOW_AUTOSIZE);
	imshow("Original Image", image);
}

Mat generateRoi(Mat image) {
	Rect rec(0, 0, image.cols / 2, image.rows / 2); //top left region with 305 width and 280 height
	Mat roi = image(rec);

	//generate roi
	namedWindow("Region of Interest - Top Left", WINDOW_AUTOSIZE);
	imshow("Region of Interest - Top Left", roi);

	//save image in jpg format
	imwrite("Top Left ROI.jpg", roi);

	return roi;
}

void generateColourHistograms(Mat rImage, Mat oImage) {
	int bins = 256;             // number of bins
	int nc = rImage.channels();    // number of channels

	vector<Mat> hist(nc);       // histogram arrays
	cout << "\nThe histogram size is " << hist.size() << endl;
	// Initalize histogram arrays
	for (int i = 0; i < nc; i++)
		hist[i] = Mat::zeros(1, bins, CV_32SC1);

	// Calculate the histogram of the image
	for (int i = 0; i < rImage.rows; i++) {
		for (int j = 0; j < rImage.cols; j++) {
			for (int k = 0; k < nc; k++) {
				uchar val = nc == 1 ? rImage.at<uchar>(i, j) : rImage.at<Vec3b>(i, j)[k];
				hist[k].at<int>(val) += 1;
			}
		}
	}

	// For each histogram arrays, obtain the maximum (peak) value
	// Needed to normalize the display later
	int hmax[3] = { 0, 0, 0 };
	for (int i = 0; i < nc; i++) {
		for (int j = 0; j < bins; j++)
			hmax[i] = hist[i].at<int>(j) > hmax[i] ? hist[i].at<int>(j) : hmax[i];
	}

	const char* wname[3] = { "blue", "green", "red" };
	Scalar colors[3] = { Scalar(255, 0, 0), Scalar(0, 255, 0), Scalar(0, 0, 255) };

	vector<Mat> canvas(nc);

	// Display each histogram in a canvas
	for (int i = 0; i < nc; i++) {
		canvas[i] = Mat(125, bins, CV_8UC3, Scalar(240,240,240)); //125 = row number

		for (int j = 0, rows = canvas[i].rows; j < bins; j++) { //canvas[i].rows = 125
			line(
				canvas[i],
				Point(j, rows),
				Point(j, rows - (hist[i].at<int>(j) * rows / hmax[i])),
				nc == 1 ? Scalar(200, 200, 200) : colors[i],
				1, 8, 0
				);
		}

		imshow(nc == 1 ? "value" : wname[i], canvas[i]); //caption of canvas = "value" or wname[i]
	}

	//plot the roi and histogram images into one combined image
	Mat combined(oImage.rows + 100, oImage.cols + 100, CV_8UC3, Scalar(255,255,255)); //create a white background image
	
	//initialize the position of the 4 images
	Rect positionRoi(20, 20, rImage.cols, rImage.rows);
	Rect positionCanvas0(340, 20, canvas[0].cols, canvas[0].rows);
	Rect positionCanvas1(20, 400, canvas[1].cols, canvas[1].rows);
	Rect positionCanvas2(340, 400, canvas[2].cols, canvas[2].rows);

	rImage.copyTo(combined(positionRoi));
	canvas[0].copyTo(combined(positionCanvas0));
	canvas[1].copyTo(combined(positionCanvas1));
	canvas[2].copyTo(combined(positionCanvas2));

	//draw a boundary line
	Rect sizeOfOriImage(0, 0, combined.cols, combined.rows);
	rectangle(combined, sizeOfOriImage, Scalar(0, 0, 255), 3, 8, 0);

	namedWindow("Combined Image", WINDOW_AUTOSIZE);
	imshow("Combined Image", combined);
	imwrite("Combined Image.jpg", combined);

	////initialize B G R arrays
	//vector<Mat> BgrPlane(3);
	//split(rImage, BgrPlane);

	//int bins = 256;
	//float range[] = { 0, 255 };
	//const float *rangeOfColourPixel = { range };
	//bool uniform = true;
	//bool accumulate = false;
	//Mat hist_b, hist_g, hist_r;

	////compute colour histograms
	//calcHist(&BgrPlane[0], 1, 0, Mat(), hist_b, 1, &bins, &rangeOfColourPixel, uniform, accumulate);
	//calcHist(&BgrPlane[1], 1, 0, Mat(), hist_g, 1, &bins, &rangeOfColourPixel, uniform, accumulate);
	//calcHist(&BgrPlane[2], 1, 0, Mat(), hist_r, 1, &bins, &rangeOfColourPixel, uniform, accumulate);

	////draw histograms
	//int hist_w = 512;
	//int hist_h = 350;
	//int bin_w = cvRound((double)hist_w / bins);

	//Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

	////normalize the histograms to fit in histImage
	//normalize(hist_b, hist_b, 0, histImage.rows, NORM_MINMAX, -1, Mat()); //input array, output array, lower limit, upper limit,
	//normalize(hist_g, hist_g, 0, histImage.rows, NORM_MINMAX, -1, Mat()); //adjust the values between the two limits, 
	//normalize(hist_r, hist_r, 0, histImage.rows, NORM_MINMAX, -1, Mat()); //output array will same as input, optional mask

	////draw each channel
	//for (int i = 1; i < bins; i++)
	//{
	//	line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist_b.at<float>(i - 1))),
	//		Point(bin_w*(i), hist_h - cvRound(hist_b.at<float>(i))),
	//		Scalar(255, 0, 0), 1, 8, 0);
	//	line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist_g.at<float>(i - 1))),
	//		Point(bin_w*(i), hist_h - cvRound(hist_g.at<float>(i))),
	//		Scalar(0, 255, 0), 1, 8, 0);
	//	line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(hist_r.at<float>(i - 1))),
	//		Point(bin_w*(i), hist_h - cvRound(hist_r.at<float>(i))),
	//		Scalar(0, 0, 255), 1, 8, 0);
	//}

	//namedWindow("Colour Histograms", WINDOW_AUTOSIZE);
	//imshow("Colour Histograms", histImage);
}

void findDominantColours(Mat image) {
	int quanStep = 32;  
	int quanBins = 256 / quanStep; //number of quantized bins            
	int nc = image.channels(); //number of channels
	vector<int> dominantColorIndex; //0-7
	vector<int> dominantColorSequenceNo; //0-7
	vector<Colour> dominantColor; //0-7
	Colour tempColour;

	//initialize histogram array
	Mat quanLabImage(10, 10, CV_8UC3, Scalar(0, 0, 0));
	Mat quanBgrImage(10, 10, CV_8UC3, Scalar(0, 0, 0));

	//quantized the colour pixel from 0-255 to 0-7
	for (int i = 0; i < pow(quanBins, 3); i++) {
		dominantColorIndex.push_back(0);
		dominantColorSequenceNo.push_back(i);
	}

	int b = quanBins / 2 - 1;
	
	while (b < 256) {
		int a = quanBins / 2 - 1;

		while (a < 256) {
			int L = quanBins / 2 - 1;

			while (L < 256) {
				tempColour.Lab_L = L;
				tempColour.Lab_a = a;
				tempColour.Lab_b = b;

				for (int row = 0; row < 10; row++) {
					for (int col = 0; col < 10; col++) {
						quanLabImage.at<Vec3b>(row, col)[0] = L;
						quanLabImage.at<Vec3b>(row, col)[1] = a;
						quanLabImage.at<Vec3b>(row, col)[2] = b;
					}
				}

				cvtColor(quanLabImage, quanBgrImage, CV_Lab2BGR);
				tempColour.RGB_B = quanBgrImage.at<Vec3b>(5, 5)[2];
				tempColour.RGB_G = quanBgrImage.at<Vec3b>(5, 5)[1];
				tempColour.RGB_R = quanBgrImage.at<Vec3b>(5, 5)[0];
				L += quanStep;
				dominantColor.push_back(tempColour);
			}
			a += quanStep;
		}
		b += quanStep;
	}

	//display quantized image 
	Mat quantizedImage(image.rows, image.cols, CV_8UC3, Scalar(0, 0, 0));

	//compute histogram of quantized image
	for (int i = 0; i < image.rows; i++) {
		for (int j = 0; j < image.cols; j++) {
			//normalize
			int hQuantizedMax[3] = { 0, 0, 0 };

			for (int k = 0; k < nc; k++) {
				uchar val = nc == 1 ? image.at<uchar>(i, j) : image.at<Vec3b>(i, j)[k];
				hQuantizedMax[k] = val / quanStep;
			}

			int colorBinIndex = (pow(quanBins, 2)*hQuantizedMax[2]) + (quanBins*hQuantizedMax[1]) + hQuantizedMax[0];

			dominantColorIndex.at(colorBinIndex)++;

			quantizedImage.at<Vec3b>(i, j)[0] = dominantColor.at(colorBinIndex).RGB_R;
			quantizedImage.at<Vec3b>(i, j)[1] = dominantColor.at(colorBinIndex).RGB_G;
			quantizedImage.at<Vec3b>(i, j)[2] = dominantColor.at(colorBinIndex).RGB_B;
		}
	}

	namedWindow("Quantized Image", WINDOW_AUTOSIZE);
	imshow("Quantized Image", quantizedImage);
	imwrite("Quantized Image.jpg", quantizedImage);

	//perform bubble sorting on dominant colour index
	int flag = 1;
	int temp;
	int numLength = pow(quanBins, 3);
	
	for (int i = 1; (i <= numLength) && flag; i++) {
		flag = 0;

		for (int j = 0; j < (numLength - 1); j++) {
			if (dominantColorIndex.at(j + 1) > dominantColorIndex.at(j)) {
				temp = dominantColorIndex.at(j);
				dominantColorIndex.at(j) = dominantColorIndex.at(j + 1);
				dominantColorIndex.at(j + 1) = temp;

				temp = dominantColorSequenceNo.at(j);
				dominantColorSequenceNo.at(j) = dominantColorSequenceNo.at(j + 1);
				dominantColorSequenceNo.at(j + 1) = temp;

				flag = 1;
			}
		}
	}

	//compute the mean value of the dominant colours
	int overallMax = 0;
	int countValue = 0;
	
	for (int i = 0; i < (pow(quanBins, 3)); i++) {
		if (dominantColorIndex.at(i) > 0) {
			countValue++;
			overallMax = overallMax < dominantColorIndex.at(i) ? dominantColorIndex.at(i) : overallMax;
		}
	}

	//display the sorted one D colour index histogram
	int thickness = 8, offset = 12;
	Mat quanDominantColourHist(150, countValue * max(thickness, offset), CV_8UC3, Scalar(255, 255, 255));

	for (int j = 0, rows = quanDominantColourHist.rows; j < countValue; j++)
	{
		line(quanDominantColourHist, Point((j * offset), rows),
			Point((j*offset), rows - (dominantColorIndex.at(j) * rows / overallMax)),
			Scalar(dominantColor.at(dominantColorSequenceNo.at(j)).RGB_R, 
			dominantColor.at(dominantColorSequenceNo.at(j)).RGB_G,
			dominantColor.at(dominantColorSequenceNo.at(j)).RGB_B),
			thickness, 8, 0
			);
	}

	namedWindow("Sorted Quantized Dominant Colour Histogram", WINDOW_AUTOSIZE);
	imshow("Sorted Quantized Dominant Colour Histogram", quanDominantColourHist);
	imwrite("Sorted Quantized Dominant Colour Histogram.jpg", quanDominantColourHist);
}

int main(void) {
	char option;
	bool terminate = false;
	Mat image = imread("lena.jpg", IMREAD_COLOR);
	Mat roi;

	if (image.empty()) {
		cout << "No image found.\n";
		system("pause");
		return -1;
	}

	while (!terminate) {
		cout << "******************* Assignment 3 *******************\n\n";
		cout << "(1)\tDisplay Original Image\n";
		cout << "(2)\tGenerate Region of Interest\n";
		cout << "(3)\tGenerate RGB Colour Histograms\n";
		cout << "(4)\tDisplay Pixel Percentage of Dominant Colours\n";
		cout << "(5)\tExit\n\n";
		cout << "Please enter your option: ";
		cin >> option;

		switch (option) {
			case '1':
				showOriginalImage(image);
				waitKey();
				break;
			case '2':
				roi = generateRoi(image);
				waitKey();
				break;
			case '3':
				roi = generateRoi(image);
				generateColourHistograms(roi, image);
				waitKey();
				break;
			case '4':
				roi = generateRoi(image);
				findDominantColours(roi);
				waitKey();
				break;
			case '5':
				return 0;
			default:
				cout << "The range of number you can enter is 1 to 5.\n";
		}

		destroyAllWindows();
		system("cls");
	}
}