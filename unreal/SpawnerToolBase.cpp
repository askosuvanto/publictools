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


#include "SpawnerToolBase.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ASpawnerToolBase::ASpawnerToolBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SpawnerToolBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpawnerToolBillboard"));
	ArrowVisual = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowVisual"));
	SpawningArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningArea"));

	SpawnerToolBillboard->SetupAttachment(RootComponent);

	ArrowVisual->SetArrowColor(FLinearColor::Yellow);
	ArrowVisual->SetupAttachment(RootComponent);

	SpawningArea->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	SpawningArea->SetGenerateOverlapEvents(false);
	SpawningArea->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpawnerToolBase::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnActorsInArray)
	{
		SpawnActorsInArray();
		bIsActive = false;
	}

	if (bRandomTimeBetweenSpawnings)
	{
		CurrentSpawningTimer = FMath::RandRange(MinTimeBetweenSpawnings, MaxTimeBetweenSpawnings);
	}
	else
	{
		CurrentSpawningTimer = TimeBetweenSpawnings;
	}
}

// Called every frame
void ASpawnerToolBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsActive)
	{
		if (bCanSpawn)
		{
			if (bSpawnAllAtOnce)
			{
				if (CleanCurrentlySpawnedActors()) // Can we spawn new actors?
				{
					for (int i = 0; i < HowManyToSpawn - CurrentlySpawnedActors.Num(); i++)
					{
						SpawnActor(GetSpawnLocation(), GetSpawnRotation(), GetSpawnableActorIndex());
					}
				}
			}
			else
			{
				if (CleanCurrentlySpawnedActors())
				{
					SpawnActor(GetSpawnLocation(), GetSpawnRotation(), GetSpawnableActorIndex());
				}
			}

			bCanSpawn = false;
		}
		else
		{
			if (CurrentSpawningTimer > 0.0f)
			{
				CurrentSpawningTimer -= DeltaTime;
			}
			else
			{
				if (bRandomTimeBetweenSpawnings)
				{
					CurrentSpawningTimer = FMath::RandRange(MinTimeBetweenSpawnings, MaxTimeBetweenSpawnings);
				}
				else
				{
					CurrentSpawningTimer = TimeBetweenSpawnings;
				}

				bCanSpawn = true;
			}
		}
	}

}

void ASpawnerToolBase::SpawnActor(FVector SpawnLocation, FRotator SpawnRotation, int SpawnIndex)
{
	UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		if (SpawnableActors.Num() > 0)
		{
			TSubclassOf<AActor> ActorTypeToSpawn = SpawnableActors[SpawnIndex];

			if (ActorTypeToSpawn != nullptr)
			{
				// Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// Spawn a new actor and save it's pointer if succesful so we know when it is destroyed
				AActor* NewActor;
				NewActor = World->SpawnActor<AActor>(ActorTypeToSpawn, SpawnLocation, SpawnRotation, ActorSpawnParams);

				if (NewActor != nullptr)
				{
					CurrentlySpawnedActors.AddUnique(NewActor);
				}

				// Spawn a visual effect if SpawningFX is set
				if (SpawningFX != nullptr)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SpawningFX, GetActorLocation(), GetActorRotation());
				}

				// Spawn a sound if SpawningSound is set
				if (SpawningSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, SpawningSound, GetActorLocation());
				}
			}
		}
		else
		{
			// ERROR no spawnable actors found
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No spawnable actors found!"));
		}
	}
}

FVector ASpawnerToolBase::GetSpawnLocation()
{
	FVector NewLocation = GetActorLocation();

	if (bRandomLocationX)
	{
		NewLocation.X += FMath::RandRange(-SpawningArea->Bounds.BoxExtent.X, SpawningArea->Bounds.BoxExtent.X);
	}

	if (bRandomLocationY)
	{
		NewLocation.Y += FMath::RandRange(-SpawningArea->Bounds.BoxExtent.Y, SpawningArea->Bounds.BoxExtent.Y);
	}

	if (bRandomLocationZ)
	{
		NewLocation.Z += FMath::RandRange(-SpawningArea->Bounds.BoxExtent.Z, SpawningArea->Bounds.BoxExtent.Z);
	}

	return NewLocation;
}

FRotator ASpawnerToolBase::GetSpawnRotation()
{
	FRotator NewRotation = GetActorRotation();

	if (bRandomRotationYaw)
	{
		NewRotation.Yaw = FMath::RandRange(0.0f, 360.0f);
	}

	if (bRandomRotationPitch)
	{
		NewRotation.Pitch = FMath::RandRange(0.0f, 360.0f);
	}

	if (bRandomRotationRoll)
	{
		NewRotation.Roll = FMath::RandRange(0.0f, 360.0f);
	}

	return NewRotation;
}

int ASpawnerToolBase::GetSpawnableActorIndex()
{
	int NewIndex = 0;

	if (bSpawnActorsInOrder)
	{
		NewIndex = CurrentSpawningIndex;

		CurrentSpawningIndex++;

		if (CurrentSpawningIndex >= SpawnableActors.Num())
		{
			CurrentSpawningIndex = 0;
		}
	}
	else
	{
		NewIndex = FMath::RandRange(0, SpawnableActors.Num() - 1);
	}

	return NewIndex;
}

bool ASpawnerToolBase::CleanCurrentlySpawnedActors()
{
	bool ReturnValue = false;

	for (int i = 0; i < CurrentlySpawnedActors.Num(); i++)
	{
		if (CurrentlySpawnedActors[i] == nullptr)
		{
			CurrentlySpawnedActors.RemoveAt(i);
		}
		else if (CurrentlySpawnedActors[i]->IsActorBeingDestroyed())
		{
			CurrentlySpawnedActors.RemoveAt(i);
		}
	}

	CurrentlySpawnedActors.Shrink();

	if (CurrentlySpawnedActors.Num() < HowManyToSpawn)
	{
		ReturnValue = true;
	}

	return ReturnValue;
}

// Spawn actors first along the Y-axis, then the X-axis and finally along the Z-axis
void ASpawnerToolBase::SpawnActorsInArray()
{
	FVector Spacing;
	Spacing.X = SpawningArea->GetLocalBounds().BoxExtent.X - SpacingInArray;
	Spacing.Y = -SpawningArea->GetLocalBounds().BoxExtent.Y;
	Spacing.Z = SpawningArea->GetLocalBounds().BoxExtent.Z - SpacingInArray;

	for (int i = 0; i < HowManyToSpawn; i++)
	{
		if (Spacing.Y > SpawningArea->GetLocalBounds().BoxExtent.Y - SpacingInArray)
		{
			Spacing.X -= SpacingInArray;
			Spacing.Y = -SpawningArea->GetLocalBounds().BoxExtent.Y + SpacingInArray;
		}
		else
		{
			Spacing.Y += SpacingInArray;
		}

		if (Spacing.X < -SpawningArea->GetLocalBounds().BoxExtent.X + SpacingInArray)
		{
			Spacing.Z -= SpacingInArray;
			Spacing.X = SpawningArea->GetLocalBounds().BoxExtent.X - SpacingInArray;
		}

		if (Spacing.Z < -SpawningArea->GetLocalBounds().BoxExtent.Z + SpacingInArray)
		{
			break;
		}

		SpawnActor(GetSpawnLocation() + Spacing, GetSpawnRotation(), GetSpawnableActorIndex());
	}
}
