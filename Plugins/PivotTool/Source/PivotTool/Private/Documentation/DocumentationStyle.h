// Copyright 2017-2021 marynate. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/ISlateStyle.h"
#include "Styling/SlateStyle.h"
#include "IDocumentation.h"

#include "DocumentationDefines.h"

namespace EXT_DOC_NAMESPACE
{

class FExtDocumentationStyle
{
public:
	static void Initialize();

	static void Shutdown();

	static TSharedPtr< class ISlateStyle > Get();

	static FName GetStyleSetName();

	static FDocumentationStyle GetDefaultDocumentationStyle();
	static FDocumentationStyle GetChangLogDocumentationStyle();

private:
	static FString InContent(const FString& RelativePath, const ANSICHAR* Extension);

private:
	static TSharedPtr< class FSlateStyleSet > StyleSet;
};

}
