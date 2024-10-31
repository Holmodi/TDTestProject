// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TicTaGameStateBase.generated.h"


UENUM(BlueprintType)
enum class EGameState : uint8
{
	Menu,
	Play,
	GameOver
};

UENUM(BlueprintType)
enum class EPlayerMark : uint8
{
	Empty   UMETA(DisplayName = "Empty"),
	PlayerX UMETA(DisplayName = "Player X"),
	PlayerO UMETA(DisplayName = "Player O"),
	Draw UMETA(DisplayName = "Draw")
};

/**
 * 
 */
UCLASS()
class TICTACTOEGAME_API ATicTaGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	
	TArray<EPlayerMark> BoardCells;

	EPlayerMark CurrentPlayer;
	EPlayerMark CurrentAiPlayer;

	EGameState CurrentGameState;

	bool bIsPlayWithAI;
	
};
