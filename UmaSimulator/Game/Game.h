#pragma once
#include <random>
#include <array>
#include "../GameDatabase/GameDatabase.h"
#include "Person.h"
#include "Action.h"

struct SearchParam;

struct Game
{
  //显示相关
  bool playerPrint;//给人玩的时候，显示更多信息

  //基本状态，不包括当前回合的训练信息
  int32_t umaId;//马娘编号，见KnownUmas.cpp
  bool isLinkUma;//是否为link马
  bool isRacingTurn[TOTAL_TURN];//这回合是否比赛
  int16_t fiveStatusBonus[5];//马娘的五维属性的成长率
  int16_t eventStrength;//每回合有（待测）概率加这么多属性，模拟支援卡事件

  int16_t turn;//回合数，从0开始，到77结束
  int16_t vital;//体力，叫做“vital”是因为游戏里就这样叫的
  int16_t maxVital;//体力上限
  int16_t motivation;//干劲，从1到5分别是绝不调到绝好调

  int16_t fiveStatus[5];//五维属性，1200以上不减半
  int16_t fiveStatusLimit[5];//五维属性上限，1200以上不减半
  int16_t skillPt;//技能点
  int16_t skillScore;//已买技能的分数

  float ptScoreRate;//每pt多少分
  int16_t failureRateBias;//失败率改变量。练习上手=-2，练习下手=2
  //bool isQieZhe;//切者  合并到ptScoreRate了
  bool isAiJiao;//爱娇
  bool isPositiveThinking;//ポジティブ思考，友人第三段出行选上的buff，可以防一次掉心情


  int16_t zhongMaBlueCount[5];//种马的蓝因子个数，假设只有3星
  int16_t zhongMaExtraBonus[6];//种马的剧本因子以及技能白因子（等效成pt），每次继承加多少。全大师杯因子典型值大约是30速30力200pt
  
  int16_t saihou;//赛后加成
  bool isRacing;//这个回合是否在比赛

  Person persons[MAX_HEAD_NUM];//最多9个头。依次是6张卡，理事长6，记者7，没带卡的凉花8（带凉花卡了那就在前6个位置，8号位置就空下了）。
  int16_t personDistribution[5][5];//每个训练有哪些人头id，personDistribution[哪个训练][第几个人头]，空位置为-1
  int lockedTrainingId;//是否锁训练，以及锁在了哪个训练。可以先不加，等ai做完了有时间再加。


  //剧本相关
  int16_t uaf_trainingColor[5];//五种训练的颜色
  int16_t uaf_trainingLevel[3][5];//三种颜色五种训练的等级
  bool uaf_winHistory[5][3][5];//运动会历史战绩
  bool uaf_lastTurnNotTrain;//上回合是否没有训练？如果没有，这回合的等级增加量+3
  int16_t uaf_xiangtanRemain;//还剩几次相谈

  int16_t uaf_buffActivated[3];//蓝红黄的buff已经触发过几次了？记录这个主要是用来识别什么时候应该增加两回合buff，比如假如训练后等级变成370，这时如果buffActivated=6则增加2回合buff并改成7（说明刚激活350级的buff），如果buffActivated=7则不增加buff（说明350级的buff已经激活过）
  int16_t uaf_buffNum[3];//蓝红黄的buff还剩几个？

  //单独处理凉花卡，因为接近必带。其他友人团队卡的以后再考虑
  int16_t lianghua_type;//0没带凉花卡，1 ssr卡，2 r卡
  int16_t lianghua_personId;//凉花卡在persons里的编号
  //int16_t lianghua_stage;//0是未点击，1是已点击但未解锁出行，2是已解锁出行    这次，凉花卡和其他友人的这个全放在Person类里了
  int16_t lianghua_outgoingUsed;//凉花的出行已经走了几段了   暂时不考虑其他友人团队卡的出行
  double lianghua_vitalBonus;//凉花卡的回复量倍数（满破1.60）
  double lianghua_statusBonus;//凉花卡的事件效果倍数（满破1.25）

