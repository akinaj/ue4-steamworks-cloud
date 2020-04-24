// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#pragma warning( push )
#pragma warning( disable : 4265 )
#include "Steamworks/Steamv146/sdk/public/steam/isteamremotestorage.h"
#pragma warning( pop ) 

#include "MyProjectGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTextReceivedDelegate, FString, SavedText);

UCLASS(Blueprintable)
class MYPROJECT_API AMyProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
    void OnFileWriteAsync(RemoteStorageFileWriteAsyncComplete_t* pCallback, bool bIOFailure);
    CCallResult<AMyProjectGameModeBase, RemoteStorageFileWriteAsyncComplete_t> m_SteamCallResultFileWriteAsync;

    void onFileReadAsync(RemoteStorageFileReadAsyncComplete_t* pCallback, bool bIOFailure);
    CCallResult<AMyProjectGameModeBase, RemoteStorageFileReadAsyncComplete_t> m_SteamCallResultFileReadAsync;

public:

    virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

    UFUNCTION(BlueprintCallable, Category = "SteamCloud")
    void SaveTextCloud(FString text);

    UFUNCTION(BlueprintCallable, Category = "SteamCloud")
    bool RequestTextCloud();

    UPROPERTY(BlueprintAssignable, Category = "SteamCloud")
    FTextReceivedDelegate OnCloudTextReceived;

private:
    static const int m_scCloudTextSizeMax = 17;
    char m_CloudText[m_scCloudTextSizeMax];
    bool m_SteamInitialized = false;
};
