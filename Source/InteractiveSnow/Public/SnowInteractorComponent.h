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

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "10"))
	float TickInterval = 0.05f;

	UPROPERTY(EditAnywhere, meta = (UIMin = "1", UIMax = "100000"))
	float MaxDistance = 70.f; // Max distance to check from the pivot of the owner actor

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "10"))
	float HoleScale = 1.f;

	UPROPERTY(EditAnywhere)
	UTexture2D* HoleTexture;

	virtual void BeginPlay() override;

	/**
	* Gets the snow component directly under the parent actor and optionally the UVs
	*
	* @param OutUVs - Stores the UVs of the hit location in this reference
	*
	* @return Snow component or null.
	*/
	UFUNCTION(BlueprintCallable)
	UInteractiveSnowComponent* GetSnowComponentUnderParent(FVector2D& OutUVs) const;
};
