#include <Application.h>
#include <Bitmap.h>
#include <Message.h>
#include <Rect.h>
#include <View.h>
#include <Window.h>

// Custom message type for dragging colors
const uint32 MSG_COLOR = 'colr';

// ColorSwatch: a draggable square
class ColorSwatch : public BView {
public:
	rgb_color fColor;

	ColorSwatch(BRect frame, rgb_color color)
		: BView(frame, "color_swatch", B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
		fColor(color)
	{
		SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	}

	void Draw(BRect) override
	{
		SetHighColor(fColor);
		FillRect(Bounds());

		// Black border
		SetHighColor(0, 0, 0);
		StrokeRect(Bounds());
	}

	void MouseDown(BPoint point) override
	{
		BMessage dragMsg(MSG_COLOR);
		dragMsg.AddData("RGBColor", B_RGB_COLOR_TYPE, &fColor, sizeof(rgb_color));

		// Start the drag directly (no custom bitmap)
		DragMessage(&dragMsg, Bounds());
	}
};

// DropZone: changes its background color when a swatch is dropped
class DropZone : public BView {
public:
	rgb_color fColor;

	DropZone(BRect frame)
		: BView(frame, "drop_zone", B_FOLLOW_ALL,
			B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
	{
		fColor = make_color(245, 245, 245);	// Light gray background
		SetViewColor(fColor);
	}

	void Draw(BRect) override
	{
		SetHighColor(0, 0, 0);
		StrokeRect(Bounds());

		DrawString("Drop a color here",
			BPoint(10, Bounds().Height() / 2));
	}

	void MessageReceived(BMessage* msg) override
	{
		if (msg->what == MSG_COLOR) {
			const rgb_color* newColor;
			ssize_t size;
			if (msg->FindData("RGBColor", B_RGB_COLOR_TYPE,
				(const void**)&newColor, &size) == B_OK) {
				fColor = *newColor;
				SetViewColor(fColor);
				Invalidate();
			}
		} else {
			BView::MessageReceived(msg);
		}
	}
};

// Main window
class DragDropWindow : public BWindow {
public:
	DragDropWindow()
		: BWindow(BRect(100, 100, 400, 300), "Drag & Drop App",
			B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		BRect bounds = Bounds();

		// Three pastel color swatches on the left
		AddChild(new ColorSwatch(BRect(10, 10, 60, 60), make_color(255, 182, 193)));	// Pink
		AddChild(new ColorSwatch(BRect(10, 70, 60, 120), make_color(152, 251, 152)));	// Green
		AddChild(new ColorSwatch(BRect(10, 130, 60, 180), make_color(173, 216, 230)));	// Blue

		// Drop zone on the right
		AddChild(new DropZone(BRect(80, 10, bounds.Width() - 10, bounds.Height() - 10)));
	}
	
	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}	
};

// Application
class DragDropApp : public BApplication {
public:
	DragDropApp()
		: BApplication("application/x-vnd.demo-dragdrop")
	{
	}

	void ReadyToRun() override
	{
		(new DragDropWindow())->Show();
	}
};

int main()
{
	DragDropApp app;
	app.Run();
	return 0;
}
