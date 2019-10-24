// Fill out your copyright notice in the Description page of Project Settings.

#include "MyController.h"

void AMyController::PlayerTick(float DeltaTime)
{
	APlayerController::PlayerTick(DeltaTime);
	/*
	if (Window.IsValid())
	{
		if (!Window->GetNativeWindow()->GetOSWindowHandle())
			Window = TSharedPtr<SPopupWindow>();
		else if (!Window->HasMouseCapture())
			Window->BringToFront();
	}
	else*/
	{
		if (WasInputKeyJustPressed(EKeys::Escape))
			Popup(SNew(SAssetBrowser));
	}
}
void AMyController::Popup(TSharedRef<SWidget> Content)
{
	TSharedPtr<SPopupWindow> Window = SNew(SPopupWindow)
		.Title(FText::FromString("Runtime AssetBrowser"))
		.ClientSize(FVector2D(480, 640));
	Window->SetContent(Content);
//	Window->SetAsModalWindow();
	Window->SetRequestDestroyWindowOverride(FRequestDestroyWindowOverride::CreateSP(Window.Get(), &SPopupWindow::CloseWindowOverride));

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
FReply SPopupWindow::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape && !InKeyEvent.IsRepeat())
	{
		RequestDestroyWindow();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}
void SPopupWindow::CloseWindowOverride(const TSharedRef<SWindow>& WindowBeingClosed)
{
	FSlateApplication::Get().RequestDestroyWindow(WindowBeingClosed);
	//	FSlateApplication::Get().ActivateGameViewport();
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
}