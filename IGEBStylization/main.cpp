#include "MyForm.h"
#include "RenderPanel.h"

[System::STAThreadAttribute]
int main(array<System::String^>^ args) {
	if (args->Length >= 1) {
		try {
			RenderPanel::fboSize = int::Parse(args[0]);
		}
		catch (...) {}
	}
	if (args->Length == 3) {
		RenderPanel::modelName = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(args[1])).ToPointer();
		RenderPanel::imageName = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(args[2])).ToPointer();
		RenderPanel::autoRun = true;
	}
	System::Windows::Forms::Application::Run(gcnew IGEBStylization::MyForm);
}
