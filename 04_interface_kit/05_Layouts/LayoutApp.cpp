#include <Application.h>
#include <Button.h>
#include <GridLayout.h>
#include <GroupLayout.h>
#include <GroupLayoutBuilder.h>
#include <LayoutBuilder.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>
#include <TextControl.h>
#include <View.h>
#include <Window.h>

class LayoutWindow : public BWindow {
public:
	LayoutWindow(BRect frame)
		: BWindow(frame, "Layout Example", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
			.AddGlue()
			.AddGrid(10, 10)
				.Add(BSpaceLayoutItem::CreateGlue(), 0, 0)
				.Add(new BStringView("title", "User Information Form"), 0, 1)
				.Add(new BStringView("name_label", "Name:"), 0, 2)
				.Add(new BTextControl("", "", NULL), 1, 2)
				.Add(new BStringView("email_label", "Email:"), 0, 3)
				.Add(new BTextControl("", "", NULL), 1, 3)
			.End()
			.AddGlue()
			.AddGroup(B_HORIZONTAL, 10)
				.Add(new BButton("ok_btn", "OK", NULL))
				.Add(new BButton("cancel_btn", "Cancel", NULL))
			.End()
			.AddGlue()
			.SetInsets(5, 5, 5, 5);
	}

	bool QuitRequested() override
	{
		be_app->PostMessage(B_QUIT_REQUESTED);
		return true;
	}
};

class LayoutApp : public BApplication {
public:
	LayoutApp()
		: BApplication("application/x-vnd.example-layout")
	{
	}
	
	virtual void ReadyToRun()
	{
		BRect r(100, 100, 400, 300);
		LayoutWindow* w = new LayoutWindow(r);
		w->Show();
	}		
};

int main()
{
	LayoutApp app;
	app.Run();
	return 0;
}
