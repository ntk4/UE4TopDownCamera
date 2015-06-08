// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

#pragma once

namespace EGameKey
{
	enum Type
	{
		Tap,
		Hold,
		Swipe,
		SwipeTwoPoints,
		Pinch,
	};
}

DECLARE_DELEGATE_RetVal(bool, FActionButtonDelegate);
DECLARE_DELEGATE_RetVal(FText, FGetQueueLength)

struct FActionButtonData
{
	FText StrButtonText;
	FText StrTooltip;
	EVisibility Visibility;
	bool bIsEnabled;
	int32 ActionCost;
	UTexture2D*	ActionIcon;
	FActionButtonDelegate TriggerDelegate;
	FGetQueueLength GetQueueLengthDelegate;

	FActionButtonData()
	{
		StrButtonText = FText::GetEmpty();
		StrTooltip = FText::GetEmpty();
		bIsEnabled = true;
		ActionCost = 0;
		Visibility = EVisibility::Hidden;
		ActionIcon = NULL;
	}
};
