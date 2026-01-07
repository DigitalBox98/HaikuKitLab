#include <Application.h>
#include <Entry.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Volume.h>
#include <Query.h>
#include <stdio.h>
#include <string.h>

class ListTextFilesApp : public BApplication
{
public:
    ListTextFilesApp()
        : BApplication("application/x-vnd.kitlab-listtextfiles")
    {
        RunQuery();
        PostMessage(B_QUIT_REQUESTED);
    }

private:
    void RunQuery()
    {
        // --- Locate the home directory ---
        BPath homePath;
        if (find_directory(B_USER_DIRECTORY, &homePath) != B_OK) {
            printf("Failed to find home directory.\n");
            return;
        }

        BDirectory homeDir(homePath.Path());
        BVolume volume;
        if (homeDir.GetVolume(&volume) != B_OK) {
            printf("Failed to get volume for home directory.\n");
            return;
        }

        // --- Display search info ---
        const char* mimeType = "text/plain";
        printf("Searching in: %s\n", homePath.Path());
        printf("Looking for files with MIME type: %s\n\n", mimeType);

        // --- Create and configure the query ---
        BQuery query;
        query.SetVolume(&volume);
        query.PushAttr("BEOS:TYPE");
        query.PushString(mimeType);
        query.PushOp(B_EQ);

        if (query.Fetch() != B_OK) {
            printf("Failed to fetch query.\n");
            return;
        }

        // --- Iterate through the results ---
        entry_ref ref;
        int32 count = 0;

        while (query.GetNextRef(&ref) == B_OK) {
            BEntry entry(&ref);
            BPath path(&entry);

            // Only show files located inside $HOME
            if (strncmp(path.Path(), homePath.Path(), strlen(homePath.Path())) == 0) {
                printf("Found: %s\n", path.Path());
                count++;
            }
        }

        // --- No result case ---
        if (count == 0)
            printf("No files found matching this query.\n");
    }
};

int main()
{
    ListTextFilesApp app;
    app.Run();
    return 0;
}
