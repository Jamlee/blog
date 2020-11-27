#pragma once
#include "Wlist.h"
class Admin {
public:
	Admin() { }
	Admin(string wordListInit, string wordDisp, string startDisp) : wordListInit(wordListInit), wordDisp(wordDisp), startDisp(startDisp) { }

	// չʾ�������(ѧϰ���ʽ���)
	void display(UI_TYPE uiType, Word word = Word(), int dispTime = 0, bool doNotCh = false) {
		SetConsoleCursorPosition(hOutput, coord);
		For(i, 0, MAX_DISP_ROW) {
			// ǰ��Ӧ���������, �������ⲻ��.
			if (uiType == START) {
				cout << dispArr[LEARNING][i] << endl;
			}
			else if (uiType == LEARNING) {
				if (i == LEARNING_WORD_POS && word.get_id() > 0)
					cout << "   { " << word.get_id() << " }" << endl;
				else if (i == LEARNING_WORD_POS + 1)
					cout << "   " << word.get_eng().substr(0, MAX_DISP_LINE - 3) << endl;
				else if (i == LEARNING_WORD_POS + 2)
					cout << "   " << word.get_snd().substr(0, MAX_DISP_LINE - 3) << endl;
				else if (doNotCh == false && i == LEARNING_WORD_POS + 3)
					cout << "   " << word.get_ch().substr(0, MAX_DISP_LINE) << endl;
				else
					cout << dispArr[LEARNING][i] << endl;
			}
		}
		if (doNotCh == true) {
			Sleep(3000);
			display(uiType, word, dispTime, false);
		}
		if (dispTime != 0)
			Sleep(dispTime * 1000);
	}

	void study(bool doNotCh = false) {
		char ch;
		bool isOk = false;
		while (true) {
			display(START);
			display(LEARNING, newWlist.get_learning_word(), 0, doNotCh);
			while (true) {
				int forgotRnk = 5;
				if (_kbhit()) {
					ch = _getch();
					ch = _getch();
					// cout << "DEBUG: ch: " << int(ch) << endl;

					// �Ҽ�: ��ʶ����.
					if (ch == 77) {
						forgotRnk = KNOW;
					}
					//// ���: ��һ������.
					//else if (ch == 75) {
					//	newWlist.prev_word();
					//}
					// �¼�: ����ʶ
					else if (ch == 80) {
						forgotRnk = KNOW;

					}
					// �ϼ�: ����
					else if (ch == 72) {
						forgotRnk = FORGOT;
					}
					// ���: ģ��
					else if (ch == 75) {
						forgotRnk = VAGUE;
					}
					else if (ch == 27) {
						return;
					}

					// ���븴ϰ�б�:
					learnedWlist.push(newWlist.get_learning_word());
					isOk = newWlist.next_word();
					if (isOk == true) {
						// ��ɽ����ѧϰ.
						++newWlist.get_today();
						// cout << newWlist.get_today() << endl;
						newWlist.update_totalDays();
						return;
					}
					break;
				}
			}
		}
	}

	void review(bool doNotCh = false) {
		char ch;
		while (true) {
			display(START);
			int curReviewWord = learnedWlist.get_review_word();
			if (curReviewWord == -1) {
				// ��ϰ���:
				// TODO: UI չʾ��ϰ���.
				return;
			}
			display(LEARNING, learnedWlist.lVec[curReviewWord], 0, doNotCh);
			while (true) {
				if (_kbhit()) {
					ch = _getch();
					ch = _getch();
					// cout << "DEBUG: ch: " << int(ch) << endl;

					// �Ҽ�: ��ʶ����.
					if (ch == 77) {
						learnedWlist.lVec[curReviewWord].reduce_rnk(1);
						// cout << learnedWlist.lVec[curReviewWord].get_id() << "  " << learnedWlist.lVec[curReviewWord].get_forgotRnk() << endl;
					}
					//// ���: ��һ������.
					//else if (ch == 75) {
					//	newWlist.prev_word();
					//}
					// �¼�: ����ʶ
					else if (ch == 80) {
						// pass
					}
					curReviewWord = learnedWlist.get_review_word();
					break;
				}
			}
		}
	}

