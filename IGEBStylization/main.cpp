#include "MyForm.h"
#include "RenderPanel.h"

[System::STAThreadAttribute]
int main(array<System::String^>^ args) {
	if (args->Length == 1) {
		try {
			RenderPanel::fboSize = int::Parse(args[0]);
		}
		catch (...) {}
	}
	System::Windows::Forms::Application::Run(gcnew IGEBStylization::MyForm);
}
