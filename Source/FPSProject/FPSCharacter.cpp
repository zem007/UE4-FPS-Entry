// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacter.h"
#include "Engine.h"
#include "FPSProjectile.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//创建一个新的摄像机组件
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	//附加到Character的胶囊体中
	FPSCameraComponent->SetupAttachment(GetCapsuleComponent());
	//将摄像机放置到距离眼睛上方不远处
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	//用pawn控制摄像机旋转
	FPSCameraComponent->bUsePawnControlRotation = true;

	//为拥有的玩家创建一个第一人称模型手臂的组件
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	//仅对拥有玩家可见
	FPSMesh->SetOnlyOwnerSee(true);
	//添加到摄像机
	FPSMesh->SetupAttachment(FPSCameraComponent);
	//禁用环境阴影，保留单一模型存在的假象
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	//拥有的玩家无法看到(自身的)第三人称模型
	GetMesh()->SetOwnerNoSee(true);
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
	}
	
}

// Called every frame
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSCharacter::StartJump);
	PlayerInputComponent->BindAction("Turn", IE_Released, this, &AFPSCharacter::StopJump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);
}

void AFPSCharacter::MoveForward(float Value) {
	
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::MoveRight(float Value) {
	FVector Direction = FRotationMatrix(Controller->GetControlRotation()).GetScaledAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void AFPSCharacter::StartJump() {
	bPressedJump = true;
}

void AFPSCharacter::StopJump() {
	bPressedJump = false;
}

void AFPSCharacter::Fire() {
	//尝试发射物体
	if (ProjectileClass) {
		//获取摄像机的变换
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		//将枪口偏移MuzzleOffset从摄像机空间变换到世界坐标系
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
		FRotator MuzzleRotation = CameraRotation;
		//将准星微微上抬
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World) {
			//生成弹射物的局部变量
			FActorSpawnParameters SpawnParams;
			//把弹射物归为此玩家
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			//在World中，在枪口生成发射物
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile) {
				//获取发射物的方向向量，传入FireInDirection函数中发射发射物
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}

	}
}