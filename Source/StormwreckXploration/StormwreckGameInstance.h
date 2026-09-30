#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "StormwreckGameInstance.generated.h"

UCLASS()
class STORMWRECKXPLORATION_API UStormwreckGameInstance
    : public UGameInstance
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Travel")
    void SetTravelPosition(int32 Q, int32 R);

    UFUNCTION(BlueprintPure, Category = "Travel")
    bool GetTravelPosition(int32& OutQ, int32& OutR) const;

    UFUNCTION(BlueprintCallable, Category = "Travel")
    void ClearTravelPosition();

private:
    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Travel",
        meta = (AllowPrivateAccess = "true")
    )
    bool bHasTravelPosition = false;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Travel",
        meta = (AllowPrivateAccess = "true")
    )
    int32 TravelQ = 0;

    UPROPERTY(
        VisibleAnywhere,
        BlueprintReadOnly,
        Category = "Travel",
        meta = (AllowPrivateAccess = "true")
    )
    int32 TravelR = 0;
};