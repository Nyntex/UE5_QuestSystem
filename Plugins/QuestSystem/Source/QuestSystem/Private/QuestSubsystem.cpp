// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSubsystem.h"
#include "QuestObject.h"

void FQuestContainer::Clear()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FQuestContainer::Clear)
	Controllers.Empty();

	for (auto QuestObject : QuestObjects)
	{
		QuestObject.Clear();
	}
	QuestObjects.Empty();
}

UQuestSubsystem::UQuestSubsystem()
	: Super()
{
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::Initialize)
	Super::Initialize(Collection);

	Quests = FQuestContainer();
}

void UQuestSubsystem::Deinitialize()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::Deinitialize)
	Quests.Clear();
	
	Super::Deinitialize();
}

UQuestObject* UQuestSubsystem::GetQuestObject(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestObject)
	int32 ControllerIndex = Quests.Controllers.Find(QuestOwner);
	if (ControllerIndex == INDEX_NONE || ControllerIndex > Quests.QuestObjects.Num())
		return nullptr;
	
	TArray<FQuestComparator> QuestObjects = Quests.QuestObjects[ControllerIndex].Get();
	for (FQuestComparator Object : QuestObjects)
	{
		if (Object.QuestClass == QuestClass)
		{
			return Object.QuestObject;
		}
	}

	return nullptr;
}

UQuestObject* UQuestSubsystem::UnlockQuest(TSubclassOf<UQuestObject> QuestToUnlock, AController* QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::UnlockQuest)
	
	return ApplyCommandToQuest(QuestToUnlock, QuestOwner, EQuestEnterCommand::UNLOCK);
}

UQuestObject* UQuestSubsystem::UnlockQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::UnlockQuestObject)

	if (!IsValid(QuestObject)) return nullptr;

	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwningController, EQuestEnterCommand::UNLOCK);
}

bool UQuestSubsystem::IsQuestUnlocked(TSubclassOf<UQuestObject> QuestToCheck, AController* QuestOwner) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::IsQuestUnlocked)
	int32 OwnerIndex = Quests.Controllers.Find(QuestOwner);
	if (OwnerIndex == INDEX_NONE || OwnerIndex >= Quests.QuestObjects.Num())
	{
		return false;
	}

	for (FQuestComparator QuestComparator : Quests.QuestObjects[OwnerIndex].Get())
	{
		if (QuestComparator.QuestClass == QuestToCheck)
		{
			return (QuestComparator.QuestObject->GetStatus() != EQuestStatus::INVALID) && (QuestComparator.QuestObject->GetStatus() != EQuestStatus::LOCKED);
		}
	}

	return false;
}

UQuestObject* UQuestSubsystem::AcceptQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AcceptQuest)

	return ApplyCommandToQuest(QuestClass, QuestOwner, EQuestEnterCommand::ACCEPT);
}

UQuestObject* UQuestSubsystem::AcceptQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AcceptQuestObject)
	
	if (!IsValid(QuestObject)) return nullptr;
	
	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwningController, EQuestEnterCommand::ACCEPT);
}

UQuestObject* UQuestSubsystem::InitializeQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::InitializeQuest)
	
	return ApplyCommandToQuest(QuestClass, QuestOwner, EQuestEnterCommand::INITIALIZE);
}

UQuestObject* UQuestSubsystem::InitializeQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::InitializeQuestObject)
	
	if (!IsValid(QuestObject)) return nullptr;
	
	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwningController, EQuestEnterCommand::INITIALIZE);
}

UQuestObject* UQuestSubsystem::StartQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::StartQuest)
	
	return ApplyCommandToQuest(QuestClass, QuestOwner, EQuestEnterCommand::START);
}

UQuestObject* UQuestSubsystem::StartQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::StartQuestObject)
	
	if (!IsValid(QuestObject)) return nullptr;
	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwningController, EQuestEnterCommand::START);
}

UQuestObject* UQuestSubsystem::ApplyCommandToQuest(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner,
	EQuestEnterCommand QuestCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::TryEnterQuestState)
	
	int32 ControllerIndex{0};
	if (!EnsureControllerEntryExists(QuestOwner, ControllerIndex)) return nullptr;
	
	FQuestComparator QuestComparator = GetQuestComparatorForController(QuestClass, ControllerIndex);
	//FQuestComparator NewComparator = FQuestComparator();
	if (QuestComparator == InvalidQuestComparator || !IsValid(QuestComparator.QuestObject))
	{
		QuestComparator = CreateNewComparator(QuestClass, QuestOwner);
		AddQuestComparator(QuestComparator, QuestOwner);
	}

	bool Success = false; 
		
	switch (QuestCommand)
	{
	case EQuestEnterCommand::UNLOCK:
		Success = QuestComparator.QuestObject->Unlock();
		break;
	case EQuestEnterCommand::ACCEPT:
		Success = QuestComparator.QuestObject->AcceptQuest();
		break;
	case EQuestEnterCommand::INITIALIZE:
		Success = QuestComparator.QuestObject->Initialize();
		break;
	case EQuestEnterCommand::START:
		Success = QuestComparator.QuestObject->StartQuest();
		break;
	default:
		break;
	}
		
	return Success ? QuestComparator.QuestObject : nullptr; 
}

