// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SAssetBrowser.h"
#include "MyController.generated.h"

class SPopupWindow : public SWindow
{
public:
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent);
	void CloseWindowOverride(const TSharedRef<SWindow>& WindowBeingClosed);
};

UCLASS()
class ARTERIESLAUNCHER_API AMyController : public APlayerController
{
	GENERATED_BODY()
	virtual void PlayerTick(float DeltaTime) override;
	void Popup(TSharedRef<SWidget> Content);
};
