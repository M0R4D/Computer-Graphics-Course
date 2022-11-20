#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "assignment.h"


unsigned char* assignment::vectorToData(vector<vector<unsigned char>> average, int isHalftone) {
	unsigned char* dataArr;
	if(isHalftone) 
		dataArr = (unsigned char*)malloc(512 * 512 * 4);
	else 
		dataArr = (unsigned char*)malloc(512 * 512);
	int counter = 0;
	int alpha = 3;
	for (int i = 0; i < average.size(); i++) {
		for (int j = 0; j < average[i].size(); j++) {
			dataArr[counter] = average[i][j];
			dataArr[counter + 1] = average[i][j];
			dataArr[counter + 2] = average[i][j];
			dataArr[counter + 3] = original[alpha];
			counter = counter + 4;
			if (j % 2 == 1) alpha = alpha + 4;
		}
	}
	return dataArr;
}


vector<vector<unsigned char>> pixelsAverage(unsigned char* data) {
	vector<vector<unsigned char>> average;
	int counter = 0;
	for (int i = 0; i < 256; i++) {
		vector<unsigned char> vec;
		for (int j = 0; j < 256; j++) {
			unsigned char ch = (data[counter] + data[counter + 1] + data[counter + 2]) / 3;
			vec.push_back(ch);
			counter = counter + 4;
		}
		average.push_back(vec);
	}
	return average;
}

void export_file1(char* fileName, vector<vector<unsigned char>> data) {
	FILE* file = fopen(fileName, "w");
	for (int i = 0; i < data.size(); i++) {
		for (int j = 0; j < data[i].size(); j++) {
			if (i < 255 || j < 255) {
				if (data[i][j] == 0) {
					fprintf(file, "0,");
				}
				else {
					fprintf(file, "1,");
				}
			}
			else if (data[i][j] == 0) {
				fprintf(file, "0");
			}
			else {
				fprintf(file, "1");
			}
		}
	}
	fclose(file);
}
void export_file2(char* fileName, vector<vector<unsigned char>> data) {
	FILE* file = fopen(fileName, "w");
	for (int i = 0; i < data.size(); i++) {
		for (int j = 0; j < data[i].size(); j++) {
			fprintf(file, "%d,", data[i][j] / 16);
		}
	}
	fclose(file);
}
unsigned char* assignment::grayScaleAlgorithmImpl() {
	return vectorToData(grayScaleData, 0);
}

vector<vector<unsigned char>> hysteresis(vector<vector<unsigned char>> data) {
	vector<vector<unsigned char>>  finalTouch;
	for (int i = 0; i < data.size(); i++) {
		vector<unsigned char> vec;
		for (int j = 0; j < data[i].size(); j++) {
			if (data[i][j] == 75) {
				if (i - 1 >= 0 & j - 1 >= 0 & i + 1 < data[i].size() & j + 1 < data[i].size()) {
					if ((data[(i - 1)][j - 1] == 255) || (data[(i - 1)][j] == 255) || (data[(i - 1)][j + 1] == 255) || (data[i][j - 1] == 255) ||
						(data[i][j + 1] == 255) || (data[(i + 1)][j - 1] == 255) || (data[(i + 1)][j] == 255) || (data[(i + 1)][j + 1] == 255)) {
						vec.push_back(255);
					}
					else {
						vec.push_back(0);
					}
				}
				else vec.push_back(255);
			}
			else {
				vec.push_back(data[i][j]);
			}
		}
		finalTouch.push_back(vec);
	}
	export_file1("../img4.txt", finalTouch);
	return finalTouch;
}

unsigned char getAngle(unsigned char byte) {
	unsigned char val = byte * (unsigned char)(180 / 3.14);
	if (val < 0)
		return val + 180;
	return val;
}

vector<vector<unsigned char>> non_max_suppression(vector<vector<unsigned char>>  data) {
	vector<vector<unsigned char>> nonmax;

	for (int i = 0; i < 256; i++) {
		vector<unsigned char> vec;
		for (int j = 0; j < 256; j++) {
			unsigned char val2 = 0;
			if (i - 1 >= 0 & j - 1 >= 0 & i + 1 < data[i].size() & j + 1 < data[i].size()) {
				unsigned char val = getAngle(data[i][j]);
				int q = 255;
				int r = 255;
				if ((0 <= val < 22.5) | (157.5 <= val <= 180))
				{
					q = data[i][j + 1]; r = data[i][j - 1];
				}
				else if (22.5 <= val < 67.5) {
					q = data[i + 1][j - 1]; r = data[i - 1][j + 1];
				}
				else if (67.5 <= val < 112.5) {
					q = data[i + 1][j]; r = data[i - 1][j];
				}
				else if (112.5 <= val < 157.5) {
					q = data[i - 1][j - 1]; r = data[i + 1][j + 1];
				}
				if ((data[i][j] >= q) & (data[i][j] >= r)) {
					val2 = data[i][j];
				}
				//threshold
				if (val2 >= 180)
					val2 = 255;
				else if (val2 <= 25)
					val2 = 0;
				else
					val2 = 75;
			}
			vec.push_back(val2);
		}
		nonmax.push_back(vec);
	}
	return hysteresis(nonmax);
}

