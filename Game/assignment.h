#pragma once
#include <vector>
using std::vector;

class assignment
{
public:
	assignment(unsigned char *);
	unsigned char *assignment::grayScaleAlgorithmImpl();
	unsigned char *edgeAlgorithmImpl();
	unsigned char *halftoneAlgorithmImpl();
	unsigned char *floydAlgorithmImpl();
	unsigned char *vectorToData(vector<vector<unsigned char>>, int);
	unsigned char *getGrayScale();
	unsigned char *getEdge();
	unsigned char *getHalftone();
	unsigned char *getFloyd();

private:
	unsigned char *original;
	vector<vector<unsigned char>> data;
	vector<vector<unsigned char>> grayScaleData;
	vector<vector<unsigned char>> edgeData;
	vector<vector<unsigned char>> halftoneData;
	vector<vector<unsigned char>> floydData;
	char edgeAlgorithmValCalc(int, int);
	bool validateInsideEdges(int, int);
	char findClosestPaletteColor(char);
	unsigned char *grayScale;
	unsigned char *edge;
	unsigned char *halftone;
	unsigned char *floyd;
};