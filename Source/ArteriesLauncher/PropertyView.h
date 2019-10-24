// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "PropertyNode.h"
/*-----------------------------------------------------------------------------
SLib3dExporterDialog
-----------------------------------------------------------------------------*/

class SPropertyView : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_TwoParams(FOnPropertyChanged, FPropertyNode*/* Property */, uint8*/* Data */)
	DECLARE_DELEGATE_RetVal_TwoParams(TSharedPtr<SWidget>, FOnContextMenuOpened, FPropertyNode*/* Property */, uint8*/* Data */)
public:
	SLATE_BEGIN_ARGS(SPropertyView){}
		SLATE_EVENT(FOnPropertyChanged, OnPropertyChanged)
		SLATE_EVENT(FOnContextMenuOpened, OnContextMenuOpened)
	SLATE_END_ARGS()
	/** SWidget functions */
	void Construct(const FArguments& InArgs);
	TSharedPtr<SWidget> OnContextMenuOpened();
	void OnPropertyChanged(FPropertyNode* PropertyNode);

	TSharedRef<ITableRow> GenerateRow(TSharedRef<FPropertyNode> Node, const TSharedRef<STableViewBase>& OwnerTable);
	void GetPropertyChildren(TSharedRef<FPropertyNode> Node, FPropertyNodeList& OutChildren);
	
	float OnGetLeftColumnWidth() const { return 1.0f - ColumnWidth; }
	float OnGetRightColumnWidth() const { return ColumnWidth; }
	void OnSetColumnWidth(float InWidth) { ColumnWidth = InWidth; }
	
	void Refresh();

	void SetObject(UObject* Object);
	void SetStructureData(UStruct* Struct, uint8* Data);
	FPropertyNodeList RootNodeList;
	TSharedPtr<SPropertyTree> PropertyTree;
	FPropertyColumnSizeData ColumnSizeData;
	FOnPropertyChanged PropertyChanged;
	FOnContextMenuOpened ContextMenuOpened;
	UStruct* RootStruct;
	uint8* RootData;
	float ColumnWidth = 0.5f;
};
