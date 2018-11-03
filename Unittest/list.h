#pragma once

#define string wchar_t *
#define MAX 1000

enum Group {N, Friend, Company, Family, None};
wchar_t *Group_String[100] = { L"",L"친구", L"회사", L"가족", L"미지정" };
#define 가 0xAC00
wchar_t kor[19] = { L'ㄱ', L'ㄲ', L'ㄴ', L'ㄷ', L'ㄸ', L'ㄹ', L'ㅁ', L'ㅂ', L'ㅃ', L'ㅅ', L'ㅆ', L'ㅇ', L'ㅈ', L'ㅉ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ' };
enum Mod { All, Group };

typedef struct Prof {
	wchar_t name[100];
	wchar_t number[100];
	wchar_t memo[100];
	int group_type;
	int favorite;
	int order;
}Prof;

class List{ // 상황봐서 struct로 수정
public :
	Prof Profile[MAX+1];
	int start, last;
	List();
	void change(string name, string number, string memo, int type, int fav, int ord);
	void add(string name, string number, string memo, int type, int fav);
	void Del( int num );
};
List s;
void List::Del(int num) {
	int i;
	for (i = num; i < s.last; i++) {
		Profile[i] = Profile[i + 1];
		Profile[i].order--;
	}
	Profile[s.last--] = Profile[0];
}
List::List() {
	start = 1;
	last = 0;
}
void List::add(string name, string number, string memo, int type, int fav) {
	Prof temp;
	last++;
	wcscpy(temp.name, name);
	wcscpy(temp.number, number);
	wcscpy(temp.memo, memo); 
	temp.group_type = type;
	temp.favorite = fav;
	temp.order = last;
	Profile[last] = temp;
}
void List::change(string name, string number, string memo, int type, int fav, int ord) {
	Prof temp;
	wcscpy(temp.name, name);
	wcscpy(temp.number, number);
	wcscpy(temp.memo, memo);
	temp.group_type = type;
	temp.favorite = fav;
	temp.order = ord;
	Profile[ord] = temp;

}