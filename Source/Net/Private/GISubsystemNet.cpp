// Fill out your copyright notice in the Description page of Project Settings.


#include "GISubsystemNet.h"

#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Kismet/GameplayStatics.h"


void UGISubsystemNet::Initialize(FSubsystemCollectionBase& Collection)
{
	// get interface
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (!OnlineSubsystem)
	{
		UE_LOG(LogTemp,Error,TEXT("OnlineSubsystem Init Failed , Return"));
		return;
	}
	OSInterface = OnlineSubsystem->GetSessionInterface();
}

UGISubsystemNet::UGISubsystemNet()
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnCreateSessionComplete_bind);
	OnUpdateSessionCompleteDelegate = FOnUpdateSessionCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnUpdateSessionComplete_bind);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnStartSessionComplete_bind);
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnEndSessionComplete_bind);
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnDestorySessionComplete_bind);
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnFindSessionComplete_bind);
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnJoinSessionComplete_bind);

	// read firend list
	OnReadFriendsListComplete = FOnReadFriendsListComplete::CreateUObject(this,&UGISubsystemNet::OnReadFriendListComplete_bind);
	// query Achievement
	OnQueryAchievementsCompleteDelegate = FOnQueryAchievementsCompleteDelegate::CreateUObject(this,&UGISubsystemNet::OnQueryAchievements_bind);
}
// create
void UGISubsystemNet::CreateSession(bool bUseLAN , int ConnectNumbers)
{
	// settings
	OSSettings.NumPrivateConnections = 0;
	OSSettings.NumPublicConnections = ConnectNumbers;
	OSSettings.bAllowInvites = true;
	OSSettings.bAllowJoinInProgress = true;
	OSSettings.bAllowJoinViaPresence = true;
	OSSettings.bAllowJoinViaPresenceFriendsOnly = true;
	OSSettings.bIsDedicated = false;
	OSSettings.bUsesPresence = true;
	OSSettings.bIsLANMatch = bUseLAN;
	OSSettings.bShouldAdvertise = true;
	// set custom data
	OSSettings.Set(FName("CUSTOM_DATA"),FString("This is Custom data"),EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	// add delegate
	OnCreateCompleteHandle = OSInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
	// create
	OSInterface->CreateSession(0,MY_SESSION_NAME,OSSettings);
}
// update session
void UGISubsystemNet::UpdateSession(int NewNumber)
{
	if (!OSInterface.IsValid()) {return;}
	OSSettings.NumPublicConnections = NewNumber;
	OnUpdateCompleteHandle = OSInterface->AddOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteDelegate);
	OSInterface->UpdateSession(MY_SESSION_NAME,OSSettings);
}
// start session
void UGISubsystemNet::StartSession()
{
	if (!OSInterface.IsValid()) {return;}
	OnStartCompleteHandle = OSInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
	OSInterface->StartSession(MY_SESSION_NAME);
}
// end session
void UGISubsystemNet::EndSession()
{
	if (!OSInterface.IsValid()) {return;}
	OnEndCompleteHandle = OSInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
	OSInterface->EndSession(MY_SESSION_NAME);
}
// destory session
void UGISubsystemNet::DestorySession()
{
	if (!OSInterface.IsValid()) {return;}
	OnDestoryCompleteHandle = OSInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	OSInterface->DestroySession(MY_SESSION_NAME);
}
// find session
void UGISubsystemNet::FindSession(bool bUseLAN , int MaxSearchResults)
{
	if (!OSInterface.IsValid()) {return;}
	OnFindCompleteHandle = OSInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
	// search session settings
	OSSearchs = MakeShareable(new FOnlineSessionSearch());
	OSSearchs->bIsLanQuery = bUseLAN;
	OSSearchs->MaxSearchResults = MaxSearchResults;
	// this setting important
	OSSearchs->QuerySettings.Set(SEARCH_PRESENCE,true,EOnlineComparisonOp::Equals);
	
	OSInterface->FindSessions(0,OUT OSSearchs.ToSharedRef());
}
// join session
void UGISubsystemNet::JoinSession()
{
	if (!OSInterface.IsValid()) {return;}
	if (OSSearchs->SearchResults.Num())
	{
		OnJoinCompleteHandle = OSInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);
		OSInterface->JoinSession(0,MY_SESSION_NAME,OSSearchs->SearchResults[0]);
	}
	
}
// get subsystem type
void UGISubsystemNet::GetSubsystemType()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		FName SubsystemName = OnlineSubsystem->GetSubsystemName();
		UE_LOG(LogTemp,Warning,TEXT("SubsystemName = %s"),*SubsystemName.ToString());
	}
}
// get player name ? steam  : yes
void UGISubsystemNet::GetPlayerName()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if(OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr.IsValid())
		{
			FString PlayerNickName = IdentityPtr->GetPlayerNickname(0);
			UE_LOG(LogTemp,Error,TEXT("Get Player Nick Name | playerNickName -> %s "),*PlayerNickName);
		}
	}
}
// get unique net id
void UGISubsystemNet::GetUniqueNetIdFromInterface()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr.IsValid())
		{
			TSharedPtr<const FUniqueNetId> UniqueNetId = IdentityPtr->GetUniquePlayerId(0);
			UE_LOG(LogTemp,Warning,TEXT("Player Unique Net Id -> %s"),*UniqueNetId->ToString());
		}
	}
}
// show friends UI ? same to = shift+tab in steam
void UGISubsystemNet::ShowFriendUI()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineExternalUIPtr ExternalUIPtr = OnlineSubsystem->GetExternalUIInterface();
		if (ExternalUIPtr.IsValid())
		{
			bool Ret = ExternalUIPtr->ShowFriendsUI(0);
			UE_LOG(LogTemp,Warning,TEXT("Show Friend Ui | Success -> %d"),Ret);
		}
	}
}
// show invite ui  ?  not work , always failed
void UGISubsystemNet::ShowInviteUI()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineExternalUIPtr ExternalUIPtr = OnlineSubsystem->GetExternalUIInterface();
		if (ExternalUIPtr.IsValid())
		{
			bool Ret = ExternalUIPtr->ShowInviteUI(0);
			UE_LOG(LogTemp,Warning,TEXT("Show Invite Ui | Success -> %d"),Ret);
		}
	}
}
// show store ui ? not work , always failed
void UGISubsystemNet::ShowStoreUI()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineExternalUIPtr ExternalUIPtr = OnlineSubsystem->GetExternalUIInterface();
		if (ExternalUIPtr.IsValid())
		{
			FShowStoreParams ShowStoreParams;
			bool Ret = ExternalUIPtr->ShowStoreUI(0,ShowStoreParams);
			UE_LOG(LogTemp,Warning,TEXT("Show store Ui | Success -> %d"),Ret);
		}
	}
}
// read friend list 
void UGISubsystemNet::ReadFriendList()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface();
		if (FriendsPtr.IsValid())
		{
			FString FriendListName;
			// bind delegate
			FriendsPtr->ReadFriendsList(0,OUT FriendListName,OnReadFriendsListComplete);
		}
	}
}

