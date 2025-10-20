#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Polygon.h>
#include <math.h>


class EscherView : public BView
{
public:
	EscherView(BRect frame)
		:
		BView(frame, "escher_view", B_FOLLOW_ALL, B_WILL_DRAW)
	{
		SetViewColor(255, 255, 255);
	}

	void Draw(BRect) override
	{
		float size = 10;
		float dx = size * 8 * 2;
		float dy = size * 8;

		// Compute grid size based on window bounds
		int rows = (int)(Bounds().Height() / dy) + 2;
		int cols = (int)(Bounds().Width() / dx) + 2;

		for (int row = 0; row < rows; ++row) {
			for (int col = 0; col < cols; ++col) {
				int pattern = (row + col) % 4;					// Cycle through four colors
				float x = col * dx + ((row % 2) * dx / 2);		// Offset every other row for tiling effect
				float y = row * dy;
				DrawPattern(BPoint(x, y), size, pattern);
			}
		}
	}

	void DrawPattern(BPoint center, float size, int pattern)
	{
		// Original 8x8 polygon pattern
		BPoint pts[19] = {
			{ size * -4.0f, size * 4.0f },     { size * -1.0f, size * 4.0f },
			{ size * -1.0f, size * 6.0f },     { size * 1.5f, size * 6.0f },
			{ size * 1.5f, size * 4.0f },      { size * 4.0f, size * 4.0f },
			{ size * 4.0f, size * 3.5f },      { size * 6.0f, size * 0.5f },
			{ size * 4.0f, size * -1.0f },     { size * 4.0f, size * -4.0f },
			{ size * 1.5f, size * -4.0f },     { size * 1.5f, size * -2.0f },
			{ size * -1.0f, size * -2.0f },    { size * -1.0f, size * -4.0f },
			{ size * -4.0f, size * -4.0f },    { size * -4.0f, size * -1.0f },
			{ size * -2.0f, size * 1.0f },     { size * -4.0f, size * 4.0f },
			{ size * -4.0f, size * 4.0f }
		};

		// Offset polygon by center position
		for (auto& p : pts) {
			p.x += center.x;
			p.y += center.y;
		}

		rgb_color colors[4] = {
			{ 180, 60, 100, 255 },
			{ 255, 220, 100, 255 },
			{ 60, 120, 180, 255 },
			{ 200, 100, 50, 255 }
		};

		SetHighColor(colors[pattern % 4]);
		FillPolygon(pts, 19);
		SetHighColor(0, 0, 0);
		StrokePolygon(pts, 19, true);
	}
};


class EscherWindow : public BWindow
{
public:
	EscherWindow(BRect f)
		:
		BWindow(f, "Escher-Style Tiling", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		AddChild(new EscherView(Bounds()));
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};


class PatternApp : public BApplication
{
public:
	PatternApp()
		:
		BApplication("application/x-vnd.example-pattern")
	{
		BRect r(100, 100, 800, 600);
		EscherWindow* w = new EscherWindow(r);
		w->Show();
	}
};


int main()
{
	PatternApp app;
	app.Run();
	return 0;
}
