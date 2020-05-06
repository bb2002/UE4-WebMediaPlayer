// Copyright (c) 2015-2020 Saint software All rights reserved.


#include "WebMediaLoader.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "GenericPlatform/GenericPlatformFile.h"

UWebMediaLoader::UWebMediaLoader()
{
	HttpModule = &FHttpModule::Get();
}

void UWebMediaLoader::SetParams(FString _MediaURL, FString _CacheFolderPath)
{
	this->MediaURL = _MediaURL;
	this->CacheFolderPath = _CacheFolderPath;
	this->MediaFileName = FPaths::GetCleanFilename(MediaURL);
}

void UWebMediaLoader::ExecuteMediaDownload()
{
	if (bUseCache) {
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		FString CacheFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + CacheFolderPath + MediaFileName;
		if (PlatformFile.FileExists(*CacheFilePath)) {
			// Use cache
			OnLoadFinished.Broadcast(true, CacheFilePath);
			return;
		}
	}

	TSharedPtr<IHttpRequest> HttpRequest = HttpModule->CreateRequest();
	HttpRequest->SetVerb("GET");
	HttpRequest->SetURL(MediaURL);
	HttpRequest->OnRequestProgress().BindUObject(this, &UWebMediaLoader::OnHttpRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWebMediaLoader::OnHttpRequestFinished);

	HttpRequest->ProcessRequest();
}

void UWebMediaLoader::OnHttpRequestProgress(FHttpRequestPtr ReqPtr, int32 SendBytes, int32 RecvBytes)
{
	int32 TotalSize = ReqPtr->GetResponse()->GetContentLength();
	float Percentage = (float)RecvBytes / TotalSize;

	OnLoadProgress.Broadcast(RecvBytes, TotalSize, Percentage);
}

void UWebMediaLoader::OnHttpRequestFinished(FHttpRequestPtr ReqPtr, FHttpResponsePtr ResPtr, bool bIsSuccessful)
{
	FString CacheSaveFolder = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + CacheFolderPath;
	FString CacheSavePath = CacheSaveFolder + MediaFileName;
	bool bSuccessful = false;

	if (ResPtr.IsValid() && EHttpResponseCodes::IsOk(ResPtr->GetResponseCode())) {
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.CreateDirectoryTree(*CacheSaveFolder);

		IFileHandle* FileHandler = PlatformFile.OpenWrite(*CacheSavePath);
		if (FileHandler != nullptr) {
			FileHandler->Write(ResPtr->GetContent().GetData(), ResPtr->GetContentLength());
			FileHandler->Flush();
			delete FileHandler;
			bSuccessful = true;
		}
	}

	OnLoadFinished.Broadcast(bSuccessful, CacheSavePath);
}