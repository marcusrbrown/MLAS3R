#pragma once
 
#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"
 
DECLARE_LOG_CATEGORY_EXTERN(MLAS3REditor, All, All)
 
class FMLAS3REditorModule: public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
