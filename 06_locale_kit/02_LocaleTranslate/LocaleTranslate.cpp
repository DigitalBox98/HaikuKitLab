#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Catalog.h>
#include <Locale.h>

// Define translation context
#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "HelloApp"

// Custom view that draws localized text
class HelloView : public BView
{
public:
	HelloView(BRect frame)
		:
		BView(frame, "hello_view", B_FOLLOW_ALL, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
	{
		SetViewColor(255, 255, 255); // White background
	}
	
	void Draw(BRect updateRect) override
	{
		SetHighColor(0, 0, 0);
		SetFont(be_bold_font);
		SetFontSize(18.0);
		
		const char* text = B_TRANSLATE("Hello, World!");
		float stringWidth = StringWidth(text);
		BRect bounds = Bounds();
		float x = (bounds.Width() - stringWidth) / 2;
		float y = bounds.Height() / 2;
		
		MovePenTo(x, y);
		DrawString(text);
	}
};

// Simple window
class HelloWindow : public BWindow
{
public:
	HelloWindow(BRect frame)
		:
		BWindow(frame, B_TRANSLATE("Hello Window"), B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		AddChild(new HelloView(Bounds()));
	}
	
	bool QuitRequested() override
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
		BApplication("application/x-vnd.translate-example")
	{
	}

	void ReadyToRun() override
	{
		BRect rect(100, 100, 400, 300);
		(new HelloWindow(rect))->Show();
	}
};

int main()
{
	HelloApp app;
	app.Run();
	return 0;
}
