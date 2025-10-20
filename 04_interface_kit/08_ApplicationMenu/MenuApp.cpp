#include <Application.h>
#include <Alert.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <StringView.h>
#include <View.h>
#include <Window.h>

// Message constants
const uint32 MSG_NEW   = 'newf';
const uint32 MSG_ABOUT = 'abou';

// Main content view
class ContentView : public BView
{
public:
	ContentView(BRect frame)
		:
		BView(frame, "content_view", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(245, 245, 245);	// Light gray background
	}

	void Draw(BRect) override
	{
		SetHighColor(0, 0, 0);
		DrawString("BView content area!", BPoint(20, 40));
	}
};


class MenuWindow : public BWindow
{
public:
	MenuWindow(BRect frame)
		:
		BWindow(frame, "Menu Example", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		// Create the menu bar
		BMenuBar* menuBar = new BMenuBar(BRect(0, 0, Bounds().Width(), 20), "menubar");

		// File menu
		BMenu* fileMenu = new BMenu("File");
		fileMenu->AddItem(new BMenuItem("New", new BMessage(MSG_NEW), 'N'));
		fileMenu->AddSeparatorItem();
		fileMenu->AddItem(new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED), 'Q'));
		menuBar->AddItem(fileMenu);

		// Help menu
		BMenu* helpMenu = new BMenu("Help");
		helpMenu->AddItem(new BMenuItem("About", new BMessage(MSG_ABOUT)));
		menuBar->AddItem(helpMenu);

		AddChild(menuBar);

		// Add a content view below the menu bar
		BRect contentRect = Bounds();
		contentRect.top = menuBar->Bounds().Height();
		AddChild(new ContentView(contentRect));
	}

	void MessageReceived(BMessage* msg) override
	{
		switch (msg->what) {
			case MSG_NEW:
				(new BAlert("new", "You selected 'New'.", "OK"))->Go();
				break;

			case MSG_ABOUT:
				(new BAlert("about", "Menu Example Application\nKitLab Series", "OK"))->Go();
				break;

			default:
				BWindow::MessageReceived(msg);
				break;
		}
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};


class MenuApp : public BApplication
{
public:
	MenuApp()
		:
		BApplication("application/x-vnd.example-menu")
	{
	}
	
	virtual void ReadyToRun()
	{
		BRect r(100, 100, 400, 300);
		MenuWindow* w = new MenuWindow(r);
		w->Show();
	}		
};


int main()
{
	MenuApp app;
	app.Run();
	return 0;
}
