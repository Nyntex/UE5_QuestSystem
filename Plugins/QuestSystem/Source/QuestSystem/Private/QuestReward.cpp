// Protected under GPL-3.0 License


#include "QuestReward.h"

#include "QuestObject.h"
#include "QuestSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UQuestReward::ClaimReward_Implementation()
{
}

UQuestSubsystem* UQuestReward::GetQuestSubsystem() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestReward::GetQuestSubsystem)
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(OwningQuest->QuestOwningController);
	return Cast<UQuestSubsystem>(GameInstance->GetSubsystem<UQuestSubsystem>());
}
