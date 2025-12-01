#include <TranslatorRoster.h>
#include <TranslatorFormats.h>
#include <DataIO.h>
#include <File.h>
#include <Entry.h>
#include <Message.h>
#include <iostream>

int main() {
    // Open the Markdown file for reading
    BFile markdownFile("./sample.md", B_READ_ONLY);
    if (markdownFile.InitCheck() != B_OK) {
        std::cerr << "Failed to open sample.md\n";
        return 1;
    }

    // Create the text file for writing (overwrite if exists)
    BFile txtFile("./sample.txt", B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
    if (txtFile.InitCheck() != B_OK) {
        std::cerr << "Failed to create sample.txt\n";
        return 1;
    }

    // Get the default translator roster
    BTranslatorRoster* roster = BTranslatorRoster::Default();
    if (!roster) {
        std::cerr << "Failed to get translator roster\n";
        return 1;
    }

    // Prepare identification info
    BMessage ext;
    ext.AddString("name", "./sample.md");
    ext.AddString("BEOS:TYPE", "text/markdown");

    translator_info tinfo;
    status_t result = roster->Identify(&markdownFile, &ext, &tinfo);

    if (result == B_OK) {
        const char* name = nullptr;
        const char* info = nullptr;
        int32 version = 0;

        // Get translator details
        if (roster->GetTranslatorInfo(tinfo.translator, &name, &info, &version) == B_OK) {
            std::cout << "Translator selected: " << name << " (version " << version << ")\n";
            std::cout << "Info: " << info << "\n";
        } else {
            std::cerr << "Error: GetTranslatorInfo() failed\n";
        }
    } else {
        std::cerr << "Identify failed: " << result << "\n";
        return 1;
    }

    // Reset file pointer to the beginning before translation
    markdownFile.Seek(0, SEEK_SET);

    // Translate the Markdown file to plain text
    status_t status = roster->Translate(&markdownFile, nullptr, &ext, &txtFile, B_TRANSLATOR_TEXT);

    if (status != B_OK) {
        std::cerr << "Translation failed: " << status << "\n";
        return 1;
    } else {
        std::cout << "Saved txt file: sample.txt\n";
    }

    return 0;
}
