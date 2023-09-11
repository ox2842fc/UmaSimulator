#include <iostream>
#include <iomanip> 
#include <sstream>
#include <fstream>
#include <cassert>
#include <thread>  // for std::this_thread::sleep_for
#include <chrono>  // for std::chrono::seconds

#include "../Game/Game.h"
#include "../GameDatabase/GameConfig.h"
#include "../Search/Search.h"
#include "windows.h"
#include <filesystem>
#include <cstdlib>
using namespace std;

inline int64_t now_ms()
{
  auto dur = std::chrono::steady_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
}

//benchmark
void main_benchmark()
{
  GameDatabase::loadUmas("../db/uma");
  GameDatabase::loadCards("../db/card");

  const int threadNum = 1;
  const double radicalFactor = 5;//������
  const int searchN = 2048;


  Search search;
  vector<Evaluator> evaluators;
  for (int i = 0; i < threadNum; i++)
    evaluators.push_back(Evaluator(NULL, 128));






  int umaId = 101101;//���Ϸ�
  int cards[6] = { 301374,301344,300104,300194,300114,301074 };//���ţ��߷壬���������������˾��

  int zhongmaBlue[5] = { 18,0,0,0,0 };
  int zhongmaBonus[6] = { 20,0,40,0,20,200 };

  auto rand = mt19937_64(114514);
  Game game;
  game.newGame(rand, false, umaId, cards, zhongmaBlue, zhongmaBonus);
  //game.print();

  const int64_t time0 = now_ms();
  search.runSearch(game, evaluators.data(), searchN, TOTAL_TURN, 27000, threadNum, radicalFactor);
  const int64_t time1 = now_ms();

  int64_t timeUsedMs = time1 - time0;
  float speed = 1000 * searchN / float(timeUsedMs);
  cout << "����������" << searchN << "����ʱ" << timeUsedMs << "ms��ƽ���ٶ�" << speed << "��ÿ�룬�߳���=" << threadNum << endl;
  cout << "��������˳�...";
  cin.get();
}