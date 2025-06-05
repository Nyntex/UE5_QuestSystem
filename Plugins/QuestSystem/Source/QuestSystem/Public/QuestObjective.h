// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestEnums.h"
#include "QuestObjective.generated.h"


class UQuestReward;
class UQuestProgressionObject;
class UQuestObject;
struct FQuestProgressor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnObjectiveStatusUpdated, UQuestObjective*, Modifier, EQuestStatus, UpdatedStatus, EQuestStatus, OldStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProgressUpdated, UObject*, ProgressAdded);

/**
 * The Quest Objective tells a quest object when it's done. A Quest Object can have multiple
 * Quest Objectives and only when every Objective is completed the quest is considered to be completed.
 *
 * The following methods should get overwritten when creating a child:
 *  - GetObjectiveDescription : The description that may get shown in the ui
 *  - Initialize : (Call Parent) Initializes the data by getting all required pointers and makes the quest ready to be started.
 *  - AddProgress : Adds progress towards the quest objective through the QuestProgressionObject
 *  - UpdateStatus : (Call Parent) Used to update the objective's status. Add functionality to when a specific status is hit.
 *  - TickObjective : If your objective requires tick you need to enable "ShouldTick" and then overwrite this event.
 *  - TryStartObjective : Override if you want to have your own starting behavior
 */
UCLASS(Category="QuestSystem|Quest|Objective", BlueprintType, Abstract, Blueprintable, EditInlineNew)
class QUESTSYSTEM_API UQuestObjective : public UObject
{
	GENERATED_BODY()
	
public:
	UQuestObjective();

//Delegates
	UPROPERTY(BlueprintAssignable, Category="QuestObjective", BlueprintReadWrite)
	FOnObjectiveStatusUpdated OnObjectiveStatusUpdatedDelegate;

	UPROPERTY(BlueprintAssignable, Category="QuestObjective", BlueprintReadWrite)
	FOnProgressUpdated OnProgressUpdatedDelegate;
	
//member	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="QuestObjective")
	EQuestStatus Status = EQuestStatus::INVALID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="QuestObjective", Instanced)
	TArray<UQuestReward*> ObjectiveRewards;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="QuestObjective")
	bool FailingObjectiveFailsQuest = true;

//methods
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective", meta=(ForceAsFunction=true))
	void AddProgress(UQuestProgressionObject* Progress);

	UFUNCTION(BlueprintCallable, Category="QuestObjective")
	void BroadcastProgress(UObject* AddedProgress);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="QuestObjective")
	FString GetObjectiveDescription() const;

	/**
	 * Initializes default variables and makes this modifier ready to start.
	 * 
	 * @param OwningQuest The owning quest that tries to initialize this modifier
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective", meta=(ForceAsFunction))
	void Initialize(UQuestObject* OwningQuest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective", meta=(ForceAsFunction))
	bool TryStartObjective(UQuestObject* Quest);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective", meta=(ForceAsFunction))
	void StartObjective(UQuestObject* Quest);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective")
	void TickObjective(UQuestObject* Quest, float DeltaTime);
	
	/**
	 * Checks owning variables to consider if the status updates.
	 * If it considers the status to update broadcast OnModifierStatusUpdatedDelegate.
	 * 
	 **/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective", meta=(ForceAsFunction))
	void UpdateStatus(EQuestStatus NewStatus);
	

	UFUNCTION(BlueprintCallable, Category="QuestObjective")
	bool NeedsTick() const {return ShouldTick; };

	UFUNCTION(BlueprintCallable, Category="QuestObjective")
	void ClaimRewards();
	
	//utility methods

	/**
	 * Forces a specific QuestStatus onto this modifier.
	 * Used mainly to forcefully fail or succeed a modifier. Use with caution!
	 **/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="QuestObjective")
	void ForceStatus(EQuestStatus NewStatus);

	UFUNCTION(BlueprintCallable, Category="QuestObjective")
	AController* GetController() const;

	UFUNCTION(BlueprintCallable, Category="QuestObjective")
	UQuestObject* GetOwningQuestObject() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="QuestObjective", meta=(AllowPrivateAccess=true))
	bool ShouldTick = false;
};
