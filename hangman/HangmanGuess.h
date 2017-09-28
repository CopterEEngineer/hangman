#pragma once
#define HangmanGuess_h

#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <time.h>

using namespace std;

#define Abs(a) (a) >=0 ? (a):(-a)

class HangmanGame
{
public:
	HangmanGame() { ; }
	~HangmanGame() { ; }

	void HangmanInit(void);
	void HangmanPlay(void);
	void HangmanReport(clock_t);

private:
	string ToLower(string s);
	bool isAlpha(string s);
	void PlayInit(void);
	void GetWordLength(string s);
	void BoardShow(string);
	void BoardShow(char);
	void ExtractWordPool(void);
	void ReducedWordPool(void);
	char Guess(void);
	void LetterFrequency(map<char, int> &M);
	void WordFrequency(map<char, int> &M);
	int isHit(string, char, vector<int>&);
	void isGameover(int, int, int&);
	void RecordUnguessed(string s);
	void RecordGuessed(string s);

private:
	int wordLength, life;
	vector<char> wordChars, alphabet;
	set<string> wordPool, reducedwordPool;
	vector<string> wordTest;
	set<char> missChars, hitChars;
	set<string> wordGuessed, wordUnGuessed;
};


template<typename A, typename B>
pair<B, A> flip_pair(const pair<A, B> &p)
{
	return pair<B, A>(p.second, p.first);
}

template<typename A, typename B>
multimap<B, A> flip_map(const map<A, B> &src)
{
	multimap<B, A> dst;
	transform(src.begin(), src.end(), inserter(dst, dst.begin()), flip_pair<A, B>);
	return dst;
}

