// Copyright (c) 2015-2020 Saint software All rights reserved.


#include "WebVideoPlayer.h"
#include "WebMediaLoader.h"
#include "Components/StaticMeshComponent.h"
#include "MediaSoundComponent.h"
#include "MediaPlayer.h"

// Sets default values
AWebVideoPlayer::AWebVideoPlayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	DisplayComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DisplayComponent"));
	SoundComponent = CreateDefaultSubobject<UMediaSoundComponent>(TEXT("SoundComponent"));

	RootComponent = RootSceneComponent;
	DisplayComponent->SetupAttachment(RootSceneComponent);
	SoundComponent->SetupAttachment(RootSceneComponent);
}

// Called when the game starts or when spawned
void AWebVideoPlayer::BeginPlay()
{
	Super::BeginPlay();
	MediaLoader = NewObject<UWebMediaLoader>();
	MediaPlayer = NewObject<UMediaPlayer>();
}

void AWebVideoPlayer::LoadVideo(FString VideoURL)
{
	MediaLoader->SetParams(VideoURL);
	MediaLoader->OnLoadProgress.AddUObject(this, &AWebVideoPlayer::OnLoadProgressCallback);
	MediaLoader->OnLoadFinished.AddUObject(this, &AWebVideoPlayer::OnLoadFinishedCallback);
	MediaLoader->ExecuteMediaDownload();
}

void AWebVideoPlayer::SetUseCache(bool bUseCache)
{
	MediaLoader->bUseCache = bUseCache;
}

void AWebVideoPlayer::SetCacheFolder(FString CacheFolderPath)
{
	MediaLoader->CacheFolderPath = CacheFolderPath;
}

UMediaPlayer * AWebVideoPlayer::GetMediaPlayer()
{
	return MediaPlayer;
}

void AWebVideoPlayer::RenderingMediaTexture_Implementation(UMediaPlayer * MP)
{
	UE_LOG(LogTemp, Error, TEXT("Fatal! AWebVideoPlayer::RenderingMediaTexture() is not overrided!"));
	UE_LOG(LogTemp, Error, TEXT("Fatal! Check your blueprint class and override RenderingMediaTexture()."));
}

void AWebVideoPlayer::OnLoadProgressCallback(int32 DownloadedSize, int32 TotalSize, float Percentage)
{
	OnVideoLoadProgress.Broadcast(DownloadedSize, TotalSize, Percentage);
}

void AWebVideoPlayer::OnLoadFinishedCallback(bool bSuccessful, FString CacheFilePath)
{
	OnVideoLoadFinished.Broadcast(bSuccessful, CacheFilePath);

	MediaPlayer->OnMediaOpened.AddDynamic(this, &AWebVideoPlayer::OnVideoReadyCallback);
	MediaPlayer->OnMediaOpenFailed.AddDynamic(this, &AWebVideoPlayer::OnVideoReadyFailedCallback);

	SoundComponent->SetMediaPlayer(MediaPlayer);	// Set AudioPlayer
	RenderingMediaTexture(MediaPlayer);				// Set MediaTexture

	if (!MediaPlayer->OpenFile(CacheFilePath)) {
		OnVideoReadyFailedCallback(CacheFilePath);
	}
}

void AWebVideoPlayer::OnVideoReadyCallback(FString OpenedUrl)
{
	OnVideoReady.Broadcast(true, MediaPlayer);
}

void AWebVideoPlayer::OnVideoReadyFailedCallback(FString OpenedUrl)
{
	OnVideoReady.Broadcast(false, MediaPlayer);
}

