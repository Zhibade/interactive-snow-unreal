// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveSnowComponent.generated.h"


class UTextureRenderTarget2D;


// This component enables the interaction with snow surfaces. It requires a static mesh component to be present on the actor.
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class INTERACTIVESNOW_API UInteractiveSnowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractiveSnowComponent(const FObjectInitializer& ObjectInitializer);

	/**
	* Draws in this component's render target at the given UVs, with the given shape texture and scale.
	*
	* @param UVs - UV location of the drawing
	* @param ShapeTexture - Texture to use when drawing
	* @param TextureScale - Scale value to apply when drawing on the texture
	*/
	UFUNCTION(BlueprintCallable)
	void DrawMaterial(FVector2D UVs, UTexture2D* ShapeTexture, float TextureScale);

protected:
	UPROPERTY()
	AActor* OwnerActor = nullptr;

	UPROPERTY()
	UTexture2D* CurrentShapeTexture;

	UPROPERTY()
	UMaterialInstanceDynamic* DrawMaterialInstance;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetDrawMaterial;

	UPROPERTY(EditAnywhere)
	int32 RenderTargetResolution = 1024;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* RenderTarget;

	virtual void BeginPlay() override;

	/**
	* Initializes dynamic material instance for this actor, and the material for the render target
	*/
	UFUNCTION(BlueprintCallable)
	void InitMaterials();

	/**
	* Logs a warning using a preset header + the given message.
	*
	* @param Message - Message to log
	*/
	UFUNCTION(BlueprintCallable)
	void LogWarning(FString Message);
};
