// pngtest.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include "PngProc.h"
#include <string.h>
#include <stdio.h>
#include "lab1.h"

void ImageProcessingGray(unsigned char* pOut
			 , unsigned char* pIn
			 , size_t nWidth
			 , size_t nHeight);


int main(int argc, char* argv[])
{
	SetConsoleOutputCP(65001);

	if (argc < 2)
	{
		printf("format: pngtest <input_file1> [input_file2] ...\n");
		return -1;
	}

	for (int fileIdx = 1; fileIdx < argc; ++fileIdx)
	{
		char szInputFileName[256];
		char szOutputFileName[256];

		strcpy(szInputFileName, argv[fileIdx]);
		strcpy(szOutputFileName, szInputFileName);
		strcat(szOutputFileName, "_out.png");

		printf("\n=== %s ===\n", szInputFileName);

		size_t nReqSize = NPngProc::readPngFile(szInputFileName, 0, 0, 0, 0);
		if (nReqSize == NPngProc::PNG_ERROR)
		{
			printf("Error reading file\n");
			continue;
		}

		unsigned char* pInputBits = new unsigned char[nReqSize];
		unsigned char* pOutputBits = new unsigned char[nReqSize];

		size_t nWidth, nHeight;
		NPngProc::readPngFileGray(szInputFileName, pInputBits, &nWidth, &nHeight);

		ImageProcessingGray(pOutputBits, pInputBits, nWidth, nHeight);

		NPngProc::writePngFile(szOutputFileName, pOutputBits, nWidth, nHeight, 8);

		delete[] pInputBits;
		delete[] pOutputBits;
	}

	return 0;
}

void ImageProcessingGray(unsigned char* pOut,
	unsigned char* pIn,
	size_t nWidth,
	size_t nHeight)
{
	int hist[256];
	computeHistogram(pIn, nWidth, nHeight, hist);
	ImageStats st = computeStats(hist, nWidth * nHeight);

	printf("Mean:         %.4f\n", st.mean);
	printf("Variance:     %.4f\n", st.variance);
	printf("Entropy:      %.4f\n", st.entropy);
	printf("Energy:       %.6f\n", st.energy);
	printf("Skewness:     %.4f\n", st.skewness);
	printf("Kurtosis:     %.4f\n", st.kurtosis);
	printf("Q1/Q2/Q3:     %.0f / %.0f / %.0f\n", st.q1, st.q2, st.q3);

	double (*glcm)[256] = new double[256][256];
	computeGLCM(pIn, nWidth, nHeight, 0, 1, glcm);
	printf("GLCM Energy:  %.6f\n", computeGLCMEnergy(glcm));
	delete[] glcm;

	addGaussianNoise(pIn, pOut, nWidth, nHeight, 500.0);
	printf("PSNR:         %.2f dB\n", computePSNR(pIn, pOut, nWidth, nHeight));
}
