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

	// Used on infinite surfaces only. Please make sure only one interactor component has this set to true.
	UPROPERTY(EditAnywhere)
	bool bIsActivePlayer = false;

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "10"))
	float TickInterval = 0.05f;

	// Max distance to check from the pivot of the owner actor to the surface at the bottom (-Z)
	UPROPERTY(EditAnywhere, meta = (UIMin = "1", UIMax = "100000"))
	float MaxDistance = 70.f;

	// Hole size in centimeters. E.g a hole size of 100 CM means the entire hole texture is 100 CM, not just the white area.
	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "10"))
	float HoleSize = 100.f;

	// Hole texture to use when displacing snow. White value is the hole shape.
	UPROPERTY(EditAnywhere)
	UTexture2D* HoleTexture;


	// --- FUNCTIONS / METHODS --- //

	virtual void BeginPlay() override;

	/**
	* Gets the rotation value to be used when drawing the material on the surface component.
	*
	* @return Rotation value as 0-1 (0 = 0 deg, 1 = 360 deg) (0 deg = Object X axis is aligned with world X axis)
	*/
	UFUNCTION(BlueprintCallable)
	float GetHoleRotation() const;

	/**
	* Gets the corresponding UV scale to be used when drawing the material on the surface component.
	*
	* @param SizeInCM - Desired size of the hole texture in CM
	* @param TargetUVLocation - UV coordinates of where the texture will be drawn to
	* @param UvChannel - UV channel used for the target UV location
	* @param Hit - Line trace hit information used when finding collision
	*
	* @return UV scale to apply to the texture (also corrects distortion)
	*/
	UFUNCTION(BlueprintCallable)
	FVector2D GetHoleUvScale(float SizeInCM, FVector2D TargetUVLocation, int32 UvChannel, const FHitResult& Hit) const;

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
