// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomUICommands/SuperManagerUICommands.h"
#define LOCTEXT_NAMESPACE "FSuperManagerModule"

void FSuperManagerUICommands::RegisterCommands()
{
	UI_COMMAND(
		LockAfterSelection,
		"Lock Actor Selection",
		"Lock Actor Selection in Level, once triggered, actor can no longer be selected",
		EUserInterfaceActionType::Button,
		FInputChord(EKeys::W,EModifierKey::Alt)
		);

	UI_COMMAND(
	UnlockAfterSelection,
	"Unlock Actor Selection",
	"Remove	selection lock on all actors",
	EUserInterfaceActionType::Button,
	FInputChord(EKeys::W,EModifierKey::Alt | EModifierKey::Shift)
	);
}

#undef LOCTEXT_NAMESPACE