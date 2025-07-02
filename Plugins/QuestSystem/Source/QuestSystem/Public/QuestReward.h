// Protected under GPL-3.0 License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestReward.generated.h"

class UQuestSubsystem;
class UQuestObject;
/**
 * 
 */
UCLASS(Category="QuestSystem|Quest|Rewards", Blueprintable, BlueprintType, Abstract, DefaultToInstanced, EditInlineNew)
class QUESTSYSTEM_API UQuestReward : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category="QuestReward", meta=(ExposeOnSpawn=true))
	UQuestObject* OwningQuest = nullptr;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="QuestReward")
	void ClaimReward();

	UFUNCTION(BlueprintCallable, Category="QuestReward", BlueprintPure)
	UQuestSubsystem* GetQuestSubsystem() const;
};
