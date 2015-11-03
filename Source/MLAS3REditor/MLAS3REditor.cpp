#include "MLAS3REditor.h"
 
IMPLEMENT_GAME_MODULE(FMLAS3REditorModule, MLAS3REditor);

DEFINE_LOG_CATEGORY(MLAS3REditor)
 
#define LOCTEXT_NAMESPACE "MyGameEditor"
 
void FMLAS3REditorModule::StartupModule()
{
	UE_LOG(MLAS3REditor, Warning, TEXT("MLAS3REditor: Log Started"));
}
 
void FMLAS3REditorModule::ShutdownModule()
{
	UE_LOG(MLAS3REditor, Warning, TEXT("MLAS3REditor: Log Ended"));
}
 
#undef LOCTEXT_NAMESPACE
