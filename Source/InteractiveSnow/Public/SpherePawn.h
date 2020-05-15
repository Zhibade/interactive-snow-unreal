// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SpherePawn.generated.h"


class UCameraComponent;
class USnowInteractorComponent;
class USpringArmComponent;


// Simple rolling sphere pawn class. Interacts with snow components.
UCLASS()
class INTERACTIVESNOW_API ASpherePawn : public APawn
{
	GENERATED_BODY()

public:
	ASpherePawn(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY()
	FVector CurrentForce = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MovementSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly)
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USnowInteractorComponent* SnowInteractorComponent;

	virtual void BeginPlay() override;

	/**
	* Updates the forward movement of the ball using the input axis value.
	*
	* @param Value - Axis value (-1 to 1)
	*/
	UFUNCTION(BlueprintCallable)
	void UpdateForwardMovement(float Value);

	/**
	* Updates the side movement of the ball using the input axis value.
	*
	* @param Value - Axis value (-1 to 1)
	*/
	UFUNCTION(BlueprintCallable)
	void UpdateSideMovement(float Value);
};
