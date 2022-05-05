// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//header
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
//
#include "Subsystems/GameInstanceSubsystem.h"
#include "GISubsystemNet.generated.h"

#define MY_SESSION_NAME "MY_SESSION_NAME"

// start session 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnStartSessionDelegate,bool,bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnEndSessionDelegate,bool,bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCSOnDestorySessionDelegate,bool,bSuccess);

/**
 * 
 */
UCLASS()
class NET_API UGISubsystemNet : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	
public:
	// init subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// bind delegate
	UGISubsystemNet();
	// create , not start
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void CreateSession(bool bUseLAN , int ConnectNumbers);
	// update , only open change public numbers
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void UpdateSession(int NewNumber);
	// start
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void StartSession();
	// end
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void EndSession();
	// destory
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void DestorySession();
	// find
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void FindSession(bool bUseLAN , int MaxSearchResults);
	// join
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void JoinSession();
	// get subsystem type
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void GetSubsystemType();
	// custom delegate
	UPROPERTY(BlueprintAssignable,Category = "CSDelegate")
	FCSOnStartSessionDelegate CSOnStartSessionDelegate;
	UPROPERTY(BlueprintAssignable,Category = "CSDelegate")
	FCSOnEndSessionDelegate CSOnEndSessionDelegate;
	UPROPERTY(BlueprintAssignable,Category = "CSDelegate")
	FCSOnDestorySessionDelegate CSOnDestorySessionDelegate;
private:
	IOnlineSessionPtr OSInterface = nullptr;
	FOnlineSessionSettings OSSettings;
	TSharedPtr<FOnlineSessionSearch> OSSearchs;

	// create
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle OnCreateCompleteHandle;
	// update
	FOnUpdateSessionCompleteDelegate OnUpdateSessionCompleteDelegate;
	FDelegateHandle OnUpdateCompleteHandle;
	// start
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle OnStartCompleteHandle;
	// end
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;
	FDelegateHandle OnEndCompleteHandle;
	// destory
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle OnDestoryCompleteHandle;
	// find
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle OnFindCompleteHandle;
	// join
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle OnJoinCompleteHandle;
protected:
	void OnCreateSessionComplete_bind(FName SessionName , bool Success);
	void OnUpdateSessionComplete_bind(FName SessionName , bool Success);
	void OnStartSessionComplete_bind(FName SessionName , bool Success);
	void OnEndSessionComplete_bind(FName SessionName , bool Success);
	void OnDestorySessionComplete_bind(FName SessionName , bool Success);
	void OnFindSessionComplete_bind(bool Success);
	void OnJoinSessionComplete_bind(FName SessionName, EOnJoinSessionCompleteResult::Type EJSCR);
};


