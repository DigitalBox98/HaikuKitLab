#include <Application.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>
#include <View.h>
#include <Window.h>


class DropView : public BView
{
public:
	DropView(BRect frame)
		:
		BView(frame, "drop_view", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(220, 220, 220);
		fLabel = "Drop files here";
	}

	void Draw(BRect) override
	{
		SetHighColor(0, 0, 0);

		float y = 20;	// starting y position
		BString line;
		int32 start = 0;
		int32 end;

		// Loop through lines separated by '\n'
		while ((end = fLabel.FindFirst('\n', start)) >= 0) {
			fLabel.CopyInto(line, start, end - start);
			MovePenTo(10, y);
			DrawString(line.String());
			y += 15;	// line spacing
			start = end + 1;
		}

		// Draw the last line (if no '\n' at the end)
		if (start < fLabel.Length()) {
			fLabel.CopyInto(line, start, fLabel.Length() - start);
			MovePenTo(10, y);
			DrawString(line.String());
		}
	}

	void SetLabel(const char* text)
	{
		fLabel = text;
		Invalidate();
	}

private:
	BString fLabel;
};


class DropWindow : public BWindow
{
public:
	DropWindow(BRect frame)
		:
		BWindow(frame, "File Drop Example", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		fDropView = new DropView(Bounds());
		AddChild(fDropView);
	}

	void MessageReceived(BMessage* message) override
	{
		if (message->what == B_REFS_RECEIVED || message->what == B_SIMPLE_DATA) {
			entry_ref ref;
			BString label("Dropped files:\n");

			for (int32 i = 0; message->FindRef("refs", i, &ref) == B_OK; i++) {
				BEntry entry(&ref, true);	// follow symlinks
				BPath path;
				if (entry.GetPath(&path) == B_OK) {
					label << path.Path() << "\n";
				}
			}

			fDropView->SetLabel(label.String());
		} else {
			BWindow::MessageReceived(message);
		}
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}

private:
	DropView* fDropView;
};


class DropApp : public BApplication
{
public:
	DropApp()
		:
		BApplication("application/x-vnd.example-dropfiles")
	{
	}
	
	virtual void ReadyToRun()
	{
		BRect r(100, 100, 500, 300);
		DropWindow* w = new DropWindow(r);
		w->Show();
	}		
};


int main()
{
	DropApp app;
	app.Run();
	return 0;
}
