#include<locale.h>

#define 가 0xAC00
wchar_t kor[19] = { L'ㄱ', L'ㄲ', L'ㄴ', L'ㄷ', L'ㄸ', L'ㄹ', L'ㅁ', L'ㅂ', L'ㅃ', L'ㅅ', L'ㅆ', L'ㅇ', L'ㅈ', L'ㅉ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ' };
enum Mod {All, Group};

/* 이름이 3글자일때만 가능
int hs(string a) {
	int temp = 361, t = 0;
	for (int i = 0; i < wcslen(a); i++) {
		printf('%s', kor[(a[i] - 가) / 28 / 21]);
		t = temp * ((a[i] - 가) / 28 / 21);
		temp = temp / 19;
	}
	printf('%d\n', t);
	return t;
}*/