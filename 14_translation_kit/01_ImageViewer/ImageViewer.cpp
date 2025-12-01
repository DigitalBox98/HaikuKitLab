#include <Application.h>
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <View.h>
#include <Window.h>

class ImageView : public BView
{
public:
	ImageView(BRect frame, BBitmap* bitmap)
		:
		BView(frame, "image_view", B_FOLLOW_ALL, B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
		fBitmap(bitmap)
	{
		SetViewColor(B_TRANSPARENT_COLOR);
	}

	void Draw(BRect) override
	{
		SetDrawingMode(B_OP_ALPHA);
		SetHighColor(255, 255, 255);
		FillRect(Bounds());

		if (fBitmap) {
			BRect imgBounds = fBitmap->Bounds();
			float imgWidth  = imgBounds.Width() + 1;
			float imgHeight = imgBounds.Height() + 1;

			// Center the image in the view
			float dx = (Bounds().Width()  - imgWidth)  / 2.0f;
			float dy = (Bounds().Height() - imgHeight) / 2.0f;

			DrawBitmap(fBitmap, BPoint(dx, dy));
		} else {
			SetHighColor(255, 0, 0);
			DrawString("Failed to load image", BPoint(10, 20));
		}

		// Draw border around view
		SetHighColor(0, 0, 0);
		StrokeRect(Bounds());
	}

private:
	BBitmap* fBitmap;
};


class ImageWindow : public BWindow
{
public:
	ImageWindow(const char* filename)
		:
		BWindow(BRect(100, 100, 500, 400), "Simple Image Viewer",
			B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		BBitmap* bmp = BTranslationUtils::GetBitmapFile(filename);
		if (!bmp) {
			return;
		}

		// Get the image size
		BRect imgBounds = bmp->Bounds();
		float width  = imgBounds.Width()  + 1;	// +1 because Width() is inclusive
		float height = imgBounds.Height() + 1;

		// Ensure minimum window size
		if (width  < 200) width  = 200;
		if (height < 200) height = 200;

		// Resize window to fit image
		ResizeTo(width, height);

		// Add view sized exactly to the image
		AddChild(new ImageView(Bounds(), bmp));

		Show();
	}
	
	virtual bool QuitRequested()
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}	
};


class ImageApp : public BApplication
{
public:
	ImageApp()
		:
		BApplication("application/x-vnd.demo-imageviewer")
	{
	}

	void ReadyToRun() override
	{
		const char* filename = "/boot/system/data/icons/haiku/preferences/32/help-about.svg";
		new ImageWindow(filename);
	}
};


int main()
{
	ImageApp app;
	app.Run();
	return 0;
}
