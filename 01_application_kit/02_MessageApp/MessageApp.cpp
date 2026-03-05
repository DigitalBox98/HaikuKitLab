#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <TextControl.h>
#include <Messenger.h>
#include <Message.h>
#include <String.h>
#include <Font.h>
#include <math.h> // for ceil()

// View that displays messages with proper line breaks
class MessageView : public BView {
public:
    MessageView(BRect frame)
        : BView(frame, "message_view", B_FOLLOW_ALL, B_WILL_DRAW)
    {
        SetViewColor(255, 255, 255); // White background
    }

    void Draw(BRect) override {
        SetHighColor(0, 0, 0); // Black text

        BFont font;
        GetFont(&font);
        font_height fh;
        font.GetHeight(&fh);
        float lineHeight = ceil(fh.ascent + fh.descent + fh.leading);

        float y = 20;
        BString line;
        const char* str = fText.String();

        while (*str) {
            if (*str == '\n') {
                DrawString(line.String(), BPoint(10, y));
                line.Truncate(0);
                y += lineHeight;
            } else {
                line << *str;
            }
            str++;
        }

        if (!line.IsEmpty()) {
            DrawString(line.String(), BPoint(10, y));
        }
    }

    void Append(const BString& text) {
        fText << text << "\n";
        Invalidate();
    }

    // MessageReceived handles its own messages
    void MessageReceived(BMessage* msg) override {
        if (msg->what == 'updt') {
            const char* recvd;
            if (msg->FindString("text", &recvd) == B_OK) {
                Append(recvd);
            }
        } else {
            BView::MessageReceived(msg);
        }
    }

private:
    BString fText;
};

// Main window + controller
class MessagingWindow : public BWindow {
public:
    MessagingWindow(BRect frame)
        : BWindow(frame, "BMessage Example", B_TITLED_WINDOW,
                  B_QUIT_ON_WINDOW_CLOSE)
    {
        BRect bounds = Bounds();

        // Message display view
        fView = new MessageView(BRect(bounds.left, bounds.top,
                                      bounds.right, bounds.top + 150));
        AddChild(fView);

        // Text input
        fTextControl = new BTextControl(BRect(10, 160, bounds.right - 10, 180),
                                        "input", "Message:", "", nullptr);
        AddChild(fTextControl);

        // Send button
        BButton* sendBtn = new BButton(BRect(10, 200, 100, 230),
                                       "send_button", "Send",
                                       new BMessage('send'));
        sendBtn->SetTarget(this); // Window handles the click
        AddChild(sendBtn);

        // Reply button
        BButton* replyBtn = new BButton(BRect(120, 200, 220, 230),
                                        "reply_button", "Reply",
                                        new BMessage('repl'));
        replyBtn->SetTarget(this);
        AddChild(replyBtn);
    }

    void MessageReceived(BMessage* msg) override {
        switch (msg->what) {
            case 'send':
            case 'repl': {
                const char* text = fTextControl->Text();
                BString prefix = (msg->what == 'send') ? "Sent: " : "Reply: ";
                BString full = prefix << text;

                // Create a message to update the view
                BMessage forwardMsg('updt');
                forwardMsg.AddString("text", full);

                // Send the message directly to the view
                BMessenger msgr(fView);
                msgr.SendMessage(&forwardMsg);

                break;
            }
            default:
                BWindow::MessageReceived(msg);
        }
    }

private:
    MessageView* fView;
    BTextControl* fTextControl;
};

// Application class
class MessagingApp : public BApplication {
public:
    MessagingApp()
        : BApplication("application/x-vnd.example-bmessenger")
    {
        BRect rect(100, 100, 400, 350);
        MessagingWindow* w = new MessagingWindow(rect);
        w->Show();
    }
};

int main() {
    MessagingApp app;
    app.Run();
    return 0;
}