// invite friend ? need a Friend list (FString)
void UGISubsystemNet::GetFriendList()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineFriendsPtr FriendsPtr = OnlineSubsystem->GetFriendsInterface();
		if (FriendsPtr.IsValid())
		{
			TArray<TSharedRef<FOnlineFriend>> Friends;
			FriendsPtr->GetFriendsList(0,MyFriendListName ,OUT Friends);
			UE_LOG(LogTemp,Warning,TEXT("Get Friend List | friends nums -> %d"),Friends.Num());
			if (Friends.Num())
			{
				for (auto Item : Friends)
				{
					FString DisplayName = Item->GetDisplayName();
					UE_LOG(LogTemp,Error,TEXT("Friend List | [ %s ]"),*DisplayName);
				}
			}
		}
	}
}
// get leader board
void UGISubsystemNet::GetLeaderBoard()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineLeaderboardsPtr LeaderboardsPtr = OnlineSubsystem->GetLeaderboardsInterface();
		if (LeaderboardsPtr.IsValid())
		{
			TArray<TSharedRef<const FUniqueNetId>> PlayerList;
			FOnlineLeaderboardReadRef LeaderboardReadRef = MakeShareable(new FOnlineLeaderboardRead);
			LeaderboardReadRef->LeaderboardName = MY_LEADERBOARD_NAME;
			LeaderboardReadRef->SortedColumn = MY_STAT_NAME;
			LeaderboardReadRef->ColumnMetadata.Add(FColumnMetaData(MY_STAT_NAME,EOnlineKeyValuePairDataType::Float));
			LeaderboardReadRef->ReadState = EOnlineAsyncTaskState::NotStarted;
			LeaderboardsPtr->ReadLeaderboards(OUT PlayerList,LeaderboardReadRef);
			UE_LOG(LogTemp,Warning,TEXT("Get Leader Board | playerlist num : %d | data num : %d "),PlayerList.Num(),LeaderboardReadRef->Rows.Num());
		}
	}
}
// add leader board data
void UGISubsystemNet::AddLeaderBoard()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineLeaderboardsPtr LeaderboardsPtr = OnlineSubsystem->GetLeaderboardsInterface();
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (LeaderboardsPtr.IsValid())
		{
			LeaderboardsPtr->FlushLeaderboards(MY_SESSION_NAME);
			FOnlineLeaderboardWrite LeaderboardWrite;
			LeaderboardWrite.LeaderboardNames.Add(MY_LEADERBOARD_NAME);
			LeaderboardWrite.SetFloatStat(MY_STAT_NAME,FMath::FRandRange(5.f,100.f));
			TSharedPtr<const FUniqueNetId> NetID = IdentityPtr->GetUniquePlayerId(0);
			LeaderboardsPtr->WriteLeaderboards(MY_SESSION_NAME,*NetID.Get(),LeaderboardWrite);
		}
	}
}

