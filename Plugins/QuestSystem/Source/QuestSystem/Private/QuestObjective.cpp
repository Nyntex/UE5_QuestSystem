// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestObjective.h"
#include "QuestObject.h"
#include "QuestReward.h"


UQuestObjective::UQuestObjective()
{
}

void UQuestObjective::BroadcastProgress(UObject* AddedProgress)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::BroadcastProgress)
	OnProgressUpdatedDelegate.Broadcast(AddedProgress);
}

bool UQuestObjective::TryStartObjective_Implementation(UQuestObject* Quest)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::TryStartObjective_Implementation)
	StartObjective(Quest);
	return Status == EQuestStatus::IN_PROGRESS;
}

UQuestObject* UQuestObjective::GetOwningQuestObject() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::GetOwningQuestObject)
	return Cast<UQuestObject>(GetOuter());
}

void UQuestObjective::ClaimRewards()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::ClaimRewards)
	if (Status != EQuestStatus::COMPLETED) return;
	
	for (UQuestReward* Reward : ObjectiveRewards)
	{
		Reward->ClaimReward();
	}
}

AController* UQuestObjective::GetController() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::GetController)
	return GetOwningQuestObject()->QuestOwningController;
}

void UQuestObjective::TickObjective_Implementation(UQuestObject* Quest, float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::TickObjective)
}

void UQuestObjective::StartObjective_Implementation(UQuestObject* Quest)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::StartObjective_Implementation)
	if (ShouldTick)
	{
		GetOwningQuestObject()->OnQuestTickDelegate.AddDynamic(this, &UQuestObjective::TickObjective);
	}
	Status = EQuestStatus::IN_PROGRESS;
}

void UQuestObjective::ForceStatus_Implementation(EQuestStatus NewStatus)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::ForceStatus_Implementation)
	UpdateStatus(NewStatus);
}

void UQuestObjective::UpdateStatus_Implementation(EQuestStatus NewStatus)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::UpdateStatus_Implementation)
	EQuestStatus OldStatus = Status;
	Status = NewStatus;
	OnObjectiveStatusUpdatedDelegate.Broadcast(this, Status, OldStatus);
}

void UQuestObjective::Initialize_Implementation(UQuestObject* OwningQuest)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::Initialize_Implementation)
	for (auto Reward : ObjectiveRewards)
	{
		Reward->OwningQuest = GetOwningQuestObject();
	}
}

void UQuestObjective::AddProgress_Implementation(UQuestProgressionObject* Progress)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::AddProgress_Implementation)
}

FString UQuestObjective::GetObjectiveDescription_Implementation() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObjective::GetObjectiveDescription_Implementation)
	return FString::Printf(TEXT("Basic Quest Modifier"));
}
