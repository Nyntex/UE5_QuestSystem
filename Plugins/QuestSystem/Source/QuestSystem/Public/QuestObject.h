// Protected under GPL-3.0 License.

#pragma once

#include "CoreMinimal.h"
#include "QuestObjective.h"
#include "IO/IoDispatcher.h"
#include "UObject/Object.h"
#include "QuestObject.generated.h"


class UQuestProgressionObject;
struct FQuestModifier;
struct FQuestProgressor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestStarted, UQuestObject*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestTick, UQuestObject*, Quest, float, DeltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestFinished, UQuestObject*, Quest, EQuestStatus, QuestFinishedStatus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestProgressUpdated, UQuestObject*, Quest, TArray<UQuestObjective*>&, QuestModifiers, UQuestProgressionObject*, Progress);



/**
 * The Abstract Quest class to derive from when creating new quests
 *
 */
UCLASS(Category="QuestSystem|Quest", BlueprintType, Blueprintable, Abstract)
class QUESTSYSTEM_API UQuestObject : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UQuestObject();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="QuestSystem|Quest")
	FName QuestName;
	
	UPROPERTY(Category="QuestSystem|Quest", BlueprintReadOnly, VisibleInstanceOnly)
	AController* QuestOwningController;
	
	UPROPERTY(BlueprintAssignable, Category="QuestSystem|Quest|Event")
	FOnQuestStarted OnQuestStartedDelegate;

	UPROPERTY(BlueprintAssignable, Category="QuestSystem|Quest|Event")
	FOnQuestTick OnQuestTickDelegate;

	UPROPERTY(BlueprintAssignable, Category="QuestSystem|Quest|Event")
	FOnQuestFinished OnQuestFinishedDelegate;

	UPROPERTY(BlueprintAssignable, Category="QuestSystem|Quest|Event")
	FOnQuestProgressUpdated OnQuestProgressUpdatedDelegate;
	
public:
	
#pragma region TickableGameObject
	virtual void Tick(float DeltaTime) override { QuestTick(DeltaTime); }
	virtual ETickableTickType GetTickableTickType() const override
	{
		return ETickableTickType::Conditional;
	}
	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FQuestObject, STATGROUP_Tickables);
	}
	virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}
	virtual bool IsTickableInEditor() const override
	{
		return false;
	}
	virtual bool IsTickable() const override //This defines whether tick is called or not
	{
		return ShouldTick;
	}

#pragma endregion TickableGameObject

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objectives", Instanced)
	TArray<UQuestObjective*> QuestObjectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rewards", Instanced)
	TArray<UQuestReward*> QuestRewards;
	
	UFUNCTION(Category="Quest", BlueprintCallable)
	TArray<FString> GetQuestModifierDescriptions() const;

	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent, meta=(ForceAsFunction))
	bool Unlock();
	
	//Don't forget to call super when overriding
	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent, meta=(ForceAsFunction))
	bool Initialize();

	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent)
	void OnModifierStatusUpdated(UQuestObjective* UpdatedModifier, EQuestStatus NewStatus, EQuestStatus OldStatus);

	UFUNCTION(Category="Quest", BlueprintCallable)
	EQuestStatus GetStatus() const { return QuestStatus; }
	
	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent)
	void ProgressQuest(UQuestProgressionObject* Progress);
	
	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent)
	void QuestFinished(EQuestStatus Status);

	UFUNCTION(Category="Quest", BlueprintCallable)
	void ClaimRewards();
	
	/**
	 * Gets called when the Quest starts. This should always broadcast the OnQuestStarted Delegate
	 */
	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent)
	void QuestStarted();
	void QuestStarted_Implementation() { OnQuestStartedDelegate.Broadcast(this); }

	/**
	 * To have this called "ShouldTick" needs to be true
	 * @param DeltaTime 
	 */
	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent)
	void QuestTick(float DeltaTime);

	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent)
	bool AcceptQuest();

	//This should not be called from blueprint unless you know what you are doing.
	//
	//Gets called from the quest subsystem when accepted quest should get started.
	UFUNCTION(Category="Quest", BlueprintCallable, BlueprintNativeEvent, meta=(ForceAsFunction))
	bool StartQuest();
	
	UFUNCTION(Category="Quest", BlueprintCallable)
	EQuestStatus TryFinishQuest();

protected:
	UPROPERTY(Category="Quest", BlueprintReadWrite)
	bool ShouldTick = false;

	UPROPERTY(Category="Quest", BlueprintReadWrite, VisibleInstanceOnly)
	EQuestStatus QuestStatus = EQuestStatus::LOCKED;

	friend class UQuestSubsystem;
};
