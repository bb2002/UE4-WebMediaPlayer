// Copyright (c) 2015-2020 Saint software All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HttpModule.h"
#include "WebMediaLoader.generated.h"

/**
 * @Date 03.18 2020
 * WebMediaLoader Plugin
 */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FMediaDownloadProgress, int32, int32, float);

DECLARE_MULTICAST_DELEGATE_TwoParams(FMediaDownloadFinished, bool, FString);

UCLASS()
class WEBMEDIAPLAYER_API UWebMediaLoader : public UObject
{
	GENERATED_BODY()
	
private:
	// Media file URL
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebMediaLoader")
	FString MediaURL;

	// Media Extension
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebMediaLoader")
	FString MediaFileName;

	// cache folder path to save
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Meta = (AllowPrivateAccess = true), Category = "WebMediaLoader")
	FString CacheFolderPath;

	/**
	 * @Note
	 * bUseCache = true, if same file is exist, download is not execute and use old file.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Meta = (AllowPrivateAccess = true), Category = "WMLSettings")
	bool bUseCache = false;

public:
	UWebMediaLoader();

	void SetParams(FString _MediaURL, FString _CacheFolderPath = "WebMediaPlayer/");

	void ExecuteMediaDownload();

private:
	void OnHttpRequestProgress(FHttpRequestPtr ReqPtr, int32 SendBytes, int32 RecvBytes);

	void OnHttpRequestFinished(FHttpRequestPtr ReqPtr, FHttpResponsePtr ResPtr, bool bIsSuccessful);

private:
	FHttpModule* HttpModule;

	FMediaDownloadProgress OnLoadProgress;
	FMediaDownloadFinished OnLoadFinished;

	friend class AWebVideoPlayer;
	friend class UWebAudioPlayerComponent;
};
