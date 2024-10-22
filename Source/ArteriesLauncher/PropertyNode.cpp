#include "PropertyNode.h"
#include "PropertyView.h"

TMap<UClass*, CreatePropertyNodeFunc> FPropertyNode::CreateFunc;
FArrayPropertyNode::FArrayPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData):FPropertyNode(InView,InParent,InProperty,InData)
{
	UArrayProperty* ArrayProperty = Cast<UArrayProperty>(Property);
	FScriptArrayHelper Helper(ArrayProperty, Data);
	for (int i = 0; i < Helper.Num(); i++)
	{
		Children.Add(CreateFunc[ArrayProperty->Inner->GetClass()](View, this, ArrayProperty->Inner, Helper.GetRawPtr(i)));
	}
}
FMapPropertyNode::FMapPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
{
	UMapProperty* MapProperty = Cast<UMapProperty>(Property);
	FScriptMapHelper Helper(MapProperty, Data);
	for (int i = 0; i < Helper.Num(); i++)
	{
		if (Helper.IsValidIndex(i))
		{
			TSharedRef<FPropertyNode> ValueNode = CreateFunc[MapProperty->ValueProp->GetClass()](View, this, MapProperty->ValueProp, Helper.GetValuePtr(i));
			TSharedRef<FPropertyNode> KeyNode = CreateFunc[MapProperty->KeyProp->GetClass()](View, &ValueNode.Get(), MapProperty->KeyProp, Helper.GetKeyPtr(i));
			ValueNode->MapKey = KeyNode;
			Children.Add(ValueNode);
		}
	}
}
FSetPropertyNode::FSetPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
{
	FScriptSetHelper Helper(Cast<USetProperty>(Property), Data);
	for (int i = 0; i < Helper.Num(); i++)
	{
		if (Helper.IsValidIndex(i))
		{
			Children.Add(CreateFunc[Helper.GetElementProperty()->GetClass()](View, this, Helper.GetElementProperty(), Helper.GetElementPtr(i)));
		}
	}
}
FStructPropertyNode::FStructPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
{
	UStructProperty* StructProperty = Cast<UStructProperty>(Property);
	for (TFieldIterator<UProperty> It(StructProperty->Struct); It; ++It)
	{
		if (UProperty* StructMember = *It)
		{
			UClass* Class = StructMember->GetClass();
			Children.Add(CreateFunc[Class](View, this, StructMember, Data + StructMember->GetOffset_ForInternal()));
		}
	}
}
FObjectPropertyNode::FObjectPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
{
	IAssetRegistry& AssetRegistry = UAssetManager::Get().GetAssetRegistry();
	FARFilter Filter;
	if (UObject* Object = GetValue())
	{
		FString PackageName = Object->GetOutermost()->GetName();
		FString Path = FPaths::GetPath(PackageName);
		Filter.PackagePaths.Add(*Path);
	}
	else
		Filter.PackagePaths.Add(TEXT("/Game/"));
	UObjectProperty* ObjProp = Cast<UObjectProperty>(Property);
	if (ObjProp->PropertyClass == UClass::StaticClass())
		Filter.ClassNames.Add(TEXT("Blueprint"));
	else
		Filter.ClassNames.Add(*ObjProp->PropertyClass->GetName());
	Filter.bRecursiveClasses = true;
	TArray<FAssetData> Assets;
	AssetRegistry.GetAssets(Filter, Assets);
	for (FAssetData& Asset : Assets)
	{
		if (ObjProp->PropertyClass == UClass::StaticClass())
		{
			UPackage* Package = Asset.GetPackage();
			UClass* Class = static_cast<UClass*>(FindObjectWithOuter(Package, ObjProp->PropertyClass));
			if (Class->HasAnyClassFlags(EClassFlags::CLASS_Abstract))
				continue;
			if (!Class->Interfaces.Num())
				continue;
		}
		DataSource.Add(MakeShareable(new FAssetData(Asset)));
	}
}
FBoolPropertyNode::FBoolPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
{
}
FTextPropertyNode::FTextPropertyNode(SPropertyView* InView, FPropertyNode* InParent, UProperty* InProperty, uint8* InData) : FPropertyNode(InView, InParent, InProperty, InData)
{
}
FName FPropertyNode::AsName()
{
	UClass* Class = Property->GetClass();
	if (Class == UNameProperty::StaticClass())
		return *(FName*)Data;
	if (Class == UStrProperty::StaticClass())
		return FName(**(FString*)Data);
	return FName();
}
int32 FPropertyNode::GetDepthToParent(const FName& Name)
{
	int32 Depth = 0;
	FPropertyNode* Node = this;
	do
	{
		if (Node->GetName() == Name)
			return Depth;
		Node = Node->Parent;
		Depth++;
	}
	while (Node);
	return -1;
}
void FPropertyNode::OnPropertyChanged()
{
	View->OnPropertyChanged(this);
}
FName FPropertyNode::GetName()
{
	if (MapKey.IsValid())
		return MapKey->AsName();
	return Property->GetFName();
}
TSharedRef<SWidget> FPropertyNode::CreateName()
{
	return SNew(STextBlock).Text(FText::FromName(GetName()));
}
TSharedRef<SWidget> FPropertyNode::CreateValue()
{
	return SNullWidget::NullWidget;
}
void FObjectPropertyNode::SetValue(UObject* Object)
{
	UObjectProperty* ObjProp = Cast<UObjectProperty>(Property);
	ObjProp->SetObjectPropertyValue(Data, Object);
	OnPropertyChanged();
}
UObject* FObjectPropertyNode::GetValue() const
{
	UObjectProperty* ObjProp = Cast<UObjectProperty>(Property);
	return ObjProp->GetObjectPropertyValue(Data);
}
TSharedRef<SWidget> FObjectPropertyNode::CreateValue()
{
	return SNew(SComboBox<TSharedPtr<FAssetData>>)
		.OptionsSource(&DataSource)
		.OnGenerateWidget_Lambda([](TSharedPtr<FAssetData> Item)
		{
			return SNew(STextBlock).Text(FText::FromName(Item->PackageName));
		})
		.OnSelectionChanged_Lambda([&](TSharedPtr<FAssetData> Item, ESelectInfo::Type)
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Property);
			UPackage* Pkg = Item->GetPackage();
			UObject* Object = (UObject*)FindObjectWithOuter(Pkg, ObjProp->PropertyClass);
			SetValue(Object);
		})
		[
			SNew(STextBlock).Text_Lambda([this]
			{
				UObject* Object = GetValue();
				return FText::FromName(Object ? Object->GetOutermost()->GetFName() : FName("None"));
			})
		];
}
void FBoolPropertyNode::SetValue(ECheckBoxState NewCheckState)
{
	UBoolProperty* BoolProp = Cast<UBoolProperty>(Property);
	BoolProp->SetPropertyValue(Data, NewCheckState == ECheckBoxState::Checked);
	OnPropertyChanged();
}
ECheckBoxState FBoolPropertyNode::GetValue() const
{
	UBoolProperty* BoolProp = Cast<UBoolProperty>(Property);
	return BoolProp->GetPropertyValue(Data) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}
