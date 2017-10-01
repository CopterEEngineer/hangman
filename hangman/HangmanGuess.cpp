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
	ifstream _wordpool("abc_in.txt"), _wordtest("abc_in.txt");
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
		sort(wordTest.begin(), wordTest.end());
		wordTest.erase(unique(wordTest.begin(), wordTest.end()), wordTest.end());
	}
	//reverse(wordTest.begin(), wordTest.end());
	char alpha[] = "etaonrishdlfcmugypwbvkjxqz";
	//char alpha[] = "abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < 26; i++)
	{
		alphabet.emplace_back(alpha[i]);
		remainChars.insert(alpha[i]);
	}
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

void HangmanGame::HangmanPlay(void)
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
			letterGuess = Guess(hitCount);

			if (isHit(s, letterGuess, letterPoss) > 0)
			{
				// hit
				for (int i = letterPoss.size() - 1; i >= 0; i--)
				{
					wordChars[letterPoss[i]] = letterGuess;
					hitCount++;
				}
				hitChars.emplace_back(letterGuess);
			}
			else
			{
				// miss
				missChars.emplace_back(letterGuess);
				life--;
			}

			BoardShow(letterGuess);

			isGameover(life, hitCount, info);
			if (info > 0)
			{
				wordUnGuessed.insert(s);
				UpdateWordPool(s);
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
	wordLength = extlength = 0;
	life = 6;
	wordChars.clear();
	missChars.clear();
	hitChars.clear();
	reducedwordPool.clear();
	reducedwordUnGd.clear();
	ExtendwordPool.clear();
	remainChars.clear();
	OrgwordPool.clear();
	char alpha[] = "abcdefghijklmnopqrstuvwxyz";
	for (int i = 0; i < 26; i++)
		remainChars.insert(alpha[i]);

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
	ExtractWordPool(wordPool);
}

void HangmanGame::ExtractWordPool(set<string> WP)
{
	// length filter
	extlength = 0;
	for (auto it=WP.begin();it!=WP.end();++it)
	{
		string word = *it;
		int temp = word.length() - wordLength;
		temp = Abs(temp);
		if (temp <= extlength)
		{
			reducedwordPool.insert(word);
		}
		if (temp <= extlength + 1)
		{
			ExtendwordPool.insert(word);
		}
	}
	//reducedwordPool = wordPool;
	OrgwordPool = reducedwordPool;

	reducedwordUnGd = wordUnGuessed;
	//for (string word : wordUnGuessed)
	//	if (Abs(word.length() - wordLength) <= 1)
	//		reducedwordUnGd.insert(word);

	if (alphabets.count(wordLength) == 0)
	{
		map<char, int> dis;
		multimap<int, char> disflip;
		vector<char> _vc;
		for (string word : reducedwordPool)
		{
			set<char> prevChars;
			for (char c : word)
			{
				if (prevChars.count(c) == 0)
				{
					if (dis.count(c) == 1)
						dis[c] += 1;
					else
						dis[c] = 0;
					prevChars.insert(c);
				}
			}
		}

		disflip = flip_map(dis);


		for (auto it = disflip.cbegin(); it != disflip.cend(); ++it)
			_vc.push_back(it->second);
		reverse(_vc.begin(), _vc.end());
		for (char c : alphabet)
		{
			if (find(_vc.begin(), _vc.end(), c) == _vc.end())
				_vc.push_back(c);
		}
		alphabets[wordLength] = _vc;
	}
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
			if (find(hitChars.begin(), hitChars.end(), guessChar) == hitChars.end())
				if(find(missChars.begin(), missChars.end(), guessChar) == missChars.end())
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
			if (find(hitChars.begin(), hitChars.end(), guessChar) == hitChars.end())
				if (find(missChars.begin(), missChars.end(), guessChar) == missChars.end())
					break;
		}

	return guessChar;
}

