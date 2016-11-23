#include "MyForm.h"

[System::STAThreadAttribute]
int main(array<System::String^>^ args) {
	System::Windows::Forms::Application::Run(gcnew IGEBStylization::MyForm);
}
