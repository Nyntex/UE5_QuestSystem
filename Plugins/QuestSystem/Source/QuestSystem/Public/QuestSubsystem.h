// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestObject.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuestSubsystem.generated.h"
	
class UQuestObject;


#pragma region QuestContainer
USTRUCT()
struct FQuestComparator
{
	GENERATED_BODY()
	
	UPROPERTY()
	UQuestObject* QuestObject = nullptr;

	UPROPERTY()
	TSubclassOf<UQuestObject> QuestClass = nullptr;
	
	
	bool operator==(const FQuestComparator& other) const
	{
		return (QuestObject == other.QuestObject) && (QuestClass == other.QuestClass) && ((QuestObject && other.QuestObject) ? (QuestObject->GetStatus() == other.QuestObject->GetStatus()) : true);
	}

	bool operator!=(const FQuestComparator& other) const
	{
		return !(*this == other);
	}
};

USTRUCT()
struct FTArrayQuestObject
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FQuestComparator> QuestObjects = TArray<FQuestComparator>();

	void Clear()
	{
		for (auto Object : QuestObjects)
		{
			Object.QuestObject->ConditionalBeginDestroy();
		}

		QuestObjects.Empty();
	}
	
	TArray<FQuestComparator>& GetRef()
	{
		return QuestObjects;
	}

	TArray<FQuestComparator> Get() const
	{
		return QuestObjects;
	}

	FQuestComparator operator[](int Index)
	{
		return QuestObjects[Index];
	}
};

USTRUCT()
struct FQuestContainer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AController*> Controllers = TArray<AController*>();

	UPROPERTY()
	TArray<FTArrayQuestObject> QuestObjects = TArray<FTArrayQuestObject>();

	void Add(FQuestComparator Comparator, AController* Owner)
	{
		if (!Owner) return;
	}
	
	void Clear();
};
#pragma endregion QuestContainer

/**
 * 
 */
UCLASS()
class QUESTSYSTEM_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UQuestSubsystem();
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UPROPERTY()
	FQuestContainer Quests = FQuestContainer();

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* GetQuestObject(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner) const;

	/**
	 * @param QuestClass 
	 * @return The first controller that has the specified quest class as an existing quest. Existing means it has been created through any means
	 */
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	AController* GetQuestOwner(TSubclassOf<UQuestObject> QuestClass) const;

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	TArray<UQuestObject*> GetQuestObjects(AController* QuestsOwner) const;

	/**
	 *	Unlocks the given quest. If the quest does not exist it gets created for the
	 *	corresponding controller.
	 * 
	 * @param QuestToUnlock
	 * @param QuestOwner The owning controller of that quest
	 * @return The quest object that has been unlocked to do further things like starting.
	 */
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* UnlockQuest(TSubclassOf<UQuestObject> QuestToUnlock, AController* QuestOwner);

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* UnlockQuestObject(UQuestObject* QuestObject);
	
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	bool IsQuestUnlocked(TSubclassOf<UQuestObject> QuestToCheck, AController* QuestOwner) const;

	/**
	 * @param QuestClass Quest class that gets accepted
	 * @param QuestOwner The owning controller of that quest
	 * @return The Quest that has been accepted. Returns NULL if the quest is not unlocked. 
	 */
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* AcceptQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner);
	
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* AcceptQuestObject(UQuestObject* QuestObject);

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* InitializeQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner);

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* InitializeQuestObject(UQuestObject* QuestObject);
	
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* StartQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner);

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* StartQuestObject(UQuestObject* QuestObject);

	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	void AddProgress(AController* QuestOwner, UQuestProgressionObject* Progressor, TSubclassOf<UQuestObject> QuestClass);
	
	/**
	 * 
	 * @param QuestClass The quest which should receive the command
	 * @param QuestOwner The owner of the quest object
	 * @param QuestCommand 
	 * @return The quest object that received the command. Returns NULL if the command failed
	 */
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	UQuestObject* ApplyCommandToQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner, EQuestEnterCommand QuestCommand);
	
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	bool EnsureControllerEntryExists(AController* Owner, int32& OwnerIndex);
	
	UFUNCTION(BlueprintCallable, Category = "QuestSystem")
	void ClearQuests();

private:
	UFUNCTION()
	FQuestComparator& GetQuestComparatorForController(TSubclassOf<UQuestObject> QuestClass, int32 ControllerIndex);

	UFUNCTION()
	FQuestComparator CreateNewComparator(TSubclassOf<UQuestObject> QuestClass, AController* Owner, bool AutoUnlocked = false);

	/**
	 *	Adds the comparison object for quests to the list of the owning controller if there is none with
	 *	the same quest class.
	 * 
	 * @param Comparator The Comparator, including the QuestObject, to add to the list of available quests
	 * @param Owner The controlling owner of the quest object
	 * @return True when the comparator has been added, meaning no other comparator has the same quest class
	 */
	UFUNCTION()
	bool AddQuestComparator(FQuestComparator& Comparator, AController* Owner);
	
	// Do not edit, this is needed to have access to an invalid QuestComparator which we can use as a non const return value
	UPROPERTY()
	FQuestComparator InvalidQuestComparator = FQuestComparator();
};

