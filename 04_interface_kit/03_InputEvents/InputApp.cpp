#include <Application.h>
#include <Window.h>
#include <View.h>
#include <stdlib.h>	// rand()


class InputView : public BView
{
public:
	InputView(BRect frame)
		:
		BView(frame, "input_view", B_FOLLOW_ALL, B_WILL_DRAW),
		fColor({255, 200, 0, 255})
	{
		SetViewColor(255, 255, 255);

		// Initial center of the circle
		fCenter = BPoint((frame.left + frame.right) / 2.0f,
			(frame.top + frame.bottom) / 2.0f);
	}

	void Draw(BRect) override
	{
		float radius = 40;

		// Filled circle
		SetHighColor(fColor);
		FillEllipse(BRect(fCenter.x - radius, fCenter.y - radius,
			fCenter.x + radius, fCenter.y + radius));

		// Outline
		SetHighColor(0, 0, 0);
		StrokeEllipse(BRect(fCenter.x - radius, fCenter.y - radius,
			fCenter.x + radius, fCenter.y + radius));
	}

	void MouseDown(BPoint point) override
	{
		fCenter = point;		// Move circle to mouse position
		Invalidate();			// Schedule redraw
		MakeFocus(true);		// View now receives key events
	}

	void KeyDown(const char* bytes, int32 numBytes) override
	{
		if (numBytes > 0 && (bytes[0] == 'c' || bytes[0] == 'C')) {
			// Randomize color on 'c' key
			fColor.red   = rand() % 256;
			fColor.green = rand() % 256;
			fColor.blue  = rand() % 256;
			Invalidate();
		} else
			BView::KeyDown(bytes, numBytes);
	}

private:
	BPoint fCenter;
	rgb_color fColor;
};


class InputWindow : public BWindow
{
public:
	InputWindow(BRect f)
		:
		BWindow(f, "Interactive Input", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		InputView* view = new InputView(Bounds());
		AddChild(view);
		view->MakeFocus(true);
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};


class InputApp : public BApplication
{
public:
	InputApp()
		:
		BApplication("application/x-vnd.example-input")
	{
	}
	
	virtual void ReadyToRun()
	{
		BRect r(100, 100, 500, 400);
		InputWindow* w = new InputWindow(r);
		w->Show();
	}	
};


int main()
{
	InputApp app;
	app.Run();
	return 0;
}
