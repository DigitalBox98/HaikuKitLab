#include <Application.h>
#include <File.h>
#include <TranslatorRoster.h>
#include <iostream>
#include <cstring>


class DocExporterApp : public BApplication
{
public:
	DocExporterApp(int argc, char** argv)
		:
		BApplication("application/x-vnd.demo-docexporter"),
		fArgc(argc),
		fArgv(argv)
	{
	}

	void ReadyToRun() override
	{
		if (fArgc < 2) {
			std::cout << "Usage: " << fArgv[0] << " <input_rtf_file>" << std::endl;
			PostMessage(B_QUIT_REQUESTED);
			return;
		}

		const char* inputFile = fArgv[1];

		// Open input RTF file
		BFile inFile(inputFile, B_READ_ONLY);
		if (inFile.InitCheck() != B_OK) {
			std::cout << "Failed to open input file " << inputFile << std::endl;
			PostMessage(B_QUIT_REQUESTED);
			return;
		}

		// Create output text file
		BFile outFile("exported_text.txt",
			B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);

		// Get the system translator roster
		BTranslatorRoster* roster = BTranslatorRoster::Default();
		if (!roster) {
			std::cout << "Failed to get translator roster" << std::endl;
			PostMessage(B_QUIT_REQUESTED);
			return;
		}

		// Convert RTF to plain text
		status_t status = roster->Translate(&inFile, nullptr, nullptr, &outFile, 'TEXT');
		if (status != B_OK) {
			std::cout << "Conversion failed: " << strerror(status) << std::endl;
		} else {
			std::cout << "RTF successfully converted to plain text: exported_text.txt" << std::endl;
		}

		PostMessage(B_QUIT_REQUESTED);	// Quit application
	}

private:
	int fArgc;
	char** fArgv;
};


int main(int argc, char** argv)
{
	DocExporterApp app(argc, argv);
	app.Run();
	return 0;
}
