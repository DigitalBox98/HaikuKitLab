#include <Application.h>
#include <Alert.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <View.h>
#include <Window.h>

// Message constants
const uint32 MSG_COPY  = 'copy';
const uint32 MSG_PASTE = 'past';
const uint32 MSG_CLEAR = 'clea';

class ContextView : public BView
{
public:
	ContextView(BRect frame)
		:
		BView(frame, "context_view", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(240, 240, 240);
	}

	void Draw(BRect) override
	{
		SetHighColor(0, 0, 0);
		DrawString("Right-click here for context menu",
			BPoint(20, Bounds().Height() / 2));
	}

	void MouseDown(BPoint point) override
	{
		uint32 buttons;
		GetMouse(&point, &buttons);

		if (buttons & B_SECONDARY_MOUSE_BUTTON) {
			ShowContextMenu(point);
		}
	}

private:
	void ShowContextMenu(BPoint point)
	{
		BPopUpMenu* menu = new BPopUpMenu("context_menu");

		menu->AddItem(new BMenuItem("Copy",  new BMessage(MSG_COPY)));
		menu->AddItem(new BMenuItem("Paste", new BMessage(MSG_PASTE)));
		menu->AddSeparatorItem();
		menu->AddItem(new BMenuItem("Clear", new BMessage(MSG_CLEAR)));

		ConvertToScreen(&point);
		BMenuItem* selected = menu->Go(point);

		if (selected != nullptr && selected->Message() != nullptr) {
			Window()->PostMessage(selected->Message());
		}

		delete menu;
	}
};


class ContextWindow : public BWindow
{
public:
	ContextWindow(BRect frame)
		:
		BWindow(frame, "Context Menu Example", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		AddChild(new ContextView(Bounds()));
	}

	void MessageReceived(BMessage* msg) override
	{
		switch (msg->what) {
			case MSG_COPY:
				(new BAlert("info", "Copy selected.", "OK"))->Go();
				break;
			case MSG_PASTE:
				(new BAlert("info", "Paste selected.", "OK"))->Go();
				break;
			case MSG_CLEAR:
				(new BAlert("info", "Clear selected.", "OK"))->Go();
				break;
			default:
				BWindow::MessageReceived(msg);
		}
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};


class ContextApp : public BApplication
{
public:
	ContextApp()
		:
		BApplication("application/x-vnd.example-contextmenu")
	{

	}
	
	virtual void ReadyToRun()
	{
		BRect r(100, 100, 400, 300);
		ContextWindow* w = new ContextWindow(r);
		w->Show();
	}		
};


int
main()
{
	ContextApp app;
	app.Run();
	return 0;
}
