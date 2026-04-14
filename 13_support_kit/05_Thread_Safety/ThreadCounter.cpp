// ThreadCounter.cpp - Demonstrates BLocker and BAutolock for thread safety

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <CheckBox.h>
#include <Locker.h>
#include <Autolock.h>
#include <OS.h>
#include <String.h>
#include <stdio.h>

const uint32 MSG_INCREMENT = 'INCR';
const uint32 MSG_DECREMENT = 'DECR';
const uint32 MSG_FAST_INCREMENT = 'FINC';
const uint32 MSG_RESET = 'RSET';
const uint32 MSG_UPDATE_DISPLAY = 'UPDT';
const uint32 MSG_TOGGLE_LOCKING = 'LOCK';

// Shared counter with thread-safe access
class ThreadSafeCounter {
public:
    ThreadSafeCounter()
        : fValue(0),
          fLocker("counter_lock"),
          fUseLocking(true)
    {
    }
    
    void Increment() {
        if (fUseLocking) {
            // Thread-safe version using BAutolock
            BAutolock autolock(fLocker);
            fValue++;
        } else {
            // UNSAFE: No locking (for demonstration)
            fValue++;
        }
    }
    
    void Decrement() {
        if (fUseLocking) {
            // Thread-safe version using BAutolock
            BAutolock autolock(fLocker);
            fValue--;
        } else {
            // UNSAFE: No locking
            fValue--;
        }
    }
    
    void Reset() {
        BAutolock autolock(fLocker);
        fValue = 0;
    }
    
    int32 GetValue() {
        if (fUseLocking) {
            BAutolock autolock(fLocker);
            return fValue;
        } else {
            // UNSAFE: No locking
            return fValue;
        }
    }
    
    void SetUseLocking(bool useLocking) {
        BAutolock autolock(fLocker);
        fUseLocking = useLocking;
    }
    
    bool UseLocking() const {
        return fUseLocking;
    }
    
private:
    int32 fValue;
    BLocker fLocker;
    bool fUseLocking;
};

// Thread function for incrementing
int32 increment_thread(void* data) {
    ThreadSafeCounter* counter = (ThreadSafeCounter*)data;
    
    // Increment 250 times with small delays
    for (int i = 0; i < 250; i++) {
        counter->Increment();
        
        // Small delay to increase chance of race conditions
        snooze(100);  // 0.1 milliseconds
    }
    
    return 0;
}

// Thread function for decrementing
int32 decrement_thread(void* data) {
    ThreadSafeCounter* counter = (ThreadSafeCounter*)data;
    
    // Decrement 250 times with small delays
    for (int i = 0; i < 250; i++) {
        counter->Decrement();
        snooze(100);
    }
    
    return 0;
}

// Thread function for fast increments (shows race conditions better)
int32 fast_increment_thread(void* data) {
    ThreadSafeCounter* counter = (ThreadSafeCounter*)data;
    
    // Rapid increments without delays
    for (int i = 0; i < 2500; i++) {
        counter->Increment();
    }
    
    return 0;
}

