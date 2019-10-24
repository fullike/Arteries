#include "PropertyView.h"
#include "Editor/EditorStyle/Public/EditorStyle.h"

#define LOCTEXT_NAMESPACE "Lib3d"
class SPropertyNodeRow : public STableRow<TSharedPtr<FPropertyNode>>
{
public:
	SLATE_BEGIN_ARGS(SPropertyNodeRow) {}
		SLATE_ARGUMENT(FPropertyColumnSizeData*, ColumnSizeData)
		SLATE_ARGUMENT(FPropertyNode*, Node)
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable)
	{
		FPropertyNode* Node = InArgs._Node;
		TSharedPtr<SWidget> Widget;
		TSharedRef<SWidget> NameWidget = SNew(SHorizontalBox).Clipping(EWidgetClipping::OnDemand)
		+ SHorizontalBox::Slot()
		.Padding(2)
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.AutoWidth()
		[
			SNew(SExpanderArrow, SharedThis(this))
		]
		+ SHorizontalBox::Slot()
		.Padding(2)
		.FillWidth(1.0f)
		[
			Node->CreateName()
		];

		TSharedRef<SWidget> ValueWidget = SNew(SHorizontalBox).Clipping(EWidgetClipping::OnDemand)
		+ SHorizontalBox::Slot()
		.Padding(2)
		.FillWidth(1.0f)
		[
			Node->CreateValue()
		];




		if (Node->HasMultipleColumns())
		{
			Widget = SNew(SSplitter)
			.Style(FEditorStyle::Get(), "DetailsView.Splitter")
			.PhysicalSplitterHandleSize(1.0f)
			.HitDetectionSplitterHandleSize(5.0f)
			+ SSplitter::Slot()
			.Value(InArgs._ColumnSizeData->LeftColumnWidth)
			.OnSlotResized(SSplitter::FOnSlotResized::CreateSP(this, &SPropertyNodeRow::OnLeftColumnResized))
			[
				NameWidget
			]
			+ SSplitter::Slot()
			.Value(InArgs._ColumnSizeData->RightColumnWidth)
			.OnSlotResized(InArgs._ColumnSizeData->OnWidthChanged)
			[
				ValueWidget
			];
		}
		else
		{
			Widget = NameWidget;
		}
		ChildSlot
		[
			SNew(SBorder)
			.BorderImage(this, &SPropertyNodeRow::GetBorderImage)
			.Padding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
			[
				Widget.ToSharedRef()
			]
		];
		auto Args = STableRow::FArguments().Style(&FEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("SceneOutliner.TableViewRow"));
		STableRow<TSharedPtr<FPropertyNode>>::ConstructInternal(Args, InOwnerTable);
	}
	void OnLeftColumnResized(float InNewWidth) {}
	const FSlateBrush* GetBorderImage() const
	{
		/*bool UseDarkTheme = false;
		if (Customization->GetWidgetRow().IsDarkThemeAttr.IsSet())
		{
		UseDarkTheme = Customization->GetWidgetRow().IsDarkThemeAttr.Get();
		}
		if (IsHighlighted())
		{
			return FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Highlighted");
		}
		else if (IsHovered())
		{
			return FEditorStyle::GetBrush("DetailsView.CategoryMiddle_Hovered");
		}
		else*/
		{
			return FEditorStyle::GetBrush("DetailsView.CategoryMiddle");
		}
	}
};



