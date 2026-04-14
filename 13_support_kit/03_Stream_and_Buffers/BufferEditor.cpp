#include <Application.h>
#include <Window.h>
#include <View.h>
#include <TextView.h>
#include <ScrollView.h>
#include <Button.h>
#include <StringView.h>
#include <FilePanel.h>
#include <File.h>
#include <Path.h>
#include <Entry.h>
#include <Alert.h>
#include <String.h>
#include <DataIO.h>
#include <stdio.h>

const uint32 MSG_LOAD_FILE = 'LOAD';
const uint32 MSG_SAVE_FILE = 'SAVF';
const uint32 MSG_CLEAR_BUFFER = 'CLRB';
const uint32 MSG_SHOW_BUFFER_INFO = 'INFO';
const uint32 MSG_COPY_TO_MEMORY = 'COPY';
const uint32 MSG_FILE_SELECTED = 'FSEL';
const uint32 MSG_SAVE_PANEL_DONE = 'SVDN';

class BufferEditorView : public BView {
public:
    BufferEditorView(BRect frame)
        : BView(frame, "buffer_view", B_FOLLOW_ALL, B_WILL_DRAW),
          fBuffer(NULL),
          fBufferSize(0),
          fOpenPanel(NULL),
          fSavePanel(NULL)
    {
        SetViewColor(240, 240, 240);
        
        BRect bounds = Bounds();
        
        // Title
        BStringView* title = new BStringView(
            BRect(10, 10, bounds.right - 10, 30),
            "title", "Buffer Editor - BDataIO Streams Demo",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        title->SetFont(be_bold_font);
        AddChild(title);
        
        // Info display
        fInfoView = new BStringView(
            BRect(10, 35, bounds.right - 10, 55),
            "info", "No file loaded - Buffer empty",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fInfoView);
        
        // Text editor
        BRect textRect(10, 65, bounds.right - B_V_SCROLL_BAR_WIDTH - 10,
                       bounds.bottom - 110);
        BRect editRect = textRect.OffsetToCopy(B_ORIGIN);
        editRect.InsetBy(5, 5);
        
        fTextView = new BTextView(textRect, "textview",
                                  editRect,
                                  B_FOLLOW_ALL,
                                  B_WILL_DRAW | B_NAVIGABLE);
        fTextView->SetWordWrap(true);
        
        BScrollView* scrollView = new BScrollView("scrollview",
                                                   fTextView,
                                                   B_FOLLOW_ALL,
                                                   0, false, true);
        AddChild(scrollView);
        
        // Buttons row 1
        float buttonTop = bounds.bottom - 95;
        
        fLoadButton = new BButton(
            BRect(10, buttonTop, 110, buttonTop + 30),
            "load", "Load File",
            new BMessage(MSG_LOAD_FILE),
            B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
        AddChild(fLoadButton);
        
        fSaveButton = new BButton(
            BRect(120, buttonTop, 220, buttonTop + 30),
            "save", "Save File",
            new BMessage(MSG_SAVE_FILE),
            B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
        AddChild(fSaveButton);
        
        fCopyButton = new BButton(
            BRect(230, buttonTop, 360, buttonTop + 30),
            "copy", "Copy to Memory",
            new BMessage(MSG_COPY_TO_MEMORY),
            B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
        AddChild(fCopyButton);
        
        // Buttons row 2
        buttonTop += 40;
        
        fClearButton = new BButton(
            BRect(10, buttonTop, 110, buttonTop + 30),
            "clear", "Clear Buffer",
            new BMessage(MSG_CLEAR_BUFFER),
            B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
        AddChild(fClearButton);
        
        fInfoButton = new BButton(
            BRect(120, buttonTop, 220, buttonTop + 30),
            "info", "Buffer Info",
            new BMessage(MSG_SHOW_BUFFER_INFO),
            B_FOLLOW_LEFT | B_FOLLOW_BOTTOM);
        AddChild(fInfoButton);
        
        // Sample text
        const char* sample = 
            "Welcome to Buffer Editor!\n\n"
            "This demo shows BDataIO streams:\n"
            "• BMallocIO - dynamic memory buffer\n"
            "• BMemoryIO - fixed memory buffer\n"
            "• BFile - file I/O (via BDataIO)\n\n"
            "Try loading a text file or editing this text,\n"
            "then save it to see streams in action!";
        
        fTextView->SetText(sample);
    }
    
    ~BufferEditorView() {
        delete fOpenPanel;
        delete fSavePanel;
        delete fBuffer;
    }
    
    void AttachedToWindow() override {
        BView::AttachedToWindow();
        
        fLoadButton->SetTarget(this);
        fSaveButton->SetTarget(this);
        fClearButton->SetTarget(this);
        fInfoButton->SetTarget(this);
        fCopyButton->SetTarget(this);
        
        fTextView->MakeFocus();
    }
    
    void MessageReceived(BMessage* message) override {
        switch (message->what) {
            case MSG_LOAD_FILE:
                HandleLoadFile();
                break;
                
            case MSG_SAVE_FILE:
                HandleSaveFile();
                break;
                
            case MSG_CLEAR_BUFFER:
                HandleClearBuffer();
                break;
                
            case MSG_SHOW_BUFFER_INFO:
                HandleShowBufferInfo();
                break;
                
            case MSG_COPY_TO_MEMORY:
                HandleCopyToMemory();
                break;
                
            case B_REFS_RECEIVED:
                HandleRefsReceived(message);
                break;
                
            case B_SAVE_REQUESTED:
                HandleSaveRequested(message);
                break;
                
            default:
                BView::MessageReceived(message);
                break;
        }
    }
    
private:
    void HandleLoadFile() {
        if (!fOpenPanel) {
            fOpenPanel = new BFilePanel(B_OPEN_PANEL,
                                        new BMessenger(this),
                                        NULL, B_FILE_NODE,
                                        false);
        }
        fOpenPanel->Show();
    }
    
    void HandleRefsReceived(BMessage* message) {
        entry_ref ref;
        if (message->FindRef("refs", &ref) != B_OK)
            return;
            
        BEntry entry(&ref);
        BPath path;
        entry.GetPath(&path);
        
        // Load file into BMallocIO
        BFile file(&ref, B_READ_ONLY);
        if (file.InitCheck() != B_OK) {
            ShowError("Failed to open file!");
            return;
        }
        
        off_t fileSize;
        file.GetSize(&fileSize);
        
        // Create BMallocIO buffer
        delete fBuffer;
        fBuffer = new BMallocIO();
        
        // Read file into buffer using BDataIO interface
        const size_t chunkSize = 4096;
        char chunk[chunkSize];
        ssize_t bytesRead;
        
        while ((bytesRead = file.Read(chunk, chunkSize)) > 0) {
            fBuffer->Write(chunk, bytesRead);
        }
        
        fBufferSize = fBuffer->BufferLength();
        
        // Display buffer contents
        fBuffer->Seek(0, SEEK_SET);
        char* text = new char[fBufferSize + 1];
        fBuffer->Read(text, fBufferSize);
        text[fBufferSize] = '\0';
        
        fTextView->SetText(text);
        delete[] text;
        
        // Update info
        BString info;
        info << "Loaded: " << path.Leaf() << " (" << fBufferSize << " bytes)";
        fInfoView->SetText(info.String());
    }
    
    void HandleSaveFile() {
        if (!fSavePanel) {
            fSavePanel = new BFilePanel(B_SAVE_PANEL,
                                        new BMessenger(this),
                                        NULL, B_FILE_NODE,
                                        false);
        }
        fSavePanel->Show();
    }
    
    void HandleSaveRequested(BMessage* message) {
        entry_ref dirRef;
        const char* name;
        
        if (message->FindRef("directory", &dirRef) != B_OK ||
            message->FindString("name", &name) != B_OK)
            return;
            
        BDirectory dir(&dirRef);
        BFile file(&dir, name, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
        
        if (file.InitCheck() != B_OK) {
            ShowError("Failed to create file!");
            return;
        }
        
        // Get text from editor
        const char* text = fTextView->Text();
        int32 length = fTextView->TextLength();
        
        // Write using BDataIO interface
        ssize_t written = file.Write(text, length);
        
        if (written == length) {
            BString info;
            info << "Saved: " << name << " (" << written << " bytes)";
            fInfoView->SetText(info.String());
            
            BAlert* alert = new BAlert("Success",
                "File saved successfully!",
                "OK", NULL, NULL,
                B_WIDTH_AS_USUAL, B_INFO_ALERT);
            alert->Go();
        } else {
            ShowError("Write error!");
        }
    }
    
    void HandleClearBuffer() {
        delete fBuffer;
        fBuffer = NULL;
        fBufferSize = 0;
        
        fTextView->SetText("");
        fInfoView->SetText("Buffer cleared");
    }
    
    void HandleShowBufferInfo() {
        BString info;
        info << "Buffer Information:\n\n";
        
        if (fBuffer) {
            info << "Type: BMallocIO (dynamic memory buffer)\n";
            info << "Buffer size: " << fBuffer->BufferLength() << " bytes\n";
            info << "Current position: " << fBuffer->Position() << "\n";
            info << "Memory allocated: ~" << fBuffer->BufferLength() << " bytes\n\n";
            info << "BMallocIO grows automatically as you write data.\n";
            info << "Perfect for building data in memory before saving.";
        } else {
            info << "No buffer allocated.\n\n";
            info << "Load a file or edit text to create a buffer.";
        }
        
        // Text view info
        int32 textLength = fTextView->TextLength();
        info << "\n\nText View:\n";
        info << "Text length: " << textLength << " characters\n";
        info << "Lines: " << fTextView->CountLines();
        
        BAlert* alert = new BAlert("Buffer Info",
            info.String(),
            "OK", NULL, NULL,
            B_WIDTH_AS_USUAL, B_INFO_ALERT);
        alert->Go();
    }
    
    void HandleCopyToMemory() {
        // Demonstrate BMemoryIO - fixed size memory buffer
        const char* text = fTextView->Text();
        int32 length = fTextView->TextLength();
        
        if (length == 0) {
            ShowError("No text to copy!");
            return;
        }
        
        // Create fixed-size memory buffer
        char* memBuffer = new char[length + 100]; // Extra space
        BMemoryIO memIO(memBuffer, length + 100);
        
        // Write text to memory buffer
        memIO.Write(text, length);
        
        // Read it back (demonstrating BDataIO interface)
        memIO.Seek(0, SEEK_SET);
        char* readBuffer = new char[length + 1];
        ssize_t bytesRead = memIO.Read(readBuffer, length);
        readBuffer[bytesRead] = '\0';
        
        BString info;
        info << "Copied to BMemoryIO buffer:\n\n";
        info << "Written: " << length << " bytes\n";
        info << "Read back: " << bytesRead << " bytes\n";
        info << "Buffer size: " << (length + 100) << " bytes\n\n";
        info << "BMemoryIO uses fixed-size memory.\n";
        info << "Unlike BMallocIO, it won't grow automatically.";
        
        delete[] readBuffer;
        delete[] memBuffer;
        
        BAlert* alert = new BAlert("Memory Copy",
            info.String(),
            "OK", NULL, NULL,
            B_WIDTH_AS_USUAL, B_INFO_ALERT);
        alert->Go();
    }
    
    void ShowError(const char* message) {
        BAlert* alert = new BAlert("Error",
            message,
            "OK", NULL, NULL,
            B_WIDTH_AS_USUAL, B_WARNING_ALERT);
        alert->Go();
    }
    
    BMallocIO* fBuffer;
    size_t fBufferSize;
    
    BTextView* fTextView;
    BStringView* fInfoView;
    BButton* fLoadButton;
    BButton* fSaveButton;
    BButton* fClearButton;
    BButton* fInfoButton;
    BButton* fCopyButton;
    
    BFilePanel* fOpenPanel;
    BFilePanel* fSavePanel;
};

class BufferEditorWindow : public BWindow {
public:
    BufferEditorWindow(BRect frame)
        : BWindow(frame, "Buffer Editor - BDataIO Demo",
                  B_TITLED_WINDOW,
                  B_QUIT_ON_WINDOW_CLOSE | B_AUTO_UPDATE_SIZE_LIMITS)
    {
        BRect viewRect = Bounds();
        BufferEditorView* view = new BufferEditorView(viewRect);
        AddChild(view);
    }
};

class BufferEditorApp : public BApplication {
public:
    BufferEditorApp()
        : BApplication("application/x-vnd.example-BufferEditor")
    {
        BRect rect(100, 100, 650, 550);
        BufferEditorWindow* window = new BufferEditorWindow(rect);
        window->Show();
    }
};

int main() {
    BufferEditorApp app;
    app.Run();
    return 0;
}
