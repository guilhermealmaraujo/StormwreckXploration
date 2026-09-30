// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGridManager.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "StormwreckGameInstance.h"

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
        );

        return;
    }

    UStormwreckGameInstance* GameInstance =
        Cast<UStormwreckGameInstance>(GetGameInstance());

    if (!GameInstance)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("HexGridManager could not find StormwreckGameInstance")
        );

        return;
    }

    int32 SavedQ = 0;
    int32 SavedR = 0;

    if (GameInstance->GetTravelPosition(SavedQ, SavedR))
    {
        FHexCoordinate SpawnHex;
        SpawnHex.Q = SavedQ;
        SpawnHex.R = SavedR;

        FVector SpawnLocation = HexToWorld(SpawnHex);

        // Conserva a altura atual do personagem.
        SpawnLocation.Z = TrackedPawn->GetActorLocation().Z;

        TrackedPawn->SetActorLocation(
            SpawnLocation,
            false,
            nullptr,
            ETeleportType::TeleportPhysics
        );

        CurrentHex = SpawnHex;
        bHasCurrentHex = true;

        UE_LOG(
            LogTemp,
            Log,
            TEXT("Player positioned at travel hex | Q: %d | R: %d"),
            SavedQ,
            SavedR
        );

        if (GEngine)
        {
            const FString Message = FString::Printf(
                TEXT("Player positioned at Hex | Q: %d | R: %d"),
                SavedQ,
                SavedR
            );

            GEngine->AddOnScreenDebugMessage(
                -1,
                5.0f,
                FColor::Green,
                Message
            );
        }
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

    if (bShowDebugGrid)
    {
        DrawDebugGrid();
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

FVector AHexGridManager::HexToWorld(
    const FHexCoordinate& Hex) const
{
    const float SqrtThree = FMath::Sqrt(3.0f);

    const float X =
        HexSize * SqrtThree *
        (static_cast<float>(Hex.Q)
            + static_cast<float>(Hex.R) / 2.0f);

    const float Y =
        HexSize * 1.5f *
        static_cast<float>(Hex.R);

    return GetActorLocation() + FVector(X, Y, 0.0f);
}

void AHexGridManager::DrawDebugHex(
    const FHexCoordinate& Hex,
    const FColor& Color) const
{
    const bool bIsHighlighted = Color == FColor::Yellow;

    // O hex selecionado fica ligeiramente acima para evitar z-fighting.
    const float HighlightHeightOffset =
        bIsHighlighted ? 2.0f : 0.0f;

    const FVector Center =
        HexToWorld(Hex)
        + FVector(
            0.0f,
            0.0f,
            DebugLineHeight + HighlightHeightOffset
        );

    constexpr int32 CornerCount = 6;

    FVector Corners[CornerCount];

    for (int32 Index = 0; Index < CornerCount; ++Index)
    {
        const float AngleDegrees =
            60.0f * static_cast<float>(Index) + 30.0f;

        const float AngleRadians =
            FMath::DegreesToRadians(AngleDegrees);

        Corners[Index] = Center + FVector(
            HexSize * FMath::Cos(AngleRadians),
            HexSize * FMath::Sin(AngleRadians),
            0.0f
        );
    }

    const float LineThickness =
        bIsHighlighted ? 8.0f : 5.0f;

    for (int32 Index = 0; Index < CornerCount; ++Index)
    {
        const int32 NextIndex =
            (Index + 1) % CornerCount;

        DrawDebugLine(
            GetWorld(),
            Corners[Index],
            Corners[NextIndex],
            Color,
            false,
            0.0f,
            0,
            LineThickness
        );
    }
}

void AHexGridManager::DrawDebugGrid() const
{
    // Primeira etapa: desenha a grade normal.
    for (int32 Q = -DebugGridRadius;
        Q <= DebugGridRadius;
        ++Q)
    {
        const int32 MinimumR = FMath::Max(
            -DebugGridRadius,
            -Q - DebugGridRadius
        );

        const int32 MaximumR = FMath::Min(
            DebugGridRadius,
            -Q + DebugGridRadius
        );

        for (int32 R = MinimumR; R <= MaximumR; ++R)
        {
            FHexCoordinate Hex;
            Hex.Q = Q;
            Hex.R = R;

            // O hex atual será desenhado separadamente.
            if (bHasCurrentHex && Hex == CurrentHex)
            {
                continue;
            }

            DrawDebugHex(Hex, FColor::Cyan);
        }
    }

    // Segunda etapa: desenha o hex atual por último.
    if (bHasCurrentHex)
    {
        DrawDebugHex(CurrentHex, FColor::Yellow);
    }
}

