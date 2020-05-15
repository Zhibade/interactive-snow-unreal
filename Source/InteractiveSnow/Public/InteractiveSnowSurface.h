// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveSnowSurface.generated.h"


class UInteractiveSnowComponent;
class UStaticMeshComponent;


// Base class for interactive snow surfaces.
// Essentially a wrapper for an actor with a static mesh component and a interactive snow component.
UCLASS()
class INTERACTIVESNOW_API AInteractiveSnowSurface : public AActor
{
	GENERATED_BODY()

public:
	AInteractiveSnowSurface(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInteractiveSnowComponent* InteractiveSnowComponent;

	virtual void BeginPlay() override;

	/**
	* Returns the interactive snow component of this actor
	*
	* @return - Interactive snow component pointer/reference
	*/
	UFUNCTION(BlueprintCallable)
	UInteractiveSnowComponent* GetInteractiveSnowComponent() const;
};
