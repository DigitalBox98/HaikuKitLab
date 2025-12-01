#include <Application.h>
#include <Bitmap.h>
#include <BitmapStream.h>
#include <File.h>
#include <TranslationUtils.h>
#include <TranslatorRoster.h>
#include <iostream>
#include <cstring>


class SaveImageApp : public BApplication
{
public:
	SaveImageApp(int argc, char** argv)
		:
		BApplication("application/x-vnd.demo-imageexporter"),
		fArgc(argc),
		fArgv(argv)
	{
	}

	void ReadyToRun() override
	{
		if (fArgc < 2) {
			std::cout << "Usage: " << fArgv[0] << " <input_image>" << std::endl;
			PostMessage(B_QUIT_REQUESTED);
			return;
		}

		const char* inputFile = fArgv[1];

		// Load the original image
		BBitmap* bmp = BTranslationUtils::GetBitmapFile(inputFile);
		if (!bmp) {
			std::cout << "Failed to load " << inputFile << std::endl;
			PostMessage(B_QUIT_REQUESTED);
			return;
		}

		// Prepare the bitmap stream
		BBitmapStream stream(bmp);

		// Get the system translator roster
		BTranslatorRoster* roster = BTranslatorRoster::Default();
		if (!roster) {
			std::cout << "Failed to get translator roster" << std::endl;
			delete bmp;
			PostMessage(B_QUIT_REQUESTED);
			return;
		}

		// Save as JPEG
		BFile outJPEG("exported_image.jpg",
			B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
		status_t status = roster->Translate(&stream, nullptr, nullptr, &outJPEG, B_JPEG_FORMAT);
		if (status != B_OK) {
			std::cout << "Failed to save JPEG: " << strerror(status) << std::endl;
		} else {
			std::cout << "Saved JPEG: exported_image.jpg" << std::endl;
		}

		// Reset the stream for PNG
		stream.Seek(0, SEEK_SET);

		// Save as PNG
		BFile outPNG("exported_image.png",
			B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
		status = roster->Translate(&stream, nullptr, nullptr, &outPNG, B_PNG_FORMAT);
		if (status != B_OK) {
			std::cout << "Failed to save PNG: " << strerror(status) << std::endl;
		} else {
			std::cout << "Saved PNG: exported_image.png" << std::endl;
		}

		delete bmp;
		PostMessage(B_QUIT_REQUESTED);	// Quit application
	}

private:
	int fArgc;
	char** fArgv;
};


int main(int argc, char** argv)
{
	SaveImageApp app(argc, argv);
	app.Run();
	return 0;
}
