#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <TextControl.h>
#include <StringView.h>
#include <Clipboard.h>
#include <Roster.h>
#include <Cursor.h>
#include <Message.h>
#include <String.h>
#include <iostream>

using std::cout;
using std::endl;

class HoverView : public BView {
public:
    HoverView(BRect frame)
        : BView(frame, "hover", B_FOLLOW_LEFT | B_FOLLOW_TOP,
                B_WILL_DRAW | B_FRAME_EVENTS)
    {
        SetViewColor(250,250,250);
    }

    void Draw(BRect updateRect) override
    {
        SetHighColor(0, 0, 0);
        DrawString("Hover here to change cursor", BPoint(10, 25));
    }

    void MouseMoved(BPoint where, uint32 transit, const BMessage* message) override
    {
    if (transit == B_ENTERED_VIEW) {
        SetViewCursor(B_CURSOR_I_BEAM, true);
    } else if (transit == B_EXITED_VIEW) {
        SetViewCursor(B_CURSOR_SYSTEM_DEFAULT, true);
    }
    }
};


// ===== Main Window =====
class MainWindow : public BWindow {
public:
    MainWindow(BRect frame)
        : BWindow(frame, "System Interaction", B_TITLED_WINDOW,
                  B_QUIT_ON_WINDOW_CLOSE)
    {
        BRect bounds = Bounds();

	    BView* background = new BView(bounds, "background", B_FOLLOW_ALL, B_WILL_DRAW);
    	background->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	    AddChild(background);


        // Input field
        fText = new BTextControl(BRect(10, 10, bounds.right - 10, 35),
                                 "text", "Text:", "", nullptr);
        background->AddChild(fText);

        // Buttons
        float y = 55;

        BButton* launchBtn = new BButton(BRect(10, y, 150, y + 30),
                                         "launch", "Launch StyledEdit",
                                         new BMessage('lapp'));
        launchBtn->SetTarget(this);
        background->AddChild(launchBtn);

        BButton* listBtn = new BButton(BRect(170, y, 320, y + 30),
                                       "list", "List running apps",
                                       new BMessage('list'));
        listBtn->SetTarget(this);
        background->AddChild(listBtn);

        y += 40;

        BButton* copyBtn = new BButton(BRect(10, y, 150, y + 30),
                                       "copy", "Copy text",
                                       new BMessage('copy'));
        copyBtn->SetTarget(this);
        background->AddChild(copyBtn);

        BButton* pasteBtn = new BButton(BRect(170, y, 320, y + 30),
                                        "paste", "Paste text",
                                        new BMessage('past'));
        pasteBtn->SetTarget(this);
        background->AddChild(pasteBtn);

        // Output label
        fOutput = new BStringView(BRect(10, y + 40, bounds.right - 10, y + 60),
                                  "output", "");
        background->AddChild(fOutput);

        // Cursor example view
        HoverView* hv = new HoverView(BRect(10, y + 80, bounds.right - 10, y + 140));
        background->AddChild(hv);
    }


    void MessageReceived(BMessage* msg) override {
        switch (msg->what) {
            
            case 'lapp': {
                status_t s = be_roster->Launch("application/x-vnd.Haiku-StyledEdit");
                fOutput->SetText(s == B_OK ? "StyledEdit launched." :
                                             "Failed to launch StyledEdit.");
                break;
            }

            case 'list': {
                BList appList;
                be_roster->GetAppList(&appList);

                cout << "Running applications:" << endl;

                for (int32 i = 0; i < appList.CountItems(); i++) {

                    // FIXED cast
                    team_id team = static_cast<team_id>((intptr_t)appList.ItemAt(i));

                    app_info info;
                    if (be_roster->GetRunningAppInfo(team, &info) == B_OK) {
                        cout << " - " << info.signature << endl;
                    }
                }

                fOutput->SetText("Application list printed to terminal.");
                break;
            }

            case 'copy': {
                if (be_clipboard->Lock()) {
                    be_clipboard->Clear();
                    BMessage* clip = be_clipboard->Data();
                    clip->AddData("text/plain", B_MIME_TYPE,
                                  fText->Text(), strlen(fText->Text()));
                    be_clipboard->Commit();
                    be_clipboard->Unlock();
                }
                fOutput->SetText("Copied to clipboard.");
                break;
            }

            case 'past': {
                if (be_clipboard->Lock()) {
                    BMessage* clip = be_clipboard->Data();
                    const char* text;
                    ssize_t size;
                    if (clip->FindData("text/plain", B_MIME_TYPE,
                                       (const void**)&text, &size) == B_OK) {
                        fText->SetText(text);
                        fOutput->SetText("Pasted from clipboard.");
                    }
                    be_clipboard->Unlock();
                }
                break;
            }

            default:
                BWindow::MessageReceived(msg);
        }
    }

private:
    BTextControl* fText;
    BStringView*  fOutput;
};


// ===== Application =====
class SystemApp : public BApplication {
public:
    SystemApp() : BApplication("application/x-vnd.example-systemkit") {}

    void ReadyToRun() override {
        MainWindow* w = new MainWindow(BRect(100, 100, 450, 350));
        w->Show();
    }
};


// ===== main =====
int main() {
    SystemApp app;
    app.Run();
    return 0;
}
