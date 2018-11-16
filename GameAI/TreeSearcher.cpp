#include <cmath>
#include <array>
#include <vector>
#include "def.h"
#include "Board.h"
#include "FileReader.h"
#include "SystemKeeper.h"
#include "TreeSearcher.h"
#include "GameManager.h"
#include "Team.h"

//-----------------ハイパーパラメータ-----------------------

//ノード展開の閾値
//下げるとノードの生成量が増えるが探索が分散する
//上げると精度は高くなるが探索域が狭くなる
#define THRESHOLD 10

//Rolloutの繰り返し回数
//下げると処理時間は短くなるが精度が下がる
//上げると精度が上がるが処理時間が長くなる
#define SEARCHNUM 10000

//探索評価バランス
//下げるとRollout結果優先（探索の分散）
//上げると探索期待値優先（探索の集中）
#define COSTPARAM 22.0

//先読みターン数
//下げると遠い先を読めなくなるが収束しやすくなる
//上げると先読みしやすくなるが収束が遅くなる
#define MAXDEPTH 15

//最終的な手の決定方法
// MAX_WITH_RESULT :最終的な評価値で手を決定
// MAX_WITH_COUNT :最終的な探索回数で手を決定(UCB値に依存)
#define MAXHANDMETHOD MAX_WITH_RESULT

//単純なAIを使うターン数
#define EASYTURN 0

//-----------------------------------------------------------

//コンストラクタ
TreeSearcher::TreeSearcher() {

	Advantage = 0;

}

//メモリのお掃除
void TreeSearcher::CleanNode(State *root) {

	root->UpdateInfo();

	//「親」と「親の親 (=Tree)」はRollOutLogのみ残す
	for (auto itr = Tree->Child.begin(); itr != Tree->Child.end(); itr++) {

		if ((*itr) == root->Parent) {

			//親ならば
			for (auto itr2 = (*itr)->Child.begin(); itr2 != (*itr)->Child.end(); itr2++) {
				if ((*itr2) != root) {

					//rootでなければ
					if (*itr2 != nullptr) {
						(*itr2)->DeleteRolloutLog();
						delete *itr2;
						*itr2 = nullptr;
					}

				}
			}

			if (*itr != nullptr) {
				(*itr)->RolloutLog.clear();
				(*itr)->Child.clear();
				delete *itr;
				*itr = nullptr;
			}

		}
		else {

			//親でなければ
			//子をすべて消去する
			(*itr)->DeleteChildNode();
			if (*itr != nullptr) {
				(*itr)->DeleteRolloutLog();
				delete *itr;
				*itr = nullptr;
			}

		}
	}

	if (Tree != nullptr) {
		Tree->RolloutLog.clear();
		Tree->Child.clear();
		delete Tree;
		Tree = nullptr;
	}

	root->Parent = nullptr;

	//過ぎたターンのログをPop
	for (auto itr = root->RolloutLog.begin(); itr != root->RolloutLog.end(); itr++) {
		if (!(*itr)->Hand.empty()) (*itr)->Hand.erase((*itr)->Hand.begin());
	}

	root->UpdateDepth();

}

//ルートノードの更新、メモリリーク防止
void TreeSearcher::MoveUpdate(const array< array<int, AGENTNUM>, TEAMNUM> &move) {

	State* newroot = FindActNode(move);

	if (newroot == nullptr) {
		//ツリー要素のメモリ解放
		if (Tree != nullptr) {
			delete Tree;
			Tree = nullptr;
		}
		return;
	}

	//不要になったノードの削除
	CleanNode(newroot);

	//根ノードの更新
	Tree = newroot;

}

//行動moveに該当するノードを探す
State* TreeSearcher::FindActNode(const array< array<int, AGENTNUM>, TEAMNUM> &move) {

	State* tmp = NULL;

	if (Tree == nullptr) return NULL;

	//行動に沿ったノードを探索
	for (auto itr = Tree->Child.begin(); itr != Tree->Child.end(); itr++) {
		if ((*itr)->VAction == move[RED]) {

			tmp = (*itr);
			for (auto itr2 = tmp->Child.begin(); itr2 != tmp->Child.end(); itr2++) {
				if ((*itr2)->VAction == move[BLUE]) return (*itr2);
			}
			return NULL;

		}
	}

	return NULL;

}