char HangmanGame::Guess(int hc)
{
	// return the letter guessing
	int maxCount = 0;
	char guessChar = '\0';
	vector<char> guessChars;
	map<char, int> distribution;

	//if (reducedwordPool.empty())
	//{
	//	int alphabetLen = alphabet.size();
	//	for (int i = 0; i < alphabetLen; i++)
	//	{
	//		guessChar = alphabet[i];
	//		if (find(hitChars.begin(), hitChars.end(), guessChar) == hitChars.end())
	//			if (find(missChars.begin(), missChars.end(), guessChar) == missChars.end())
	//				return guessChar;
	//	}
	//}

	if (hc == 0)
	{
		// get word counts which contains a certain letter
		WordFrequency(distribution);
		//InitLtFrequency(distribution);
		//LetterFrequency(distribution);
	}
	else
	{
		// get word counts which have certain pattern
		HangmanNB(distribution);
	}

	// remain chars filter
	for (char c : hitChars)
		distribution.erase(c);
	for (char c : missChars)
		distribution.erase(c);

	// get the most chars
	while (true)
	{
		int maxCount = 0;
		for (auto it = distribution.cbegin(); it != distribution.cend(); ++it)
		{
			if (it->second == maxCount)
				guessChars.push_back(it->first);
			if (it->second > maxCount)
			{
				guessChars.clear();
				guessChars.push_back(it->first);
				maxCount = it->second;
			}
		}

		if (guessChars.size() == 1)
		{
			guessChar = guessChars[0];
			break;
		}
		else
		{
			// deal with tie
			vector<char> _guessChars;
			if (reducedwordUnGd.empty())
				_guessChars = guessChars;
			else 
			{
				for (string word : reducedwordUnGd)
				{
					for (char c : guessChars)
					{
						if (word.find(c) == string::npos)
							_guessChars.push_back(c);
					}
				}
			}
			
			if (_guessChars.size() == 1)
			{
				guessChar = _guessChars[0];
				break;
			}
			else if (_guessChars.size() == 0 && reducedwordUnGd.size() != 0)
			{
				for (char c : guessChars)
					distribution.erase(c);
				if (distribution.empty())
					break;
			}
			else
			{
				vector<char> _alpha = alphabet;
				for (char c : alphabet)
					if (find(hitChars.begin(), hitChars.end(), c) == hitChars.end())
						if (find(missChars.begin(), missChars.end(), c) == missChars.end())
							_alpha.push_back(c);

				int minP = _alpha.size() - 1;
				for (int i = 0, p = 0; i<_guessChars.size(); i++)
				{
					p = find(_alpha.begin(), _alpha.end(), _guessChars[i]) - _alpha.begin();
					if (p < minP)
					{
						guessChar = _guessChars[i];
						minP = i;
					}
				}
				break;
			}
		}
	}
	
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

void HangmanGame::InitLtFrequency(map<char, int> &distribution)
{
	// counting the number of words based on the initial letter
	for (string word : reducedwordPool)
	{
		char c = word[0];
		if (distribution.count(c) == 0)
			distribution[c] += 1;
		else
			distribution[c] = alphabet.size();
	}

	vector<char> _a = alphabets[wordLength];
	for (int i = _a.size() - 1; i >= 0; i--)
		if (distribution.count(_a[i]) == 0)
			distribution[_a[i]] = _a.size() - i - 1;
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

	vector<char> _a = alphabets[wordLength];
	for (int i = _a.size() - 1; i >= 0; i--)
		if (distribution.count(_a[i]) == 0)
			distribution[_a[i]] = _a.size() - i - 1;
}

void HangmanGame::HangmanNB(map<char, int> &distribution)
{
	// counting the number of words which have certain pattern
	int p = 0;
	vector<int> ps;
	set<string> tempwordPool;
	set<string> tempWPUnGd;

	tempwordPool = findPattern(2, ExtendwordPool);
	reducedwordUnGd = findPattern(2, reducedwordUnGd);

	if (tempwordPool.size() == 0)
		WordFrequency(distribution);
	else
	{
		// remain chars
		for (char c : alphabet)
		{
			if (find(hitChars.begin(), hitChars.end(), c) != hitChars.end())
				remainChars.erase(c);
			if (find(missChars.begin(), missChars.end(), c) != missChars.end())
				remainChars.erase(c);
		}
		for (string word : tempwordPool)
		{
			set<char> prevChars;
			for (char c : word)
			{
				if (remainChars.count(c) != 0 && prevChars.count(c) == 0)
				{
					if (distribution.count(c) == 1)
						distribution[c] += 1;
					else
						distribution[c] = alphabet.size();
					prevChars.insert(c);
				}
			}
		}

		vector<char> _a = alphabets[wordLength];
		for (int i = _a.size() - 1; i >= 0; i--)
			if (distribution.count(_a[i]) == 0)
				distribution[_a[i]] = _a.size() - i - 1;
	}
}

set<string> HangmanGame::findPattern(int kind, set<string> &S)
{
	switch (kind)
	{
	case 1:
		return _findPattern1(S);
		break;
	case 2:
		return _findPattern2(S);
		break;
	}
}

set<string> HangmanGame::_findPattern1(set<string> &S)
{
	set<string> tempwordPool = S;
	// hit chars filter
	int charCount = 0;
	for (char c : hitChars)
	{
		charCount++;
		int bp = 0;
		vector<int> bps;

		// char guessed positions
		bp = find(wordChars.begin(), wordChars.end(), c) - wordChars.begin();
		while (bp >= 0 && bp < wordLength)
		{
			bps.push_back(bp);
			bp = find(wordChars.begin() + bp + 1, wordChars.end(), c) - wordChars.begin();
		}

		for (string word : S)
		{
			int p = 0;
			vector<int> ps;
			p = word.find(c);
			while (p != string::npos)
			{
				ps.push_back(p);
				p = word.find(c, p + 1);
			}

			if (ps.empty())
				// word has no hit char
				tempwordPool.erase(word);
			else
			{
				if (bps.size() != ps.size())
					// word has hit char, but different counts
					tempwordPool.erase(word);
				else if (bps.size() > 1)
				{
					// word has hit char, with same counts ( > 1)
					vector<int> dbps, dps;
					for (int i = 0; i < bps.size() - 1; i++)
					{
						dbps.push_back(bps[i + 1] - bps[i]);
						dps.push_back(ps[i + 1] - ps[i]);
					}
					// word has hit char, but with different relative distance
					if (dbps != dps)
						tempwordPool.erase(word);
				}

			}
		}
	}
	return tempwordPool;
}

set<string> HangmanGame::_findPattern2(set<string> &S)
{	
	vector<int> ps;
	set<string> tempwordPool = S;

	for (int i = 0, p = 0; i < wordChars.size(); i++, p++)
	{
		if (wordChars[i] != '_')
		{
			ps.push_back(p);
		}
	}
	
	for (string s : S)
	{
		int p = s.find(wordChars[ps[0]]);
		if (p == string::npos)
			tempwordPool.erase(s);
		else
		{
			if (ps.size() == 1 && s.find(wordChars[ps[0]], p + 1) == string::npos)
				continue;
			else
			{
				if (p + ps.back() > s.length())
					tempwordPool.erase(s);
				else
				{
					for (int i = 1; i < ps.size(); i++)
						if (s[p + ps[i] - ps[0]] != wordChars[ps[i]])
							tempwordPool.erase(s);
				}
				for (char c : hitChars)
					if (s.find(c, p + ps.back() - ps[0] + 1) != string::npos)
						tempwordPool.erase(s);
				continue;
			}
		}
	}
	return tempwordPool;
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
	ReducedWordPool(wordPool);
}

void HangmanGame::ReducedWordPool(set<string> WS)
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
	while (reducedwordPool.empty())
	{
#ifdef _DEBUG
		printf("Warning: Empty Word Pool! \n");
#endif // _DEBUG
		extlength++;
		for (auto it=WS.begin();it!=WS.end();++it)
		{
			string word = *it;
			int temp = word.length() - wordLength;
			temp = Abs(temp);
			if (temp <= extlength)
				if (OrgwordPool.find(word) == OrgwordPool.end())
					reducedwordPool.insert(word);
			if (temp <= extlength + 1)
				ExtendwordPool.insert(word);
		}
		OrgwordPool = reducedwordPool;
	}
}

