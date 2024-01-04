// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWidgets/AdvanceDeletionWidget.h"
#include "DebugHeader.h"
#include "SuperManager.h"
#include "Widgets/Layout/SScrollBox.h"

#define ListALL TEXT("List All Available Assets")
#define ListUnused TEXT("List Unused Assets")
#define ListSameName TEXT("List Assets With Same Name")

void SAdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;
	
	StoredAssetsData = InArgs._AssetsDataToStore;
	DisplayedAssetsData = StoredAssetsData;

	CheckBoxesArray.Empty();
	AssetsDataToDeleteArray.Empty();
	ComboBoxSourceItems.Empty();

	ComboBoxSourceItems.Add(MakeShared<FString>(ListALL));
	ComboBoxSourceItems.Add(MakeShared<FString>(ListUnused));
	ComboBoxSourceItems.Add(MakeShared<FString>(ListSameName));

	
	
	FSlateFontInfo TitleTextFont = GetEmbossedTextFont();
	TitleTextFont.Size = 30;
	
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Advance Deletion")))
			.Font(TitleTextFont)
			.Justification(ETextJustify::Center)
			.ColorAndOpacity(FColor::White)
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				ConstructComboBox()
			]

			// Help text for Combo box slot
			+SHorizontalBox::Slot()
			.FillWidth(.6f)
			[
				ConstructComboHelpTexts(
		TEXT("Specify the listing condition in the drop. Left mouse button click to go to where asset is located"),
				ETextJustify::Center)
			]

			// Help text for Combo box slot
			+SHorizontalBox::Slot()
			.FillWidth(.1f)
			[
				ConstructComboHelpTexts(
			TEXT("Current Folder:\n") + InArgs._CurrentSelectedFolder,
				ETextJustify::Right)
			]
		]
		
		+SVerticalBox::Slot()
		.VAlign(VAlign_Fill) // 이 값으로 설정해야 스크롤이 작동  	
		[
			SNew(SScrollBox)

			+SScrollBox::Slot()
			[
				ConstructAssetListView()
			]
		]
		
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteAllButton()
			]

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructSelectAllButton()
			]

			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeselectAllButton()
			]
		]
	];
}

void SAdvanceDeletionTab::RefreshAssetListView()
{
	CheckBoxesArray.Empty();
	AssetsDataToDeleteArray.Empty();
	
	if(ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
	}
}


#pragma region ComboBoxForListingCondition

TSharedRef<SComboBox<TSharedPtr<FString>>> SAdvanceDeletionTab::ConstructComboBox()
{
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox
	=SNew(SComboBox<TSharedPtr<FString>>)
	.OptionsSource(&ComboBoxSourceItems)
	.OnGenerateWidget(this, &SAdvanceDeletionTab::OnGenerateComboContent)
	.OnSelectionChanged(this,&SAdvanceDeletionTab::OnComboSelectionChanged)
	[
		SAssignNew(ComboDisplayTextBlock,STextBlock)
		.Text(FText::FromString(TEXT("List Assets Option")))
	];

	return ConstructedComboBox;
}

TSharedRef<SWidget> SAdvanceDeletionTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<STextBlock> ConstructedComboText = SNew(STextBlock).
	Text(FText::FromString(*SourceItem.Get()));

	return ConstructedComboText;
}

void SAdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)
{
	DebugHeader::Print(*SelectedOption.Get(),FColor::Cyan);

	ComboDisplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));

	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	// Pass data for our module to filter based on the selected option
	if(*SelectedOption.Get() == ListALL)
	{
		// List all sstored asset data
		DisplayedAssetsData = StoredAssetsData;
		RefreshAssetListView();
		
	}
	else if(*SelectedOption.Get() == ListUnused)
	{
		// List all unused assets
		SuperManagerModule.ListUnusedAssetsForAssetList(StoredAssetsData,DisplayedAssetsData);
		RefreshAssetListView();
	}
	else if(*SelectedOption.Get() == ListSameName)
	{
		// List out all assets with same name
		SuperManagerModule.ListSameNameAssetsForAssetList(StoredAssetsData,DisplayedAssetsData);
		RefreshAssetListView();
	}
}

#pragma endregion

