// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class ISceneOutliner;
class ISceneOutlinerColumn;

class FSuperManagerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

#pragma region ContentBrowserMenuExtention

	void InitCBMenuExtention();

	TArray<FString> FolderPathSelected;
	TSharedRef<FExtender> CustomCBMenuExtender(const TArray<FString>& SelecetedPaths);

	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnDeleteUnusedAssetbuttonClicked();
	void OnDeleteEmptyFoldersButtonClicked();
	void OnAdvanceDeletionButtonClicked();

	void FixupRedirectors();
#pragma endregion 

#pragma region CustomEditorTab
	void RegisterAdvanceDeletionTab();

	TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& SpawnTabArgs);
	TSharedPtr<SDockTab> ConstructedDockTab;
	TArray<TSharedPtr<FAssetData>> GetAllAssetDataUnderSelectedFolder();

	void OnAdvanceDeletionTabClosed(TSharedRef<SDockTab> TabToClose);
#pragma endregion

#pragma region LevelEditorMenuExtension

	void InitLevelEditorExtension();

	TSharedRef<FExtender> CustomLevelEditorMenuExtender(const TSharedRef<FUICommandList> UICommandList, const TArray<AActor*> SelectedActors);

	void AddLevelEditorMenuEntry(class FMenuBuilder& MenuBuilder);

	void OnLockActorSelectionButtonClicked();
	void OnUnlockActorSelectionButtonClicked();
#pragma endregion 

#pragma region SelectionLock

	void InitCustomSelectionEvent();

	void OnActorSelected(UObject* SelectedObject);

	void LockActorSelection(AActor* ActorToProcess);
	void UnlockActorSelection(AActor* ActorToProcess);

	void RefreshSceneOutliner();

#pragma endregion 

#pragma region CustomEditorUICommands

	TSharedPtr<class FUICommandList> CustomUICommands;

	void InitCustomUICommands();

	void OnSelectionLockHotKeyPressed();
	void OnUnlockActorSelectionLockHotKeyPressed();

#pragma endregion
	
#pragma region SceneOutlinerExtension

	void InitSceneOutlinerColumnExtension();

	TSharedRef<ISceneOutlinerColumn> OnCreateSelectionLockColumn(ISceneOutliner& SceneOutliner);
	
#pragma endregion 
	
	TWeakObjectPtr<class UEditorActorSubsystem> WeakEditorActorSubsystem;

	bool GetEditorActorSubsystem();
public:

#pragma region ProcessDataForAdvanceDeletionTab
	
	bool DeleteSingleAssetForAssetList(const FAssetData& AssetDataToDelete);
	bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete);
	void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter,
									  TArray<TSharedPtr<FAssetData>>& OutUnusedAssetsData);
	void ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter,
									  TArray<TSharedPtr<FAssetData>>& OutSameNameAssetsData);
	void SyncCBToClickedAssetForAssetList(const FString& AssetPathToSync);
#pragma endregion

	bool CheckIsActorSelectionLocked(AActor* ActorToProcess);
	void ProcessLockingForOutliner(AActor* ActorToProcess, bool bShouldLock);
};
