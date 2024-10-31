// Fill out your copyright notice in the Description page of Project Settings.


#include "TicTacGameMode.h"

void ATicTacGameMode::MarkCell(int32 CellIndex, EPlayerMark PlayerMark) const
{
	ATicTaGameStateBase* TicTaGameStateBase = GetGameState<ATicTaGameStateBase>();
	TicTaGameStateBase->BoardCells[CellIndex] = PlayerMark;
	OnGameUpdateView.Broadcast(TicTaGameStateBase->BoardCells);
}

void ATicTacGameMode::StartGame(bool bIsvsai)
{
	if (ATicTaGameStateBase* TicTaGameStateBase = GetGameState<ATicTaGameStateBase>())
	{
		TicTaGameStateBase->BoardCells.Init(EPlayerMark::Empty,9);
		TicTaGameStateBase->CurrentPlayer = GetRandomPlayer();
		TicTaGameStateBase->CurrentGameState = EGameState::Play;
		TicTaGameStateBase->bIsPlayWithAI = bIsvsai;
		OnGameBegin.Broadcast();
		if (bIsvsai)
		{
			TicTaGameStateBase->CurrentAiPlayer = GetRandomPlayer();
			if (TicTaGameStateBase->CurrentPlayer == TicTaGameStateBase->CurrentAiPlayer)
			{
				AIMove();
			}
		}
	}
}

void ATicTacGameMode::BackToMenu()
{
	ATicTaGameStateBase* TicTaGameStateBase = GetGameState<ATicTaGameStateBase>();
	TicTaGameStateBase->CurrentGameState = EGameState::Menu;
	OnMenu.Broadcast();
}

