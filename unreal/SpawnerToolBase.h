/**
 * MIT License
 * 
 * Copyright (c) 2022 Asko Suvanto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerToolBase.generated.h"

class UNiagaraSystem;
class USoundBase;
class UBillboardComponent;
class UArrowComponent;
class UBoxComponent;

UCLASS()
class TOPDOWNSCROLLER_API ASpawnerToolBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnerToolBase();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Spawn single actor from spawnable actors list */
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnActor(FVector SpawnLocation, FRotator SpawnRotation, int SpawnIndex);

public:
	/** Activate or deactivate this spawner */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bIsActive = true;

	/** Rotate the spawned actor randomly aound Z-axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bRandomRotationYaw = false;

	/** Rotate the spawned actor randomly around Y-axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bRandomRotationPitch = false;

	/** Rotate the spawned actor randomly around X-axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bRandomRotationRoll = false;

	/** Give the spawned actor random location on X-axis within the Spawning Area */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bRandomLocationX = false;

	/** Give the spawned actor random location on Y-axis within the Spawning Area */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bRandomLocationY = false;

	/** Give the spawned actor random location on Z-axis within the Spawning Area */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bRandomLocationZ = false;

	/** Spawn actors all at one time or wait between each spawning */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bSpawnAllAtOnce = false;

	/** Spawn the actors from Spawnable Actors in numeric order or in random order */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bSpawnActorsInOrder = true;

	/** The maximum amout of actors this spawner can spawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (UIMin = 1))
	int HowManyToSpawn = 1;

	/** The time this spawner waits before spawning again in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (UIMin = 0.0f))
	float TimeBetweenSpawnings = 1.0f;

	/** Array of spawnable actor classes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AActor>> SpawnableActors;

	/** Niagara system that is spawned when an actor is spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning | Visuals")
	UNiagaraSystem* SpawningFX;

	/** Sound that is played when an actor is spawned */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning | Visuals")
	USoundBase* SpawningSound;

	/** Adding billboard so it's easier to select the spawner tool in viewport */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning | Visuals")
	UBillboardComponent* SpawnerToolBillboard;

	/** By default an actor is spawned at the arrow's starting point rotated at the arrow's direction */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning | Visuals")
	UArrowComponent* ArrowVisual;

	/** The boxes bounds limit the spawning area */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning | Visuals")
	UBoxComponent* SpawningArea;

private:
	/** Get the spawner's location and add any chosen randomizeing options */
	FVector GetSpawnLocation();

	/** Get the spawners rotation and add any chosen randomizing options */
	FRotator GetSpawnRotation();

	/** Get the current index or a random one */
	int GetSpawnableActorIndex();

	/** Check if any saved actors have been or are being destroyed and remove them from the array */
	bool CleanCurrentlySpawnedActors();

	/** How much time is left until next spawning */
	float CurrentSpawningTimer = 0.0f;

	/** Array of spawned actors' pointers */
	TArray<AActor*> CurrentlySpawnedActors;

	/** The current position in the Spawnable Actors array */
	int CurrentSpawningIndex = 0;

	/** Can an actor be spawned now or are we waiting until the next spawning */
	bool bCanSpawn = false;

};