void UGISubsystemNet::QueryAchievements()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineAchievementsPtr AchievementsPtr = OnlineSubsystem->GetAchievementsInterface();
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (AchievementsPtr.IsValid())
		{
			TSharedPtr<const FUniqueNetId> NetID = IdentityPtr->GetUniquePlayerId(0);
			TArray<FOnlineAchievement> Achievements;
			AchievementsPtr->QueryAchievements(*(NetID.Get()),OnQueryAchievementsCompleteDelegate);
			UE_LOG(LogTemp,Warning,TEXT("ReadAchievements | find %d "),Achievements.Num());
		}
	}
}

// read Achievements 
void UGISubsystemNet::ReadAchievements()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineAchievementsPtr AchievementsPtr = OnlineSubsystem->GetAchievementsInterface();
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (AchievementsPtr.IsValid())
		{
			TSharedPtr<const FUniqueNetId> NetID = IdentityPtr->GetUniquePlayerId(0);
			TArray<FOnlineAchievement> Achievements;
			AchievementsPtr->GetCachedAchievements(*(NetID.Get()),OUT Achievements);
			UE_LOG(LogTemp,Warning,TEXT("ReadAchievements | find %d "),Achievements.Num());
		}
	}
}
// reset achievements
void UGISubsystemNet::ResetAchievements()
{
	IOnlineSubsystem * OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineAchievementsPtr AchievementsPtr = OnlineSubsystem->GetAchievementsInterface();
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (AchievementsPtr.IsValid())
		{
			TSharedPtr<const FUniqueNetId> NetID = IdentityPtr->GetUniquePlayerId(0);
			AchievementsPtr->ResetAchievements(*(NetID.Get()));
		}
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------

// join session bind
void UGISubsystemNet::OnJoinSessionComplete_bind(FName SessionName, EOnJoinSessionCompleteResult::Type EJSCR)
{
	UE_LOG(LogTemp,Warning,TEXT("Join Session Complete -> bindSendName: %s , RetID: %d"),*SessionName.ToString(),EJSCR);
	OSInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinCompleteHandle);
	if (EJSCR == EOnJoinSessionCompleteResult::Type::Success)
	{
		APlayerController * PC = UGameplayStatics::GetPlayerController(this,0);
		// get travel url form session
		FString TravelURL = "";
		OSInterface->GetResolvedConnectString(MY_SESSION_NAME,OUT TravelURL);
		UE_LOG(LogTemp,Warning,TEXT("Session id str = %s"),*TravelURL);
		if (PC && TravelURL != "")
		{	
			PC->ClientTravel(TravelURL,ETravelType::TRAVEL_Absolute);
		}
	}
}
// read friend list from server bind
void UGISubsystemNet::OnReadFriendListComplete_bind(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName,
	const FString& ErrorStr)
{
	UE_LOG(LogTemp,Error,TEXT("Read Friend List Result : %d  , Lsit Name : %s"),bWasSuccessful,*ListName);
	MyFriendListName = ListName;
}
// query achievements bind
void UGISubsystemNet::OnQueryAchievements_bind(const FUniqueNetId& UniqueNetId, const bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("QueryAchievementComplete -> Success : %d"),Success);
}


// find session bind
void UGISubsystemNet::OnFindSessionComplete_bind(bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("Find Session Complete -> Result: %d , find num -> %d"),Success,OSSearchs->SearchResults.Num());
	OSInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindCompleteHandle);
	if (OSSearchs->SearchResults.Num())
	{
		FString CustomData = "";
		OSSearchs->SearchResults[0].Session.SessionSettings.Get(FName("CUSTOM_DATA"),OUT CustomData);
		UE_LOG(LogTemp,Error,TEXT("This is my custom session data : %s"),*CustomData);
	}
}


// destory session bind
void UGISubsystemNet::OnDestorySessionComplete_bind(FName SessionName, bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("Destory Session Complete -> NAME: %s ,Result: %d"),*SessionName.ToString(),Success);
	OSInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestoryCompleteHandle);
	CSOnDestorySessionDelegate.Broadcast(Success);
} 

// end session bind
void UGISubsystemNet::OnEndSessionComplete_bind(FName SessionName, bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("End Session Complete -> NAME: %s ,Result: %d"),*SessionName.ToString(),Success);
	OSInterface->ClearOnEndSessionCompleteDelegate_Handle(OnEndCompleteHandle);
	CSOnEndSessionDelegate.Broadcast(Success);
}
// start session bind
void UGISubsystemNet::OnStartSessionComplete_bind(FName SessionName, bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("Start Session Complete -> NAME: %s ,Result: %d"),*SessionName.ToString(),Success);
	OSInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartCompleteHandle);
	CSOnStartSessionDelegate.Broadcast(Success);
}

// update session bind
void UGISubsystemNet::OnUpdateSessionComplete_bind(FName SessionName, bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("Update Session Complete -> NAME: %s ,Result: %d"),*SessionName.ToString(),Success);
	OSInterface->ClearOnUpdateSessionCompleteDelegate_Handle(OnUpdateCompleteHandle);
}

// create session bind
void UGISubsystemNet::OnCreateSessionComplete_bind(FName SessionName, bool Success)
{
	UE_LOG(LogTemp,Warning,TEXT("Create Session Complete -> NAME:  %s  , Result: %d "),*SessionName.ToString(),Success);
	OSInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateCompleteHandle);
}
