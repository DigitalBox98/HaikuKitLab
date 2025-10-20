#include <Application.h>
#include <Window.h>
#include <View.h>


class CircleView : public BView
{
public:
	CircleView(BRect frame)
		:
		BView(frame, "circle_view", B_FOLLOW_NONE, B_WILL_DRAW)
	{
		SetViewColor(255, 255, 255);
	}

	void Draw(BRect) override
	{
		BRect bounds = Bounds();

		// Compute circle center and radius based on view size
		BPoint center((bounds.left + bounds.right) / 2.0f,
			(bounds.top + bounds.bottom) / 2.0f);
		float radius = std::min(bounds.Width(), bounds.Height()) / 4.0f;

		// Filled yellow circle
		SetHighColor(255, 255, 200);
		FillEllipse(BRect(center.x - radius, center.y - radius,
			center.x + radius, center.y + radius));

		// Outline
		SetHighColor(0, 0, 0);
		StrokeEllipse(BRect(center.x - radius, center.y - radius,
			center.x + radius, center.y + radius));
	}
};


class CircleWindow : public BWindow
{
public:
	CircleWindow(BRect f)
		:
		BWindow(f, "Simple Yellow Circle", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		AddChild(new CircleView(Bounds()));
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};


class DrawApp : public BApplication
{
public:
	DrawApp()
		:
		BApplication("application/x-vnd.example-draw")
	{
		BRect r(100, 100, 400, 400);
		CircleWindow* w = new CircleWindow(r);
		w->Show();
	}
};


int main()
{
	DrawApp app;
	app.Run();
	return 0;
}
