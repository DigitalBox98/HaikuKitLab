#include <Application.h>
#include <Window.h>
#include <TextView.h>
#include <ScrollView.h>
#include <Message.h>
#include <String.h>
#include <stdio.h>

const uint32 MSG_CUSTOM = 'CMSG';

class ReceiverWindow : public BWindow {
public:
    ReceiverWindow()
        : BWindow(BRect(100, 100, 600, 400), "Message Receiver",
                  B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
    {
        BRect textRect = Bounds();
        textRect.InsetBy(5, 5);
        
        fTextView = new BTextView(textRect, "textview", 
                                  textRect.OffsetToCopy(B_ORIGIN),
                                  B_FOLLOW_ALL_SIDES,
                                  B_WILL_DRAW);
        fTextView->MakeEditable(false);
        fTextView->SetWordWrap(true);
        
        BScrollView* scrollView = new BScrollView("scrollview", fTextView,
                                                   B_FOLLOW_ALL_SIDES, 0,
                                                   false, true);
        AddChild(scrollView);
        
        AddMessage("=== Receiver Application Started ===\n");
        AddMessage("Signature: application/x-vnd.example-Receiver\n");
        AddMessage("Waiting for messages...\n\n");
    }
    
    virtual void MessageReceived(BMessage* message) {
        switch (message->what) {
            case MSG_CUSTOM: {
                BString text;
                text << "Message received!\n";
                text << "  Code: 0x" << B_HOST_TO_BENDIAN_INT32(MSG_CUSTOM) << "\n";
                
                const char* data;
                if (message->FindString("data", &data) == B_OK) {
                    text << "  Content: \"" << data << "\"\n";
                }
                
                int32 count;
                if (message->FindInt32("count", &count) == B_OK) {
                    text << "  Counter: " << count << "\n";
                }
                
                text << "\n";
                AddMessage(text.String());
                break;
            }
            
            default:
                BWindow::MessageReceived(message);
                break;
        }
    }
    
private:
    void AddMessage(const char* text) {
        fTextView->Insert(fTextView->TextLength(), text, strlen(text));
        fTextView->ScrollToSelection();
    }
    
    BTextView* fTextView;
};

class ReceiverApp : public BApplication {
public:
    ReceiverApp()
        : BApplication("application/x-vnd.example-Receiver")
    {
        fWindow = new ReceiverWindow();
        fWindow->Show();
    }
    
    virtual void MessageReceived(BMessage* message) {
        // Forward to window for display
        fWindow->PostMessage(message);
    }
    
private:
    ReceiverWindow* fWindow;
};

int main() {
    ReceiverApp app;
    app.Run();
    return 0;
}
