// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SnowInteractorComponent.generated.h"


class Texture2D;
class UInteractiveSnowComponent;


// Component that makes an actor interact with a snow surface/component
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIVESNOW_API USnowInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USnowInteractorComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY()
	FVector LastLocation = FVector::ZeroVector;


	// --- EXPOSED PROPERTIES --- //

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "10"))
	float TickInterval = 0.05f;

	UPROPERTY(EditAnywhere, meta = (UIMin = "1", UIMax = "100000"))
	float MaxDistance = 70.f; // Max distance to check from the pivot of the owner actor

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "10"))
	float HoleSize = 100.f; // Scale in CM

	UPROPERTY(EditAnywhere)
	UTexture2D* HoleTexture;


	// --- FUNCTIONS / METHODS --- //

	virtual void BeginPlay() override;

	/**
	* Gets the corresponding UV scale to be used when drawing the material on the surface component.
	*
	* @param SizeInCM - Desired size of the hole texture in CM
	* @param TargetUVLocation - UV coordinates of where the texture will be drawn to.
	* @param Hit - Line trace hit information used when finding collision
	*
	* @return UV scale to apply to the texture (also corrects distortion)
	*/
	UFUNCTION(BlueprintCallable)
	FVector2D GetHoleUvScale(float SizeInCM, FVector2D TargetUVLocation, const FHitResult& Hit) const;

	/**
	* Gets the snow component directly under the parent actor and optionally the UVs
	*
	* @param OutUVs - Stores the UVs of the hit location in this reference
	* @param OutHit - Stores the collision hit information in this reference
	*
	* @return Snow component or null.
	*/
	UFUNCTION(BlueprintCallable)
	UInteractiveSnowComponent* GetSnowComponentUnderParent(FVector2D& OutUVs, FHitResult& Hit) const;
};
