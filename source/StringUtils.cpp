
#include "StringUtils.h"
#include <sstream>

using namespace std;
using namespace Timewhale;

vector<string> Timewhale::split (const string &line, const string &delimiters) {
	vector<string> words;
	size_t end = 0;
	for (;;) {
		size_t start = line.find_first_not_of (delimiters, end);
		if (start == string::npos) break;
		end = line.find_first_of (delimiters, start);
		words.push_back (line.substr (start, end - start));
	}
	return words;
}

wstring Timewhale::toWideString (const string &str) {
    auto itor = str.begin();
    auto end = str.end();

    wstringstream wss;
    wss.str(L"");

    for (; itor != end; ++itor) {
        wss << *itor;
    }

    return wss.str();
}