  bool lianghua_guyouEffective;//凉花固有是否生效，每次randomDistributeCards的时候检查这个，如果ssr凉花羁绊大于等于60且lianghua_guyouEffective=false，则设为true并重置Person.distribution


  //可以通过上面的信息计算获得的非独立的信息，每回合更新一次，不需要录入

  int16_t uaf_trainLevelColorTotal[3];//三种颜色的等级总和
  int16_t uaf_colorWinCount[3];//三种颜色分别累计win过多少次
  int16_t uaf_trainingBonus;//剧本训练加成（取决于三种颜色的win数），每半年更新一次

  int16_t trainValue[5][6];//训练数值的总数（下层+上层），第一个数是第几个训练，第二个数依次是速耐力根智pt
  int16_t trainVitalChange[5];//训练后的体力变化（负的体力消耗）
  int16_t failRate[5];//训练失败率
  int16_t uaf_trainLevelGain[5];//五个训练分别加多少训练等级（不是总数，只看当前训练头顶的数字）,不考虑100级溢出
  int16_t trainShiningNum[5];//每个训练有几个彩圈

  //训练数值计算的中间变量，存下来方便手写逻辑对相谈后属性进行估计
  bool uaf_haveLose;//uaf大会是否已经输过（输过一次就不需要凑全win了，最后必定少全属性25）
  bool uaf_haveLoseColor[3];//uaf大会每种颜色是否已经输过（输过一次这种颜色就不需要凑全win了，训练亏定了）
  int16_t trainValueLower[5][6];//训练数值的下层，第一个数是第几个训练，第二个数依次是速耐力根智pt体力
  double trainValueCardMultiplier[5];//支援卡乘区=(1+总训练加成)(1+干劲系数*(1+总干劲加成))(1+0.05*总卡数)(1+友情1)(1+友情2)...

  bool cardEffectCalculated;//支援卡效果是否已经计算过？相谈后不需要重新计算，分配卡组或者读json时需要置为false
  CardTrainingEffect cardEffects[6];




  //游戏流程相关------------------------------------------------------------------------------------------

public:

  void newGame(std::mt19937_64& rand,
    bool enablePlayerPrint,
    int newUmaId,
    int umaStars,
    int newCards[6],
    int newZhongMaBlueCount[5],
    int newZhongMaExtraBonus[6]);//重置游戏，开局。umaId是马娘编号


  //这个操作是否允许且合理
  //不允许的包括：本次需要相谈次数大于剩余相谈次数，在前十几个以及ura期间的三个回合比赛。
  //不合理的包括：相谈了不训练，把不存在的颜色相谈成其他颜色，选择的训练不是相谈的原色和目标色
  bool isLegal(Action action) const;

  //进行Action后一直往后进行，直到下一次需要玩家决策（跳过比赛回合）。如果回合数>=78则什么都不做直接return（但不要报错或者闪退）
  void applyTrainingAndNextTurn(
    std::mt19937_64& rand,
    Action action);

  int finalScore() const;//最终总分
  bool isEnd() const;//是否已经终局



  //原则上这几个private就行，如果private在某些地方非常不方便那就改成public
  void randomDistributeCards(std::mt19937_64& rand);//随机分配人头
  void calculateTrainingValue();//计算所有训练分别加多少，并计算失败率、训练等级提升等
  bool applyTraining(std::mt19937_64& rand, Action action);//处理 训练/出行/比赛 本身，包括友人点击事件，不包括固定事件和剧本事件。如果不合法，则返回false，且保证不做任何修改
  void checkEventAfterTrain(std::mt19937_64& rand);//检查固定事件和随机事件，并进入下一个回合

