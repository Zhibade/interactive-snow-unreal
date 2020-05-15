// Originally made by Jose Ivan Lopez Romo (https://www.ivanlopezr.com)


#include "SpherePawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "SnowInteractorComponent.h"


constexpr float FORCE_COEFFICIENT = 500000.f;


ASpherePawn::ASpherePawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(true);

	RootComponent = Mesh;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("Camera Arm"));
	SpringArm->SetupAttachment(Mesh);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = 550.f;
	SpringArm->SetRelativeRotation(FRotator(-70.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	SnowInteractorComponent = CreateDefaultSubobject<USnowInteractorComponent>("SnowInteractor");
	AddOwnedComponent(SnowInteractorComponent);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ASpherePawn::Tick(float DeltaTime)
{
	Mesh->AddForce(CurrentForce * DeltaTime);
}

void ASpherePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &ASpherePawn::UpdateForwardMovement);
	InputComponent->BindAxis("MoveRight", this, &ASpherePawn::UpdateSideMovement);
}

void ASpherePawn::BeginPlay()
{
	Super::BeginPlay();
}

void ASpherePawn::UpdateForwardMovement(float Value)
{
	CurrentForce.X = Value * MovementSpeed * FORCE_COEFFICIENT;
}

void ASpherePawn::UpdateSideMovement(float Value)
{
	CurrentForce.Y = Value * MovementSpeed * FORCE_COEFFICIENT;
}
