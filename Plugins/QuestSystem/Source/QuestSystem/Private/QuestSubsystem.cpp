// Protected under GPL-3.0 License


#include "QuestSubsystem.h"
#include "QuestObject.h"
#include "QuestProgressionObject.h"
#include "Kismet/GameplayStatics.h"

UQuestSubsystem::UQuestSubsystem()
	: Super()
{
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::Initialize)
	Super::Initialize(Collection);

	Quests.Empty();
	Quests = TMap<FString, FTArrayQuestComparator>();
}

void UQuestSubsystem::Deinitialize()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::Deinitialize)
	Quests.Empty();
	
	Super::Deinitialize();
}

UQuestObject* UQuestSubsystem::GetQuestObject(TSubclassOf<UQuestObject> QuestClass, FString QuestOwner) const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::GetQuestObject)
	auto* QuestComparatorArray = Quests.Find(QuestOwner);
	if (!QuestComparatorArray)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "UQuestSubsystem::GetQuestObject - FTArrayQuestComparator is missing");
		return nullptr;
	}

	
	
	TArray<FQuestComparator> QuestObjects = QuestComparatorArray->Get();
	for (FQuestComparator Object : QuestObjects)
	{
		if (Object.QuestClass == QuestClass)
		{
		GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "UQuestSubsystem::GetQuestObject - Found given quest");
			return Object.QuestObject;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, "UQuestSubsystem::GetQuestObject - Quest is missing");
	return nullptr;
}

UQuestObject* UQuestSubsystem::UnlockQuest(TSubclassOf<UQuestObject> QuestToUnlock, FString QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::UnlockQuest)
	
	return ApplyCommandToQuest(QuestToUnlock, QuestOwner, EQuestEnterCommand::UNLOCK);
}

UQuestObject* UQuestSubsystem::UnlockQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::UnlockQuestObject)

	if (!IsValid(QuestObject)) return nullptr;

	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwner, EQuestEnterCommand::UNLOCK);
}

bool UQuestSubsystem::IsQuestUnlocked(TSubclassOf<UQuestObject> QuestToCheck, FString QuestOwner) const
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

UQuestObject* UQuestSubsystem::AcceptQuest(TSubclassOf<UQuestObject> QuestClass, FString QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AcceptQuest)

	return ApplyCommandToQuest(QuestClass, QuestOwner, EQuestEnterCommand::ACCEPT);
}

UQuestObject* UQuestSubsystem::AcceptQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AcceptQuestObject)
	
	if (!IsValid(QuestObject)) return nullptr;
	
	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwner, EQuestEnterCommand::ACCEPT);
}

UQuestObject* UQuestSubsystem::InitializeQuest(TSubclassOf<UQuestObject> QuestClass, FString QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::InitializeQuest)
	
	return ApplyCommandToQuest(QuestClass, QuestOwner, EQuestEnterCommand::INITIALIZE);
}

UQuestObject* UQuestSubsystem::InitializeQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::InitializeQuestObject)
	
	if (!IsValid(QuestObject)) return nullptr;
	
	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwner, EQuestEnterCommand::INITIALIZE);
}

UQuestObject* UQuestSubsystem::StartQuest(TSubclassOf<UQuestObject> QuestClass, FString QuestOwner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::StartQuest)
	
	return ApplyCommandToQuest(QuestClass, QuestOwner, EQuestEnterCommand::START);
}

UQuestObject* UQuestSubsystem::StartQuestObject(UQuestObject* QuestObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::StartQuestObject)
	
	if (!IsValid(QuestObject)) return nullptr;
	return ApplyCommandToQuest(QuestObject->GetClass(), QuestObject->QuestOwner, EQuestEnterCommand::START);
}

UQuestObject* UQuestSubsystem::ApplyCommandToQuest(TSubclassOf<UQuestObject> QuestClass, FString QuestOwner,
	EQuestEnterCommand QuestCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::TryEnterQuestState)
	
	if (!EnsurePlayerEntryExists(QuestOwner)) return nullptr;
	if (!IsValid(QuestClass)) return nullptr;
	
	FQuestComparator QuestComparator = GetQuestComparatorForPlayer(QuestClass, QuestOwner);
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

FString UQuestSubsystem::GetQuestOwner(TSubclassOf<UQuestObject> QuestClass) const
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

	return "";
}

void UQuestSubsystem::AddProgress(FString QuestOwner, UQuestProgressionObject* Progressor, TSubclassOf<UQuestObject> QuestClass)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AddProgress)
	if (!EnsurePlayerEntryExists(QuestOwner) || !Progressor) return;
	if (QuestClass && !IsValid(GetQuestObject(QuestClass, QuestOwner))) return;

	if (!QuestClass)
	{
		auto QuestObjects = GetQuestObjects(QuestOwner);
		for (auto QuestObject : QuestObjects)
		{
			if (!IsValid(QuestObject)) continue;
			QuestObject->ProgressQuest(Progressor);
			//a quest might consume the progressor and we don't want to add more progress when it gets destroyed
			if (!IsValid(Progressor)) break;
		}
		return;
	}
	
	GetQuestObject(QuestClass, QuestOwner)->ProgressQuest(Progressor);
}

void UQuestSubsystem::ClearQuests()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::ClearQuests)
	Quests.Empty();
}

FQuestComparator& UQuestSubsystem::GetQuestComparatorForPlayer(TSubclassOf<UQuestObject> QuestClass,
                                                                   const FString Controller)
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

FQuestComparator UQuestSubsystem::CreateNewComparator(TSubclassOf<UQuestObject> QuestClass, FString Owner, bool AutoUnlocked)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::CreateNewComparator)

	if (!QuestClass || Owner.IsEmpty())
	{
		InvalidQuestComparator = FQuestComparator();
		return InvalidQuestComparator;
	}
	
	if (!EnsurePlayerEntryExists(Owner)) 
	{
		InvalidQuestComparator = FQuestComparator();
		return InvalidQuestComparator;
	}
	
	FQuestComparator NewComparator;
	NewComparator.QuestObject = NewObject<UQuestObject>(this, QuestClass);
	NewComparator.QuestClass = QuestClass;
	NewComparator.QuestObject->QuestOwner = Owner;
	NewComparator.QuestObject->QuestStatus = AutoUnlocked ? EQuestStatus::UNLOCKED : EQuestStatus::LOCKED;
	
	return NewComparator;
	
}

bool UQuestSubsystem::AddQuestComparator(FQuestComparator& Comparator, FString Owner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::AddQuestComparator)
	
	if (Owner.IsEmpty()) return false;
	if (Comparator == InvalidQuestComparator) return false;

	if (!EnsurePlayerEntryExists(Owner)) return false;

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

TArray<UQuestObject*> UQuestSubsystem::GetQuestObjects(FString QuestsOwner) const
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


bool UQuestSubsystem::EnsurePlayerEntryExists(FString Owner)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UQuestSubsystem::EnsureControllerEntryExists)

	if (Owner.IsEmpty()) return false;
	
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