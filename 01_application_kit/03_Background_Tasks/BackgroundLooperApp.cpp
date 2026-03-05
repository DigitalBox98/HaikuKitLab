#include <Application.h>
#include <Window.h>
#include <Looper.h>
#include <Handler.h>
#include <Message.h>
#include <Messenger.h>
#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <String.h>

enum {
    MSG_DO_WORK = 'dwrk',
    MSG_WORK_DONE = 'wdon',
    MSG_START = 'strt'
};

// ----- Worker Handler -----
class WorkerHandler : public BHandler {
public:
    WorkerHandler(const char* name) : BHandler(name) {}

    virtual void MessageReceived(BMessage* msg) override {
        switch (msg->what) {
            case MSG_DO_WORK: {
                int32 taskId;
                BMessenger replyTo;
                if (msg->FindInt32("taskId", &taskId) == B_OK &&
                    msg->FindMessenger("return", &replyTo) == B_OK) {

                    BMessage update(MSG_WORK_DONE);
                    update.AddInt32("taskId", taskId);
                    update.AddInt32("status", 1); // Processing
                    replyTo.SendMessage(&update);

                    snooze(3000000); // 3 seconds

                    update.ReplaceInt32("status", 2); // Done
                    replyTo.SendMessage(&update);
                }
                break;
            }
            default:
                BHandler::MessageReceived(msg);
        }
    }
};

// ----- Worker Looper -----
class WorkerLooper : public BLooper {
public:
    WorkerLooper() : BLooper("WorkerLooper") {
        fHandler = new WorkerHandler("WorkerHandler");
        AddHandler(fHandler);
    }
    BHandler* Handler() { return fHandler; }
private:
    WorkerHandler* fHandler;
};

// ----- Main window -----
class MainWindow : public BWindow {
public:
    MainWindow(BRect frame)
        : BWindow(frame, "BLooper & BHandler GUI Example",
                  B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
    {
        BView* view = new BView(Bounds(), "mainView",
                                B_FOLLOW_ALL, B_WILL_DRAW);
        view->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
        AddChild(view);

        fLogView = new BStringView(BRect(10, 50, Bounds().Width()-10, Bounds().Height()-10),
                                   "logView", "", B_FOLLOW_ALL_SIDES);
        view->AddChild(fLogView);

        fButton = new BButton(BRect(10, 10, 150, 35), "startButton", "Start",
                              new BMessage(MSG_START));
        view->AddChild(fButton);

        fWorkerLooper = new WorkerLooper();
        fWorkerLooper->Run();

        fTasksTotal = 3;
        fTasksCompleted = 0;
    }

    virtual void MessageReceived(BMessage* msg) override {
        switch (msg->what) {
            case MSG_START:
                StartTasks();
                break;

            case MSG_WORK_DONE: {
                int32 taskId, status;
                if (msg->FindInt32("taskId", &taskId) == B_OK &&
                    msg->FindInt32("status", &status) == B_OK) {

                    BString line;
                    if (status == 1)
                        line << "[Worker] Task " << taskId << " is processing...";
                    else {
                        line << "[Worker] Task " << taskId << " done.";
                        fTasksCompleted++;
                    }
                    AppendLog(line);

                    if (fTasksCompleted == fTasksTotal)
                        AppendLog("[Main] All tasks completed!");
                }
                break;
            }

            default:
                BWindow::MessageReceived(msg);
        }
    }

private:
    void StartTasks() {
        fTasksCompleted = 0;
        AppendLog("[Main] -----------------");
        AppendLog("[Main] Starting tasks...");
        for (int i = 1; i <= fTasksTotal; i++) {
            BMessage msg(MSG_DO_WORK);
            msg.AddInt32("taskId", i);
            msg.AddMessenger("return", BMessenger(this));
            BMessenger(fWorkerLooper->Handler()).SendMessage(&msg);

            BString line;
            line << "[Main] Sent task " << i << " to worker...";
            AppendLog(line);
        }
    }

    void AppendLog(const BString& line) {
        fLogText << line << "\n";
        fLogView->SetText(fLogText.String());
    }

private:
    WorkerLooper* fWorkerLooper;
    BStringView* fLogView;
    BButton* fButton;
    BString fLogText;
    int fTasksTotal;
    int fTasksCompleted;
};

// ----- Application -----
class BackgroundLooper : public BApplication {
public:
    BackgroundLooper() : BApplication("application/x-vnd.Haiku.BackgroundLooper") {}
    virtual void ReadyToRun() override {
        BWindow* window = new MainWindow(BRect(100, 100, 500, 300));
        window->Show();
    }
};

int main() {
    BackgroundLooper app;
    app.Run();
    return 0;
}
