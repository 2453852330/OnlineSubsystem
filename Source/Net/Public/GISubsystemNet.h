// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//header
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
//
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GISubsystemNet.generated.h"

// define
#define MY_SESSION_NAME "MY_SESSION_NAME"
#define MY_LEADERBOARD_NAME "MY_LEADERBOARD_NAME"
#define MY_STAT_NAME "MY_STAT_SCORE"
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
	//------------------------------------------------------------------
	// get subsystem type
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void GetSubsystemType();
	// get name ? steam
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void GetPlayerName();
	// get unique net id ?
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void GetUniqueNetIdFromInterface();
	// show friend ui ? ok
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void ShowFriendUI();
	// show invite ui ? not work
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void ShowInviteUI();
	// show store ui ? not work
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void ShowStoreUI();
	//------------------------------------------------------------------
	// read friend list , with delegate
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void ReadFriendList();
	// get friends list ? need read friend list first
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void GetFriendList();
	// get leader board
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void GetLeaderBoard();
	// set leader board
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void AddLeaderBoard();
	// query Achievements
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void QueryAchievements();
	// read Achievements , need query first
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void ReadAchievements();
	// dev function , only run in dev -> reset achivevment
	UFUNCTION(BlueprintCallable,Category = "SessionSubsystem")
	void ResetAchievements();
	
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
	FString MyFriendListName = "";
	
	
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
	// read friend finish
	FOnReadFriendsListComplete OnReadFriendsListComplete;
	// query achivevments finish
	FOnQueryAchievementsCompleteDelegate OnQueryAchievementsCompleteDelegate;
protected:
	void OnCreateSessionComplete_bind(FName SessionName , bool Success);
	void OnUpdateSessionComplete_bind(FName SessionName , bool Success);
	void OnStartSessionComplete_bind(FName SessionName , bool Success);
	void OnEndSessionComplete_bind(FName SessionName , bool Success);
	void OnDestorySessionComplete_bind(FName SessionName , bool Success);
	void OnFindSessionComplete_bind(bool Success);
	void OnJoinSessionComplete_bind(FName SessionName, EOnJoinSessionCompleteResult::Type EJSCR);

	// read friend list
	void OnReadFriendListComplete_bind(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr);
	// query achievement
	void OnQueryAchievements_bind(const FUniqueNetId& UniqueNetId, const bool Success);
};