#pragma region RowWidgetForAssetListView

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructComboHelpTexts(const FString& TextContent,
	ETextJustify::Type TextJustify)
{
	TSharedRef<STextBlock> ConstructedHelpText =
	SNew(STextBlock)
	.Text(FText::FromString(TextContent))
	.Justification(TextJustify)
	.AutoWrapText(true);

	return ConstructedHelpText;
}

TSharedRef<ITableRow> SAdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
                                                                const TSharedRef<STableViewBase>& OwnerTable)
{
	if(!AssetDataToDisplay.IsValid()) return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);
	
	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString();

	FSlateFontInfo AssetClassNameFont = GetEmbossedTextFont();
	AssetClassNameFont.Size = 10;

	FSlateFontInfo AssetNameFont = GetEmbossedTextFont();
	AssetNameFont.Size = 15;
	
	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =
	SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable).Padding(FMargin(6.f))
	[
		SNew(SHorizontalBox)

		// 체크박스
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)	
		.VAlign(VAlign_Center)   
		.FillWidth(.05f) 
		[
			ConstructCheckBox(AssetDataToDisplay)			
		]
		// 에셋 클래스 보여주기
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Fill)
		.FillWidth(.55f)
		[
			ConstructTextForRowWidget(DisplayAssetClassName, AssetClassNameFont)
		]
		// 에셋 이름 보여주기
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Fill)
		[
			ConstructTextForRowWidget(DisplayAssetName,AssetNameFont)	
		]
		
		// 버튼 보여주기
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Fill)
		[
			ConstructButtonForRowWidget(AssetDataToDisplay)
		]
	];
	
	return ListViewRowWidget;
}

void SAdvanceDeletionTab::OnRowWidgetMouseButtonClicked(TSharedPtr<FAssetData> ClickedData)
{
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	SuperManagerModule.SyncCBToClickedAssetForAssetList(ClickedData->GetSoftObjectPath().ToString());
}

TSharedRef<SCheckBox> SAdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SCheckBox> ConstructedCheckBox =
	SNew(SCheckBox) // 체크박스 생성
	.Type(ESlateCheckBoxType::CheckBox) // 체크박스 타입
	.OnCheckStateChanged(this,&SAdvanceDeletionTab::OnCheckBoxStateChanged,AssetDataToDisplay)
	.Visibility(EVisibility::Visible); // 가시성

	CheckBoxesArray.Add(ConstructedCheckBox);

	return ConstructedCheckBox;
}

// 체크박스 상태가 변할 때 호출되는 함수
void SAdvanceDeletionTab::OnCheckBoxStateChanged(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	switch (NewState)
	{
	case ECheckBoxState::Unchecked:
		if(AssetsDataToDeleteArray.Contains(AssetData))
		{
			AssetsDataToDeleteArray.Remove(AssetData);
		}
		break;
	case ECheckBoxState::Checked:
		AssetsDataToDeleteArray.AddUnique(AssetData);
		break;
	case ECheckBoxState::Undetermined:
		break;
	}
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent,
	const FSlateFontInfo& FontToUse)
{
	TSharedRef<STextBlock> ConstructedTextBlock = 
	SNew(STextBlock)
	.Text(FText::FromString(TextContent))
	.Font(FontToUse)
	.ColorAndOpacity(FColor::White);

	return ConstructedTextBlock;
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData>& AssetDataToDisplay)
{
	TSharedRef<SButton> ConstructedButton = SNew(SButton)
	.Text(FText::FromString(TEXT("Delete")))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeleteButtonClicked, AssetDataToDisplay);

	return ConstructedButton;
}

FReply SAdvanceDeletionTab::OnDeleteButtonClicked(TSharedPtr<FAssetData> ClickedAssetData)
{
	// 모듈 불러오기
	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));

	// 모듈에서 에셋을 삭제
	const bool BAssetDeleted = SuperManagerModule.DeleteSingleAssetForAssetList(*ClickedAssetData.Get());
	
	if(BAssetDeleted)
	{
		// Updating the list source items
		if(StoredAssetsData.Contains(ClickedAssetData))
		{
			StoredAssetsData.Remove(ClickedAssetData);
		}

		if(DisplayedAssetsData.Contains(ClickedAssetData))
		{
			DisplayedAssetsData.Remove(ClickedAssetData);
		}
		// Refresh the list
		RefreshAssetListView();
	}
	 
 	return FReply::Handled();	
}

