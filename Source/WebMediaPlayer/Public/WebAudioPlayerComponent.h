// Copyright (c) 2015-2020 Saint software All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WebAudioPlayerComponent.generated.h"

/**
 * Call when download is progressing.
 * @Param	DownloadedSize	download complete size
 * @Param	TotalSize		File size to download.
 * @Param	Percentage		download percentage (0.0F ~ 1.0F)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAudioLoadProgress, int32, DownloadedSize, int32, TotalSize, float, Percentage);

/**
 *  Call when download is finished.
 * @Param	bSuccess		result of download
 * @Param	TotalSize		cache save path
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAudioLoadFinished, bool, bSuccessful, FString, SavedPath);

/**
 *  Call when video is ready.
 * @Param	bSuccess		result of video state
 * @Param	MediaPlayer		MediaPlayer instance to control video
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAudioReady, bool, bSuccessful, UMediaPlayer*, MediaPlayer);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WEBMEDIAPLAYER_API UWebAudioPlayerComponent : public USceneComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class UWebMediaLoader* MediaLoader;

	UPROPERTY()
	class UMediaPlayer* MediaPlayer;

	UPROPERTY()
	class UMediaSoundComponent* SoundComponent;

public:
	UPROPERTY(BlueprintAssignable)
	FAudioLoadProgress OnAudioLoadProgress;

	UPROPERTY(BlueprintAssignable)
	FAudioLoadFinished OnAudioLoadFinished;

	UPROPERTY(BlueprintAssignable)
	FAudioReady OnAudioReady;

public:	
	// Sets default values for this component's properties
	UWebAudioPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable, Category = "WebAudioPlayer")
	void LoadAudio(FString AudioURL);

	UFUNCTION(BlueprintCallable, Category = "WebAudioPlayer|Settings")
	void SetUseCache(bool bUseCache);

	UFUNCTION(BlueprintCallable, Category = "WebAudioPlayer|Settings")
	void SetCacheFolder(FString CacheFolderPath);

	UFUNCTION(BlueprintPure, Category = "WebVideoPlayer")
	class UMediaPlayer* GetMediaPlayer();

private:
	UFUNCTION()
	void OnLoadProgressCallback(int32 DownloadedSize, int32 TotalSize, float Percentage);

	UFUNCTION()
	void OnLoadFinishedCallback(bool bSuccessful, FString CacheFilePath);

	UFUNCTION()
	void OnAudioReadyCallback(FString OpenedUrl);

	UFUNCTION()
	void OnAudioReadyFailedCallback(FString OpenedUrl);
};
