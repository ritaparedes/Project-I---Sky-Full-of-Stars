// Fill out your copyright notice in the Description page of Project Settings.
// TODO: Optimize With FName Instead Of Strings
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "QuickPollDataRequestSubsystem.generated.h"

// JSON Structures Accessible In Blueprints
USTRUCT(BlueprintType)
struct FPollOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString label;

	UPROPERTY(BlueprintReadOnly)
	FString value;

	UPROPERTY(BlueprintReadOnly)
	int32 votes;
};

USTRUCT(BlueprintType)
struct FPoll
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString title;

	UPROPERTY(BlueprintReadOnly)
	FString desc;

	UPROPERTY(BlueprintReadOnly)
	TArray<FPollOptions> options;
};

USTRUCT(BlueprintType)
struct FPollsOnServer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TArray<FPoll> polls;
};

// Blueprint Event For Now To Let Us Know This Has Been Completed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReceivedQuickPollData, const FPollsOnServer&, Results);

UCLASS(Config="QuickPollDataRequestSubsystem")
class QUICKPOLLREQUEST_API UQuickPollDataRequestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public: // Public Functions
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const { return true; }

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) {}

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() { SaveConfig(); }

	UFUNCTION(BlueprintCallable)
	void OverrideServerEndpointConnection(const FString& NewServerEndpoint);
	
	UFUNCTION(BlueprintCallable)
	void RequestPollData(const FString& PollName = "/api/polls");

	UFUNCTION(BlueprintCallable)
	bool GetPollByName(const FString& PollName, FPoll& Poll);
	
	UFUNCTION(BlueprintCallable)
	bool GetVotesForPollAndCategory(const FString& PollName, const FString& PollOption, int& Value);
	
public: // Events
	
	UPROPERTY(BlueprintAssignable)
	FOnReceivedQuickPollData OnReceivedQuickPollDataEvent; 

private: // Config Variables
	
	UPROPERTY(Config)
	FString ServerEndpoint = "127.0.0.1:5000";
	
	UPROPERTY()
	FPollsOnServer LatestResults;
};