void SPropertyView::Construct(const FArguments& InArgs)
{
	PropertyChanged = InArgs._OnPropertyChanged;
	ContextMenuOpened = InArgs._OnContextMenuOpened;
	if (!FPropertyNode::CreateFunc.Num())
	{
		FPropertyNode::CreateFunc.Add(UArrayProperty::StaticClass(), FArrayPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(UMapProperty::StaticClass(), FMapPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(USetProperty::StaticClass(), FSetPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(UStructProperty::StaticClass(), FStructPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(UObjectProperty::StaticClass(), FObjectPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(UBoolProperty::StaticClass(), FBoolPropertyNode::Create);

		FPropertyNode::CreateFunc.Add(UByteProperty::StaticClass(), FNumberPropertyNode<uint8>::Create);
		FPropertyNode::CreateFunc.Add(UUInt16Property::StaticClass(), FNumberPropertyNode<uint16>::Create);
		FPropertyNode::CreateFunc.Add(UUInt32Property::StaticClass(), FNumberPropertyNode<uint32>::Create);
		FPropertyNode::CreateFunc.Add(UUInt64Property::StaticClass(), FNumberPropertyNode<uint64>::Create);

		FPropertyNode::CreateFunc.Add(UInt8Property::StaticClass(), FNumberPropertyNode<int8>::Create);
		FPropertyNode::CreateFunc.Add(UInt16Property::StaticClass(), FNumberPropertyNode<int16>::Create);
		FPropertyNode::CreateFunc.Add(UIntProperty::StaticClass(), FNumberPropertyNode<int32>::Create);
		FPropertyNode::CreateFunc.Add(UInt64Property::StaticClass(), FNumberPropertyNode<int64>::Create);

		FPropertyNode::CreateFunc.Add(UFloatProperty::StaticClass(), FNumberPropertyNode<float>::Create);
		FPropertyNode::CreateFunc.Add(UDoubleProperty::StaticClass(), FNumberPropertyNode<double>::Create);

		FPropertyNode::CreateFunc.Add(UNameProperty::StaticClass(), FTextPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(UStrProperty::StaticClass(), FTextPropertyNode::Create);
		FPropertyNode::CreateFunc.Add(UTextProperty::StaticClass(), FTextPropertyNode::Create);
	}

	ColumnSizeData.LeftColumnWidth = TAttribute<float>(this, &SPropertyView::OnGetLeftColumnWidth);
	ColumnSizeData.RightColumnWidth = TAttribute<float>(this, &SPropertyView::OnGetRightColumnWidth);
	ColumnSizeData.OnWidthChanged = SSplitter::FOnSlotResized::CreateSP(this, &SPropertyView::OnSetColumnWidth);

	PropertyTree = SNew(SPropertyTree)
		.TreeItemsSource(&RootNodeList)
		.OnGenerateRow(this, &SPropertyView::GenerateRow)
		.OnGetChildren(this, &SPropertyView::GetPropertyChildren)
		.OnContextMenuOpening(this, &SPropertyView::OnContextMenuOpened)
		/*
		.HeaderRow(HeaderRow)
		.OnSelectionChanged(this, &FLib3dEditorToolkit::OnSelectionChanged)
		.OnContextMenuOpening(this, &FLib3dEditorToolkit::BuildMenuWidget)
		.SelectionMode(ESelectionMode::Multi)*/;

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("Docking.Background"))
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("DetailsView.CategoryTop"))
			[
				PropertyTree.ToSharedRef()
			]
		]
	];
}
void SPropertyView::OnPropertyChanged(FPropertyNode* PropertyNode)
{
	PropertyChanged.ExecuteIfBound(PropertyNode, RootData);
}
TSharedPtr<SWidget> SPropertyView::OnContextMenuOpened()
{
	TArray<TSharedRef<FPropertyNode>> Items = PropertyTree->GetSelectedItems();
	if (Items.Num())
	{
		TSharedRef<FPropertyNode> Item = Items[0];
		return ContextMenuOpened.Execute(&Item.Get(), RootData);
	}
	return SNullWidget::NullWidget;
}
TSharedRef<ITableRow> SPropertyView::GenerateRow(TSharedRef<FPropertyNode> Node, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SPropertyNodeRow, OwnerTable).Node(&Node.Get()).ColumnSizeData(&ColumnSizeData);
}
void SPropertyView::GetPropertyChildren(TSharedRef<FPropertyNode> Node, FPropertyNodeList& OutChildren)
{
	for (TSharedRef<FPropertyNode>& Child : Node->Children)
		OutChildren.Add(Child);
}
void SPropertyView::Refresh()
{
	RootNodeList.Empty();
	for (TFieldIterator<UProperty> It(RootStruct, EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		if (UProperty* StructMember = *It)
		{
			/*
			FName OutCategoryName(NAME_None);
			static const FName CategoryKey(TEXT("Category"));
			UPackage* Package = StructMember->GetOutermost();
			UMetaData* MetaData = Package->GetMetaData();
			if (MetaData->HasValue(StructMember, CategoryKey))
			OutCategoryName = FName(*MetaData->GetValue(StructMember, CategoryKey));
			*/
			UClass* Class = StructMember->GetClass();
			if (FPropertyNode::CreateFunc.Contains(Class))
				RootNodeList.Add(FPropertyNode::CreateFunc[Class](this, NULL, StructMember, RootData + StructMember->GetOffset_ForInternal()));
		}
	}
	PropertyTree->RequestTreeRefresh();
}
void SPropertyView::SetObject(UObject* Object)
{
	RootStruct = Object ? Object->GetClass() : NULL;
	RootData = (uint8*)Object;
	Refresh();
}
void SPropertyView::SetStructureData(UStruct* Struct, uint8* Data)
{
	RootStruct = Struct;
	RootData = Data;
	Refresh();

}
#undef LOCTEXT_NAMESPACE