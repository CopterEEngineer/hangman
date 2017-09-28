#include "stdafx.h"
#include "HangmanGuess.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <time.h>


void HangmanGame::HangmanInit(void)
{
	// save in sample words
	string _word;
	ifstream _wordpool("words_in.txt"), _wordtest("words_os.txt");
	if (_wordpool.is_open())
	{
		while (getline(_wordpool, _word))
			if (isAlpha(_word))
				wordPool.insert(ToLower(_word));
		_wordpool.close();
	}
	if (_wordtest.is_open())
	{
		while (getline(_wordtest, _word)) 
			if (isAlpha(_word))
				wordTest.emplace_back(ToLower(_word));
		_wordtest.close();
	}
	//reverse(wordTest.begin(), wordTest.end());
	//char alpha[] = "etaonrishdlfcmugypwbvkjxqz";
	char alpha[] = "abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < 26; i++)
		alphabet.emplace_back(alpha[i]);
}

string HangmanGame::ToLower(string s)
{
	for (int i = s.length() - 1; i>= 0; i--)
		s[i] = tolower(s[i]);
	return s;
}

bool HangmanGame::isAlpha(string s)
{
	for (int i = s.length() - 1; i >= 0; i--)
		if (!isalpha(s[i]))
			return false;
	return true;
}

void HangmanGame::HangmanPlay()
{
	char letterGuess;
	vector<int> letterPoss;
	int info = 0;
	int testSize = wordTest.size();
	cout << "Hangman Game Starts!" << endl << endl;
	
	for(int i = 0; i < testSize; i++)
	{
		string s = wordTest[i];
		PlayInit();
		GetWordLength(s);
		ExtractWordPool();

		BoardShow(s);

		for (int hitCount = 0; wordLength > 0; )
		{
			letterGuess = Guess();

			if (isHit(s, letterGuess, letterPoss) > 0)
			{
				// hit
				for (int i = letterPoss.size() - 1; i >= 0; i--)
				{
					wordChars[letterPoss[i]] = letterGuess;
					hitCount++;
				}
				hitChars.insert(letterGuess);
			}
			else
			{
				// miss
				missChars.insert(letterGuess);
				life--;
			}

			BoardShow(letterGuess);

			isGameover(life, hitCount, info);
			if (info > 0)
			{
				wordUnGuessed.insert(s);
				break;
			}
			else if (info < 0)
			{
				wordGuessed.insert(s);
				break;
			}

			ReducedWordPool();
		}

	}
}

void HangmanGame::PlayInit(void)
{
	// init member variables
	wordLength = 0;
	life = 6;
	wordChars.clear();
	missChars.clear();
	hitChars.clear();
}

void HangmanGame::GetWordLength(string s)
{
	wordLength = s.length();
	for (int i = 0; i < wordLength; i++)
		wordChars.push_back('_');
}

void HangmanGame::BoardShow(string s)
{
	cout << endl << "target word: " << s << endl;

#ifdef _DEBUG
	for (char c : wordChars)
		cout << c << " ";

	cout << " missed: ";
	for (char c : missChars)
		cout << c << " ";

	cout << endl;
#endif // DEBUG

}

void HangmanGame::BoardShow(char c)
{
#ifdef _DEBUG
	cout << "guess: " << c << endl;
	for (char c : wordChars)
		cout << c << " ";

	cout << " missed: ";
	for (char c : missChars)
		cout << c << " ";

	cout << endl;
#endif // DEBUG

}

void HangmanGame::ExtractWordPool(void)
{
	// length filter
	for (string word : wordPool)
		if (word.length() == wordLength)
			reducedwordPool.insert(word);
	//reducedwordPool = wordPool;

	// clear previous gama data
	missChars.clear();
	hitChars.clear();
}

char HangmanGame::Guess(void)
{
	// return the letter guessing
	int maxCount = 0;
	char guessChar = '\0';
	vector<char> guessChars;
	map<char, int> distribution;
	multimap<int, char> distriflip;

	if (reducedwordPool.empty())
	{
		int alphabetLen = alphabet.size();
		for (int i = 0; i < alphabetLen; i++)
		{
			guessChar = alphabet[i];
			if (hitChars.find(guessChar) == hitChars.end() && missChars.find(guessChar) == missChars.end())
				return guessChar;
		}
	}

	// get word counts which contains a certain letter
	WordFrequency(distribution);
	//LetterFrequency(distribution);

	// sort letters by counts
	distriflip = flip_map(distribution);

	// generate chars order, the last one has the most counts
	for (auto &stat : distriflip)
		guessChars.push_back(stat.second);

	if (!guessChars.empty())
		for (int i = guessChars.size() - 1; i >= 0; i--)
		{
			guessChar = guessChars[i];
			if (hitChars.find(guessChar) == hitChars.end() && missChars.find(guessChar) == missChars.end())
				break;
		}
	/*if (!guessChars.empty())
		guessChar = guessChars[guessChars.size() - 1];*/

	return guessChar;
}