//有効と思われる手を返す（モンテカルロ木探索）
/*
board		:現在の盤面
us			:自分のチーム
enemy		:相手のチーム
leaveturn	:ゲーム終了までの残りターン数（最終ターン数 - 現在のターン数 + 1）
*/
array<int, AGENTNUM> TreeSearcher::SearchNextAction(const Board &board, const Team &us, const Team &enemy, const int &leaveturn) {

	//最初5ターンはEasyAISoft
	if (GameManager::System->GetNowTurn() <= EASYTURN) return us.GetEasyAISoft(board, enemy);

	//根ノード（深さ0）を生成
	if (Tree == nullptr) {
		Tree = new State(
			NULL,								//親ノードは無し
			board,								//現在の状態の盤面を渡す
			array<int, AGENTNUM>{NONE, NONE},	//行動は無し（なにもboardやteamに適用しない）
			array<Team, TEAMNUM>{Team(us), Team(enemy)},	//自分と相手をチームとして渡す
			0,									//根ノードなので深さは0
			NEXTCOLOR(us.MyColor),				//深さ1を自分の色にするので、自分の色ではない方を選択
			vector<GameLog*>(0)					//ロールアウトなし
		);
	}

	//selectionにより選択されたノード
	State* select = NULL;

	//SEARCHNUM回繰り返す
	for (int s = 0; s < FileReader::GetRolloutNum(); s++) {

		//制御関数に従って葉ノードを選択する
		select = Tree->Selection();

		//Rolloutによるノードの評価
		select->Evaluation(leaveturn);

		//回数の更新、閾値を超えていれば拡張
		select->Expansion(leaveturn);

		//結果を上層に反映する
		select->Backup();

	}

	Advantage = -1 * Tree->Qvalue;

	//最終的な手の選択
	if(MAXHANDMETHOD == MAX_WITH_RESULT)	return Tree->MaxResultHand();
	else									return Tree->MaxCountHand();

}

//ツリーのリセット（局所収束対策）
void TreeSearcher::ResetTree() {

	if (Tree == nullptr) return;

	Tree->DeleteChildNode();
	Tree = nullptr;

}

/*

	【メモ：ゲームツリー構造について】
	・深さ0は根ノード、現在の状況とする
	・深さが奇数のノード(1, 3, 5,...)は自分が行動をした後の状況とする
		VBoard	: 行動前の盤面。1つ前のノードのVBoardにactを適用させる。
		VTeam	: 行動前のエージェント。添字は[RED], [BLUE]で格納する。次のノードに渡す際に順番を変えたりはしない。
		VAction	: 自分のチームの行動
		Color	: 自分のチームの色（RED, BLUE）
		Depth	: 深さ。これが奇数か偶数かによって、ノードが自分か相手かわかる。

	・深さが偶数のノード(2, 4, 6,...)は相手が行動をした後の状況とする
		VBoard	: 行動前の盤面。2つ前のノードのVBoardにactを適用させる。
		VTeam	: 行動前のエージェント。添字は[RED], [BLUE]で格納する。次のノードに渡す際に順番を変えたりはしない。
		VAction	: 相手のチームの行動
		Color	: 相手のチームの色（RED, BLUE）
		Depth	: 深さ。これが奇数か偶数かによって、ノードが自分か相手かわかる。

*/

//ノード：コンストラクタ
/*
	parent	:親要素のポインタ
	board	:act適用前の盤面
	act		:この要素で行われる行動
	team	:act適用前のチーム
	deep	:この要素の深さ（根を0とする）
	color	:ノードのチーム

	※注意 :仮引数board, agentは行動actを適用する前のオブジェクト
*/
State::State(
	State* parent,
	const Board &board,
	const array<int, AGENTNUM> &act,
	const array<Team, TEAMNUM> &team,
	const int &deep,
	const int &color,
	const vector<GameLog*> &log
) :
	Depth(deep),
	Color(color),
	VBoard(board),
	VAction(act),
	VTeam(team),
	Parent(parent),
	RolloutLog(log)
{
	Count = (int)RolloutLog.size();
	WinFromLog();
	CalcUCB();
}

