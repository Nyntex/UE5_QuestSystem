// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSubsystem.h"
#include "QuestObject.h"

UQuestSubsystem::UQuestSubsystem()
	: Super()
{
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::Initialize)
	Super::Initialize(Collection);

	Quests.Empty();
	Quests = TMap<AController*, FTArrayQuestComparator>();
}

void UQuestSubsystem::Deinitialize()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::Deinitialize)
	Quests.Empty();
	
	Super::Deinitialize();
}

UQuestObject* UQuestSubsystem::GetQuestObject(TSubclassOf<UQuestObject> QuestClass, AController* QuestOwner) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestObject)
	auto* QuestComparatorArray = Quests.Find(QuestOwner);
	if (!QuestComparatorArray)
		return nullptr;
	
	TArray<FQuestComparator> QuestObjects = QuestComparatorArray->Get();
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
	auto* QuestComparatorArray = Quests.Find(QuestOwner);
	if (!QuestComparatorArray)
	{
		return false;
	}

	for (FQuestComparator QuestComparator : QuestComparatorArray->Get())
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
	
	if (!EnsureControllerEntryExists(QuestOwner)) return nullptr;
	
	FQuestComparator QuestComparator = GetQuestComparatorForController(QuestClass, QuestOwner);
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
	
	for (auto Iterator : Quests)
	{
		for (FQuestComparator& Comparator : Iterator.Value.GetRef())
		{
			if (Comparator.QuestClass !=  QuestClass) continue;

			if (!IsValid(Comparator.QuestObject)) continue;

			if (Comparator.QuestObject->GetStatus() == EQuestStatus::LOCKED ||
				Comparator.QuestObject->GetStatus() == EQuestStatus::INVALID) continue;

			return Iterator.Key;
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
	Quests.Empty();
}

FQuestComparator& UQuestSubsystem::GetQuestComparatorForController(TSubclassOf<UQuestObject> QuestClass,
	const AController* Controller)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestComparatorForController)
	TArray<FQuestComparator>& QuestComparators = Quests.Find(Controller)->GetRef();

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
	
	if (!EnsureControllerEntryExists(Owner)) 
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

	if (!EnsureControllerEntryExists(Owner)) return false;

	TArray<FQuestComparator>& QuestComparators = Quests.Find(Owner)->GetRef();

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
	if (!Quests.Find(QuestsOwner)) return {};

	TArray<UQuestObject*> QuestObjects = TArray<UQuestObject*>();
	QuestObjects.Reserve(Quests.Find(QuestsOwner)->Get().Num());
	
	for (auto QuestComparator : Quests.Find(QuestsOwner)->Get())
	{
		QuestObjects.Add(QuestComparator.QuestObject);
	}

	return QuestObjects;
}


bool UQuestSubsystem::EnsureControllerEntryExists(AController* Owner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::EnsureControllerEntryExists)

	if (!IsValid(Owner)) return false;
	
	//Is Controller known, if not, add it to the array
	if (!Quests.Find(Owner))
	{
		Quests.Add(Owner);
	}

	//Does the controller have quests, controller index == quest objects index
	//If it has quests check for the given class
	//otherwise create the index for quest objects
	auto* QuestComparatorArray = Quests.Find(Owner);
	if (QuestComparatorArray->GetRef().Num() <= 0)
	{
		*QuestComparatorArray = FTArrayQuestComparator();
	}

	return Quests.Find(Owner) && QuestComparatorArray->GetRef().Num() >= 0;
		
	
	// return OwnerIndex < Quests.QuestObjects.Num() &&
	// 		OwnerIndex >= 0 &&
	// 		Quests.QuestObjects.Num() >= 0;
}