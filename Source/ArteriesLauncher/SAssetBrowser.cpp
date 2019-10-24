// Fill out your copyright notice in the Description page of Project Settings.

#include "SAssetBrowser.h"
#include "SlateOptMacros.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

template< class T >
inline T* LoadMainAsset(const TCHAR* Name)
{
	UPackage* Package = LoadPackage(nullptr, Name, LOAD_None);
	return Package ? static_cast<T*>(FindObjectWithOuter(Package, T::StaticClass())) : NULL;
}

#define LOCTEXT_NAMESPACE "ArteriesLauncher"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAssetBrowser::Construct(const FArguments& InArgs)
{
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Windows/Window01")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Stairs/Stairs01")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Doors/Door01")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Doors/Door02")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Walls/Wall")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/House")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/FlowerBed")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Blocks")));
	ChildSlot
	[
		SNew( SVerticalBox )
		+ SVerticalBox::Slot()
		[
			SAssignNew(TileView,STileView<AArteriesActor*>)
			.ListItemsSource(&Data)
			.OnGenerateTile(this, &SAssetBrowser::MakeDataTile)
		//	.OnContextMenuOpening(this, &SLib3dWorldBrowser::OnContextMenuOpening)
			.ItemWidth(144)
			.ItemHeight(176)
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
TSharedRef<ITableRow> SAssetBrowser::MakeDataTile(AArteriesActor* Item, const TSharedRef<STableViewBase>& Owner)
{
	TSharedPtr< STableRow<AArteriesActor*> > TableRowWidget;
	SAssignNew( TableRowWidget, STableRow<AArteriesActor*>, Owner )
	.Style(&FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row"))
	.Cursor( EMouseCursor::GrabHand );
//	.OnDragDetected( this, &SLib3dWorldBrowser::OnDraggingAssetItem );
	
	UPackage* Package = Item->GetOutermost();
	FString ShortName = FPaths::GetCleanFilename(Package->GetName());

	LoadThumbnail(Item);


	TSharedPtr<SBorder> Border;
	SAssignNew(Border, SBorder)
	.Padding(8)
	.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
	.BorderBackgroundColor(FLinearColor(1, 1, 1, 0.45f))
	[

	SNew( SVerticalBox )
	+ SVerticalBox::Slot()
//	.AutoHeight()
	[
		SNew(SImage)
		.Image(Brushes[Item].Get())
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(FText::FromString(ShortName))
	]
	];
	TableRowWidget->SetContent(Border.ToSharedRef());
	return TableRowWidget.ToSharedRef();
}
void SAssetBrowser::LoadThumbnail(AArteriesActor* Item)
{
	if (!Brushes.Contains(Item))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>("ImageWrapper");
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

		UPackage* Package = Item->GetOutermost();
		FString ShortName = FPaths::GetCleanFilename(Package->GetName());


		TArray<uint8> Content;
		FFileHelper::LoadFileToArray(Content, *(ShortName + TEXT(".jpg")));
		if (!ImageWrapper->SetCompressed(Content.GetData(), Content.Num()))
			return;
		// get the raw image data
		const TArray<uint8>* RawImageData = nullptr;
		if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawImageData) || RawImageData == nullptr)
			return;
		// make a dynamic image
		FName ResourceName(*Item->GetName());
		Brushes.Add(Item, FSlateDynamicImageBrush::CreateWithImageData(ResourceName, FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()), *RawImageData));
	}
}
#undef LOCTEXT_NAMESPACE
