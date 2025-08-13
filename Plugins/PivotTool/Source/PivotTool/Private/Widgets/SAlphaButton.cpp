// Copyright 2017-2021 marynate. All Rights Reserved.

#include "SAlphaButton.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"


FReply SAlphaButton::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!IsHovered())
	{
		return FReply::Unhandled();
	}
	return SButton::OnMouseButtonDown(MyGeometry, MouseEvent);
}

FReply SAlphaButton::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsHovered())
	{
		return FReply::Unhandled();
	}
	return SButton::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

FReply SAlphaButton::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!IsHovered())
	{
		return FReply::Unhandled();
	}
	return SButton::OnMouseButtonUp(MyGeometry, MouseEvent);
}

FCursorReply SAlphaButton::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const 
{
	if (!IsHovered())
	{
		return FCursorReply::Unhandled();
	}

	return SButton::OnCursorQuery(MyGeometry, CursorEvent);
}

TSharedPtr<IToolTip> SAlphaButton::GetToolTip()
{
	if (IsHovered())
	{
		return SWidget::GetToolTip();
	}
	return NULL;
}