unsigned char* assignment::edgeAlgorithmImpl() {
	vector<vector<unsigned char>> edge;
	int counter = 0;
	for (int i = 0; i < 256; i++) {
		vector<unsigned char> vec;
		unsigned char val = 0;
		for (int j = 0; j < 256; j++, counter++) {
			val = edgeAlgorithmValCalc(i, j);
			vec.push_back(val);
		}
		edge.push_back(vec);
	}
	return vectorToData(non_max_suppression(edge), 0);
}

unsigned char assignment::edgeAlgorithmValCalc(int i, int j) {
	unsigned char val = 0;
	if (validateInsideEdges(i, j))
	{
		char gx = 	data[i - 1][j - 1] + 2 * data[i - 1][j] + 
					data[i - 1][j + 1] - data[i + 1][j - 1] - 
					2 * data[i + 1][j] - data[i + 1][j + 1];
		char gy = 	-data[i + 1][j - 1] - 2 * data[i][j - 1] -
					data[i - 1][j - 1] + data[i + 1][j + 1] + 
					2 * data[i][j + 1] + data[i - 1][j + 1];
		val = std::abs(gx) + std::abs(gy);
	}
	return val;
}

bool assignment::validateInsideEdges(int i, int j) {
	return (i - 1 >= 0 & j - 1 >= 0 & i + 1 < 256 & j + 1 < 256);
}

unsigned char* assignment::halftoneAlgorithmImpl() {
	for (int i = 0; i < 256; i++) {
		vector<unsigned char> vec;
		vector<unsigned char> vec1;
		for (int j = 0; j < 256; j++) {
			int steps = data[i][j] / 51;
			vec.push_back(0);
			vec.push_back(0);
			vec1.push_back(0);
			vec1.push_back(0);
			int counter = 0;
			while (steps != counter) {
				if (counter == 0)
					vec1[vec1.size() - 2] = 255;
				if (counter == 1)
					vec[vec1.size() - 1] = 255;
				if (counter == 2)
					vec1[vec1.size() - 1] = 255;
				if (counter == 3)
					vec[vec1.size() - 2] = 255;
				counter++;
			}
		}
		halftoneData.push_back(vec);
		halftoneData.push_back(vec1);
	}
	export_file1("../img5.txt", halftoneData);
	return vectorToData(halftoneData, 1);
}

unsigned char* assignment::floydAlgorithmImpl() {
	for (int x = 1; x < 255; x++) {
		for (int y = 1; y < 255; y++) {
			char oldPixel = data[x][y];
			char newPixel = findClosestPaletteColor(oldPixel);
			floydData[x][y] = newPixel;
			char quantError = oldPixel - newPixel;
			floydData[x + 1][y] = data[x + 1][y] + quantError * 7 / 16;
			floydData[x - 1][y + 1] = data[x - 1][y + 1] + quantError * 1 / 16;
			floydData[x][y + 1] = data[x][y + 1] + quantError * 5 / 16;
			floydData[x + 1][y + 1] = data[x + 1][y + 1] + quantError * 3 / 16;
		}
	}
	export_file2("../img6.txt", floydData);
	return vectorToData(floydData, 0);
}

char assignment::findClosestPaletteColor(char pixel) {
	// converting greyscale pixel values from a high to a low bit depth (8-bit greyscale to 1-bit black-and-white) 
	// may perform just a simple rounding 
	return pixel / 255;
}

assignment::assignment(unsigned char* data) :
	original(data), data(pixelsAverage(data)), grayScaleData(pixelsAverage(data)),
	edgeData(pixelsAverage(data)), halftoneData(), floydData(pixelsAverage(data))
	, grayScale(grayScaleAlgorithmImpl()), edge(edgeAlgorithmImpl()), halftone(halftoneAlgorithmImpl()), floyd(floydAlgorithmImpl()) {}

unsigned char* assignment::getGrayScale() {
	return this->grayScale;
}

unsigned char* assignment::getEdge() {
	return this->edge;
}

unsigned char* assignment::getHalftone() {
	return this->halftone;
}

unsigned char* assignment::getFloyd() {
	return this->floyd;
}