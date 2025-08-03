#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>
#include <windows.h>

using namespace std;

struct Word {
    string english;
    string japanese;
};

random_device rd;
mt19937 gen(rd());

vector<string> makeChoices(const string& correct, const vector<Word>& wordList) {
    vector<string> choices{ correct };
    uniform_int_distribution<> dist(0, wordList.size() - 1);
    while (choices.size() < 4) {
        string candidate = wordList[dist(gen)].japanese;
        if (find(choices.begin(), choices.end(), candidate) == choices.end()) {
            choices.push_back(candidate);
        }
    }
    shuffle(choices.begin(), choices.end(), gen);
    return choices;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);  

    vector<Word> wordList;
    ifstream file("words_utf8.txt");
    if (!file) {
        cerr << "words_utf8.txt を開けませんでした．" << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string index, english, japanese;
        getline(ss, index, '\t');
        getline(ss, english, '\t');
        getline(ss, japanese, '\t');
        if (!english.empty() && !japanese.empty()) {
            wordList.push_back({ english, japanese });
        }
    }
    file.close();

    int Start, End, questionCount;
    cout << "出題範囲を入力してください（例: 1 500）: ";
    cin >> Start >> End;

    if (Start < 1 || End > wordList.size() || Start > End) {
        cerr << "無効な範囲です．" << endl;
        return 1;
    }


    int Size = End - Start + 1;

    cout << "出題する問題数（最大 " << Size << "）: ";
    cin >> questionCount;
    if (questionCount < 1 || questionCount > Size) {
        cerr << "無効な問題数です．" << endl;
        return 1;
    }

    vector<Word> targetList(wordList.begin() + (Start - 1), wordList.begin() + End);
    shuffle(targetList.begin(), targetList.end(), gen);

    int correctCount = 0;

    for (int i = 0; i < questionCount; ++i) {
        Word quiz = targetList[i];
        vector<string> choices = makeChoices(quiz.japanese, wordList);

        cout << "【第 " << (i + 1) << " 問】 " << quiz.english << " の意味は？" << endl;
        for (int j = 0; j < 4; ++j) {
            cout << j + 1 << ". " << choices[j] << endl;
        }

        int answer;
        cout << ">> ";
        cin >> answer;

        if (answer >= 1 && answer <= 4 && choices[answer - 1] == quiz.japanese) {
            cout << "正解！" << endl << endl;
            correctCount++;
        }
        else {
            cout << "不正解．正解は「" << quiz.japanese << "」でした．" << endl << endl;
        }
    }

    cout << "---------------------------------------" << endl;
    cout << "正解数： " << correctCount << " / " << questionCount << endl;
    cout << "正答率： " << (correctCount * 100 / questionCount) << " %" << endl;

    cout << "\n【出題された単語一覧】" << endl;
    vector<Word>::iterator it;
    for (it = targetList.begin(); it != targetList.begin() + questionCount; ++it) {
        cout << "- " << it->english << " : " << it->japanese << endl;
    }

    return 0;
}
