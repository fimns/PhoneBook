#include<locale.h>

#define �� 0xAC00
wchar_t kor[19] = { L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��', L'��' };
enum Mod {All, Group};

/* �̸��� 3�����϶��� ����
int hs(string a) {
	int temp = 361, t = 0;
	for (int i = 0; i < wcslen(a); i++) {
		printf('%s', kor[(a[i] - ��) / 28 / 21]);
		t = temp * ((a[i] - ��) / 28 / 21);
		temp = temp / 19;
	}
	printf('%d\n', t);
	return t;
}*/