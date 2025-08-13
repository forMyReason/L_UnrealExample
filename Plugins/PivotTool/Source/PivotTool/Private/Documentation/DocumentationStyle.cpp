// Copyright 2017-2021 marynate. All Rights Reserved.

#include "DocumentationStyle.h"

#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Application/SlateApplication.h"
#include "EditorStyleSet.h"

#define IMAGE_PLUGIN_BRUSH( RelativePath, ... ) FSlateImageBrush( FExtDocumentationStyle::InContent( RelativePath, ".png" ), __VA_ARGS__ )

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(((FSlateStyleSet*)(&FAppStyle::Get()))->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(((FSlateStyleSet*)(&FAppStyle::Get()))->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(((FSlateStyleSet*)(&FAppStyle::Get()))->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

FString FExtDocumentationStyle::InContent(const FString& RelativePath, const ANSICHAR* Extension)
{
	static FString ContentDir = IPluginManager::Get().FindPlugin(DocumentationHostPluginName)->GetContentDir();
	return (ContentDir / RelativePath) + Extension;
}

TSharedPtr< FSlateStyleSet > FExtDocumentationStyle::StyleSet = nullptr;
TSharedPtr< class ISlateStyle > FExtDocumentationStyle::Get() { return StyleSet; }

FName FExtDocumentationStyle::GetStyleSetName()
{
	static FName MyDocumentationStyleName(*DocumentationStyleSetName);
	return MyDocumentationStyleName;
}

FDocumentationStyle FExtDocumentationStyle::GetDefaultDocumentationStyle()
{
	FDocumentationStyle DocumentationStyle;
	{
		DocumentationStyle
			.ContentStyle(TEXT("Tutorials.Content.Text"))
			.BoldContentStyle(TEXT("Tutorials.Content.TextBold"))
			.NumberedContentStyle(TEXT("Tutorials.Content.Text"))
			.Header1Style(TEXT("Tutorials.Content.HeaderText1"))
			.Header2Style(TEXT("Tutorials.Content.HeaderText2"))
			.HyperlinkStyle(TEXT("Tutorials.Content.Hyperlink"))
			.HyperlinkTextStyle(TEXT("Tutorials.Content.HyperlinkText"))
			.SeparatorStyle(TEXT("Tutorials.Separator"));
	}
	return DocumentationStyle;
}

FDocumentationStyle FExtDocumentationStyle::GetChangLogDocumentationStyle()
{
	FDocumentationStyle DocumentationStyle;
	{
		DocumentationStyle
			.ContentStyle(TEXT("ChangeLog.Content.Text"))
			.BoldContentStyle(TEXT("ChangeLog.Content.TextBold"))
			.NumberedContentStyle(TEXT("ChangeLog.Content.Text"))
			.Header1Style(TEXT("ChangeLog.Content.HeaderText1"))
			.Header2Style(TEXT("ChangeLog.Content.HeaderText2"))

			.HyperlinkStyle(TEXT("Tutorials.Content.Hyperlink"))
			.HyperlinkTextStyle(TEXT("Tutorials.Content.HyperlinkText"))
			.SeparatorStyle(TEXT("Tutorials.Separator"));
	}
	return DocumentationStyle;
}

void FExtDocumentationStyle::Initialize()
{
	// Const icon sizes
	const FVector2D Icon8x8(8.0f, 8.0f);
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon28x28(28.0f, 28.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate")); 
	//StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin(DocumentationHostPluginName)->GetBaseDir() / TEXT("Resources"));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));

	const FTextBlockStyle& NormalText = FAppStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText");

	// Documentation
	{
		// Documentation tooltip defaults
		const FSlateColor HyperlinkColor(FLinearColor(0.1f, 0.1f, 0.5f));
		{
			const FTextBlockStyle DocumentationTooltipText = FTextBlockStyle(NormalText)
				.SetFont(DEFAULT_FONT("Regular", 9))
				.SetColorAndOpacity(FLinearColor::Black);
			StyleSet->Set("Documentation.SDocumentationTooltip", FTextBlockStyle(DocumentationTooltipText));

			const FTextBlockStyle DocumentationTooltipTextSubdued = FTextBlockStyle(NormalText)
				.SetFont(DEFAULT_FONT("Regular", 8))
				.SetColorAndOpacity(FLinearColor(0.1f, 0.1f, 0.1f));
			StyleSet->Set("Documentation.SDocumentationTooltipSubdued", FTextBlockStyle(DocumentationTooltipTextSubdued));

			const FTextBlockStyle DocumentationTooltipHyperlinkText = FTextBlockStyle(NormalText)
				.SetFont(DEFAULT_FONT("Regular", 8))
				.SetColorAndOpacity(HyperlinkColor);
			StyleSet->Set("Documentation.SDocumentationTooltipHyperlinkText", FTextBlockStyle(DocumentationTooltipHyperlinkText));

			const FButtonStyle DocumentationTooltipHyperlinkButton = FButtonStyle()
				.SetNormal(BORDER_BRUSH("Old/HyperlinkDotted", FMargin(0, 0, 0, 3 / 16.0f), HyperlinkColor))
				.SetPressed(FSlateNoResource())
				.SetHovered(BORDER_BRUSH("Old/HyperlinkUnderline", FMargin(0, 0, 0, 3 / 16.0f), HyperlinkColor));
			StyleSet->Set("Documentation.SDocumentationTooltipHyperlinkButton", FButtonStyle(DocumentationTooltipHyperlinkButton));
		}

		// Documentation defaults
		const FTextBlockStyle DocumentationText = FTextBlockStyle(NormalText)
			.SetColorAndOpacity(FLinearColor::Black)
			.SetFont(DEFAULT_FONT("Regular", 11));

		const FTextBlockStyle DocumentationHyperlinkText = FTextBlockStyle(DocumentationText)
			.SetColorAndOpacity(HyperlinkColor);

		const FTextBlockStyle DocumentationHeaderText = FTextBlockStyle(NormalText)
			.SetColorAndOpacity(FLinearColor::Black)
			.SetFont(DEFAULT_FONT("Black", 32));

		const FButtonStyle DocumentationHyperlinkButton = FButtonStyle()
			.SetNormal(BORDER_BRUSH("Old/HyperlinkDotted", FMargin(0, 0, 0, 3 / 16.0f), HyperlinkColor))
			.SetPressed(FSlateNoResource())
			.SetHovered(BORDER_BRUSH("Old/HyperlinkUnderline", FMargin(0, 0, 0, 3 / 16.0f), HyperlinkColor));

		StyleSet->Set("Documentation.Content", FTextBlockStyle(DocumentationText));

		const FHyperlinkStyle DocumentationHyperlink = FHyperlinkStyle()
			.SetUnderlineStyle(DocumentationHyperlinkButton)
			.SetTextStyle(DocumentationText)
			.SetPadding(FMargin(0.0f));
		StyleSet->Set("Documentation.Hyperlink", DocumentationHyperlink);

		StyleSet->Set("Documentation.Hyperlink.Button", FButtonStyle(DocumentationHyperlinkButton));
		StyleSet->Set("Documentation.Hyperlink.Text", FTextBlockStyle(DocumentationHyperlinkText));
		StyleSet->Set("Documentation.NumberedContent", FTextBlockStyle(DocumentationText));
		StyleSet->Set("Documentation.BoldContent", FTextBlockStyle(DocumentationText)
			.SetTypefaceFontName(TEXT("Bold")));

		StyleSet->Set("Documentation.Header1", FTextBlockStyle(DocumentationHeaderText)
			.SetFontSize(32));

		StyleSet->Set("Documentation.Header2", FTextBlockStyle(DocumentationHeaderText)
			.SetFontSize(24));

		StyleSet->Set("Documentation.Separator", new BOX_BRUSH("Common/Separator", 1 / 4.0f, FLinearColor(1, 1, 1, 0.5f)));
	}

	{
		//StyleSet->Set("Documentation.ToolTip.Background", new BOX_BRUSH("Tutorials/TutorialContentBackground", FMargin(4 / 16.0f)));
	}

	// Tutorial
	{
		const FTextBlockStyle RichTextNormal = FTextBlockStyle()
			.SetFont(DEFAULT_FONT("Regular", 11))
			.SetColorAndOpacity(FSlateColor::UseForeground())
			.SetShadowOffset(FVector2D::ZeroVector)
			.SetShadowColorAndOpacity(FLinearColor::Black)
			.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f))
			.SetHighlightShape(BOX_BRUSH("Common/TextBlockHighlightShape", FMargin(3.f / 8.f)));

		StyleSet->Set("Tutorials.Content.Text", RichTextNormal);

		StyleSet->Set("Tutorials.Content.TextBold", FTextBlockStyle(RichTextNormal)
			.SetFont(DEFAULT_FONT("Bold", 11)));

		StyleSet->Set("Tutorials.Content.HeaderText1", FTextBlockStyle(RichTextNormal)
			.SetFontSize(20));

		StyleSet->Set("Tutorials.Content.HeaderText2", FTextBlockStyle(RichTextNormal)
			.SetFontSize(16));

		const FButtonStyle RichTextHyperlinkButton = FButtonStyle()
			.SetNormal(BORDER_BRUSH("Old/HyperlinkDotted", FMargin(0, 0, 0, 3 / 16.0f), FLinearColor::Blue))
			.SetPressed(FSlateNoResource())
			.SetHovered(BORDER_BRUSH("Old/HyperlinkUnderline", FMargin(0, 0, 0, 3 / 16.0f), FLinearColor::Blue));

		const FTextBlockStyle RichTextHyperlinkText = FTextBlockStyle(RichTextNormal)
			.SetColorAndOpacity(FLinearColor::Blue);

		StyleSet->Set("Tutorials.Content.HyperlinkText", RichTextHyperlinkText);

		const FHyperlinkStyle RichTextHyperlink = FHyperlinkStyle()
			.SetUnderlineStyle(RichTextHyperlinkButton)
			.SetTextStyle(RichTextHyperlinkText)
			.SetPadding(FMargin(0.0f));
		StyleSet->Set("Tutorials.Content.Hyperlink", RichTextHyperlink);

		StyleSet->Set("Tutorials.Content.ExternalLink", new IMAGE_BRUSH("Tutorials/ExternalLink", Icon16x16, FLinearColor::Blue));

		StyleSet->Set("Tutorials.Separator", new BOX_BRUSH("Common/Separator", 1 / 4.0f, FLinearColor::Black));
	}

	// ChangLog
	{
		const FTextBlockStyle RichTextNormal = FTextBlockStyle()
			.SetFont(DEFAULT_FONT("Regular", 10))
			.SetColorAndOpacity(FSlateColor::UseForeground())
			.SetShadowOffset(FVector2D::ZeroVector)
			.SetShadowColorAndOpacity(FLinearColor::Black)
			.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f))
			.SetHighlightShape(BOX_BRUSH("Common/TextBlockHighlightShape", FMargin(3.f / 8.f)));

		const FLinearColor ColorChangeLogText(0.8f, 0.8f, 0.8f, 0.7f);
		StyleSet->Set("ChangeLog.Content.Text", FTextBlockStyle(RichTextNormal)
			.SetColorAndOpacity(ColorChangeLogText));

		StyleSet->Set("ChangeLog.Content.TextBold", FTextBlockStyle(RichTextNormal)
			.SetFont(DEFAULT_FONT("Bold", 10)));

		StyleSet->Set("ChangeLog.Content.HeaderText1", FTextBlockStyle(RichTextNormal)
			.SetFont(DEFAULT_FONT("Bold", 10)));

		StyleSet->Set("ChangeLog.Content.HeaderText2", FTextBlockStyle(RichTextNormal)
			.SetFont(DEFAULT_FONT("Bold", 11)));
	}

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};


void FExtDocumentationStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

#undef IMAGE_PLUGIN_BRUSH
#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef DEFAULT_FONT


#ifdef EXT_DOC_NAMESPACE
#undef EXT_DOC_NAMESPACE
#endif