#include <Application.h>
#include <Window.h>
#include <Button.h>
#include <TextControl.h>
#include <StringView.h>
#include <Messenger.h>
#include <Message.h>
#include <Roster.h>
#include <String.h>
#include <Alert.h>

const uint32 MSG_SEND_BY_SIGNATURE = 'SNDS';
const uint32 MSG_SEND_BY_TARGET = 'SNDT';
const uint32 MSG_CUSTOM = 'CMSG';

class SenderWindow : public BWindow {
public:
    SenderWindow()
        : BWindow(BRect(150, 150, 550, 350), "Message Sender",
                  B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE),
          fMessageCount(0)
    {
	    BView* background = new BView(Bounds(), "background", B_FOLLOW_ALL, B_WILL_DRAW);
    	background->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	    AddChild(background);
    	
        BRect frame = Bounds();
        frame.InsetBy(10, 10);
        
        // Title
        BStringView* title = new BStringView(
            BRect(frame.left, frame.top, frame.right, frame.top + 20),
            "title", "Inter-application Message Sending");
        title->SetFont(be_bold_font);
        background->AddChild(title);
        
        // Text field
        fTextControl = new BTextControl(
            BRect(frame.left, frame.top + 30, frame.right, frame.top + 55),
            "text", "Message:", "Hello from Sender!",
            NULL, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        background->AddChild(fTextControl);
        
        // Send by signature button
        fSignatureButton = new BButton(
            BRect(frame.left, frame.top + 70, frame.left + 180, frame.top + 100),
            "sigbtn", "Send by Signature",
            new BMessage(MSG_SEND_BY_SIGNATURE));
        background->AddChild(fSignatureButton);
        
        // Send by target button
        fTargetButton = new BButton(
            BRect(frame.right - 180, frame.top + 70, frame.right, frame.top + 100),
            "tgtbtn", "Send by Target",
            new BMessage(MSG_SEND_BY_TARGET));
        background->AddChild(fTargetButton);
        
        // Info
        BString info;
        info << "Method 1: Uses receiver app signature\n";
        info << "Method 2: Uses be_roster to find target\n\n";
        info << "Receiver application must be running!";
        
        BStringView* infoView = new BStringView(
            BRect(frame.left, frame.top + 115, frame.right, frame.top + 180),
            "info", info.String());
        infoView->SetAlignment(B_ALIGN_CENTER);
        background->AddChild(infoView);
    }
    
    virtual void MessageReceived(BMessage* message) {
        switch (message->what) {
            case MSG_SEND_BY_SIGNATURE:
                SendBySignature();
                break;
                
            case MSG_SEND_BY_TARGET:
                SendByTarget();
                break;
                
            default:
                BWindow::MessageReceived(message);
                break;
        }
    }
    
private:
    void SendBySignature() {
        // Method 1: Create a BMessenger with signature
        const char* signature = "application/x-vnd.example-Receiver";
        BMessenger messenger(signature);
        
        if (!messenger.IsValid()) {
            BAlert* alert = new BAlert("Error",
                "Unable to find receiver application!\n"
                "Make sure it is running.",
                "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT);
            alert->Go();
            return;
        }
        
        BMessage msg(MSG_CUSTOM);
        msg.AddString("data", fTextControl->Text());
        msg.AddInt32("count", ++fMessageCount);
        
        status_t status = messenger.SendMessage(&msg);
        if (status == B_OK) {
            BString text;
            text << "Message #" << fMessageCount << " sent by signature!";
            ShowSuccess(text.String());
        }
    }
    
    void SendByTarget() {
        // Method 2: Use be_roster to find the application
        const char* signature = "application/x-vnd.example-Receiver";
        team_id team = be_roster->TeamFor(signature);
        
        if (team < 0) {
            BAlert* alert = new BAlert("Error",
                "Receiver application not found!",
                "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT);
            alert->Go();
            return;
        }
        
        BMessenger messenger(NULL, team);
        
        BMessage msg(MSG_CUSTOM);
        msg.AddString("data", fTextControl->Text());
        msg.AddInt32("count", ++fMessageCount);
        
        status_t status = messenger.SendMessage(&msg);
        if (status == B_OK) {
            BString text;
            text << "Message #" << fMessageCount << " sent by target (team_id)!";
            ShowSuccess(text.String());
        }
    }
    
    void ShowSuccess(const char* text) {
        BAlert* alert = new BAlert("Success", text, "OK",
                                   NULL, NULL, B_WIDTH_AS_USUAL,
                                   B_INFO_ALERT);
        alert->Go();
    }
    
    BTextControl* fTextControl;
    BButton* fSignatureButton;
    BButton* fTargetButton;
    int32 fMessageCount;
};

class SenderApp : public BApplication {
public:
    SenderApp()
        : BApplication("application/x-vnd.example-Sender")
    {
        fWindow = new SenderWindow();
        fWindow->Show();
    }
    
private:
    SenderWindow* fWindow;
};

int main() {
    SenderApp app;
    app.Run();
    return 0;
}
