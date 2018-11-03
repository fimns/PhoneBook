#pragma once
#pragma warning (disable:4996)

#include<stdio.h>
#include<stdlib.h>
#include<gtest/gtest.h>
#include"list.h"

Mod mode = All;
void Search(string temp);
void Mode_Change();
void Gov(int a1, string name, string number, string memo, int type, int fav);
bool *find; //ShowTest¿ë º¯¼ö

/*TEST(InputTest ,add) {
	EXPECT_STREQ(L"±è¼º¿ø", s.Profile[s.last].name);
	EXPECT_STREQ(L"010-1234-5678", s.Profile[s.last].number);
	EXPECT_STREQ(L"Memo", s.Profile[s.last].memo);
	EXPECT_EQ(Friend, s.Profile[s.last].group_type);
	EXPECT_EQ(0, s.Profile[s.last].favorite);
}

TEST(DelTest, del) {
	s.add(L"±è¼º¿ø", L"010-1234-5678", L"Memo", Friend, 0);
	s.add(L"±è¼º¿ø2", L"010-1234-5679", L"Memo2", Company, 1);
	s.Del(1);
	EXPECT_STREQ(L"±è¼º¿ø2", s.Profile[1].name);
	EXPECT_STREQ(L"010-1234-5679", s.Profile[1].number);
	EXPECT_STREQ(L"Memo2", s.Profile[1].memo);
	EXPECT_EQ(Company, s.Profile[1].group_type);
	EXPECT_EQ(1, s.Profile[1].favorite);
}

TEST(ShowTest, show) {
	Mode_Change();
	EXPECT_EQ(Group, mode);
}

TEST(SearchTest, Search) {
	s.add(L"±è¼º¿ø", L"010-1234-5678", L"Memo", Friend, 0);
	s.add(L"±è¼ºÈÆ", L"010-1234-5679", L"Memo2", Company, 1);
	s.add(L"ÀÌ¼º¿ø", L"010-1234-5555", L"Memo3", Company, 1);
	Search(L"±è¼º¿ø");
	EXPECT_TRUE(find[1]);
	EXPECT_FALSE(find[2]);
	Search(L"¤¡¤µ¤·");
	EXPECT_TRUE(find[1]);
	Search(L"010-1234-5679");
	EXPECT_TRUE(find[2]);
	
}*/

TEST(ChangeTest, Change) {
	s.add(L"±è¼º¿ø", L"010-1234-5678", L"Memo", Friend, 0);
	s.add(L"±è¼ºÈÆ", L"010-1234-5679", L"Memo2", Company, 1);
	s.add(L"ÀÌ¼º¿ø", L"010-1234-5555", L"Memo3", Company, 1);
	Gov(2, L"¹Ú¼º¿ø", L"010-4321-5678", L"Memo2-2", Family, 0);
	EXPECT_STREQ(L"¹Ú¼º¿ø", s.Profile[2].name);
	EXPECT_STREQ(L"010-4321-5678", s.Profile[2].number);
	EXPECT_STREQ(L"Memo2-2", s.Profile[2].memo);
	EXPECT_EQ(Family, s.Profile[2].group_type);
	EXPECT_EQ(0, s.Profile[2].favorite);
}
int main(int argc, char **argv) {
	//	system("mode con cols=80 lin	es=25");
	_wsetlocale(LC_ALL, L"Korean");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
void Search(string temp) {
	find = (bool *)malloc(sizeof(bool) * MAX);
	int i, j, chk;
	for (i = 1; i <= MAX; i++) find[i] = false;
	for (i = s.start; i <= s.last; i++) {
		chk = 1;
		if (wcscmp(s.Profile[i].name, temp) == 0 || wcscmp(s.Profile[i].number, temp) == 0) find[i] = true;

		for (j = 0; j < wcslen(temp); j++) {
			if (kor[(s.Profile[i].name[j] - 0xAC00) / 28 / 21] != (temp[j])) chk = 0;
		}
		if (chk == 1) find[i] = true;
	}
}

void Gov(int a1, string name, string number, string memo, int type, int fav) {
	if (s.start <= a1 && a1 <= s.last) {
		wcscpy(s.Profile[a1].name, name);
		wcscpy(s.Profile[a1].number, number);
		wcscpy(s.Profile[a1].memo, memo);
		s.Profile[a1].group_type = type;
		s.Profile[a1].favorite = fav;
	}
}