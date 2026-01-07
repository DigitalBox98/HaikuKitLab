#include <Application.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
#include <stdio.h>

class SimpleStorageApp : public BApplication
{
public:
    SimpleStorageApp()
        :
        BApplication("application/x-vnd.kitlab-storagekit")
    {
        WriteAndReadExample();
    }

    void WriteAndReadExample()
    {
        // Find the user's home directory
        BPath homePath;
        if (find_directory(B_USER_DIRECTORY, &homePath) != B_OK)
            return;

        homePath.Append("SampleStorage.txt");

        // --- Write the file ---
        BFile file(homePath.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
        if (file.InitCheck() != B_OK) {
            printf("Failed to create file.\n");
            return;
        }

        const char* text = "Hello, Storage Kit!\nThis file was created by a Haiku app.\n";
        file.Write(text, strlen(text));
        file.Unset(); // Close the file

        // --- Read it back ---
        BFile readFile(homePath.Path(), B_READ_ONLY);
        if (readFile.InitCheck() != B_OK) {
            printf("Failed to open file for reading.\n");
            return;
        }

        char buffer[256];
        ssize_t bytesRead = readFile.Read(buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';

        printf("File contents:\n%s\n", buffer);

        PostMessage(B_QUIT_REQUESTED);  // Quit application
    }
};

int
main()
{
    SimpleStorageApp app;
    app.Run();
    return 0;
}
