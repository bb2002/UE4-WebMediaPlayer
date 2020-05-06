// Copyright (c) 2015-2020 Saint software All rights reserved.


#include "WebAudioPlayerComponent.h"
#include "MediaPlayer.h"
#include "WebMediaLoader.h"
#include "MediaSoundComponent.h"

// Sets default values for this component's properties
UWebAudioPlayerComponent::UWebAudioPlayerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SoundComponent = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("SoundComponent"));
	SoundComponent->SetupAttachment(this);
}


// Called when the game starts
void UWebAudioPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	MediaLoader = NewObject<UWebMediaLoader>();
	MediaPlayer = NewObject<UMediaPlayer>();
}

void UWebAudioPlayerComponent::LoadAudio(FString AudioURL)
{
	MediaLoader->SetParams(AudioURL);
	MediaLoader->OnLoadProgress.AddUObject(this, &UWebAudioPlayerComponent::OnLoadProgressCallback);
	MediaLoader->OnLoadFinished.AddUObject(this, &UWebAudioPlayerComponent::OnLoadFinishedCallback);
	MediaLoader->ExecuteMediaDownload();
}

void UWebAudioPlayerComponent::SetUseCache(bool bUseCache)
{
	MediaLoader->bUseCache = bUseCache;
}

void UWebAudioPlayerComponent::SetCacheFolder(FString CacheFolderPath)
{
	MediaLoader->CacheFolderPath = CacheFolderPath;
}

UMediaPlayer * UWebAudioPlayerComponent::GetMediaPlayer()
{
	return MediaPlayer;
}

void UWebAudioPlayerComponent::OnLoadProgressCallback(int32 DownloadedSize, int32 TotalSize, float Percentage)
{
	OnAudioLoadProgress.Broadcast(DownloadedSize, TotalSize, Percentage);
}

void UWebAudioPlayerComponent::OnLoadFinishedCallback(bool bSuccessful, FString CacheFilePath)
{
	OnAudioLoadFinished.Broadcast(bSuccessful, CacheFilePath);

	MediaPlayer->OnMediaOpened.AddDynamic(this, &UWebAudioPlayerComponent::OnAudioReadyCallback);
	MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &UWebAudioPlayerComponent::OnAudioReadyFailedCallback);
	SoundComponent->SetMediaPlayer(MediaPlayer);

	if (!MediaPlayer->OpenFile(CacheFilePath)) {
		OnAudioReadyFailedCallback(CacheFilePath);
	}
}

void UWebAudioPlayerComponent::OnAudioReadyCallback(FString OpenedUrl)
{
	OnAudioReady.Broadcast(true, MediaPlayer);
}

void UWebAudioPlayerComponent::OnAudioReadyFailedCallback(FString OpenedUrl)
{
	OnAudioReady.Broadcast(false, MediaPlayer);
}

