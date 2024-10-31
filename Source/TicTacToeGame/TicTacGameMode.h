// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TicTaGameStateBase.h"
#include "GameFramework/GameModeBase.h"
#include "TicTacGameMode.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMenu);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChanged, EPlayerMark, Winner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameUpdateView, const TArray<EPlayerMark>&, BoardCells);


UCLASS()
class TICTACTOEGAME_API ATicTacGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	
	ATicTacGameMode();
	UFUNCTION(BlueprintCallable,category="TicTacToeGame")
	void PlayerInput(int32 CellIndex);

	UFUNCTION(BlueprintCallable,category="TicTacToeGame")
	void StartGame(bool bIsvsai);

	UFUNCTION(BlueprintCallable, category = "TicTacToeGame")
	void BackToMenu();


	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnMenu OnMenu;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnGameBegin OnGameBegin;

	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnGameStateChanged OnGameStateChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Game Events")
	FOnGameUpdateView OnGameUpdateView;



protected:
	virtual void BeginPlay() override;
private:
	void MarkCell(int32 CellIndex, EPlayerMark PlayerMark) const;

	EPlayerMark CheckWin() const;

	static EPlayerMark GetRandomPlayer();

	void AIMove();

	int32 MinMax(TArray<EPlayerMark>& Board, int Depth, bool IsMaximizing, EPlayerMark AIPlayer, int32 Alpha, int32 Beta);

	int32 EvaluateBoard(const TArray<EPlayerMark>& Board) const;
};
