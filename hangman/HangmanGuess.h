#pragma once
#define HangmanGuess_h

#include <set>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <time.h>
#include <omp.h>

using namespace std;

#define Abs(a) ((a) >=0 ? (a):(-a))

class HangmanGame
{
public:
	HangmanGame() { ; }
	~HangmanGame() { ; }

	void HangmanInit(void);
	void HangmanPlay(void);
	void HangmanReport(clock_t);
	void HangmanReport(clock_t, int);

	friend void HangmanPlayMP(HangmanGame, clock_t);

//private:
	string ToLower(string s);
	bool isAlpha(string s);
	void PlayInit(void);
	void GetWordLength(string s);
	void BoardShow(string);
	void BoardShow(char);
	void ExtractWordPool(void);
	void ExtractWordPool(set<string>);
	void ReducedWordPool(void);
	void ReducedWordPool(set<string>);
	void UpdateWordPool(string);
	void UpdateWordPool(string, set<string>&);
	char Guess(void);
	char Guess(int);
	void LetterFrequency(map<char, int> &M);
	void InitLtFrequency(map<char, int> &M);
	void WordFrequency(map<char, int> &M);
	void HangmanNB(map<char, int> &distribution);
	int isHit(string, char, vector<int>&);
	void isGameover(int, int, int&);
	void RecordUnguessed(string s);
	void RecordGuessed(string s);
	float WinChance(int, set<string>, set<char>);
	float WinChance(int, set<string>, set<char>, char);
	char maxChanceChar(int);

	set<string> findPattern(int, set<string>&);
	set<string> _findPattern1(set<string>&);
	set<string> _findPattern2(set<string>&);
	int LevenshteinDistance(string, string);
	int FreqDistance(string, string);

//private:
	int wordLength, life, extlength;
	vector<char> wordChars, alphabet;
	vector<char> missChars, hitChars;

	set<string> wordPool, OrgwordPool, reducedwordPool, ExtendwordPool;
	set<string> wordGuessed, wordUnGuessed, reducedwordUnGd;
	vector<string> deleteWords;

	vector<string> wordTest;
	set<char> remainChars;
	map<int, vector<char>> alphabets;
};


void HangmanPlayMP(clock_t);
void HangmanReport(clock_t, set<string>&, set<string>&, int);

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

