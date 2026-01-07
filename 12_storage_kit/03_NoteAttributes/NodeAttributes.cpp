#include <Application.h>
#include <Entry.h>
#include <Node.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
#include <fs_attr.h>        // for attr_info
#include <stdio.h>
#include <string.h>

class NodeAttributesApp : public BApplication
{
public:
	NodeAttributesApp()
		:
		BApplication("application/x-vnd.kitlab-nodeattributes")
	{
		RunExample();
		PostMessage(B_QUIT_REQUESTED);	// Quit the application
	}

	void RunExample()
	{
		// Find the user's home directory
		BPath homePath;
		if (find_directory(B_USER_DIRECTORY, &homePath) != B_OK) {
			printf("Failed to find home directory.\n");
			return;
		}

		// Complete file path
		homePath.Append("NodeFile9.txt");

		// Display where the file will be created
		printf("File location: %s\n", homePath.Path());

		// Create the file if it doesn’t exist
		BEntry entry(homePath.Path(), true);
		if (!entry.Exists()) {
			BFile file(homePath.Path(), B_READ_WRITE | B_CREATE_FILE);
			if (file.InitCheck() != B_OK) {
				printf("Failed to create file.\n");
				return;
			}
		}

		// Get a valid entry_ref
		entry_ref ref;
		entry.GetRef(&ref);

		// Create a BNode to manipulate attributes
		BNode node(&ref);
		if (node.InitCheck() != B_OK) {
			printf("Failed to initialize BNode.\n");
			return;
		}

		// Write an attribute
		const char* comment = "This is a BFS attribute example.";
		ssize_t bytesWritten = node.WriteAttr("Comment", B_STRING_TYPE, 0, comment, strlen(comment) + 1);
		if (bytesWritten < 0) {
			printf("Failed to write attribute.\n");
			return;
		} else {
			printf("Wrote %zd bytes to attribute.\n", bytesWritten);
		}

		// Read the attribute
		char buffer[256];
		ssize_t bytesRead = node.ReadAttr("Comment", B_STRING_TYPE, 0, buffer, sizeof(buffer) - 1);
		if (bytesRead > 0) {
			buffer[bytesRead] = '\0';
			printf("Comment attribute: %s\n", buffer);
		} else {
			printf("Failed to read attribute.\n");
		}

		// Get attribute information
		attr_info info;
		if (node.GetAttrInfo("Comment", &info) == B_OK) {
			printf("Attribute size: %lld, type code: %c%c%c%c\n",
			       info.size,
			       (info.type >> 24) & 0xFF,
			       (info.type >> 16) & 0xFF,
			       (info.type >> 8) & 0xFF,
			       info.type & 0xFF);
		}
	}
};

int main()
{
	NodeAttributesApp app;
	app.Run();
	return 0;
}
