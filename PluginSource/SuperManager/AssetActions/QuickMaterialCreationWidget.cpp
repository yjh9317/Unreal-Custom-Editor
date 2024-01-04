#include "QuickMaterialCreationWidget.h"

#include "AssetToolsModule.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"

#pragma region QuickMaterialCreationCore

void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
{
	if(bCustomMaterialName) // 생성하려는 Material 이름 확인
	{
		if(MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_"))) // 이름이 비어있거나 M_이면 return
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a valid name"));
			return;
		}
	}
		
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData(); // 선택한 에셋 데이터
	TArray<UTexture2D*> SelectedTexturesArray; // 텍스쳐 배열
	FString SelectedTextureFolderPath;
	uint32 PinsConnectedCounter = 0;

	if(!ProcessSelectedData(SelectedAssetsData,SelectedTexturesArray, SelectedTextureFolderPath)) { MaterialName = TEXT("M_"); return;}

	if(CheckIsNameUsed(SelectedTextureFolderPath, MaterialName)) { MaterialName = TEXT("M_"); return;}

	UMaterial* CreatedMaterial = CreateMaterialAsset(MaterialName, SelectedTextureFolderPath);

	if(!CreatedMaterial)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Failed to create material"));
		return;
	}

	for(UTexture2D* SelectedTexture : SelectedTexturesArray)
	{
		if(!SelectedTexture) continue;

		switch (ChannelPackingType)
		{
		case E_ChannelPackingType::ECPT_NoChannelPacking:
			Default_CreateMaterialNodes(CreatedMaterial,SelectedTexture,PinsConnectedCounter);	
			break;
		case E_ChannelPackingType::ECPT_ORM:
			ORM_CreateMaterialNodes(CreatedMaterial,SelectedTexture,PinsConnectedCounter);
			break;
		case E_ChannelPackingType::ECPT_MAX:
			break;
		default:
			break;
		}
		
	}

	if(PinsConnectedCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully connected")
			+ FString::FromInt(PinsConnectedCounter) + TEXT(" pins"));
	}

	if(bCreatedMaterialInstance)
	{
		CreateMaterialInstanceAsset(CreatedMaterial,MaterialName, SelectedTextureFolderPath);
	}

	MaterialName = TEXT("M_");

}
#pragma endregion 

#pragma region QuickMaterialCreation

// Process the selcted data, will filter out textures, and return false if non-texture selected 
bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProcess, TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackagePath)
{
	if(SelectedDataToProcess.Num() == 0) // 없다면 반환
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No Texture Selected"));
		return false;
	}

	bool bMaterialNameSet = false;
	
	for(const FAssetData& SelectedData : SelectedDataToProcess)
	{
		UObject* SelectedAsset = SelectedData.GetAsset();
		
		if(!SelectedAsset) continue; // 에셋이 없다면 continue

		UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset);

		if(!SelectedTexture) // 텍스쳐가 아니라면 반환
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please select only Texture")
				+ SelectedAsset->GetName() + TEXT(" is not a texture"));
			
			return false;
		}

		OutSelectedTexturesArray.Add(SelectedTexture); // 텍스쳐를 참조 배열에 추가

		if(OutSelectedTexturePackagePath.IsEmpty())
		{
			OutSelectedTexturePackagePath = SelectedData.PackagePath.ToString(); // 선택한 데이터 경로 저장
		}

		// 이름 바꾸기
		if(!bCustomMaterialName && !bMaterialNameSet)
		{
			MaterialName = SelectedAsset->GetName();
			MaterialName.RemoveFromStart(TEXT("T_"));
			MaterialName.InsertAt(0, TEXT("M_"));
			bMaterialNameSet = true;
		}
	}
	return true;
}

//Will return true if the material name is used by asset under the specified folder
bool UQuickMaterialCreationWidget::CheckIsNameUsed(const FString& FolderPathToCheck, const FString& MaterialNameToCheck)
{
	// 경로에 있는 모든 에셋 경로
	TArray<FString> ExistingAssetsPath = UEditorAssetLibrary::ListAssets(FolderPathToCheck, false);

	for(const FString& ExistingAssetPath : ExistingAssetsPath)
	{
		// 경로를 제외한 에셋 이름
		const FString ExistingAssetName = FPaths::GetBaseFilename(ExistingAssetPath);

		// 만약 중복되는 이름이 있다면 반환
		if(ExistingAssetName.Equals(MaterialNameToCheck))
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,MaterialNameToCheck + TEXT(" is already used by asset"));
			return true; 
		}
	}
	return false;
}

// Create Material
UMaterial* UQuickMaterialCreationWidget::CreateMaterialAsset(const FString& NameOfTheMaterial,
	const FString& PathToPutMaterial)
{
	FAssetToolsModule&  AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

	UMaterialFactoryNew* MaterialFactory = NewObject<UMaterialFactoryNew>();
	// 이름, 경로, 클래스, 클래스 팩토리
	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(NameOfTheMaterial,PathToPutMaterial,UMaterial::StaticClass(),MaterialFactory);
	
	return Cast<UMaterial>(CreatedObject);
}

