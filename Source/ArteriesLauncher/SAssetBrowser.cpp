// Fill out your copyright notice in the Description page of Project Settings.

#include "SAssetBrowser.h"
#include "SlateOptMacros.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Editor/EditorStyle/Public/EditorStyle.h"

#define LOCTEXT_NAMESPACE "ArteriesLauncher"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
SAssetBrowser::SAssetBrowser() :Actor(NULL)
{
	FModuleManager::LoadModuleChecked<IEditorStyleModule>("EditorStyle");
}
void SAssetBrowser::Construct(const FArguments& InArgs)
{
	auto AddData = [&](TCHAR* Path)
	{
		UBlueprintGeneratedClass* BP = LoadMainAsset<UBlueprintGeneratedClass>(Path);
		BP->AddToRoot();
		Data.Add(BP);
	};
	AddData(TEXT("/Game/ArteriesBP/Windows/Window01"));
	AddData(TEXT("/Game/ArteriesBP/Stairs/Stairs01"));
	AddData(TEXT("/Game/ArteriesBP/Doors/Door01"));
	AddData(TEXT("/Game/ArteriesBP/Doors/Door02"));
	AddData(TEXT("/Game/ArteriesBP/Walls/Wall"));
	AddData(TEXT("/Game/ArteriesBP/House"));
	AddData(TEXT("/Game/ArteriesBP/FlowerBed"));
	AddData(TEXT("/Game/ArteriesBP/Blocks"));
	ChildSlot
	[
		SNew(SSplitter)
		.Style(FEditorStyle::Get(), "DetailsView.Splitter")
		.PhysicalSplitterHandleSize(1.0f)
		.HitDetectionSplitterHandleSize(5.0f)
		.Orientation(EOrientation::Orient_Vertical)
		+ SSplitter::Slot()
		.Value(0.5f)
		[
			SAssignNew(TileView, STileView<UBlueprintGeneratedClass*>)
			.ListItemsSource(&Data)
			.OnGenerateTile(this, &SAssetBrowser::MakeDataTile)
			.OnSelectionChanged(this, &SAssetBrowser::OnTileViewSelectionChanged)
			//	.OnContextMenuOpening(this, &SLib3dWorldBrowser::OnContextMenuOpening)
			.ItemWidth(144)
			.ItemHeight(176)
		]
		+ SSplitter::Slot()
		.Value(0.5f)
		[
			SAssignNew(PropertyView, SPropertyView)
			.OnPropertyChanged(this, &SAssetBrowser::OnPropertyChanged)
		//	.OnContextMenuOpened(this, &SOSMControlPanel::OnOSMContextMenuOpened)
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
TSharedRef<ITableRow> SAssetBrowser::MakeDataTile(UBlueprintGeneratedClass* Item, const TSharedRef<STableViewBase>& Owner)
{
	TSharedPtr< STableRow<UBlueprint*> > TableRowWidget;
	SAssignNew( TableRowWidget, STableRow<UBlueprint*>, Owner )
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
void SAssetBrowser::LoadThumbnail(UBlueprintGeneratedClass* Item)
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
void SAssetBrowser::OnTileViewSelectionChanged(UBlueprintGeneratedClass* Item, ESelectInfo::Type SelectInfo)
{
	if (Actor)
	{
		GWorld->DestroyActor(Actor);
		Actor = NULL;
	}
	Actor = Cast<AArteriesActor>(GWorld->SpawnActor(Item, &FTransform::Identity));
	Actor->Build(true);
	PropertyView->SetObject(Actor);
}
void SAssetBrowser::OnPropertyChanged(FPropertyNode* PropertyNode, uint8* Data)
{
	Actor->Build(true);
}
#undef LOCTEXT_NAMESPACE