//ノード：デストラクタ
State::~State() {

	RolloutLog.clear();
	Child.clear();

}

//ノード：行動選択
State* State::Selection() {

	State* max = NULL;		//移動先の子ノード
	double vm = -DBL_MAX;	//探索期待値の最大点

							//ツリーの末端要素であるかを判定
	if (Child.empty() == true) return this;

	//期待報酬Q(s,a) + コストC(s,a) が最大の子要素のアドレスをmaxに格納
	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if ((*itr)->UCB > vm) {
			vm = (*itr)->UCB;
			max = (*itr);
		}
	}

	//子要素で再起処理
	return max->Selection();

}

//ノード：条件に応じて拡張
void State::Expansion(const int &leaveturn) {

	//閾値に達しているか、既に拡張されているかのチェック
	if (!Child.empty() || !(Count > THRESHOLD*(Depth + 1))) return;
	if (leaveturn * 2 - 1 - Depth <= 0) return;

	//閾値に達していてかつ、拡張されていない場合拡張する
	GenerateChild();

}

//Rollout試行回数をカウント
void State::UpdateCount() {

	Count++;

	if (Parent != nullptr) Parent->UpdateCount();

}

//ノード：子ノードの拡張
void State::GenerateChild() {

	int i, j;
	array<Team, TEAMNUM> copyteam = VTeam;
	Board copyboard = VBoard;
	vector<Agent*> VAgentsArray{
		&(copyteam[RED].Agents[AGENT1]),
		&(copyteam[RED].Agents[AGENT2]),
		&(copyteam[BLUE].Agents[AGENT1]),
		&(copyteam[BLUE].Agents[AGENT2])
	};

	//次が自分のノードのとき
	if (Parent != nullptr && Color == BLUE) {

		//盤面更新
		copyteam[RED].SetAgentAction(Parent->VAction);
		copyteam[BLUE].SetAgentAction(VAction);
		SystemKeeper::CheckConflict(copyboard, VAgentsArray);
		copyboard.Update(VAgentsArray);

		//エージェント更新
		copyteam[RED].Update();
		copyteam[BLUE].Update();

	}

	auto iscan = SystemKeeper::GetCanActionList(copyboard, copyteam[NEXTCOLOR(Color)]);
	auto LogList = DevideRolloutLog();

	//現在の状態から行動可能なすべての要素を生成する
	for (i = 0; i < ACTIONNUM; i++) {
		for (j = 0; j < ACTIONNUM; j++) {

			if (iscan[i][j] == false) continue;

			if (Color == RED) {
				//次が相手のノードのとき
				Child.push_back(new State(
					this,
					VBoard,
					array<int, 2>{i + 1, j + 1},
					VTeam,
					Depth + 1,
					BLUE,
					LogList[i][j]
				));
			}
			else if (Color == BLUE) {
				//次が自分のノードのとき
				Child.push_back(new State(
					this,
					copyboard,
					array<int, 2>{i + 1, j + 1},
					copyteam,
					Depth + 1,
					RED,
					LogList[i][j]
				));
			}

		}
	}

	//Rolloutのログの削除
	RolloutLog.clear();

}

//ノード：評価値（勝率）の算出
void State::Evaluation(const int &leaveturn) {

	//訪れた回数の更新
	UpdateCount();

	//1回も訪れていないときは、勝率を0にする
	if (Count == 0) {
		ResultSum = 0;
		Qvalue = 0;
		return;
	}

	//Rolloutの結果を反映
	double ro = Rollout(leaveturn);

	//if (Color == RED)	ResultSum = ((Result * (Count - 1)) + ro) / Count;
	if (Color == RED)	ResultSum += ro;
	else				ResultSum -= ro;
	Qvalue = ResultSum / Count;
	

}

