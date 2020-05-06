// Copyright (c) 2015-2020 Saint software All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WebVideoPlayer.generated.h"

/**
 * Call when download is progressing.
 * @Param	DownloadedSize	download complete size
 * @Param	TotalSize		File size to download.
 * @Param	Percentage		download percentage (0.0F ~ 1.0F)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FVideoLoadProgress, int32, DownloadedSize, int32, TotalSize, float, Percentage);

/**
 *  Call when download is finished.
 * @Param	bSuccess		result of download
 * @Param	TotalSize		cache save path
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVideoLoadFinished, bool, bSuccessful, FString, SavedPath);

/**
 *  Call when video is ready.
 * @Param	bSuccess		result of video state
 * @Param	MediaPlayer		MediaPlayer instance to control video
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVideoReady, bool, bSuccessful, UMediaPlayer*, MediaPlayer);

class UMediaPlayer;

UCLASS()
class WEBMEDIAPLAYER_API AWebVideoPlayer : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebVideoPlayer")
	class UWebMediaLoader* MediaLoader;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebVideoPlayer")
	USceneComponent* RootSceneComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebVideoPlayer")
	class UStaticMeshComponent* DisplayComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebVideoPlayer")
	class UMediaSoundComponent* SoundComponent;

	UPROPERTY()
	UMediaPlayer* MediaPlayer;

public:
	UPROPERTY(BlueprintAssignable)
	FVideoLoadProgress OnVideoLoadProgress;

	UPROPERTY(BlueprintAssignable)
	FVideoLoadFinished OnVideoLoadFinished;

	UPROPERTY(BlueprintAssignable)
	FVideoReady OnVideoReady;
	
public:	
	// Sets default values for this actor's properties
	AWebVideoPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable,  Category = "WebVideoPlayer")
	void LoadVideo(FString VideoURL);

	UFUNCTION(BlueprintCallable, Category = "WebVideoPlayer|Settings")
	void SetUseCache(bool bUseCache);

	UFUNCTION(BlueprintCallable, Category = "WebVideoPlayer|Settings")
	void SetCacheFolder(FString CacheFolderPath);

	UFUNCTION(BlueprintPure, Category = "WebVideoPlayer")
	UMediaPlayer* GetMediaPlayer();
	

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WebVideoPlayer|Settings")
	void RenderingMediaTexture(UMediaPlayer* MP);
	void RenderingMediaTexture_Implementation(UMediaPlayer* MP);

private:
	UFUNCTION()
	void OnLoadProgressCallback(int32 DownloadedSize, int32 TotalSize, float Percentage);

	UFUNCTION()
	void OnLoadFinishedCallback(bool bSuccessful, FString CacheFilePath);

	UFUNCTION()
	void OnVideoReadyCallback(FString OpenedUrl);

	UFUNCTION()
	void OnVideoReadyFailedCallback(FString OpenedUrl);
};