TSharedRef<SWidget> FBoolPropertyNode::CreateValue()
{
	return SNew(SCheckBox)
		.OnCheckStateChanged(this, &FBoolPropertyNode::SetValue)
		.IsChecked(this, &FBoolPropertyNode::GetValue);
}
FText FTextPropertyNode::GetValue() const
{
	UClass* Class = Property->GetClass();
	if (Class == UNameProperty::StaticClass())
		return FText::FromName(*(FName*)Data);
	if (Class == UStrProperty::StaticClass())
		return FText::FromString(*(FString*)Data);
	if (Class == UTextProperty::StaticClass())
		return *(FText*)Data;
	return FText();
}
void FTextPropertyNode::SetValue(const FText& InText, ETextCommit::Type InCommitType)
{
	UClass* Class = Property->GetClass();
	if (Class == UNameProperty::StaticClass())
	{
		*(FName*)Data = FName(*InText.ToString());
		if (!((FName*)Data)->IsValid())
		{
			int k = 0;
		}
	}
	if (Class == UStrProperty::StaticClass())
		*(FString*)Data = InText.ToString();
	if (Class == UTextProperty::StaticClass())
		*(FText*)Data = InText;
	OnPropertyChanged();
}
TSharedRef<SWidget> FTextPropertyNode::CreateValue()
{
	return SNew(SEditableTextBox)
		.Text(this, &FTextPropertyNode::GetValue)
		.OnTextCommitted(this, &FTextPropertyNode::SetValue);
}
