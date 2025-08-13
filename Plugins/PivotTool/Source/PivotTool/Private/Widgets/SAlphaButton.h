// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "Widgets/Input/SButton.h"
#include "Engine/Texture2D.h"

class SAlphaButton : public SButton 
{
public:

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;

	virtual TSharedPtr<IToolTip> GetToolTip() override;
};