EPlayerMark ATicTacGameMode::CheckWin() const
{
	TArray<EPlayerMark>& Board = GetGameState<ATicTaGameStateBase>()->BoardCells;
	const TArray<TArray<int>> winPatterns = {
		{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // 横
		{0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // 竖
		{0, 4, 8}, {2, 4, 6} // 斜
	};

	for (const TArray<int>& pattern : winPatterns)
	{
		EPlayerMark firstMark = Board[pattern[0]];
		if (firstMark != EPlayerMark::Empty && 
			firstMark == Board[pattern[1]] && 
			firstMark == Board[pattern[2]])
		{
			return firstMark; 
		}
	}
	// draw
	bool isBoardFull = true;
	for (const EPlayerMark& cell : Board)
	{
		if (cell == EPlayerMark::Empty)
		{
			isBoardFull = false;
			break;
		}
	}
	if (isBoardFull)
	{
		return EPlayerMark::Draw;
	}
	
	return EPlayerMark::Empty; 
}

EPlayerMark ATicTacGameMode::GetRandomPlayer()
{
	return (FMath::RandBool()) ? EPlayerMark::PlayerX : EPlayerMark::PlayerO;
}

int32 ATicTacGameMode::MinMax(TArray<EPlayerMark>& Board, int Depth, bool IsMaximizing, EPlayerMark AIPlayer, int32 Alpha, int32 Beta)
{
	EPlayerMark Result = CheckWin();

	if (Result != EPlayerMark::Empty)
	{
		return EvaluateBoard(Board);
	}

	if (IsMaximizing)
	{
		int32 BestScore = -1000;

		for (int32 i = 0; i < 9; i++)
		{
			if (Board[i] == EPlayerMark::Empty)
			{
				Board[i] = AIPlayer;

				if (CheckWin() == AIPlayer)
				{
					// 如果这一步能直接赢，立即返回一个极高的分数
					Board[i] = EPlayerMark::Empty;  
					return 1000 - Depth;  // 极高分数，Depth 确保更快的胜利优先
				}

				int32 Score = MinMax(Board, Depth + 1, false, AIPlayer, Alpha, Beta);

				Board[i] = EPlayerMark::Empty;
				BestScore = FMath::Max(Score, BestScore);
				Alpha = FMath::Max(Alpha, BestScore);

				if (Beta <= Alpha)
				{
					break;
				}
			}
		}
		return BestScore;
	}
	else
	{
		int32 BestScore = 1000;
		EPlayerMark OpponentMark = (AIPlayer == EPlayerMark::PlayerX) ? EPlayerMark::PlayerO : EPlayerMark::PlayerX;

		for (int32 i = 0; i < 9; i++)
		{
			if (Board[i] == EPlayerMark::Empty)
			{
				Board[i] = OpponentMark;
				int32 Score = MinMax(Board, Depth + 1, true, AIPlayer, Alpha, Beta);
				Board[i] = EPlayerMark::Empty;

				BestScore = FMath::Min(Score, BestScore);

				Beta = FMath::Min(Beta, BestScore);

				if (Beta <= Alpha)
				{
					break;
				}
			}
		}
		return BestScore;
	}
}

int32 ATicTacGameMode::EvaluateBoard(const TArray<EPlayerMark>& Board) const
{
	EPlayerMark Winner = CheckWin();
	ATicTaGameStateBase* TicTaGameStateBase = GetGameState<ATicTaGameStateBase>();

	if (Winner == TicTaGameStateBase->CurrentPlayer)
		return 10;
	else if (Winner == EPlayerMark::Draw)
		return 0;
	else if (Winner != EPlayerMark::Empty)
		return -10;

	return 0;
}

void ATicTacGameMode::PlayerInput(int32 CellIndex)
{
	ATicTaGameStateBase* TicTaGameStateBase = GetGameState<ATicTaGameStateBase>();
	if (CellIndex < 0 || CellIndex > 8 || TicTaGameStateBase->BoardCells[CellIndex] != EPlayerMark::Empty || TicTaGameStateBase->CurrentGameState != EGameState::Play)
	{
		return;
	}
	MarkCell(CellIndex, TicTaGameStateBase->CurrentPlayer);
	EPlayerMark Result = CheckWin();
	if (Result != EPlayerMark::Empty)
	{
		TicTaGameStateBase->CurrentGameState = EGameState::GameOver;
		OnGameStateChanged.Broadcast(Result);
	}
	else{
		TicTaGameStateBase->CurrentPlayer = (TicTaGameStateBase->CurrentPlayer == EPlayerMark::PlayerX ? EPlayerMark::PlayerO : EPlayerMark::PlayerX);
	}
	if (TicTaGameStateBase->bIsPlayWithAI && TicTaGameStateBase->CurrentPlayer == TicTaGameStateBase->CurrentAiPlayer)
	{
		AIMove();
	}
}

void ATicTacGameMode::AIMove()
{
	ATicTaGameStateBase* TicTaGameStateBase = GetGameState<ATicTaGameStateBase>();
	TArray<EPlayerMark>& Board = TicTaGameStateBase->BoardCells;

	int32 BestScore = -1000;
	TArray<int32> BestMoves;//记录所有最佳得分策略，随机选择一个

	int32 Alpha = -1000;
	int32 Beta = 1000;

	for (int32 i = 0; i < 9; i++)
	{
		if (Board[i] == EPlayerMark::Empty)
		{
			Board[i] = TicTaGameStateBase->CurrentPlayer;
			int32 Score = MinMax(Board, 0, false, TicTaGameStateBase->CurrentPlayer,Alpha,Beta);
			Board[i] = EPlayerMark::Empty;

			if (Score > BestScore)
			{
				BestScore = Score;
				BestMoves.Empty();  
				BestMoves.Add(i);   
			}
			else if (Score == BestScore)
			{
				BestMoves.Add(i);  
			}
		}
	}
	if (BestMoves.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, BestMoves.Num() - 1);
		int32 BestMove = BestMoves[RandomIndex];
		PlayerInput(BestMove);
	}
}


void ATicTacGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetGameState<ATicTaGameStateBase>()->BoardCells.Init(EPlayerMark::Empty,9);
	OnMenu.Broadcast();
}

ATicTacGameMode::ATicTacGameMode()
{

}

