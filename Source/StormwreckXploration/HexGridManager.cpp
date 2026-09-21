// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGridManager.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHexGridManager::AHexGridManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHexGridManager::BeginPlay()
{
	Super::BeginPlay();

    TrackedPawn = UGameplayStatics::GetPlayerPawn(this, 0);

    if (!TrackedPawn) 
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("HexGridManager could not find player pawn")
        )
    }
	
}

// Called every frame
void AHexGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!TrackedPawn) 
    {   
        return;
    }

    const FHexCoordinate NewHex = WorldToHex(TrackedPawn->GetActorLocation());

    if (!bHasCurrentHex || NewHex != CurrentHex)
    {
        CurrentHex = NewHex;
        bHasCurrentHex = true;

        const FString Message = FString::Printf(
            TEXT("Entered Hex | Q: %d | R: %d"),
            CurrentHex.Q,
            CurrentHex.R
        );

        UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Green,
                Message
            );
        }
    }
}

FHexCoordinate AHexGridManager::WorldToHex(
    const FVector& WorldLocation) const
{
    // A posição do próprio manager representa a origem Q:0, R:0.
    const FVector LocalPosition =
        WorldLocation - GetActorLocation();

    const float SqrtThree = FMath::Sqrt(3.0f);

    // Conversão para uma grade pointy-top.
    const float FractionalQ =
        ((SqrtThree / 3.0f) * LocalPosition.X
            - (1.0f / 3.0f) * LocalPosition.Y)
        / HexSize;

    const float FractionalR =
        ((2.0f / 3.0f) * LocalPosition.Y)
        / HexSize;

    return RoundAxial(FractionalQ, FractionalR);
}

FHexCoordinate AHexGridManager::RoundAxial(
    const float Q,
    const float R) const
{
    // Coordenadas axiais convertidas temporariamente para cube coordinates.
    const float CubeX = Q;
    const float CubeZ = R;
    const float CubeY = -CubeX - CubeZ;

    int32 RoundedX = FMath::RoundToInt(CubeX);
    int32 RoundedY = FMath::RoundToInt(CubeY);
    int32 RoundedZ = FMath::RoundToInt(CubeZ);

    const float XDifference = FMath::Abs(RoundedX - CubeX);
    const float YDifference = FMath::Abs(RoundedY - CubeY);
    const float ZDifference = FMath::Abs(RoundedZ - CubeZ);

    // Corrige o eixo que sofreu o maior erro de arredondamento.
    if (XDifference > YDifference && XDifference > ZDifference)
    {
        RoundedX = -RoundedY - RoundedZ;
    }
    else if (YDifference > ZDifference)
    {
        RoundedY = -RoundedX - RoundedZ;
    }
    else
    {
        RoundedZ = -RoundedX - RoundedY;
    }

    FHexCoordinate Result;
    Result.Q = RoundedX;
    Result.R = RoundedZ;

    return Result;
}

