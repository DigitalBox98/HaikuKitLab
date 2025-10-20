#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <String.h>

// Message constant for the button click event
const uint32 kButtonClicked = 'btcl';

class ButtonWindow : public BWindow
{
public:
	ButtonWindow(BRect frame)
		:
		BWindow(frame, "Button Example", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE),
		fClickCount(0)
	{
		// Background view to host controls
		BRect bounds = Bounds();
		BView* background = new BView(bounds, "background", B_FOLLOW_ALL, B_WILL_DRAW);
		background->SetViewColor(220, 220, 220);
		AddChild(background);
		
		// Button that triggers the counter
		BRect buttonRect(20, 20, 120, 50);
		BButton* button = new BButton(buttonRect, "clickme", "Click Me", 
		                               new BMessage(kButtonClicked));
		background->AddChild(button);
		
		// Initial label
		BRect labelRect(20, 60, 280, 80);
		fLabel = new BStringView(labelRect, "label", "Click the button");
		fLabel->SetFont(be_plain_font);
		background->AddChild(fLabel);
	}
	
	virtual ~ButtonWindow()
	{
	}
	
	virtual bool QuitRequested()
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
	
	void MessageReceived(BMessage* message) override
	{
		switch (message->what) {
			case kButtonClicked:
				fClickCount++;
				{
					BString labelText;
					labelText.SetToFormat("Clicked %d time%s", 
					                      fClickCount, 
					                      fClickCount == 1 ? "" : "s");
					fLabel->SetText(labelText.String());
				}
				break;
			default:
				BWindow::MessageReceived(message);
		}
	}

private:
	BStringView* fLabel;
	int32 fClickCount;
};

class ButtonApp : public BApplication
{
public:
	ButtonApp()
		:
		BApplication("application/x-vnd.example-buttonapp")
	{
	}
	
	virtual void ReadyToRun()
	{
		BRect rect(100, 100, 320, 200);
		ButtonWindow* window = new ButtonWindow(rect);
		window->Show();
	}
};

// Main program
int main()
{
	ButtonApp app;
	app.Run();
	return 0;
}