//ノード：Rolloutによる結果の予測をし、勝ったらtrue、引き分け又は負けたらfalseを返す
int State::Rollout(const int &leaveturn) {

	array<Team, TEAMNUM> VirtualTeam = VTeam;
	Board VirtualBoard = VBoard;
	SystemKeeper VirtualSystem((leaveturn > FileReader::GetSearchDeep()) ? FileReader::GetSearchDeep() : leaveturn);
	int Winner = -1;
	GameLog* Log = new GameLog;
	vector<Agent*> agents = { &(VirtualTeam[RED].Agents[AGENT1]), &(VirtualTeam[RED].Agents[AGENT2]), &(VirtualTeam[BLUE].Agents[AGENT1]), &(VirtualTeam[BLUE].Agents[AGENT2]) };

	VirtualBoard.Update(agents);
	VirtualSystem.CalcScore(VirtualBoard);

	//初ターンの処理
	if (Parent != nullptr) {

		//エージェントの思考
		//MYTEAMはVAction
		if (Color == RED) {
			VirtualTeam[RED].SetAgentAction(VAction);
			VirtualTeam[BLUE].EasyAI(VirtualBoard, VirtualTeam[RED]);
		}
		else {
			VirtualTeam[RED].SetAgentAction(Parent->VAction);
			VirtualTeam[BLUE].SetAgentAction(VAction);
		}

		//衝突確認
		SystemKeeper::CheckConflict(VirtualBoard, agents);

		//ログを記録
		Log->Hand.push_back(array< array<int, AGENTNUM>, TEAMNUM>{VirtualTeam[RED].GetAgentAction(), VirtualTeam[BLUE].GetAgentAction()});

		//ボード更新
		VirtualBoard.Update(agents);

		//エージェント更新
		VirtualTeam[RED].Update();
		VirtualTeam[BLUE].Update();

		//スコア計算
		VirtualSystem.CalcScore(VirtualBoard);

		//ターン更新
		VirtualSystem.Update();

	}

	//ゲーム通りに進める
	while (true) {

		//エージェントの思考
		VirtualTeam[RED].EasyAI(VirtualBoard, VirtualTeam[BLUE]);
		VirtualTeam[BLUE].EasyAI(VirtualBoard, VirtualTeam[RED]);

		//衝突確認
		SystemKeeper::CheckConflict(VirtualBoard, agents);

		//ログを記録
		Log->Hand.push_back(array< array<int, AGENTNUM>, TEAMNUM>{VirtualTeam[RED].GetAgentAction(), VirtualTeam[BLUE].GetAgentAction()});

		//ボード更新
		VirtualBoard.Update(agents);

		//エージェント更新
		VirtualTeam[RED].Update();
		VirtualTeam[BLUE].Update();

		//スコア計算
		VirtualSystem.CalcScore(VirtualBoard);

		if (VirtualSystem.isGameFin() == true) break;

		//ターン更新
		VirtualSystem.Update();

	}



	//勝敗決定
	Winner = VirtualSystem.JudgeWinner();

	//ログを記録
	//Log->Winner = Winner;
	Log->Winner = VirtualSystem.GetScore(RED_TOTAL) - VirtualSystem.GetScore(BLUE_TOTAL);
	RolloutLog.push_back(Log);

	//結果を返す
	return Log->Winner;

}

//ノード：バックプロパゲーション
void State::Backup() {

	//葉ノード以外は子ノードの全ての評価値を足したものを自身の評価値（勝率）とする
	if (!Child.empty()) {

		ResultSum = 0;
		Qvalue = 0;
		for (auto itr = Child.begin(); itr != Child.end(); itr++) {
			ResultSum -= (*itr)->ResultSum;
		}
		Qvalue = ResultSum / Count;

	}

	//子ノードのUCB値を再計算
	for (auto itr = Child.begin(); itr != Child.end(); itr++) (*itr)->CalcUCB();

	//親ノードで再起
	if (Parent != nullptr) Parent->Backup();

}