void HangmanGame::LetterFrequency(map<char, int> &distribution)
{
	// counting the number of letters in a certain word pool
	for (string word : reducedwordPool)
	{
		for (char c : word)
		{

			if (distribution.count(c) == 1)
				distribution[c] += 1;
			else
				distribution[c] = alphabet.size();
		}
	}

	for (int i = alphabet.size() - 1; i >= 0; i--)
		if (distribution.count(alphabet[i]) == 0)
			distribution[alphabet[i]] = i;
	//for (char c : alphabet)
	//	if (distribution.count(c) == 0)
	//		distribution[c] = 0;
}

void HangmanGame::WordFrequency(map<char, int> &distribution)
{
	// counting the number of words which contains a certain letter
	for (string word : reducedwordPool)
	{
		set<char> prevChars;
		for (char c : word)
		{
			//if (prevChars.count(c) + hitChars.count(c) + missChars.count(c) == 0)
			if (prevChars.count(c) == 0)
			{
				if (distribution.count(c) == 1)
					distribution[c] += 1;
				else
					distribution[c] = alphabet.size();
				prevChars.insert(c);
			}

		}
	}

	for (int i = alphabet.size() - 1; i >= 0; i--)
		if (distribution.count(alphabet[i]) == 0)
			distribution[alphabet[i]] = i;
	//for (char c : alphabet)
	//	if (distribution.count(c) == 0)
	//		distribution[c] = 0;
}

int HangmanGame::isHit(string s, char w, vector<int> &p)
{
	// judge whether the guessing letter hits, return the position
	int position = s.find(w);
	p = {};
	while (position != string::npos)
	{
		p.push_back(position);
		position = s.find(w, position + 1);
	}

	return p.size();
}

void HangmanGame::ReducedWordPool(void)
{
	set<string> tempwordPool = reducedwordPool;
	for (string word : tempwordPool)
	{
		// hit chars filter
		for (char c : hitChars)
			if (word.find(c) == string::npos)
				reducedwordPool.erase(word);
		// miss chars filter
		for (char c : missChars)
			if (word.find(c) != string::npos)
				reducedwordPool.erase(word);
	}
	if (reducedwordPool.empty())
	{
#ifdef _DEBUG
		printf("Warning: Empty Word Pool! \n");
#endif // _DEBUG
		reducedwordPool = tempwordPool;
	}
	
}

void HangmanGame::isGameover(int l, int h, int &info)
{
	info = 0;
	if (l <= 0)
		info = 1;
	else
		if (h == wordLength)
			info = -1;
}

void HangmanGame::RecordUnguessed(string s)
{
	wordUnGuessed.insert(s);
}

void HangmanGame::RecordGuessed(string s)
{
	wordGuessed.insert(s);
}

void HangmanGame::HangmanReport(clock_t t0)
{
	cout << endl << endl;
	printf("Number of word tested: %d\n", wordTest.size());
	printf("Number of word guessed correctly: %d \n", wordGuessed.size());
	printf("Correct Guessed (%%) : %.2f\n", 100.0 * wordGuessed.size() / wordTest.size());
	printf("Time to run: %.2f seconds\n", (double(clock() - t0)) / CLOCKS_PER_SEC);

	ofstream outUnGuessed("wordUnGuessed.txt"), outGuessed("wordGuessed.txt"), HangmanReport("report.txt");
	if (outUnGuessed.is_open())
		for (string s : wordUnGuessed)
			outUnGuessed << s << endl;
	if (outGuessed.is_open())
		for (string s : wordGuessed)
			outGuessed << s << endl;
	if (HangmanReport.is_open())
	{
		HangmanReport << "Number of word tested: " << wordTest.size() << endl;
		HangmanReport << "Number of word guessed correctly: " << wordGuessed.size() << endl;
		HangmanReport << "Correct Guessed (%): " << 100.0 * wordGuessed.size() / wordTest.size() << endl;
		HangmanReport << "Time to run: " << (double(clock() - t0)) / CLOCKS_PER_SEC << " seconds" << endl;
	}
}