void UQuickMaterialCreationWidget::Default_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture,
	uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode =
		NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if(!TextureSampleNode) return;

	if(!CreatedMaterial->GetEditorOnlyData()->BaseColor.IsConnected())
	{
		if(TryConnectBaseColor(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}
	if(!CreatedMaterial->GetEditorOnlyData()->Metallic.IsConnected())
	{
		if(TryConnectMetailic(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->Roughness.IsConnected())
	{
		if(TryConnectRoughness(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->Normal.IsConnected())
	{
		if(TryConnectNormal(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.IsConnected())
	{
		if(TryConnectAO(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	DebugHeader::Print(TEXT("Failed to connect the texture ") + SelectedTexture->GetName(),FColor::Red);
}

void UQuickMaterialCreationWidget::ORM_CreateMaterialNodes(UMaterial* CreatedMaterial, UTexture2D* SelectedTexture,
	uint32& PinsConnectedCounter)
{
	UMaterialExpressionTextureSample* TextureSampleNode =
		NewObject<UMaterialExpressionTextureSample>(CreatedMaterial);

	if(!TextureSampleNode) return;

	if(!CreatedMaterial->GetEditorOnlyData()->BaseColor.IsConnected())
	{
		if(TryConnectBaseColor(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->Normal.IsConnected())
	{
		if(TryConnectNormal(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter++;
			return;
		}
	}

	if(!CreatedMaterial->GetEditorOnlyData()->Roughness.IsConnected())
	{
		if(TryConnectORM(TextureSampleNode,SelectedTexture,CreatedMaterial))
		{
			PinsConnectedCounter +=3;
			return;
		}
	}
}
#pragma endregion 

#pragma region CreateMaterialNodesConnectPins

bool UQuickMaterialCreationWidget::TryConnectBaseColor(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for(const FString& BaseColorName : BaseColorArray)
	{
		if(SelectedTexture->GetName().Contains(BaseColorName))
		{
			// Connect pins to base color socket here
			TextureSampleNode->Texture = SelectedTexture;

			CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode); // Add Texture Node in Material
			CreatedMaterial->GetExpressionInputForProperty(MP_BaseColor)->Connect(0,TextureSampleNode); // Connect BaseColor
			CreatedMaterial->PostEditChange(); // update

			TextureSampleNode->MaterialExpressionEditorX -= 600.f; // X_Position of Texture Sample Node in Editor  
			return true;
		}
	}

	return false;
}

bool UQuickMaterialCreationWidget::TryConnectMetailic(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for(const FString& MetailicName : MetallicArray)
	{
		if(SelectedTexture->GetName().Contains(MetailicName))
		{
			// 텍스쳐 기본 세팅
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;	// Alpha값은 사용하지 않으므로 false 
			SelectedTexture->PostEditChange();
			
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_Metallic)->Connect(0,TextureSampleNode);
			SelectedTexture->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.f;
			TextureSampleNode->MaterialExpressionEditorY += 240.f;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectRoughness(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for(const FString& RoughnessName : RoughnessArray)
	{
		if(SelectedTexture->GetName().Contains(RoughnessName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetExpressionCollection().AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_Roughness)->Connect(0,TextureSampleNode);
			SelectedTexture->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.f;
			TextureSampleNode->MaterialExpressionEditorY += 480.f;
			
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectNormal(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for(const FString& NormalName : NormalArray)
	{
		if(SelectedTexture->GetName().Contains(NormalName))
		{
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Normal;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_Normal)->Connect(0,TextureSampleNode);
			SelectedTexture->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.f;
			TextureSampleNode->MaterialExpressionEditorY += 720.f;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectAO(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for(const FString& AOName : AmbientOcclusionArray)
	{
		if(SelectedTexture->GetName().Contains(AOName))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Default;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();
			
			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.AddExpression(TextureSampleNode);
			CreatedMaterial->GetExpressionInputForProperty(MP_AmbientOcclusion)->Connect(0,TextureSampleNode);
			SelectedTexture->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.f;
			TextureSampleNode->MaterialExpressionEditorY += 960.f;
			return true;
		}
	}
	return false;
}

bool UQuickMaterialCreationWidget::TryConnectORM(UMaterialExpressionTextureSample* TextureSampleNode,
	UTexture2D* SelectedTexture, UMaterial* CreatedMaterial)
{
	for(const FString& ORM_Name :ORMArray)
	{
		if(SelectedTexture->GetName().Contains(ORM_Name))
		{
			SelectedTexture->CompressionSettings = TextureCompressionSettings::TC_Masks;
			SelectedTexture->SRGB = false;
			SelectedTexture->PostEditChange();

			TextureSampleNode->Texture = SelectedTexture;
			TextureSampleNode->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Masks;

			CreatedMaterial->GetEditorOnlyData()->ExpressionCollection.AddExpression(TextureSampleNode);
			// 1번 R 2번 G 3번 B
			CreatedMaterial->GetEditorOnlyData()->AmbientOcclusion.Connect(1,TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Roughness.Connect(2,TextureSampleNode);
			CreatedMaterial->GetEditorOnlyData()->Metallic.Connect(3,TextureSampleNode);
			CreatedMaterial->PostEditChange();

			TextureSampleNode->MaterialExpressionEditorX -= 600.f;
			TextureSampleNode->MaterialExpressionEditorY += 240.f;
			return true;
		}
	}
	return false;
}

#pragma endregion 

UMaterialInstanceConstant* UQuickMaterialCreationWidget::CreateMaterialInstanceAsset(UMaterial* CreatedMaterial,
	FString NameOfMaterialInstance, const FString& PathToPutMI)
{
	NameOfMaterialInstance.RemoveFromStart(TEXT("M_"));
	NameOfMaterialInstance.InsertAt(0,TEXT("MI_"));

	UMaterialInstanceConstantFactoryNew* MIFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();
	
	FAssetToolsModule&  AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(NameOfMaterialInstance,PathToPutMI,
	UMaterialInstanceConstant::StaticClass(),MIFactoryNew);

	if(UMaterialInstanceConstant* CreatedMI = Cast<UMaterialInstanceConstant>(CreatedObject))
	{
		CreatedMI->SetParentEditorOnly(CreatedMaterial);
		CreatedMI->PostEditChange();
		CreatedMaterial->PostEditChange();

		return CreatedMI;
	}
	
	return nullptr;	
}
