// Fill out your copyright notice in the Description page of Project Settings.


#include "GISubsystemNet.h"

#include "OnlineSubsystemUtils.h"
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
