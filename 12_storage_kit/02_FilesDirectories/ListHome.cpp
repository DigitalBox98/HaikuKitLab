#include <Application.h>
#include <Directory.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Path.h>
#include <stdio.h>

class ListHomeApp : public BApplication
{
public:
	ListHomeApp()
		:
		BApplication("application/x-vnd.kitlab-listhome")
	{
		ListEntries();
		PostMessage(B_QUIT_REQUESTED);	// Quit application
	}

	void ListEntries()
	{
		BPath homePath;
		if (find_directory(B_USER_DIRECTORY, &homePath) != B_OK) {
			printf("Failed to find home directory.\n");
			return;
		}

		BDirectory homeDir(homePath.Path());
		if (homeDir.InitCheck() != B_OK) {
			printf("Failed to open home directory.\n");
			return;
		}

		BEntry entry;
		while (homeDir.GetNextEntry(&entry) == B_OK) {
			char name[B_FILE_NAME_LENGTH];
			entry.GetName(name);

			if (entry.IsDirectory())
				printf("[DIR]  %s\n", name);
			else
				printf("[FILE] %s\n", name);
		}
	}
};

int
main()
{
	ListHomeApp app;
	app.Run();
	return 0;
}
