// Protected under GPL-3.0 License.


#include "QuestObject.h"
#include "Kismet/GameplayStatics.h"
#include "QuestProgressionObject.h"
#include "QuestReward.h"

UQuestObject::UQuestObject()
{
}

bool UQuestObject::Initialize_Implementation()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::Initialize);

	if (QuestStatus != EQuestStatus::ACCEPTED) return false;
	
	for (UQuestObjective* Objective : QuestObjectives)
	{
		Objective->Initialize(this);
	}

	for (auto Reward : QuestRewards)
	{
		Reward->OwningQuest = this;
	}
	
	QuestStatus = EQuestStatus::STARTING;

	return true;
}

void UQuestObject::ProgressQuest_Implementation(UQuestProgressionObject* Progress)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::ProgressQuest);
	if (QuestStatus != EQuestStatus::IN_PROGRESS) return;
	if (!Progress) return;
	
	bool Consumed = false;
	for (UQuestObjective* Objective : QuestObjectives)
	{
		if (Objective->GetClass() == Progress->ObjectiveToProgress)
		{
			Objective->AddProgress(Progress, Consumed);
			OnQuestProgressUpdatedDelegate.Broadcast(this, QuestObjectives, Progress);
			if (Consumed) break;
		}
	}

	Progress->ConditionalBeginDestroy(); //now we don't need it anymore
	
	TryFinishQuest();
}

void UQuestObject::ClaimRewards()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::ClaimRewards);
	if (QuestStatus != EQuestStatus::COMPLETED) return;
	for (UQuestObjective* Objective : QuestObjectives)
	{
		Objective->ClaimRewards();
	}

	for (auto Reward : QuestRewards)
	{
		Reward->ClaimReward();
	}
}

bool UQuestObject::AcceptQuest_Implementation()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::AcceptQuest_Implementation);
	if (QuestStatus != EQuestStatus::UNLOCKED) return false;
	
	QuestStatus = EQuestStatus::ACCEPTED;
	for (UQuestObjective* Objective : QuestObjectives)
	{
		Objective->UpdateStatus(EQuestStatus::ACCEPTED);
	}

	return true;
}

EQuestStatus UQuestObject::TryFinishQuest()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::TryFinishQuest);
	
	EQuestStatus FinishStatus = EQuestStatus::COMPLETED;
	for (UQuestObjective* Objective : QuestObjectives)
	{
		switch (Objective->Status)
		{
		case EQuestStatus::COMPLETED:
			break;
		case EQuestStatus::FAILED:
			if (!Objective->FailingObjectiveFailsQuest) continue;
			FinishStatus = EQuestStatus::FAILED;
			break;
		default:
			FinishStatus = EQuestStatus::INVALID;
			break;
		}
	}

	switch (FinishStatus)
	{		
	case EQuestStatus::COMPLETED:
	case EQuestStatus::FAILED:
		QuestFinished(FinishStatus);
	default:
		break;
	}

	return FinishStatus;
}

AController* UQuestObject::GetOwningController_Implementation()
{
	return UGameplayStatics::GetPlayerController(this, 0);
}

TArray<FString> UQuestObject::GetQuestModifierDescriptions() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::GetQuestObjectiveDescriptions);
	TArray<FString> QuestDescriptions;
	QuestDescriptions.Reserve(QuestObjectives.Num());

	for (TObjectPtr<UQuestObjective> Objective : QuestObjectives)
	{
		QuestDescriptions.Add(Objective->GetObjectiveDescription());
	}

	return QuestDescriptions;
}

bool UQuestObject::Unlock_Implementation()
{
	if (QuestStatus != EQuestStatus::LOCKED) return false;
	QuestStatus = EQuestStatus::UNLOCKED;
	return true;
}

void UQuestObject::OnModifierStatusUpdated_Implementation(UQuestObjective* UpdatedModifier, EQuestStatus NewStatus,
                                                          EQuestStatus OldStatus)
{
}


void UQuestObject::QuestFinished_Implementation(EQuestStatus Status)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::QuestFinished_Implementation);

	for (TObjectPtr<UQuestObjective> Objective : QuestObjectives)
	{
		switch (Objective->Status)
		{
			case EQuestStatus::IN_PROGRESS:
			case EQuestStatus::STARTING:
				Objective->ForceStatus(Status);
			default:
				break;
		}
	}

	QuestStatus = Status;
	
	OnQuestFinishedDelegate.Broadcast(this, Status);
}

void UQuestObject::QuestTick_Implementation(float DeltaTime)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::QuestTick_Implementation)
	OnQuestTickDelegate.Broadcast(this, DeltaTime);
}

bool UQuestObject::StartQuest_Implementation()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestObject::StartQuest_Implementation)

	if (QuestStatus != EQuestStatus::STARTING) return false; 
	
	for (auto Objective : QuestObjectives)
	{
		Objective->TryStartObjective(this);
		
		if (Objective->NeedsTick())
		{
			ShouldTick = true;
		}

	}

	QuestStatus = EQuestStatus::IN_PROGRESS;
	
	QuestStarted();
	
	return true;
}
