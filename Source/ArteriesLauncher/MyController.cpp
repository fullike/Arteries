// Fill out your copyright notice in the Description page of Project Settings.

#include "MyController.h"
#include "Classes/Engine/AssetManager.h"
AMyController::AMyController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}
void AMyController::BeginPlay()
{
	APlayerController::BeginPlay();
	IAssetRegistry& AssetRegistry = UAssetManager::Get().GetAssetRegistry();
	TArray<FString> PathsToScan;
	PathsToScan.Add(TEXT("/Game/"));
	AssetRegistry.ScanPathsSynchronous(PathsToScan);
	Window = SNew(SWindow)
		.Title(FText::FromString("Runtime AssetBrowser"))
		.HasCloseButton(false)
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		.AutoCenter(EAutoCenter::None)
		.ClientSize(FVector2D(480, 640));
	AssetBrowser = SNew(SAssetBrowser);
	Window->SetContent(AssetBrowser.ToSharedRef());
	TSharedPtr<SWindow> TopLevelWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
	if (TopLevelWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(Window.ToSharedRef(), TopLevelWindow.ToSharedRef());
	}
	else
	{
		FSlateApplication::Get().AddWindow(Window.ToSharedRef(), true);
	}
}
void AMyController::Tick(float DeltaSeconds)
{
	APlayerController::Tick(DeltaSeconds);
	if (AssetBrowser->Actor && AssetBrowser->Actor->IsRunning())
	{
		if (Window->IsVisible())
			Window->HideWindow();
	}
	else
	{
		if (!Window->IsVisible())
			Window->ShowWindow();
	}
}