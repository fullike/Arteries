// Fill out your copyright notice in the Description page of Project Settings.

#include "SAssetBrowser.h"
#include "SlateOptMacros.h"

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
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Doors/Door01")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Doors/Door02")));
	Data.Add(LoadMainAsset<AArteriesActor>(TEXT("/Game/ArteriesBP/Stairs/Stairs01")));

	ChildSlot
	[
		SNew( SVerticalBox )
		+ SVerticalBox::Slot()
		[
			SAssignNew(TileView,STileView<AArteriesActor*>)
			.ListItemsSource(&Data)
			.OnGenerateTile(this, &SAssetBrowser::MakeDataTile)
		//	.OnContextMenuOpening(this, &SLib3dWorldBrowser::OnContextMenuOpening)
			.ItemWidth(128)
			.ItemHeight(176)
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
TSharedRef<ITableRow> SAssetBrowser::MakeDataTile(AArteriesActor* Item, const TSharedRef<STableViewBase>& Owner)
{
	TSharedPtr< STableRow<AArteriesActor*> > TableRowWidget;
	SAssignNew( TableRowWidget, STableRow<AArteriesActor*>, Owner )
	.Style(&FTableRowStyle::GetDefault())
	.Cursor( EMouseCursor::GrabHand );
//	.OnDragDetected( this, &SLib3dWorldBrowser::OnDraggingAssetItem );
	
	TSharedPtr<SVerticalBox> Box;
	SAssignNew( Box,SVerticalBox )
	+ SVerticalBox::Slot()
//	.AutoHeight()
	[
		SNew(SImage)
		.Image(this, &SAssetBrowser::GetThumbnail, Item)
	]
	+ SVerticalBox::Slot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(FText::FromString(Item->GetName()))
	];
	TableRowWidget->SetContent(Box.ToSharedRef());
	return TableRowWidget.ToSharedRef();
}
const FSlateBrush* SAssetBrowser::GetThumbnail(AArteriesActor* Item) const
{
	return NULL;
}
#undef LOCTEXT_NAMESPACE
