// ConsoleApplication3.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/features2d.hpp"

#include  <fstream>
#include "ml.hpp"
#include "objdetect.hpp"

using namespace std;
using namespace cv;
using namespace cv::ml;

bool sortLefttoRight(const Rect &lhs, const Rect &rhs) {
	return lhs.x < rhs.x;
}

int main(int argc, char** argv)
{
	/*
	//clear data in "maxVal.txt" in the begining
	ofstream clearfile;
	clearfile.open("C:/Users/Diane/source/repos/ConsoleApplication3/maxVal.txt", std::ofstream::out | std::ofstream::trunc);
	clearfile.close();

	ofstream clearfile2;
	clearfile2.open("C:/Users/Diane/source/repos/ConsoleApplication3/SVM2.txt", std::ofstream::out | std::ofstream::trunc);
	clearfile2.close();

	vector<Mat> allimage;

	////////////save all images (350) inside "allimage"
	//loop all 35 classes
	for (int i = 0; i <= 35; i++) {

		//read all .jpg from "data" folder
		char filenames2[128];

		sprintf_s(filenames2, "C:/Users/Diane/source/repos/ConsoleApplication3/Training/%d/*.jpg", i);

		String folderpath = filenames2;
		vector<String> filenames;
		glob(folderpath, filenames);

		//loop for each image
		for (int j = 0; j < filenames.size(); j++)
		{

			//read image
			Mat immat = imread(filenames[j]);
			resize(immat, immat, Size(64, 128));
			Mat gray;
			cvtColor(immat, gray, CV_BGR2GRAY);
			allimage.push_back(gray);

		}
	}

	HOGDescriptor *hogDesc = new HOGDescriptor();

	std::vector<float> desc; //
	hogDesc->compute(allimage.at(0), desc);

	int featureSize = desc.size(); //
	int numberOfSamples = 350;

	cv::Mat samples(numberOfSamples, featureSize, CV_32FC1); //

	for (int j = 0; j < 350; j++) {
		hogDesc->compute(allimage.at(j), desc);

		for (int i = 0; i < featureSize; i++) //
			samples.ptr<float>(j)[i] = desc[i]; //

	}

	cv::Mat labels(numberOfSamples, 1, CV_32SC1); //

	for (int i = 0; i < 35; i++) {
		labels.rowRange((i * 10), ((i + 1) * 10)) = i;
	}

	double minVal, maxVal;
	Point minLoc, maxLoc;
	int mul = 1000;

	vector<double> max;

	// normalize values to be in the range from zero to 1000 by dividing by maximum value
	for (int j = 0; j < featureSize; j++) {
		minMaxLoc(samples.col(j), &minVal, &maxVal, &minLoc, &maxLoc);

		if (maxVal != 0)
			samples.col(j) = samples.col(j) / maxVal * mul;

		max.push_back(maxVal);

		//save maxVal for normalization
		ofstream myfile;
		myfile.open("C:/Users/Diane/source/repos/ConsoleApplication3/maxVal.txt", std::ofstream::app);
		myfile << maxVal << "\n";
		myfile.close();
	}

	cv::Ptr<cv::ml::SVM> svm = cv::ml::SVM::create(); //
	svm->setType(cv::ml::SVM::C_SVC); //
	svm->setKernel(cv::ml::SVM::INTER); //

	cv::Ptr<cv::ml::TrainData> trainingData = //
		cv::ml::TrainData::create(samples, cv::ml::SampleTypes::ROW_SAMPLE, labels); //

	svm->train(trainingData); //

	svm->save("C:/Users/Diane/source/repos/ConsoleApplication3/SVM2.txt");
	*/

	//waitKey(50000);

	cv::namedWindow("Detector");

	for (int fileNum = 1; fileNum <= 25; fileNum++) {
		std::string file;
		file = std::to_string(fileNum);

		cv::Mat image = cv::imread("C:/Users/Diane/source/repos/ConsoleApplication3/Testing/license-plate" + file + ".jpg");

		//float newHeight = (1000.0 / (float) image.size().width) * (float) image.size().height;
		//resize(image, image, Size(1000, (int)newHeight));

		if ((!image.data))                             // Check for invalid input
		{
			cout << "Could not open or find the image" << endl;
			return -1;
		}
		Mat grayImage;
		cvtColor(image, grayImage, CV_BGR2GRAY);

		Mat grayImage2;
		cvtColor(image, grayImage2, CV_BGR2GRAY);

		Ptr<MSER> ptrMSER = MSER::create();
		vector<vector<Point> > points;
		vector<Rect> rects;
		ptrMSER->detectRegions(grayImage, points, rects);

		Mat output(image.size(), CV_8UC3);
		output = Scalar(255, 255, 255);

		RNG rng;

		for (vector<vector<Point> >::reverse_iterator it = points.rbegin();
			it != points.rend(); ++it) {
			Vec3b c(rng.uniform(0, 254),
				rng.uniform(0, 254), rng.uniform(0, 254));

			for (vector<Point>::iterator itPts = it->begin();
				itPts != it->end(); ++itPts) {
				if (output.at<Vec3b>(*itPts)[0] == 255) {
					output.at<Vec3b>(*itPts) = c;
				}
			}
		}

		vector<Rect> r;

		vector<Rect>::iterator itr = rects.begin();
		vector<vector<Point> >::iterator itp = points.begin();
		for (; itr != rects.end(); ++itr, ++itp) {

			//rectangle(grayImage, *itr, Scalar(255), 2);
			//rectangle(grayImage, *itr, CV_RGB(0, 255, 0));

			if (itr->height <= itr->width * 1.75 || itr->height > itr->width * 3.5) {
				continue;
			}

			Rect max = *itr;

			for (vector<Rect>::iterator itr2 = rects.begin(); itr2 != rects.end(); itr2++) {
				if (
					itr2->height > itr2->width * 1.75
					&& abs(itr2->x - max.x) < 5
					&& abs(itr2->y - max.y) < 5
					) {

					if (itr2->area() > max.area()) {
						max = *itr2;
					}
				}
			}

			if (std::find(r.begin(), r.end(), max) == r.end()) {
				r.push_back(max);
			}
		}

		vector<Rect> licenceCharacters;
		int count = 0;
		int num = 0;

		for (vector<Rect>::iterator i = r.begin(); i != r.end(); ++i) {
			for (vector<Rect>::iterator i2 = r.begin(); i2 != r.end(); ++i2) {

				int largerWidth;
				int largerHeight;

				if (i2->width > i->width) {
					largerWidth = i2->width;
				}
				else {
					largerWidth = i->width;
				}

				if (i2->height > i->height) {
					largerHeight = i2->height;
				}
				else {
					largerHeight = i->height;
				}

				if (
					abs(i2->width - i->width) < largerWidth * 0.2
					&& abs(i2->height - i->height) < largerHeight * 0.2
					&& abs(i2->y - i->y) < largerHeight / 3
					&& abs(i2->x - i->x) < largerWidth * 3
					&& abs(i2->x - i->x) > largerWidth * 0.8
					) {
					//std::cout << abs(i2->x - i->x) << " " << largerWidth << "   ";
					count++;
				}
			}

			//std::cout << "\n";
			if (count > 0) {

				licenceCharacters.push_back(*i);

				//rectangle(grayImage, *i, CV_RGB(0, 255, 0));

				num++;
				//std::cout << num << " " << i->x << " " << i->y << " " << i->width << " " << i->height << " " << i->area() << "\n";


				count = 0;
			}
		}

		//cv::resizeWindow("Image", Size(1000, 750));
		//cv::imshow("Image", grayImage2);
		//cv::waitKey(50);

		if (!licenceCharacters.empty()) {

			//cv::imshow("rectangles", grayImage);
			//cv::waitKey(50);

			sort(licenceCharacters.begin(), licenceCharacters.end(), sortLefttoRight);

			Mat crop;
			
			int height = grayImage.size().height;

			Mat additionalHeight;
			if (height <= licenceCharacters.size() * 60) {
				int addHeight = licenceCharacters.size() * 60 + 1 - height;

				resize(grayImage, additionalHeight, Size(grayImage.size().width, addHeight));
				additionalHeight = Scalar(0);

				vconcat(grayImage2, additionalHeight, grayImage2);

				height += addHeight;
			}

			Mat side;
			resize(grayImage, side, Size(30, height));
			side = Scalar(0);

			Mat result;
			hconcat(side, grayImage2, result);
			//cv::resizeWindow("Detector", Size(1000, 750));
			cv::imshow("Detector", result);
			cv::waitKey(50);

			cout << "Licence Plate = ";

			for (vector<Rect>::iterator i = licenceCharacters.begin(); i != licenceCharacters.end(); ++i) {

				Mat resized;
				resize(grayImage(*i), resized, Size(30, 60));

				rectangle(grayImage, *i, CV_RGB(0, 255, 0));
				rectangle(grayImage2, *i, CV_RGB(0, 255, 0));
				//cv::resizeWindow("Image", Size(1000, 750));
				//cv::imshow("Image", grayImage2);

				if (i == licenceCharacters.begin()) {
					crop = resized;
				}
				else {
					vconcat(crop, resized, crop);
				}

				//cv::imwrite("C:/Users/Diane/source/repos/ConsoleApplica tion3/" + file + std::to_string(test) + ".jpg", grayImage(resized));

				// ?
				Mat gray;
				resize(resized, gray, Size(64, 128));

				vector<float> desc;
				HOGDescriptor *hogDesc = new HOGDescriptor();
				hogDesc->compute(gray, desc);

				int featureSize = desc.size();
				Mat input(1, featureSize, CV_32FC1);
				vector<double> maxVal(featureSize);
				ifstream alldata;

				alldata.open("C:/Users/Diane/source/repos/ConsoleApplication3/maxVal.txt", std::ofstream::in);

				for (int j = 0; j < featureSize; j++) {
					alldata >> maxVal[j];
				}

				for (int i = 0; i < featureSize; i++) {
					input.col(i) = desc[i] / maxVal.at(i) * 1000;
				}
				// ?

				char output;

				Ptr<SVM> svmNew = SVM::create();
				svmNew = SVM::load("C:/Users/Diane/source/repos/ConsoleApplication3/svm2.txt");

				output = svmNew->predict(input);

				switch (output)
				{
				case 0:
					output = '0';
					break;
				case 1:
					output = '1';
					break;
				case 2:
					output = '2';
					break;
				case 3:
					output = '3';
					break;
				case 4:
					output = '4';
					break;
				case 5:
					output = '5';
					break;
				case 6:
					output = '6';
					break;
				case 7:
					output = '7';
					break;
				case 8:
					output = '8';
					break;
				case 9:
					output = '9';
					break;
				case 10:
					output = 'A';
					break;
				case 11:
					output = 'B';
					break;
				case 12:
					output = 'C';
					break;
				case 13:
					output = 'D';
					break;
				case 14:
					output = 'E';
					break;
				case 15:
					output = 'F';
					break;
				case 16:
					output = 'G';
					break;
				case 17:
					output = 'H';
					break;
				case 18:
					output = 'I';
					break;
				case 19:
					output = 'J';
					break;
				case 20:
					output = 'K';
					break;
				case 21:
					output = 'L';
					break;
				case 22:
					output = 'M';
					break;
				case 23:
					output = 'N';
					break;
				case 24:
					output = 'O';
					break;
				case 25:
					output = 'P';
					break;
				case 26:
					output = 'Q';
					break;
				case 27:
					output = 'R';
					break;
				case 28:
					output = 'S';
					break;
				case 29:
					output = 'T';
					break;
				case 30:
					output = 'U';
					break;
				case 31:
					output = 'V';
					break;
				case 32:
					output = 'W';
					break;
				case 33:
					output = 'X';
					break;
				case 34:
					output = 'Y';
					break;
				case 35:
					output = 'Z';
					break;
				default:
					output = '?';
					break;
				}

				height -= 60;

				resize(grayImage, side, Size(30, height));
				side = Scalar(0);

				vconcat(crop, side, side);
				hconcat(side, grayImage2, result);
				//cv::resizeWindow("Detector", Size(1000, 750));
				cv::imshow("Detector", result);
				cv::waitKey(50);

				cout << output;

				//cv::imshow("Cropped", crop);
				//cv::waitKey(10);
			}

			//cv::imwrite("C:/Users/Diane/source/repos/ConsoleApplication3/" + file + "test2" + ".jpg", grayImage);

			cout << "\n";

			//cv::imshow("Cropped", crop);
			//cv::waitKey(10);
		}

		//cv::imshow("rectangles", grayImage);
		//cv::waitKey(50);

	}

	cv::waitKey(50);
	return 0;
}