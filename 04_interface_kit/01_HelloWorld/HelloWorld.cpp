#include <Application.h>
#include <Window.h>
#include <View.h>

// Custom view that draws text
class HelloView : public BView
{
public:
	HelloView(BRect frame)
		:
		BView(frame, "hello_view", B_FOLLOW_ALL, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
	{
		SetViewColor(255, 255, 255);  // White background
	}
	
	virtual ~HelloView()
	{
	}
	
	void Draw(BRect updateRect) override
	{
		// Draw black text with system bold font
		SetHighColor(0, 0, 0);
		SetFont(be_bold_font);
		SetFontSize(18.0);
		
		// Position and draw the text
		const char* text = "Hello, World!";
		float stringWidth = StringWidth(text);
		BRect bounds = Bounds();
		float x = (bounds.Width() - stringWidth) / 2;
		float y = bounds.Height() / 2;
		
		MovePenTo(x, y);
		DrawString(text);
	}
};

// Simple window that adds the custom view
class HelloWindow : public BWindow
{
public:
	HelloWindow(BRect frame)
		:
		BWindow(frame, "Hello Window", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		BRect viewRect = Bounds();  // Use full window bounds
		HelloView* view = new HelloView(viewRect);
		AddChild(view);  // Attach view to window
	}
	
	virtual ~HelloWindow()
	{
	}
	
	virtual bool QuitRequested()
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};

// Application class
class HelloApp : public BApplication
{
public:
	HelloApp()
		:
		BApplication("application/x-vnd.example-helloworld")
	{
	}
	
	virtual void ReadyToRun()
	{
		// Create window after app initialization
		BRect rect(100, 100, 400, 300);
		HelloWindow* window = new HelloWindow(rect);
		window->Show();
	}
};

int
main()
{
	HelloApp app;
	app.Run();
	return 0;
}
