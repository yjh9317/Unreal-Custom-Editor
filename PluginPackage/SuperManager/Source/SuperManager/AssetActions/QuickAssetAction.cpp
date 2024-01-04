#include "QuickAssetAction.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Misc/MessageDialog.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"



void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if(NumOfDuplicates <= 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 Counter = 0;
	
	for(const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for(int32 i = 0; i < NumOfDuplicates; i++)
		{
			const FString SourceAssetPath = SelectedAssetData.GetSoftObjectPath().ToString();
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i);
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(),NewDuplicatedAssetName);

			if(UEditorAssetLibrary::DuplicateAsset(SourceAssetPath,NewPathName)) // 복사
			{
				UEditorAssetLibrary::SaveAsset(NewPathName, false);	// 저장
				++Counter;
			}
		}
	}

	if(Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + "files"));
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;
	
	for(UObject* SelectedObject : SelectedObjects)
	{
		if(!SelectedObject) continue;

		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());
		
		if(!PrefixFound || PrefixFound->IsEmpty())
		{
			DebugHeader::Print(TEXT("Failed to find prefix for class") + SelectedObject->GetClass()->GetName(),FColor::Red);
			continue;
		}

		FString OldName = SelectedObject->GetName();

		// StartsWith 함수는 특정 문자열로 시작하는지 확인하는 함수
		if(OldName.StartsWith(*PrefixFound))
		{
			DebugHeader::Print(OldName + TEXT(" already has prefix added"),FColor::Red);
			continue;
		}

		if(SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
		}

		const FString NewNameWithPrefix = *PrefixFound + OldName;

		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewNameWithPrefix);

		++Counter;
	}

	if(Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully rename " + FString::FromInt(Counter)));
	}
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	FixupRedirectors();
	
	for(const FAssetData& SelecetedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferencers =  UEditorAssetLibrary::FindPackageReferencersForAsset(SelecetedAssetData.GetSoftObjectPath().ToString());

		if(AssetReferencers.Num() == 0)
		{
			UnusedAssetsData.Add(SelecetedAssetData);
		}
	}

	if(UnusedAssetsData.Num() == 0 )
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused asset found among seleceted assets"), false);
		return;
	}

	const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);

	if(NumOfAssetsDeleted == 0) return;

	DebugHeader::ShowNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(NumOfAssetsDeleted) + TEXT(" unused assets"));
}

void UQuickAssetAction::FixupRedirectors()
{
	TArray<UObjectRedirector*> RedirectorsToFixArray;

	FAssetRegistryModule& AssetRegistryModule = 
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Emplace("/Game");
	Filter.ClassPaths.Emplace("ObjectRedirector");

	TArray<FAssetData> OutRedirectors;
	AssetRegistryModule.Get().GetAssets(Filter,OutRedirectors);

	for(const FAssetData& RedirectorData : OutRedirectors)
	{
		if(UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
		{
			RedirectorsToFixArray.Add(RedirectorToFix);
		}
	}
	FAssetToolsModule& AssetToolsModule = 
	FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
