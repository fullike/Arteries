// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STreeView.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Classes/Engine/AssetManager.h"

struct FPropertyColumnSizeData
{
	TAttribute<float> LeftColumnWidth;
	TAttribute<float> RightColumnWidth;
	SSplitter::FOnSlotResized OnWidthChanged;
	void SetColumnWidth(float InWidth) { OnWidthChanged.ExecuteIfBound(InWidth); }
};
class FPropertyNode;
class SPropertyView;

#define DECLARE_PROPERTY_NODE(NodeType) \
	static TSharedRef<FPropertyNode> Create(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData){return MakeShareable(new NodeType(InView, InParent, InProperty, InData));}	\
	NodeType(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData);


typedef TSharedRef<FPropertyNode>(*CreatePropertyNodeFunc)(SPropertyView*, FPropertyNode*, UProperty*, uint8*);
typedef TArray<TSharedRef<FPropertyNode>> FPropertyNodeList;
typedef STreeView<TSharedRef<FPropertyNode>> SPropertyTree;

class FPropertyNode : public TSharedFromThis<FPropertyNode>
{
public:
	static TMap<UClass*, CreatePropertyNodeFunc> CreateFunc;
	FPropertyNode() : View(NULL), Parent(NULL), Property(NULL), Data(NULL){}
	FPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData):View(InView),Parent(InParent),Property(InProperty),Data(InData) {}
	virtual ~FPropertyNode() {}
	virtual FName GetName();
	virtual TSharedRef<SWidget> CreateName();
	virtual TSharedRef<SWidget> CreateValue();
	virtual bool HasMultipleColumns() { return true; }
	void OnPropertyChanged();
	FName AsName();
	int32 GetDepthToParent(const FName& Name);
	SPropertyView* View;
	FPropertyNode* Parent;
	TSharedPtr<FPropertyNode> MapKey;
	FPropertyNodeList Children;
	UProperty* Property;
	uint8* Data;
};
class FArrayPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FArrayPropertyNode)
};
class FMapPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FMapPropertyNode)
};
class FSetPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FSetPropertyNode)
};
class FStructPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FStructPropertyNode)
};
class FObjectPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FObjectPropertyNode)
	void SetValue(UObject* Object);
	UObject* GetValue() const;
	virtual TSharedRef<SWidget> CreateValue();
	TArray<TSharedPtr<FAssetData>> DataSource;
};
class FBoolPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FBoolPropertyNode)
	void SetValue(ECheckBoxState NewCheckState);
	ECheckBoxState GetValue() const;
	virtual TSharedRef<SWidget> CreateValue();
};
template<typename NumericType>
class FNumberPropertyNode : public FPropertyNode
{
public:
	static TSharedRef<FPropertyNode> Create(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) { return MakeShareable(new FNumberPropertyNode<NumericType>(InView, InParent, InProperty, InData)); }
	FNumberPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
	{
	}
	NumericType GetValue() const
	{
		return *(NumericType*)Data;
	}
	void SetValue(NumericType NewValue, ETextCommit::Type TextCommitType)
	{
		*(NumericType*)Data = NewValue;
		OnPropertyChanged();
	}
	virtual TSharedRef<SWidget> CreateValue()
	{
		//SNumericEntryBox
		return SNew(SSpinBox<NumericType>)
			.MinValue(TOptional<NumericType>())
			.MaxValue(TOptional<NumericType>())
			.Value(this, &FNumberPropertyNode<NumericType>::GetValue)
			.OnValueCommitted(this, &FNumberPropertyNode<NumericType>::SetValue);
	}
};
class FTextPropertyNode : public FPropertyNode
{
public:
	DECLARE_PROPERTY_NODE(FTextPropertyNode)
	FText GetValue() const;
	void SetValue(const FText& InText, ETextCommit::Type InCommitType);
	virtual TSharedRef<SWidget> CreateValue();
};
