#include "StormwreckGameInstance.h"

void UStormwreckGameInstance::SetTravelPosition(
    const int32 Q,
    const int32 R)
{
    TravelQ = Q;
    TravelR = R;
    bHasTravelPosition = true;

    UE_LOG(
        LogTemp,
        Log,
        TEXT("Travel position saved | Q: %d | R: %d"),
        TravelQ,
        TravelR
    );
}

bool UStormwreckGameInstance::GetTravelPosition(
    int32& OutQ,
    int32& OutR) const
{
    OutQ = TravelQ;
    OutR = TravelR;

    return bHasTravelPosition;
}

void UStormwreckGameInstance::ClearTravelPosition()
{
    bHasTravelPosition = false;
    TravelQ = 0;
    TravelR = 0;
}