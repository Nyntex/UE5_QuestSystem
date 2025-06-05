// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EQuestStatus : uint8
{
	INVALID,
	LOCKED,
	UNLOCKED,
	ACCEPTED,
	STARTING,
	IN_PROGRESS,
	COMPLETED,
	FAILED,
};

UENUM(BlueprintType)
enum class EQuestEnterCommand : uint8
{
	UNLOCK,
	ACCEPT,
	INITIALIZE,
	START,
};