// Fill out your copyright notice in the Description page of Project Settings.

#include "MyController.h"

AMyController::AMyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
void AMyController::BeginPlay()
{
	APlayerController::BeginPlay();
	TSharedPtr<SWindow> Window = SNew(SWindow)
		.Title(FText::FromString("Runtime AssetBrowser"))
		.HasCloseButton(false)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		.AutoCenter(EAutoCenter::None)
		.ClientSize(FVector2D(480, 640));
	Window->SetContent(SNew(SAssetBrowser));
//	Window->SetAsModalWindow();
//	Window->SetRequestDestroyWindowOverride(FRequestDestroyWindowOverride::CreateSP(Window.Get(), &SPopupWindow::CloseWindowOverride));

	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (TopLevelWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), TopLevelWindow.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef(), true);
	}


	//	FSlateApplication::Get().AddModalWindow(Window.ToSharedRef(), ParentWindow.ToSharedRef());
	//	FSlateApplication::Get().AddWindowAsNativeChild( Window.ToSharedRef(), ParentWindow.ToSharedRef() );
	//	Window->BringToFront(true);
	//	Window->BringToFront();
	//	Window->ShowWindow();
}