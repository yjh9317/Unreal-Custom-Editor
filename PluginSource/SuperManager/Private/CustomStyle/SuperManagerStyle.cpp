// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomStyle/SuperManagerStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/StyleColors.h"

FName FSuperManagerStyle::StyleSetName = FName("SuperManagerStyle");
TSharedPtr<FSlateStyleSet> FSuperManagerStyle::CreatedSlateStyleSet = nullptr;

void FSuperManagerStyle::InitializeIcons()
{
	if(!CreatedSlateStyleSet.IsValid())
	{
		CreatedSlateStyleSet = CreateSlateStyleSet();
		FSlateStyleRegistry::RegisterSlateStyle(*CreatedSlateStyleSet);
	}
}

TSharedRef<FSlateStyleSet> FSuperManagerStyle::CreateSlateStyleSet()
{
	TSharedRef<FSlateStyleSet> CustomStyleSet = MakeShareable(new FSlateStyleSet(StyleSetName)); // Slate Style 생성

	// 플러그인 경로를 찾고 그다음 Icon이 있는 경로 찾기
	const FString IconDirectory = IPluginManager::Get().FindPlugin(TEXT("SuperManager"))->GetBaseDir() / "Resources";

	CustomStyleSet->SetContentRoot(IconDirectory); // 루트 설정

	const FVector2D Icon16x16 (16.f,16.f);
	CustomStyleSet->Set("ContentBrowser.DeleteUnusedAssets",
		new FSlateImageBrush(IconDirectory/"DeleteUnusedAsset.png",Icon16x16)); // 이미지 설정(경로,크기)

	CustomStyleSet->Set("ContentBrowser.DeleteEmptyFolders",
	new FSlateImageBrush(IconDirectory/"DeleteEmptyFolders.png",Icon16x16)); // 이미지 설정(경로,크기)

	CustomStyleSet->Set("ContentBrowser.AdvanceDeletion",
	new FSlateImageBrush(IconDirectory/"AdvanceDeletion.png",Icon16x16)); // 이미지 설정(경로,크기)

	CustomStyleSet->Set("LevelEditor.LockSelection",
	new FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16)); // 이미지 설정(경로,크기)

	CustomStyleSet->Set("LevelEditor.UnlockSelection",
	new FSlateImageBrush(IconDirectory/"SelectionUnlock.png",Icon16x16)); // 이미지 설정(경로,크기)

	const FCheckBoxStyle& SelectionLockToggleButton = FCheckBoxStyle()
	.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
	.SetPadding(FMargin(10.f))
	// UnCheck images
	.SetUncheckedImage(FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16, FStyleColors::White25))
	.SetUncheckedHoveredImage(FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16, FStyleColors::AccentBlue))
	.SetUncheckedPressedImage(FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16, FStyleColors::Foreground))
	// Check images
	.SetCheckedImage(FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16, FStyleColors::Foreground))
	.SetCheckedHoveredImage(FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16, FStyleColors::AccentBlack))
	.SetCheckedPressedImage(FSlateImageBrush(IconDirectory/"SelectionLock.png",Icon16x16, FStyleColors::AccentGray));

	CustomStyleSet->Set("SceneOutliner.SelectionLock",SelectionLockToggleButton);
	return CustomStyleSet;
}

void FSuperManagerStyle::ShutDown()
{
	if(CreatedSlateStyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*CreatedSlateStyleSet);
		CreatedSlateStyleSet.Reset();
	}
}


