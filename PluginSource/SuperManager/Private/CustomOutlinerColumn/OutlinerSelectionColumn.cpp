// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomOutlinerColumn/OutlinerSelectionColumn.h"
#include "CustomStyle/SuperManagerStyle.h"
#include "ActorTreeItem.h"
#include "SuperManager.h"

SHeaderRow::FColumn::FArguments FOutlinerSelectionColumn::ConstructHeaderRowColumn()
{
	SHeaderRow::FColumn::FArguments ConstructedHeaderRow =
	SHeaderRow::Column(GetColumnID())
	.FixedWidth(24.f)
	.HAlignHeader(HAlign_Center)
	.VAlignHeader(VAlign_Center)
	.HAlignCell(HAlign_Center)
	.VAlignCell(VAlign_Center)
	.DefaultTooltip(FText::FromString(TEXT("Actor Selection Lock - Press icon to lock actor selection")))
	[
		SNew(SImage)
		.ColorAndOpacity(FSlateColor::UseForeground())
		.Image(FSuperManagerStyle::GetCreatedSlateStyleSet()->GetBrush(FName("LevelEditor.LockSelection")))
	];
	
	return ConstructedHeaderRow;
}

const TSharedRef<SWidget> FOutlinerSelectionColumn::ConstructRowWidget(FSceneOutlinerTreeItemRef TreeItem,
	const STableRow<FSceneOutlinerTreeItemPtr>& Row)
{
	FActorTreeItem* ActorTreeItem =	TreeItem->CastTo<FActorTreeItem>();

	if(!ActorTreeItem || !ActorTreeItem->IsValid()) return SNullWidget::NullWidget;

	FSuperManagerModule& SuperManagerModule =
	FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	// 체크 되었는지 ( Locked 태그를 가졌는지) 확인
	const bool bIsActorSelectionLocked = 
	SuperManagerModule.CheckIsActorSelectionLocked(ActorTreeItem->Actor.Get());


	const FCheckBoxStyle& ToggleButtonStyle =
	FSuperManagerStyle::GetCreatedSlateStyleSet()->GetWidgetStyle<FCheckBoxStyle>(FName("SceneOutliner.SelectionLock"));
	// 체크박스 생성
	TSharedRef<SCheckBox> ConstructedRowWidgetCheckBox =
	SNew(SCheckBox)
	.Visibility(EVisibility::Visible)
	.Type(ESlateCheckBoxType::ToggleButton)
	.Style(&ToggleButtonStyle)
	.HAlign(HAlign_Center)
	.IsChecked(bIsActorSelectionLocked? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
	.OnCheckStateChanged(this, & FOutlinerSelectionColumn::OnRowWidgetCheckStateChanged,ActorTreeItem->Actor);
	
	
	return ConstructedRowWidgetCheckBox;
}

void FOutlinerSelectionColumn::OnRowWidgetCheckStateChanged(ECheckBoxState NewState,
	TWeakObjectPtr<AActor> CorrespondingActor)
{
	FSuperManagerModule& SuperManagerModule =
	FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		SuperManagerModule.ProcessLockingForOutliner(CorrespondingActor.Get(),false);
		break;
	case ECheckBoxState::Checked:
		SuperManagerModule.ProcessLockingForOutliner(CorrespondingActor.Get(),true);
		break;
	case ECheckBoxState::Undetermined:
		break;
	default:
		break;
	}
}
