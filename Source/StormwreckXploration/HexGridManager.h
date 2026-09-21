#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexGridManager.generated.h"

class APawn;

USTRUCT(BlueprintType)
struct FHexCoordinate
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex Grid")
    int32 Q = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hex Grid")
    int32 R = 0;

    bool operator==(const FHexCoordinate& Other) const
    {
        return Q == Other.Q && R == Other.R;
    }

    bool operator!=(const FHexCoordinate& Other) const
    {
        return !(*this == Other);
    }
};

UCLASS()
class STORMWRECKXPLORATION_API AHexGridManager : public AActor
{
    GENERATED_BODY()

public:
    AHexGridManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintPure, Category = "Hex Grid")
    FHexCoordinate WorldToHex(const FVector& WorldLocation) const;

protected:
    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Hex Grid",
        meta = (ClampMin = "1.0")
    )
    float HexSize = 500.0f;

    UPROPERTY(
        VisibleInstanceOnly,
        BlueprintReadOnly,
        Category = "Hex Grid"
    )
    FHexCoordinate CurrentHex;

private:
    FHexCoordinate RoundAxial(float Q, float R) const;

    UPROPERTY(Transient)
    TObjectPtr<APawn> TrackedPawn;

    bool bHasCurrentHex = false;

};