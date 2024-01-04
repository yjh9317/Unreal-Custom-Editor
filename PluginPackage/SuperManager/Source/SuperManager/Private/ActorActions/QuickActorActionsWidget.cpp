// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorActions/QuickActorActionsWidget.h"
#include "SubSystems/EditorActorSubsystem.h"
#include "DebugHeader.h"

bool UQuickActorActionsWidget::GetEditorActionSubsystem()
{
	if(!EditorActorSubsystem)
	{
		EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();		
	}
	return EditorActorSubsystem != nullptr;
}

void UQuickActorActionsWidget::SelectAllActorsWithSimilarName()
{
	if(!GetEditorActionSubsystem()) return;

	TArray<AActor*> SelectedActros = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 SelectionCounter = 0;

	if(SelectedActros.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}
	
	if(SelectedActros.Num()>1)
	{
		DebugHeader::ShowNotifyInfo(TEXT("You can only selected one actor"));
		return;
	}

	FString SelectedActorName = SelectedActros[0]->GetActorLabel();
	const FString& NameToSearch = SelectedActorName.LeftChop(4); // LeftChop 오른쪽에서 4개 문자 삭제

	TArray<AActor*> AllLevelActors = EditorActorSubsystem->GetAllLevelActors();

	for(AActor* ActorInLevel : AllLevelActors)
	{
		if(!ActorInLevel) continue;

		if(ActorInLevel->GetActorLabel().Contains(NameToSearch,SearchCase))
		{
			EditorActorSubsystem->SetActorSelectionState(ActorInLevel,true);
			SelectionCounter++;
		}
	}

	if(SelectionCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully selected ") + FString::FromInt(SelectionCounter) + TEXT(" actors"));
	}
	else
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor with similar name found"));
	}
}

void UQuickActorActionsWidget::DuplicateActors()
{
	if(!GetEditorActionSubsystem()) return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 Counter = 0;

	if(SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	if(NumberOfDuplicates <= 0 || OffsetDist <= 0)
	{ 
		DebugHeader::ShowNotifyInfo(TEXT("Didn`t specify a number of duplications or an offset distance"));
		return;
	}

	for(AActor* SelectedActor : SelectedActors)
	{
		if(!SelectedActor) continue;

		for(int32 i = 0 ; i <NumberOfDuplicates; i++)
		{
			AActor* DuplicatedActor = EditorActorSubsystem->DuplicateActor(SelectedActor,SelectedActor->GetWorld());

			if(!DuplicatedActor) continue;

			const float DuplicationOffsetDist = (i+1)*OffsetDist;

			switch(AxisForDuplication)
			{
			case E_DuplicationAxis::EDA_XAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(DuplicationOffsetDist,0.f,0.f));
				break;
			case E_DuplicationAxis::EDA_YAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.f,DuplicationOffsetDist,0.f));
				break;
			case E_DuplicationAxis::EDA_ZAxis:
				DuplicatedActor->AddActorWorldOffset(FVector(0.f,0.f,DuplicationOffsetDist));
				break;
			case E_DuplicationAxis::EDA_MAX:	
				break;
			}
			EditorActorSubsystem->SetActorSelectionState(DuplicatedActor, true);
			Counter++;
			
		}
	}
	if(Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated ") +
			FString::FromInt(Counter) + TEXT(" actors"));
	}
}

void UQuickActorActionsWidget::RandomizeActorTransform()
{
	const bool bConditionNotSet =
		!RandomActorRotation.bRandomizeRotYaw &&
		!RandomActorRotation.bRandomizeRotPitch &&
		!RandomActorRotation.bRandomizeRotRoll &&
		!bRandomizeScale &&
		!bRandomizeOffset;

	if(bConditionNotSet)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No variation condition selected"));
		return;
	}
	
	if(!GetEditorActionSubsystem()) return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 Counter = 0;

	if(SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	for(AActor* SelectedActor : SelectedActors)
	{
		if(!SelectedActor) continue;

		if(RandomActorRotation.bRandomizeRotYaw)
		{
			const float RandomRotYawValue = FMath::RandRange(RandomActorRotation.RotYawMin,RandomActorRotation.RotYawMax);
			SelectedActor->AddActorWorldRotation(FRotator(0.f,RandomRotYawValue,0.f));
		}

		if(RandomActorRotation.bRandomizeRotPitch)
		{
			const float RandomRotPitchValue = FMath::RandRange(RandomActorRotation.RotPitchMin,RandomActorRotation.RotPitchMax);
			SelectedActor->AddActorWorldRotation(FRotator(RandomRotPitchValue,0.f,0.f));
		}

		if(RandomActorRotation.bRandomizeRotRoll)
		{
			const float RandomRotRollValue = FMath::RandRange(RandomActorRotation.RotRollMin,RandomActorRotation.RotRollMax);
			SelectedActor->AddActorWorldRotation(FRotator(0.f,0.f,RandomRotRollValue));
		}

		if(bRandomizeScale)
		{
			SelectedActor->SetActorScale3D(FVector(FMath::RandRange(ScaleMin,ScaleMax)));
		}

		if(bRandomizeOffset)
		{
			const float RandomOffsetValue = FMath::RandRange(OffsetMin,OffsetMax);
			SelectedActor->AddActorWorldOffset(FVector(RandomOffsetValue,RandomOffsetValue,0.f));
		}
		Counter++;
	}

	if(Counter> 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully set ") + FString::FromInt(Counter) + TEXT(" actors"));
	}
}

void UQuickActorActionsWidget::ResetRotation()
{
	if(!GetEditorActionSubsystem()) return;

	TArray<AActor*> SelectedActors = EditorActorSubsystem->GetSelectedLevelActors();
	uint32 Counter = 0;

	if(SelectedActors.Num() == 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("No actor selected"));
		return;
	}

	for(AActor* SelectedActor : SelectedActors)
	{
		if(!SelectedActor) continue;
		const FRotator ZeroRotator = FRotator::ZeroRotator;
		SelectedActor->SetActorRotation(ZeroRotator);
	}
	
	if(Counter> 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully reset ") + FString::FromInt(Counter) + TEXT(" actors"));
	}
}


