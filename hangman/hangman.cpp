// hangman.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HangmanGuess.h"
#include <iostream>

//using namespace std;

int main()
{
	HangmanGame hg;
	clock_t tStart;

	tStart = clock();
	hg.HangmanInit();
	hg.HangmanPlay();
	hg.HangmanReport(tStart);
	
	system("pause");
	return 0;
}

