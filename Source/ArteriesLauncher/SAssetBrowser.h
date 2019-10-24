// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Images/SImage.h"
#include "PropertyView.h"
#include "ArteriesActor.h"
/**
 * 
 */
class ARTERIESLAUNCHER_API SAssetBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetBrowser){}
	SLATE_END_ARGS()
	SAssetBrowser();
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	TSharedRef<ITableRow> MakeDataTile(UBlueprint* Item, const TSharedRef<STableViewBase>& Owner);
	void LoadThumbnail(UBlueprint* Item);
	void OnTileViewSelectionChanged(UBlueprint* Item, ESelectInfo::Type SelectInfo);
	void OnPropertyChanged(FPropertyNode* PropertyNode, uint8* Data);
private:
	AArteriesActor* Actor;
	TArray<UBlueprint*> Data;
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<SPropertyView> PropertyView;
	TSharedPtr<STileView<UBlueprint*>> TileView;
	TMap<UBlueprint*, TSharedPtr<FSlateDynamicImageBrush>> Brushes;
};
