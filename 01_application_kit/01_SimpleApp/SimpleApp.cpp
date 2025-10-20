#include <Application.h>
#include <Window.h>

// A simple window class
class SimpleWindow : public BWindow
{
public:
	SimpleWindow(BRect frame)
		:
		BWindow(frame, "A Haiku Window", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
	}
	
	virtual ~SimpleWindow()
	{
	}
	
	virtual bool QuitRequested()
	{
		// Notify the application to quit
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};

// A simple application class
class SimpleApplication : public BApplication
{
public:
	SimpleApplication()
		:
		BApplication("application/x-vnd.example-simpleapp")
	{
	}
	
	virtual void ReadyToRun()
	{
		// Create window after application is fully initialized
		BRect rect(100, 100, 400, 300);
		SimpleWindow* window = new SimpleWindow(rect);
		window->Show();
	}
};

// Main function
int main()
{
	SimpleApplication app;
	app.Run(); // Start the main event loop
	return 0;
}