void HangmanGame::UpdateWordPool(string s)
{
	UpdateWordPool(s, wordPool);
}

void HangmanGame::UpdateWordPool(string s, set<string> &WS)
{
	//vector<string> _vs;
	//vector<int> _ld;
	//vector<int> _fdist;
	//for (string word : OrgwordPool)
	//{
	//	bool flg = true;
	//	for (char c : hitChars)
	//	{
	//		if (word.find(c) == string::npos)
	//		{
	//			flg &= false;
	//			break;
	//		}
	//	}
	//	if (flg)
	//	{
	//		for (char c : missChars)
	//		{
	//			if (word.find(c) != string::npos)
	//			{
	//				_vs.push_back(word);
	//				//WS.erase(word);
	//				//deleteWords.emplace_back(word);
	//			}
	//		}
	//	}
	//}
	//if (!_vs.empty())
	//{
	//	float _ld_agv = 0;
	//	for (string s2 : _vs)
	//	{
	//		_ld.push_back(LevenshteinDistance(s, s2));	
	//		_ld_agv += _ld.back();
	//	}
	//	_ld_agv /= _ld.size();
	//	int minD = *min_element(_ld.begin(), _ld.end());
	//	for (int i = 0; i < _ld.size(); i++)
	//	{
	//		if (_ld[i] >= _ld_agv)
	//		{//if (_ld[i] >= wordLength / 2)
	//			WS.erase(_vs[i]);
	//			//deleteWords.emplace_back(_vs[i]);
	//		}
	//	}
	//}

	//if (!_vs.empty())
	//{
	//	float _df_agv = 0; 
	//	for (string s2 : _vs)
	//	{
	//		_fdist.push_back(FreqDistance(s, s2));
	//		_df_agv += _fdist.back();
	//	}
	//	_df_agv /= _fdist.size();
	//	for (int i = 0; i < _fdist.size(); i++)
	//	{
	//		if (_fdist[i] >= _df_agv) //if (_fdist[i] >= _df_agv)
	//		{			
	//			WS.erase(_vs[i]);
	//		}
	//	}
	//	
	//}
	WS.insert(s);
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


float HangmanGame::WinChance(int lf, set<string> pool, set<char> subalpha)
{
	if (subalpha.empty())
		return 1.0;
	else if (lf == 0)
		return 0.0;
	else if (lf > 0 && pool.size() <= 1)
		return 1.0;
	else
	{
		set<char> _subalpha = subalpha;
		float winchance = 0.0;
		for (char c: subalpha)
		{
			set<string> tempwordPool = pool;
			float _winchance = 0.0;
			int dlf = 0;
			for (string word : pool)
				if (word.find(c) == string::npos)
					tempwordPool.erase(word);
			if (tempwordPool.empty())
				dlf = 1;
			_winchance = 1.0 * tempwordPool.size() / pool.size();
			_subalpha.erase(c);
			_winchance *= WinChance(lf - dlf, tempwordPool, _subalpha);
			winchance += _winchance;
		}
		return winchance;
	}
}

float HangmanGame::WinChance(int lf, set<string> pool, set<char> r, char c)
{
	if (r.empty())
		return 1.0;
	else if (lf == 0)
		return 0.0;
	else if (lf > 0 && pool.size() == 1)
		return 1.0;
	else
	{
		set<string> rPool = pool;
		float _winchance = 0.0;
		int dlf = 0;
		for (string word : pool)
			if (word.find(c) == string::npos)
				rPool.erase(word);
		if (rPool.empty())
			dlf = 1;

		r.erase(c);
		for (char _c : r)
			_winchance += 1.0*rPool.size() / pool.size() * WinChance(lf - dlf, pool, r, _c);
		return _winchance;
	}
}

char HangmanGame::maxChanceChar(int lf)
{
	for (char c : alphabet)
	{
		if (find(hitChars.begin(), hitChars.end(), c) != hitChars.end())
			remainChars.erase(c);
		if (find(missChars.begin(), missChars.end(), c) != missChars.end())
			remainChars.erase(c);
	}

	char letterGuess = '\0';
	float winchance, maxwinchance;
	maxwinchance = -1.0;
	
	set<char> _remainChars = remainChars;
	for (char c : remainChars)
	{
		_remainChars.erase(c);
		winchance = WinChance(lf, reducedwordPool, _remainChars);
		if (winchance > maxwinchance)
		{
			letterGuess = c;
			maxwinchance = winchance;
		}
	}

	//for (char c : remainChars)
	//{
	//	winchance = WinChance(lf, reducedwordPool, remainChars, c);
	//	if (winchance > maxwinchance)
	//	{
	//		letterGuess = c;
	//		maxwinchance = winchance;
	//	}
	//}

	return letterGuess;
}

int HangmanGame::LevenshteinDistance(string s1, string s2)
{
	int n = s1.length();
	int m = s2.length();
	if (m == 0)return n;
	if (n == 0)return m;
	// construct a matrix
	typedef vector<vector<int>> Tmatrix;
	Tmatrix matrix(n + 1);
	for (int i = 0; i <= n; i++) matrix[i].resize(m + 1);

	//step 2 Initialize  

	for (int i = 1; i <= n; i++) matrix[i][0] = i;
	for (int i = 1; i <= m; i++) matrix[0][i] = i;

	//step 3  
	for (int i = 1; i <= n; i++)
	{
		const char si = s1[i - 1];
		//step 4  
		for (int j = 1; j <= m; j++)
		{

			const char dj = s2[j - 1];
			//step 5  
			int cost;
			if (si == dj) {
				cost = 0;
			}
			else {
				cost = 1;
			}
			//step 6  
			const int above = matrix[i - 1][j] + 1;
			const int left = matrix[i][j - 1] + 1;
			const int diag = matrix[i - 1][j - 1] + cost;
			matrix[i][j] = min(above, min(left, diag));

		}
	}//step7  
	return matrix[n][m];
}

int HangmanGame::FreqDistance(string s1, string s2)
{
	vector<int> f1, f2;
	vector<char> aorder;
	int p = 0;
	aorder = alphabets[wordLength];
	for (int i = 0; i < s1.length(); i++)
	{
		p = find(aorder.begin(), aorder.end(), s1[i]) - aorder.begin() + 1;
		f1.emplace_back(p);
	}
	for (int i = 0; i < s2.length(); i++)
	{
		p = find(aorder.begin(), aorder.end(), s2[i]) - aorder.begin() + 1;
		f2.emplace_back(p);
	}
	int m = f1.size();
	int n = f2.size();
	int dist = 0;

	while (m > f2.size()) f2.emplace_back(0);
	while (n > f1.size()) f1.emplace_back(0);

	sort(f1.begin(), f1.end());
	sort(f2.begin(), f2.end());

	for (int i = 0; i < f1.size(); i++)
	{
		dist += (f1[i] - f2[i]) * (f1[i] - f2[i]);
	}
	return dist;
}


void HangmanPlayMP(HangmanGame hg, clock_t t0)
{
	set<string> wordGuessed, wordUnGuessed;
	omp_lock_t writeLock;
	omp_init_lock(&writeLock);
	int wsize = 0;
	hg.HangmanInit();

	int testSize = hg.wordTest.size();
	cout << "Hangman Game Starts! Parallel Mode." << endl << endl;

	
#pragma omp parallel num_threads(6) firstprivate(hg)
	{
		char letterGuess;
		vector<int> letterPoss;
		int info = 0;
		int i, istart, iend;
		int nthrds = omp_get_num_threads(), id = omp_get_thread_num();
		if (testSize / nthrds == 0)
		{
			istart = id*(testSize / nthrds);
			iend = (id + 1)*(testSize / nthrds);
		}
		else
		{
			istart = id*(testSize / nthrds + 1);
			iend = (id + 1)*(testSize / nthrds + 1);
		}
		iend = (iend < testSize ? iend : testSize);

		for (i = istart; i < iend; i++)
		{
			string s = hg.wordTest[i];
			hg.PlayInit();
			hg.GetWordLength(s);
			hg.ExtractWordPool();

			hg.BoardShow(s);

			for (int hitCount = 0; hg.wordLength > 0;)
			{
				letterGuess = hg.Guess(hitCount);

				if (hg.isHit(s, letterGuess, letterPoss) > 0)
				{
					// hit
					for (int i = letterPoss.size() - 1; i >= 0; i--)
					{
						hg.wordChars[letterPoss[i]] = letterGuess;
						hitCount++;
					}
					hg.hitChars.emplace_back(letterGuess);
				}
				else
				{
					// miss
					hg.missChars.emplace_back(letterGuess);
					hg.life--;
				}

				hg.BoardShow(letterGuess);

				hg.isGameover(hg.life, hitCount, info);
				if (info > 0)
				{
					hg.wordUnGuessed.insert(s);
					wordUnGuessed.insert(s);

					//omp_set_lock(&writeLock);
					hg.UpdateWordPool(s);
					//omp_unset_lock(&writeLock);
					break;
				}
				else if (info < 0)
				{
					hg.wordGuessed.insert(s);
					wordGuessed.insert(s);
					break;
				}

				hg.ReducedWordPool();
			}
		}

		omp_set_lock(&writeLock);
		ofstream outdeleteWords("deleteWord.txt");		
		if (outdeleteWords.is_open())
		{
			for (string s : hg.deleteWords)
				outdeleteWords << s << endl;
			outdeleteWords.close();
		}
		wsize += hg.wordPool.size();
		omp_unset_lock(&writeLock);
	}
	cout << "Final word pool size: " << wsize << endl;
	HangmanReport(t0, wordGuessed, wordUnGuessed, testSize);
}

void HangmanReport(clock_t t0, set<string> &sg, set<string> &sug, int ts)
{
	ofstream outUnGuessed("wordUnGuessed.txt"), outGuessed("wordGuessed.txt"), HangmanReport("report.txt");
	cout << endl << endl;
	printf("Number of word tested: %d\n", ts);
	printf("Number of word guessed correctly: %d \n", sg.size());
	printf("Correct Guessed (%%) : %.2f\n", 100.0 * sg.size() / ts);
	printf("Time to run: %.2f seconds\n", (double(clock() - t0)) / CLOCKS_PER_SEC);
	if (outUnGuessed.is_open())
	{
		for (string s : sug)
			outUnGuessed << s << endl;
		outUnGuessed.close();
	}
	if (outGuessed.is_open())
	{
		for (string s : sg)
			outGuessed << s << endl;
		outGuessed.close();
	}

	if (HangmanReport.is_open())
	{
		HangmanReport << "Number of word tested: " << ts << endl;
		HangmanReport << "Number of word guessed correctly: " << sg.size() << endl;
		HangmanReport << "Correct Guessed (%): " << 100.0 * sg.size() / ts << endl;
		HangmanReport << "Time to run: " << (double(clock() - t0)) / CLOCKS_PER_SEC << " seconds" << endl;
		HangmanReport.close();
	}
}
