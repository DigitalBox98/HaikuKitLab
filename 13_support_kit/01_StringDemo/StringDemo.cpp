#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <TextControl.h>
#include <String.h>

// Custom view that displays the transformed text
class ResultView : public BView
{
public:
    ResultView(BRect frame)
        :
        BView(frame, "result_view", B_FOLLOW_ALL, B_WILL_DRAW | B_NAVIGABLE)
    {
        SetViewColor(240, 240, 240);
    }

    void SetResultText(const BString& text)
    {
        fResult = text;
        Invalidate();   // Triggers a redraw
    }

    void Draw(BRect updateRect) override
    {
        SetHighColor(0, 0, 0);
        MovePenTo(10, 25);
        DrawString(fResult.String());
    }

private:
    BString fResult;
};

// Main window that contains UI elements
class StringWindow : public BWindow
{
public:
    StringWindow(BRect frame)
        :
        BWindow(frame, "BString Demo", B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
    {
        BRect bounds = Bounds();

        // Text input field
        fInput = new BTextControl(
            BRect(10, 10, bounds.Width() - 10, 40),
            "input",
            "Input:",
            "",
            NULL);

        AddChild(fInput);

        // Transform button
        fButton = new BButton(
            BRect(10, 50, 120, 80),
            "transform",
            "Transform",
            new BMessage('tran'));

        AddChild(fButton);

        // Result view
        fResultView = new ResultView(
            BRect(10, 90, bounds.Width() - 10, bounds.Height() - 10));

        AddChild(fResultView);
    }

    void MessageReceived(BMessage* message) override
    {
        if (message->what == 'tran') {
            HandleTransform();
        } else {
            BWindow::MessageReceived(message);
        }
    }

private:
    void HandleTransform()
    {
        BString text = fInput->Text();

        // Perform several BString operations
        text.Trim();                    // Remove leading/trailing whitespace
        text.ReplaceAll("haikuos", "Haiku");    // Search & replace
        text.ToUpper();                 // Convert to uppercase

        // Build the final display string using SetToFormat()
        BString formatted;
        formatted.SetToFormat("Transformed text: %s", text.String());

        fResultView->SetResultText(formatted);
    }

private:
    BTextControl* fInput;
    BButton* fButton;
    ResultView* fResultView;
};

// Application class
class StringApp : public BApplication
{
public:
    StringApp()
        :
        BApplication("application/x-vnd.example-bstring")
    {
        BRect rect(100, 100, 500, 300);
        StringWindow* window = new StringWindow(rect);
        window->Show();
    }
};

int
main()
{
    StringApp app;
    app.Run();
    return 0;
}
