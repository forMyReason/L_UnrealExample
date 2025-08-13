// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "SPivotToolEditor.h"
#include "Toolkits/BaseToolkit.h"

class FPivotToolEdModeToolkit : public FModeToolkit
{
public:

	virtual void Init(const TSharedPtr< class IToolkitHost >& InitToolkitHost) override;

	FPivotToolEdModeToolkit();

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return PivotToolEditorWidget; }

	void SaveUISetting(UPivotToolUISetting* InUISetting);
	void LoadUISetting(const UPivotToolUISetting* InUISetting);

protected:
	
private:
	TSharedPtr<SPivotToolEditor> PivotToolEditorWidget;
};
