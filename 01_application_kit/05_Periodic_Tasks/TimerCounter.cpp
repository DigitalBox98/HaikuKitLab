#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <MessageRunner.h>
#include <MessageQueue.h>
#include <String.h>
#include <stdio.h>

const uint32 MSG_TIMER_TICK = 'TICK';
const uint32 MSG_START_TIMER = 'STRT';
const uint32 MSG_STOP_TIMER = 'STOP';
const uint32 MSG_RESET_COUNTER = 'RSET';

class TimerView : public BView {
public:
    TimerView(BRect frame)
        : BView(frame, "timer_view", B_FOLLOW_ALL, B_WILL_DRAW),
          fCounter(0),
          fMessageRunner(NULL),
          fIsRunning(false)
    {
        SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        
        BRect bounds = Bounds();
        
        // Counter display
        fCounterView = new BStringView(
            BRect(20, 20, bounds.right - 20, 60),
            "counter", "Counter: 0",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        fCounterView->SetFont(be_bold_font);
        fCounterView->SetFontSize(24);
        AddChild(fCounterView);
        
        // Queue info display
        fQueueView = new BStringView(
            BRect(20, 70, bounds.right - 20, 90),
            "queue", "Messages in queue: 0",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fQueueView);
        
        // Status display
        fStatusView = new BStringView(
            BRect(20, 100, bounds.right - 20, 120),
            "status", "Status: Stopped",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fStatusView);
        
        // Start button
        fStartButton = new BButton(
            BRect(20, 140, 120, 170),
            "start", "Start",
            new BMessage(MSG_START_TIMER),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fStartButton);
        
        // Stop button
        fStopButton = new BButton(
            BRect(130, 140, 230, 170),
            "stop", "Stop",
            new BMessage(MSG_STOP_TIMER),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        fStopButton->SetEnabled(false);
        AddChild(fStopButton);
        
        // Reset button
        fResetButton = new BButton(
            BRect(240, 140, 340, 170),
            "reset", "Reset",
            new BMessage(MSG_RESET_COUNTER),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fResetButton);
        
        // Info text
        BStringView* info = new BStringView(
            BRect(20, 190, bounds.right - 20, 250),
            "info",
            "This demo uses BMessageRunner to send periodic messages.\n"
            "The counter increments every second when running.\n"
            "Watch the message queue count update in real-time!",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        info->SetFontSize(10);
        AddChild(info);
    }
    
    ~TimerView() {
        StopTimer();
    }
    
    void AttachedToWindow() override {
        BView::AttachedToWindow();
        
        // Set this view as the target for button messages
        fStartButton->SetTarget(this);
        fStopButton->SetTarget(this);
        fResetButton->SetTarget(this);
    }
    
    void MessageReceived(BMessage* message) override {
        switch (message->what) {
            case MSG_START_TIMER:
                StartTimer();
                break;
                
            case MSG_STOP_TIMER:
                StopTimer();
                break;
                
            case MSG_RESET_COUNTER:
                ResetCounter();
                break;
                
            case MSG_TIMER_TICK:
                HandleTimerTick();
                break;
                
            default:
                BView::MessageReceived(message);
                break;
        }
    }
    
private:
    void StartTimer() {
        if (fIsRunning)
            return;
            
        // Create a BMessageRunner that sends MSG_TIMER_TICK every second
        BMessage timerMsg(MSG_TIMER_TICK);
        bigtime_t interval = 1000000; // 1 second in microseconds
        
        fMessageRunner = new BMessageRunner(
            BMessenger(this),  // Target: this view
            &timerMsg,         // Message to send
            interval,          // Interval: 1 second
            -1                 // Count: -1 means infinite
        );
        
        if (fMessageRunner->InitCheck() == B_OK) {
            fIsRunning = true;
            fStartButton->SetEnabled(false);
            fStopButton->SetEnabled(true);
            fStatusView->SetText("Status: Running");
        }
    }
    
    void StopTimer() {
        if (!fIsRunning)
            return;
            
        delete fMessageRunner;
        fMessageRunner = NULL;
        
        fIsRunning = false;
        fStartButton->SetEnabled(true);
        fStopButton->SetEnabled(false);
        fStatusView->SetText("Status: Stopped");
    }
    
    void ResetCounter() {
        fCounter = 0;
        UpdateDisplay();
    }
    
    void HandleTimerTick() {
        fCounter++;
        UpdateDisplay();
        UpdateQueueInfo();
    }
    
    void UpdateDisplay() {
        BString text;
        text << "Counter: " << fCounter;
        fCounterView->SetText(text.String());
    }
    
    void UpdateQueueInfo() {
        // Access the message queue of our window's looper
        BWindow* window = Window();
        if (window) {
            BMessageQueue* queue = window->MessageQueue();
            if (queue) {
                queue->Lock();
                int32 count = queue->CountMessages();
                queue->Unlock();
                
                BString text;
                text << "Messages in queue: " << count;
                fQueueView->SetText(text.String());
            }
        }
    }
    
    int32 fCounter;
    BMessageRunner* fMessageRunner;
    bool fIsRunning;
    
    BStringView* fCounterView;
    BStringView* fQueueView;
    BStringView* fStatusView;
    BButton* fStartButton;
    BButton* fStopButton;
    BButton* fResetButton;
};

class TimerWindow : public BWindow {
public:
    TimerWindow(BRect frame)
        : BWindow(frame, "Timer & Message Queue Demo",
                  B_TITLED_WINDOW,
                  B_QUIT_ON_WINDOW_CLOSE | B_NOT_RESIZABLE)
    {
        BRect viewRect = Bounds();
        TimerView* view = new TimerView(viewRect);
        AddChild(view);
    }
};

class TimerApp : public BApplication {
public:
    TimerApp()
        : BApplication("application/x-vnd.example-TimerCounter")
    {
        BRect rect(100, 100, 500, 400);
        TimerWindow* window = new TimerWindow(rect);
        window->Show();
    }
};

int main() {
    TimerApp app;
    app.Run();
    return 0;
}