class CounterView : public BView {
public:
    CounterView(BRect frame)
        : BView(frame, "counter_view", B_FOLLOW_ALL, B_WILL_DRAW),
          fCounter(new ThreadSafeCounter()),
          fActiveThreads(0)
    {
        SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        
        BRect bounds = Bounds();
        float left = 10;
        float right = bounds.right - 10;
        float top = 10;
        
        // Title
        BStringView* title = new BStringView(
            BRect(left, top, right, top + 20),
            "title", "Thread-Safe Counter - BLocker Demo",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        title->SetFont(be_bold_font);
        AddChild(title);
        top += 35;
        
        // Counter display
        fCounterView = new BStringView(
            BRect(left, top, right, top + 40),
            "counter", "Counter: 0",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        fCounterView->SetFont(be_bold_font);
        fCounterView->SetFontSize(28);
        AddChild(fCounterView);
        top += 55;
        
        // Thread count display
        fThreadView = new BStringView(
            BRect(left, top, right, top + 20),
            "threads", "Active threads: 0",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fThreadView);
        top += 30;
        
        // Locking status
        fLockingView = new BStringView(
            BRect(left, top, right, top + 20),
            "locking", "Thread safety: ENABLED",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fLockingView);
        top += 35;
        
        // Locking toggle
        fLockingCheck = new BCheckBox(
            BRect(left, top, right, top + 25),
            "lockcheck", "Use thread-safe locking (BAutolock)",
            new BMessage(MSG_TOGGLE_LOCKING),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        fLockingCheck->SetValue(B_CONTROL_ON);
        AddChild(fLockingCheck);
        top += 40;
        
        // Info text
        BStringView* info = new BStringView(
            BRect(left, top, right, top + 60),
            "info",
            "Try disabling locking and running Fast Increment\n"
            "multiple times. Race conditions will cause incorrect\n"
            "results. With locking enabled, the counter is always accurate.",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        info->SetFontSize(10);
        AddChild(info);
        top += 70;
        
        // Buttons row 1
        fIncrementButton = new BButton(
            BRect(left, top, left + 140, top + 30),
            "inc", "Increment (1000x)",
            new BMessage(MSG_INCREMENT),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fIncrementButton);
        
        fDecrementButton = new BButton(
            BRect(left + 150, top, left + 290, top + 30),
            "dec", "Decrement (1000x)",
            new BMessage(MSG_DECREMENT),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fDecrementButton);
        top += 40;
        
        // Buttons row 2
        fFastButton = new BButton(
            BRect(left, top, left + 140, top + 30),
            "fast", "Fast Increment (10k)",
            new BMessage(MSG_FAST_INCREMENT),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fFastButton);
        
        fResetButton = new BButton(
            BRect(left + 150, top, left + 290, top + 30),
            "reset", "Reset Counter",
            new BMessage(MSG_RESET),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fResetButton);
        top += 50;
        
        // Explanation
        BStringView* explanation = new BStringView(
            BRect(left, top, right, top + 80),
            "explain",
            "BLocker provides mutual exclusion - only one thread can\n"
            "access the protected resource at a time.\n\n"
            "BAutolock automatically locks on creation and unlocks\n"
            "on destruction (RAII pattern).",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        explanation->SetFontSize(10);
        AddChild(explanation);
        
        // Start update thread
        StartUpdateThread();
    }
    
    ~CounterView() {
        fUpdateThreadRunning = false;
        
        status_t status;
        wait_for_thread(fUpdateThread, &status);
        
        delete fCounter;
    }
    
    void AttachedToWindow() override {
        BView::AttachedToWindow();
        
        fIncrementButton->SetTarget(this);
        fDecrementButton->SetTarget(this);
        fFastButton->SetTarget(this);
        fResetButton->SetTarget(this);
        fLockingCheck->SetTarget(this);
    }
    
    void MessageReceived(BMessage* message) override {
        switch (message->what) {
            case MSG_INCREMENT:
                LaunchIncrementThread();
                break;
                
            case MSG_DECREMENT:
                LaunchDecrementThread();
                break;
                
            case MSG_FAST_INCREMENT:
                LaunchFastIncrementThread();
                break;
                
            case MSG_RESET:
                ResetCounter();
                break;
                
            case MSG_UPDATE_DISPLAY:
                UpdateDisplay();
                break;
                
            case MSG_TOGGLE_LOCKING:
                ToggleLocking();
                break;
                
            default:
                BView::MessageReceived(message);
                break;
        }
    }
    
private:
	void LaunchIncrementThread() {
	    // Launch 4 threads increment in parallel
	    for (int i = 0; i < 4; i++) {
	        atomic_add((int32*)&fActiveThreads, 1);
	        thread_id thread = spawn_thread(increment_thread, "increment",
	                                        B_NORMAL_PRIORITY, fCounter);
	        resume_thread(thread);
	        
	        spawn_thread(monitor_thread, "monitor", B_NORMAL_PRIORITY,
	                     new MonitorData{this, thread});
	    }
	}
	
	void LaunchDecrementThread() {
	    // Launch 4 threads decrement in parallel
	    for (int i = 0; i < 4; i++) {
	        atomic_add((int32*)&fActiveThreads, 1);
	        thread_id thread = spawn_thread(decrement_thread, "decrement",
	                                        B_NORMAL_PRIORITY, fCounter);
	        resume_thread(thread);
	        
	        spawn_thread(monitor_thread, "monitor", B_NORMAL_PRIORITY,
	                     new MonitorData{this, thread});
	    }
	}
    
	void LaunchFastIncrementThread() {
	    // Launch 4 threads in parallel
	    for (int i = 0; i < 4; i++) {
	        atomic_add((int32*)&fActiveThreads, 1);
	        thread_id thread = spawn_thread(fast_increment_thread, "fast_increment",
	                                        B_NORMAL_PRIORITY, fCounter);
	        resume_thread(thread);
	        
	        spawn_thread(monitor_thread, "monitor", B_NORMAL_PRIORITY,
	                     new MonitorData{this, thread});
	    }
	}
    
    void ResetCounter() {
        fCounter->Reset();
        UpdateDisplay();
    }
    
    void ToggleLocking() {
        bool enabled = (fLockingCheck->Value() == B_CONTROL_ON);
        fCounter->SetUseLocking(enabled);
        
        if (enabled) {
            fLockingView->SetText("Thread safety: ENABLED ✓");
        } else {
            fLockingView->SetText("Thread safety: DISABLED ⚠ (UNSAFE!)");
        }
    }
    
    void UpdateDisplay() {
        int32 value = fCounter->GetValue();
        
        BString text;
        text << "Counter: " << value;
        fCounterView->SetText(text.String());
        
        text = "";
        text << "Active threads: " << fActiveThreads;
        fThreadView->SetText(text.String());
    }
    
    void StartUpdateThread() {
        fUpdateThreadRunning = true;
        fUpdateThread = spawn_thread(update_thread, "update_display",
                                      B_NORMAL_PRIORITY, this);
        resume_thread(fUpdateThread);
    }
    
    // Thread to periodically update display
    static int32 update_thread(void* data) {
        CounterView* view = (CounterView*)data;
        
        while (view->fUpdateThreadRunning) {
            BMessage msg(MSG_UPDATE_DISPLAY);
            BMessenger(view).SendMessage(&msg);
            snooze(100000);  // Update every 0.1 seconds
        }
        
        return 0;
    }
    
    // Monitor thread completion
    struct MonitorData {
        CounterView* view;
        thread_id thread;
    };
    
    static int32 monitor_thread(void* data) {
        MonitorData* monitorData = (MonitorData*)data;
        
        status_t status;
        wait_for_thread(monitorData->thread, &status);
        
        atomic_add((int32*)&monitorData->view->fActiveThreads, -1);
        
        delete monitorData;
        return 0;
    }
    
    ThreadSafeCounter* fCounter;
    vint32 fActiveThreads;
    
    BStringView* fCounterView;
    BStringView* fThreadView;
    BStringView* fLockingView;
    BCheckBox* fLockingCheck;
    BButton* fIncrementButton;
    BButton* fDecrementButton;
    BButton* fFastButton;
    BButton* fResetButton;
    
    thread_id fUpdateThread;
    volatile bool fUpdateThreadRunning;
};

class CounterWindow : public BWindow {
public:
    CounterWindow(BRect frame)
        : BWindow(frame, "Thread-Safe Counter",
                  B_TITLED_WINDOW,
                  B_QUIT_ON_WINDOW_CLOSE | B_NOT_RESIZABLE)
    {
        BRect viewRect = Bounds();
        CounterView* view = new CounterView(viewRect);
        AddChild(view);
    }
};

class CounterApp : public BApplication {
public:
    CounterApp()
        : BApplication("application/x-vnd.example-ThreadCounter")
    {
        BRect rect(100, 100, 500, 500);
        CounterWindow* window = new CounterWindow(rect);
        window->Show();
    }
};

int main() {
    CounterApp app;
    app.Run();
    return 0;
}
