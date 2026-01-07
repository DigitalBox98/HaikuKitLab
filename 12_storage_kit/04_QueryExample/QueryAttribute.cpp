#include <Application.h>
#include <Entry.h>
#include <Directory.h>
#include <FindDirectory.h>
#include <Path.h>
#include <Volume.h>
#include <Query.h>
#include <stdio.h>
#include <string.h>

class ListCommentFilesApp : public BApplication
{
public:
    ListCommentFilesApp()
        : BApplication("application/x-vnd.kitlab-listcommentfiles")
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

        // --- Display query information ---
        printf("Searching in: %s\n", homePath.Path());
        printf("Looking for files that have a 'Comment' attribute.\n\n");

        const char* mimeType = "text/plain";
        
        // --- Create and configure the query ---
        BQuery query;
        query.SetVolume(&volume);

        // Look for any file that has the text/plain mime type and the "Comment" attribute
        query.PushAttr("BEOS:TYPE");
        query.PushString(mimeType);
        query.PushOp(B_EQ);
        
        query.PushAttr("Comment");
        query.PushString("*");
        query.PushOp(B_EQ);
        
        // Comnine both conditions
        query.PushOp(B_AND);

        // Run the query
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

        // --- Handle empty result ---
        if (count == 0)
            printf("No files found with a 'Comment' attribute.\n");
    }
};

int main()
{
    ListCommentFilesApp app;
    app.Run();
    return 0;
}