//ノード：UCB（探索期待値）を計算
void State::CalcUCB() {

	//1回も訪れていなければ、探索期待値を最大にする
	if (Count == 0) {
		UCB = DBL_MAX;
		return;
	}

	//UCBの式に従って求める
	UCB = (Qvalue/FileReader::GetCostParam()) + (sqrt(2 * log(Parent->Count) / (double)Count));

}

//ノード：子ノードの中で最も探索回数が多い手を返す
array<int, AGENTNUM> State::MaxCountHand() {

	int c = INT_MIN;	//探索回数の最大数
	State* s = NULL;	//探索回数が最大の要素

	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if (c <= (*itr)->Count) {
			c = (*itr)->Count;
			s = (*itr);
		}
	}

	return s->VAction;

}

//ノード：子ノードの中で最も平均スコアが高い手を返す
array<int, AGENTNUM> State::MaxResultHand() {

	double c = INT_MIN;	//平均スコアの最大数
	State* s = NULL;	//平均スコアが最大の要素

	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if (c <= (*itr)->Qvalue) {
			c = (*itr)->Qvalue;
			s = (*itr);
		}
	}

	return s->VAction;

}

//ノード：ログを行動ごとに分割して返す
array< array< vector<GameLog*>, ACTIONNUM>, ACTIONNUM > State::DevideRolloutLog() {

	array< array< vector<GameLog*>, ACTIONNUM>, ACTIONNUM > ans;
	int Aturn = (Depth / 2);
	array<int, AGENTNUM> tmp;

	for (auto itr = RolloutLog.begin(); itr != RolloutLog.end(); itr++) {
		tmp = (*itr)->Hand[Aturn][NEXTCOLOR(Color)];	//!!!ターン数を考量しなければならない depth/2
		ans[tmp[AGENT1] - 1][tmp[AGENT2] - 1].push_back(*itr);
	}

	return ans;

}

//渡されたログの配列からスコアを算出
void State::WinFromLog() {

	Qvalue = 0;
	ResultSum = 0;
	for (auto itr = RolloutLog.begin(); itr != RolloutLog.end(); itr++) {
		if (Color == RED)	ResultSum += (*itr)->Winner;
		else				ResultSum -= (*itr)->Winner;
	}

	if (Count == 0) return;
	Qvalue /= (double)Count;

}

//持っている全てのログを削除
void State::DeleteRolloutLog() {

	for (auto itr = RolloutLog.begin(); itr != RolloutLog.end(); itr++) {
		if (*itr != nullptr) {
			delete *itr;
			*itr = nullptr;
		}
	}

	RolloutLog.clear();

}

//全ての子ノードを削除
void State::DeleteChildNode() {

	if (Child.empty()) return;

	for (auto itr = Child.begin(); itr != Child.end(); itr++) {
		if (*itr != nullptr) {

			//再起
			(*itr)->DeleteChildNode();

			//全てのRolloutログを消した後、ノードを削除
			(*itr)->DeleteRolloutLog();
			delete *itr;
			*itr = nullptr;

		}
	}

	Child.clear();

}

//ノードの深さを2上げる
void State::UpdateDepth() {

	if (!Child.empty()) {
		for (auto itr = Child.begin(); itr != Child.end(); itr++) (*itr)->UpdateDepth();
	}

	Depth -= 2;

	if (Depth < 0) exit(-1);

}

void State::UpdateInfo() {

	vector<Agent*> Agents = {
		&(VTeam[RED].Agents[AGENT1]),
		&(VTeam[RED].Agents[AGENT2]),
		&(VTeam[BLUE].Agents[AGENT1]),
		&(VTeam[BLUE].Agents[AGENT2])
	};

	//盤面更新
	VTeam[RED].SetAgentAction(Parent->VAction);
	VTeam[BLUE].SetAgentAction(VAction);
	SystemKeeper::CheckConflict(VBoard, Agents);
	VBoard.Update(Agents);

	//エージェント更新
	VTeam[RED].Update();
	VTeam[BLUE].Update();

}