AController* UQuestSubsystem::GetQuestOwner(TSubclassOf<UQuestObject> QuestClass) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestOwner)
	for (int i = 0; i < Quests.Controllers.Num(); i++)
	{
		for (int j = 0; Quests.QuestObjects[i].Get().Num(); j++)
		{
			if (Quests.QuestObjects[i].Get()[j].QuestClass != QuestClass) continue;
			
			if (!IsValid(Quests.QuestObjects[i].Get()[j].QuestObject)) continue;

			if (Quests.QuestObjects[i].Get()[j].QuestObject->GetStatus() == EQuestStatus::INVALID ||
				Quests.QuestObjects[i].Get()[j].QuestObject->GetStatus() == EQuestStatus::LOCKED) continue;
			
			return Quests.Controllers[i];
		}
	}

	return nullptr;
}

void UQuestSubsystem::AddProgress(AController* QuestOwner, UQuestProgressionObject* Progressor, TSubclassOf<UQuestObject> QuestClass)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AddProgress)
	GetQuestObject(QuestClass, QuestOwner)->ProgressQuest(Progressor);
}

void UQuestSubsystem::ClearQuests()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::ClearQuests)
	Quests.Clear();
}

FQuestComparator& UQuestSubsystem::GetQuestComparatorForController(TSubclassOf<UQuestObject> QuestClass,
	int32 ControllerIndex)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestComparatorForController)
	TArray<FQuestComparator>& QuestComparators = Quests.QuestObjects[ControllerIndex].GetRef();

	for (int i = 0; i < QuestComparators.Num(); i++)
	{
		if (QuestComparators[i].QuestClass == QuestClass)
		{
			return QuestComparators[i];
		}
	}

	InvalidQuestComparator = FQuestComparator();
	return InvalidQuestComparator;
}

FQuestComparator UQuestSubsystem::CreateNewComparator(TSubclassOf<UQuestObject> QuestClass, AController* Owner, bool AutoUnlocked)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::CreateNewComparator)

	if (!QuestClass || !Owner)
	{
		InvalidQuestComparator = FQuestComparator();
		return InvalidQuestComparator;
	}
	
	int32 ControllerIndex{};
	if (!EnsureControllerEntryExists(Owner, ControllerIndex)) 
	{
		InvalidQuestComparator = FQuestComparator();
		return InvalidQuestComparator;
	}
	
	FQuestComparator NewComparator;
	NewComparator.QuestObject = NewObject<UQuestObject>(this, QuestClass);
	NewComparator.QuestClass = QuestClass;
	NewComparator.QuestObject->QuestOwningController = Owner;
	NewComparator.QuestObject->QuestStatus = AutoUnlocked ? EQuestStatus::UNLOCKED : EQuestStatus::LOCKED;
	
	return NewComparator;
	
}

bool UQuestSubsystem::AddQuestComparator(FQuestComparator& Comparator, AController* Owner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AddQuestComparator)
	
	if (!Owner) return false;
	if (Comparator == InvalidQuestComparator) return false;

	int32 ControllerIndex{0};
	if (!EnsureControllerEntryExists(Owner, ControllerIndex)) return false;

	TArray<FQuestComparator>& QuestComparators = Quests.QuestObjects[ControllerIndex].GetRef();

	for (FQuestComparator& AvailableComparator : QuestComparators)
	{
		if (AvailableComparator.QuestClass == Comparator.QuestClass && !AvailableComparator.QuestObject)
		{
			FQuestComparator NewComparator = CreateNewComparator(Comparator.QuestClass, Owner);
			if (NewComparator == InvalidQuestComparator) return false;
			AvailableComparator = NewComparator;
			return true;
		}
		else if (AvailableComparator.QuestClass == Comparator.QuestClass)
		{
			return false;
		}
	}

	QuestComparators.Add(Comparator);
	return true;
}

TArray<UQuestObject*> UQuestSubsystem::GetQuestObjects(AController* QuestsOwner) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestObject)
	int32 ControllerIndex = Quests.Controllers.Find(QuestsOwner);

	TArray<UQuestObject*> QuestObjects = TArray<UQuestObject*>();
	QuestObjects.Reserve(Quests.QuestObjects[ControllerIndex].Get().Num());
	
	for (auto QuestComparator : Quests.QuestObjects[ControllerIndex].Get())
	{
		QuestObjects.Add(QuestComparator.QuestObject);
	}

	return QuestObjects;
}


bool UQuestSubsystem::EnsureControllerEntryExists(AController* Owner, int32& OwnerIndex)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::EnsureControllerEntryExists)

	if (!IsValid(Owner)) return false;
	
	//Is Controller known, if not, add it to the array
	OwnerIndex = Quests.Controllers.Find(Owner);
	if (OwnerIndex == INDEX_NONE)
	{
		OwnerIndex = Quests.Controllers.Add(Owner);
	}

	//Does the controller have quests, controller index == quest objects index
	//If it has quests check for the given class
	//otherwise create the index for quest objects
	if (OwnerIndex >= Quests.QuestObjects.Num())
	{
		Quests.QuestObjects.Add(FTArrayQuestObject());
	}
	
	return OwnerIndex < Quests.QuestObjects.Num() &&
			OwnerIndex >= 0 &&
			Quests.QuestObjects.Num() >= 0;
}