	void modify_plan() {
		char ch;

		while (true) {
			display(START);
			display(LEARNING, Word(0, "= PLEASE INPUT: " + to_string(newWlist.get_everyDayLearn()) + " Words/Day =", "Day " + to_string(newWlist.get_today()) + ", Total " + to_string(newWlist.get_totalDays()) + " days.", "# SELECTED! MODIFY Learned Per Day..."));
			while (true) {
				if (_kbhit()) {
					ch = _getch();
					ch = _getch();
					// cout << "DEBUG: ch: " << int(ch) << endl;

					// �Ҽ�: ����.
					if (ch == 77) {
						++newWlist.get_everyDayLearn();
						newWlist.update_totalDays();
						newWlist.update_begendDays();
					}
					// ���: ����
					else if (ch == 75) {
						if (newWlist.get_everyDayLearn() != 1) {
							--newWlist.get_everyDayLearn();
							newWlist.update_totalDays();
							newWlist.update_begendDays();
						}
					}
					// �س�: ѡ����:
					else if (ch == 13) {
						return;
					}
					break;
				}
			}
		}
	}

	// ��disp�ļ����ݶ��뻺��, ����display��ʱ��ֱ��������漴��.
	void read_disp(string fileName, int dispIdx) {
		FILE* fp;
		fp = fopen(fileName.c_str(), "r");
		char buf[100010] = "";
		int cnt = 0;
		while (!feof(fp)) {
			fgets(buf, 100000, fp);
			string s(buf);
			dispArr[dispIdx][cnt++] = s.substr(0, s.size() - 1);
		}
	}

	void main() {
		// ����"��ѧϰ����"��UI����.
		read_disp(wordDisp, LEARNING);
		// ����"��ӭ����"
		read_disp(startDisp, START);

		char ch;
		string startTips[START_TIPS_NUM] = {
			">> NEW STUDY",
			">> REVIEW   ",
			">> SET PLAN "
		};

		// ���е��ʱ�:
		newWlist = NewWlist(wordListInit);

		bool isFirst = true;
		while (true) {
			// չʾ��ӭ����.
			display(START);

			int curChoose = 0;
			while (true) {
				if (isFirst == false)
					display(LEARNING, Word(0, "== Please use the up and down key. ==", startTips[curChoose % START_TIPS_NUM], "# SELECT MODE #"));
				if (_kbhit()) {
					// �����¼�������, ע����Ҫ��vs���Ļ���������, �������������¼���Ϊ�����ı������Ի��������ַ�.
					ch = _getch();
					// cout << int(ch) << "  " << curChoose << endl;
					// ���¼�ѡ��ģʽ.
					if (ch == 72)
						curChoose--;
					else if (ch == 80)
						curChoose++;

					// �س�ѡ��:
					else if (ch == 13) {
						curChoose %= START_TIPS_NUM;
						break;
					}
					while (curChoose < 0)
						curChoose += START_TIPS_NUM;
					display(LEARNING, Word(0, "== Please use the up and down key. ==", startTips[curChoose % START_TIPS_NUM], "# SELECT MODE #"));
				}
			}
			isFirst = false;

			display(START);

			time_t startTime = time(NULL);
			bool doNotCh = false;
			// ��ʱѡ������Ӧ��ģʽ, ��ת������Ӧ�Ľ���. ����curChooseѡ��.
			switch (curChoose) {
			case 0:
				// ��ʼ�µ�ѧϰ.
				display(START);
				display(LEARNING, Word(0, "== TODAY's Goal: " + to_string(newWlist.get_endToday() - newWlist.get_begToday()) + " Words ==", "Day " + to_string(newWlist.get_today()), "# SELECTED! Hide Chinese, press [H] #"));
				startTime = time(NULL);
				while (time(NULL) - startTime < 9) {
					display(LEARNING, Word(0, "== TODAY's Goal: " + to_string(newWlist.get_endToday() - newWlist.get_begToday()) + " Words ==", "Day " + to_string(newWlist.get_today()) + ", Total " + to_string(newWlist.get_totalDays()) + " days. (" + to_string(9 - (time(NULL) - startTime)) + "s LEFT)", "# SELECTED! Hide Chinese, press [H] #"));
					// cout << int(ch) << "  " << curChoose << endl;
					if (_kbhit()) {
						ch = _getch();
						if (ch == 104) {
							doNotCh = true;
						}
						else if (ch == 13)
							break;
					}
				}
				study(doNotCh);
				break;
			case 1:
				// ��ʼ��ϰ.
				review(doNotCh);
				break;
			case 2:
				// �鿴/�޸�ѧϰ�ƻ�.
				modify_plan();
				break;
			}
		}
	}

private:
	string wordListInit, wordDisp, startDisp; // �ļ���.
	NewWlist newWlist;
	LearnedWlist learnedWlist;
	string dispArr[MAX_DISP_NUM][MAX_DISP_ROW + 10];
};