TSharedRef<SListView<TSharedPtr<FAssetData>>> SAdvanceDeletionTab::ConstructAssetListView()
{
	ConstructedAssetListView = SNew(SListView<TSharedPtr<FAssetData>>)
	.ItemHeight(24.f)	
	.ListItemsSource(&DisplayedAssetsData)
	.OnGenerateRow(this,&SAdvanceDeletionTab::OnGenerateRowForList)
	.OnMouseButtonClick(this,&SAdvanceDeletionTab::OnRowWidgetMouseButtonClicked);

	// TSharedPtr -> TSharedRef 변환하는 함수
	return ConstructedAssetListView.ToSharedRef(); 
}

#pragma endregion

#pragma region TabButtons

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeleteAllButton()
{
	TSharedRef<SButton> DeleteButton = SNew(SButton)
	.ContentPadding(FMargin(5.f))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeleteAllButtonClicked);

	DeleteButton->SetContent(ConstructTextForTabButtons(TEXT("Delete All")));

	return DeleteButton;
}

FReply SAdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	if(AssetsDataToDeleteArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("No Asset currently selected"));
		return FReply::Handled();
	}

	TArray<FAssetData> AssetDataToDelete;

	for(const TSharedPtr<FAssetData>& Data : AssetsDataToDeleteArray)
	{
		AssetDataToDelete.Add(*Data.Get());
	}
	// Pass data to our module for deletion

	FSuperManagerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperManagerModule>(TEXT("SuperManager"));
	const bool bAssetsDeleted = SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetDataToDelete);

	if(bAssetsDeleted)
	{
		for(const TSharedPtr<FAssetData>& DeletedData : AssetsDataToDeleteArray)
		{
			// Updating the stored assets data
			if(StoredAssetsData.Contains(DeletedData))
			{
				StoredAssetsData.Remove(DeletedData);
			}

			if(DisplayedAssetsData.Contains(DeletedData))
			{
				DisplayedAssetsData.Remove(DeletedData);
			}
		}

		RefreshAssetListView();
	}
	
	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructSelectAllButton()
{
	TSharedRef<SButton> SelectAllButton = SNew(SButton)
	.ContentPadding(FMargin(5.f))
	.OnClicked(this,&SAdvanceDeletionTab::OnSelectAllButtonClicked);

	SelectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Select All")));

	return SelectAllButton;
}

FReply SAdvanceDeletionTab::OnSelectAllButtonClicked()
{
	if(CheckBoxesArray.Num() == 0) return FReply::Handled();

	for(const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
	{
		if(!CheckBox->IsChecked())
		{
			// 체크되지 않는 체크박스에 대해 체크로 만드는 함수
			CheckBox->ToggleCheckedState();				
		}
	}
	
	return FReply::Handled();
}

TSharedRef<SButton> SAdvanceDeletionTab::ConstructDeselectAllButton()
{
	TSharedRef<SButton> DeselectAllButton = SNew(SButton)
	.ContentPadding(FMargin(5.f))
	.OnClicked(this,&SAdvanceDeletionTab::OnDeselectButtonClicked);

	DeselectAllButton->SetContent(ConstructTextForTabButtons(TEXT("Deselect All")));

	return DeselectAllButton;
}

FReply SAdvanceDeletionTab::OnDeselectButtonClicked()
{
	if(CheckBoxesArray.Num() == 0) return FReply::Handled();

	for(const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
	{
		if(CheckBox->IsChecked())
		{
			// 체크박스의 상태를 반대로 만드는 함수
			CheckBox->ToggleCheckedState();				
		}
	}
	
	return FReply::Handled();
}

TSharedRef<STextBlock> SAdvanceDeletionTab::ConstructTextForTabButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetEmbossedTextFont();
	ButtonTextFont.Size = 15;
	
	
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
	.Text(FText::FromString(TextContent))
	.Font(ButtonTextFont)
	.Justification(ETextJustify::Center);

	return ConstructedTextBlock;
}

#pragma endregion
