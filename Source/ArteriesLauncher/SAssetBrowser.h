// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"
#include "Widgets/Images/SImage.h"
#include "ArteriesActor.h"
/**
 * 
 */
class ARTERIESLAUNCHER_API SAssetBrowser : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetBrowser){}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	TSharedRef<ITableRow> MakeDataTile(AArteriesActor* Item, const TSharedRef<STableViewBase>& Owner);
	void LoadThumbnail(AArteriesActor* Item);
private:
	TArray<AArteriesActor*> Data;
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<STileView<AArteriesActor*>> TileView;
	TMap<AArteriesActor*, TSharedPtr<FSlateDynamicImageBrush>> Brushes;
};
