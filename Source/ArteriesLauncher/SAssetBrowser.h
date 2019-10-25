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
	TSharedRef<ITableRow> MakeDataTile(UBlueprintGeneratedClass* Item, const TSharedRef<STableViewBase>& Owner);
	void LoadThumbnail(UBlueprintGeneratedClass* Item);
	void OnTileViewSelectionChanged(UBlueprintGeneratedClass* Item, ESelectInfo::Type SelectInfo);
	void OnPropertyChanged(FPropertyNode* PropertyNode, uint8* Data);
private:
	AArteriesActor* Actor;
	TArray<UBlueprintGeneratedClass*> Data;
	TSharedPtr<FUICommandList> CommandList;
	TSharedPtr<SPropertyView> PropertyView;
	TSharedPtr<STileView<UBlueprintGeneratedClass*>> TileView;
	TMap<UBlueprintGeneratedClass*, TSharedPtr<FSlateDynamicImageBrush>> Brushes;
};

template< class T >
inline T* LoadMainAsset(const TCHAR* Name)
{
	UPackage* Package = LoadPackage(nullptr, Name, LOAD_None);
	return Package ? static_cast<T*>(FindObjectWithOuter(Package, T::StaticClass())) : NULL;
}