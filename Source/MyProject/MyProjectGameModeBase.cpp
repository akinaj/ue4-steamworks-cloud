// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MyProjectGameModeBase.h"
#include "Engine/Engine.h"
#include "Steamworks/Steamv146/sdk/public/steam/steam_api.h"

void AMyProjectGameModeBase::OnFileWriteAsync(RemoteStorageFileWriteAsyncComplete_t* pCallback, bool bIOFailure)
{
    check(!bIOFailure);
    
    if (pCallback && pCallback->m_eResult == k_EResultOK)
    {
        UE_LOG(LogExec, Log, TEXT("[Steam] file write success"));
    }
    else
    {
        UE_LOG(LogExec, Error, TEXT("[Steam] file write fail"));
    }
}

void AMyProjectGameModeBase::onFileReadAsync(RemoteStorageFileReadAsyncComplete_t* pCallback, bool bIOFailure)
{
    check(!bIOFailure);

    if (pCallback && pCallback->m_eResult == k_EResultOK)
    {
        SteamRemoteStorage()->FileReadAsyncComplete(pCallback->m_hFileReadAsync, m_CloudText, pCallback->m_cubRead);
        OnCloudTextReceived.Broadcast(FString(m_CloudText));
        UE_LOG(LogExec, Log, TEXT("[Steam] cloud text received"));
    }
    else
    {
        UE_LOG(LogExec, Error, TEXT("[Steam] file read fail"));
    }
}

void AMyProjectGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    if (!SteamAPI_Init())
    {
        UE_LOG(LogExec, Error, TEXT("[Steam] SteamAPI Init failure"));
        FGenericPlatformMisc::RequestExit(false);
    }
    else
    {
        m_SteamInitialized = true;
        UE_LOG(LogExec, Log, TEXT("[Steam] SteamAPI Init success"));
    }
}

void AMyProjectGameModeBase::SaveTextCloud(FString text)
{
    check(m_SteamInitialized);
    if (text.Len() >= m_scCloudTextSizeMax)
    {
        UE_LOG(LogExec, Error, TEXT("[Steam] Text is too long"));
        return;
    }
    SteamAPICall_t call = SteamRemoteStorage()->FileWriteAsync("savedtext.ini", TCHAR_TO_ANSI(*text), sizeof(text));
    m_SteamCallResultFileWriteAsync.Set(call, this, &AMyProjectGameModeBase::OnFileWriteAsync);
}

bool AMyProjectGameModeBase::RequestTextCloud()
{
    check(m_SteamInitialized);

    // zero the buff
    memset(m_CloudText, 0, m_scCloudTextSizeMax);

    // get file size to determine buffer size
    int size = SteamRemoteStorage()->GetFileSize("savedtext.ini");
    if (size <= 0)
    {
        UE_LOG(LogExec, Log, TEXT("[Steam] Cloud save doesn't exist"));
        return false;
    }

    // check buff size
    check(size <= m_scCloudTextSizeMax);

    SteamAPICall_t call = SteamRemoteStorage()->FileReadAsync("savedtext.ini", 0, size);
    m_SteamCallResultFileReadAsync.Set(call, this, &AMyProjectGameModeBase::onFileReadAsync);
    
    return true;
}