  void checkFixedEvents(std::mt19937_64& rand);//每回合的固定事件，包括剧本事件和固定比赛和部分马娘事件等
  void checkRandomEvents(std::mt19937_64& rand);//模拟支援卡事件和随机马娘事件（随机加羁绊，体力，心情，掉心情等）

  //常用接口-----------------------------------------------------------------------------------------------

  bool loadGameFromJson(std::string jsonStr);

  //神经网络输入，初版不需要实现
  void getNNInputV1(float* buf, const SearchParam& param) const;

  void print() const;//用彩色字体显示游戏内容
  void printFinalStats() const;//显示最终结果




  //各种辅助函数与接口，可以根据需要增加或者删减-------------------------------------------------------------------------------
  static inline int convertTrainingLevel(int x) //转换后的训练等级从0开始，0是lv1，4是lv5
  {
    return x < 20 ? 0 : x < 30 ? 1 : x < 40 ? 2 : x < 50 ? 3 : 4;
  }
  inline bool isXiahesu() const //是否为夏合宿
  {
    return (turn >= 36 && turn <= 39) || (turn >= 60 && turn <= 63);
  }
  int uaf_competitionFinishedNum() const;//已经几次uaf大会了
  bool isXiangtanLegal(int x) const;//此相谈是否合法且有意义
  void xiangtanAndRecalculate(int x, bool forHandwrittenLogic);//相谈，并重新计算属性值，如果forHandwrittenLogic则是给手写逻辑用的，可以略微不准确
  void runRace(int basicFiveStatusBonus, int basicPtBonus);//把比赛奖励加到属性和pt上，输入是不计赛后加成的基础值


  int calculateRealStatusGain(int idx, int value) const;//考虑1200以上为2的倍数的实际属性增加值

  void addStatus(int idx, int value);//增加属性值，并处理溢出
  void addAllStatus(int value);//同时增加五个属性值
  void addVital(int value);//增加或减少体力，并处理溢出
  void addMotivation(int value);//增加或减少心情，同时考虑“isPositiveThinking”
  void addJiBan(int idx,int value);//增加羁绊，并考虑爱娇

  void addStatusFriend(int idx, int value);//友人卡事件，增加属性值或者pt（idx=5），考虑事件加成
  void addVitalFriend(int value);//友人卡事件，增加体力，考虑回复量加成

  void uaf_checkNewBuffAfterLevelGain();//训练或者友人出行后，检查是否有新的蓝红黄buff
  void uaf_runCompetition(int n);//第n次uaf大会

  float getSkillScore() const;//技能分，输入神经网络之前也可能提前减去
  int getTrainingLevel(int trainIdx) const;//计算训练等级，1~19,20~21,...50~100
  bool isCardShining(int personIdx, int trainIdx) const;    // 判断指定卡是否闪彩。普通卡看羁绊与所在训练，团队卡看friendOrGroupCardStage
  //bool trainShiningCount(int trainIdx) const;    // 指定训练彩圈数 //uaf不一定有用
  int calculateFailureRate(int trainType, double failRateMultiply) const;//计算训练失败率，failRateMultiply是训练失败率乘数=(1-支援卡1的失败率下降)*(1-支援卡2的失败率下降)*...
  //void calculateTrainingValueSingle(int trainType);//计算每个训练加多少   uaf剧本可能五个训练一起算比较方便


  //友人卡相关事件
  void handleFriendUnlock(std::mt19937_64& rand);//友人外出解锁
  void handleFriendOutgoing(std::mt19937_64& rand);//友人外出
  void handleFriendClickEvent(std::mt19937_64& rand, int atTrain);//友人事件（お疲れ様）
  void handleFriendFixedEvent();//友人固定事件，拜年+结算
  void checkLianghuaGuyou();//读入json或每次randomDistributeCards的时候检查这个，如果ssr凉花羁绊大于等于60且lianghua_guyouEffective=false，则设为true并重新构造Person.distribution使得凉花固有生效

  //显示事件
  void printEvents(std::string s) const;//用绿色字体显示事件

};

