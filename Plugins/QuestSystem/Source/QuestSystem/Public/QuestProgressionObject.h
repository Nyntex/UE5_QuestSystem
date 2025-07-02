// Protected under GPL-3.0 License.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "QuestProgressionObject.generated.h"

class UQuestObjective;
/**
 * 
 */
UCLASS(Category="QuestSystem|Quest|Objective|Progressor",Abstract, Blueprintable, BlueprintType, meta=(EditInlineNew=true), DefaultToInstanced)
class QUESTSYSTEM_API UQuestProgressionObject : public UObject
{
	GENERATED_BODY()

public:
	UQuestProgressionObject();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestObjective", meta=(ExposeOnSpawn=true))
	TSubclassOf<UQuestObjective> ObjectiveToProgress;
	
};
