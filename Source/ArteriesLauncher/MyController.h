// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SAssetBrowser.h"
#include "MyController.generated.h"

UCLASS()
class ARTERIESLAUNCHER_API AMyController : public APlayerController
{
	GENERATED_UCLASS_BODY()
protected:
	virtual void BeginPlay() override;
};
