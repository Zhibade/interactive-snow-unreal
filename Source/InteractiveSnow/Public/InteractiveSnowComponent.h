// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "InteractiveSnowComponent.generated.h"


// This component enables the interaction with snow surfaces. It requires a static mesh component to be present on the actor.
// NOTE: Requires 0-1 UVs
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
	void DrawMaterial(FVector2D UVs, UTexture2D* ShapeTexture, FVector2D TextureScale);

protected:
	UPROPERTY()
	AActor* OwnerActor = nullptr;

	UPROPERTY()
	UTexture2D* CurrentShapeTexture;

	UPROPERTY()
	UMaterialInstanceDynamic* DrawMaterialInstance;

	UPROPERTY()
	UMaterialInstanceDynamic* TextureCopyMaterialInstance;

	UPROPERTY()
	UTextureRenderTarget2D* PrevRenderTarget;

	UPROPERTY()
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* BaseMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetCopyMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* RenderTargetDrawMaterial;

	UPROPERTY(EditAnywhere)
	int32 RenderTargetResolution = 1024;

	virtual void BeginPlay() override;

	/**
	* Creates and initializes a new render target texture of the given resolution and format
	*
	* @param Resolution - Pixel resolution in X and Y
	* @param Format - Format to use for this render target
	*
	* @return New initialized render target
	*/
	UFUNCTION(BlueprintCallable)
	UTextureRenderTarget2D* CreateRenderTarget(int32 Resolution, ETextureRenderTargetFormat Format);

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
