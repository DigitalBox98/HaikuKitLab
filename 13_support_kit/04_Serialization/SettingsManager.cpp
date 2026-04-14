// SettingsManager.cpp - Demonstrates BArchivable serialization
// Compilation: gcc -o SettingsManager SettingsManager.cpp -lbe

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Button.h>
#include <TextControl.h>
#include <CheckBox.h>
#include <StringView.h>
#include <ColorControl.h>
#include <Slider.h>
#include <File.h>
#include <Path.h>
#include <FindDirectory.h>
#include <Alert.h>
#include <Message.h>
#include <String.h>
#include <stdio.h>

const uint32 MSG_SAVE_SETTINGS = 'SAVE';
const uint32 MSG_LOAD_SETTINGS = 'LOAD';
const uint32 MSG_RESET_SETTINGS = 'RSET';
const uint32 MSG_APPLY_SETTINGS = 'APLY';
const uint32 MSG_SHOW_ARCHIVE = 'SHOW';

// Custom settings class using BArchivable
class AppSettings : public BArchivable {
public:
    AppSettings()
        : BArchivable(),
          fWindowWidth(600),
          fWindowHeight(450),
          fBackgroundColor(make_color(245, 245, 245)),
          fFontSize(12),
          fAutoSave(true),
          fDefaultText("Welcome to Settings Manager!")
    {
    }
    
    // Constructor for unarchiving
    AppSettings(BMessage* archive)
        : BArchivable(archive)
    {
        // Extract data from archive
        if (archive->FindInt32("width", &fWindowWidth) != B_OK)
            fWindowWidth = 600;
            
        if (archive->FindInt32("height", &fWindowHeight) != B_OK)
            fWindowHeight = 450;
            
        // Color is stored as int32 (rgb_color packed)
        int32 colorValue;
        if (archive->FindInt32("bg_color", &colorValue) == B_OK) {
            fBackgroundColor = *((rgb_color*)&colorValue);
        } else {
            fBackgroundColor = make_color(245, 245, 245);
        }
        
        if (archive->FindInt32("font_size", &fFontSize) != B_OK)
            fFontSize = 12;
            
        if (archive->FindBool("auto_save", &fAutoSave) != B_OK)
            fAutoSave = true;
            
        const char* text;
        if (archive->FindString("default_text", &text) == B_OK)
            fDefaultText = text;
        else
            fDefaultText = "Welcome!";
    }
    
    // Required: Create instance from archive
    static BArchivable* Instantiate(BMessage* archive) {
        if (!validate_instantiation(archive, "AppSettings"))
            return NULL;
        return new AppSettings(archive);
    }
    
    // Required: Save to archive
    status_t Archive(BMessage* archive, bool deep = true) const {
        status_t status = BArchivable::Archive(archive, deep);
        if (status != B_OK)
            return status;
            
        // Add class name for instantiation
        archive->AddString("class", "AppSettings");
        
        // Store all settings
        archive->AddInt32("width", fWindowWidth);
        archive->AddInt32("height", fWindowHeight);
        
        // Pack rgb_color into int32
        int32 colorValue = *((int32*)&fBackgroundColor);
        archive->AddInt32("bg_color", colorValue);
        
        archive->AddInt32("font_size", fFontSize);
        archive->AddBool("auto_save", fAutoSave);
        archive->AddString("default_text", fDefaultText.String());
        
        return B_OK;
    }
    
    // Getters
    int32 WindowWidth() const { return fWindowWidth; }
    int32 WindowHeight() const { return fWindowHeight; }
    rgb_color BackgroundColor() const { return fBackgroundColor; }
    int32 FontSize() const { return fFontSize; }
    bool AutoSave() const { return fAutoSave; }
    const char* DefaultText() const { return fDefaultText.String(); }
    
    // Setters
    void SetWindowWidth(int32 width) { fWindowWidth = width; }
    void SetWindowHeight(int32 height) { fWindowHeight = height; }
    void SetBackgroundColor(rgb_color color) { fBackgroundColor = color; }
    void SetFontSize(int32 size) { fFontSize = size; }
    void SetAutoSave(bool autoSave) { fAutoSave = autoSave; }
    void SetDefaultText(const char* text) { fDefaultText = text; }
    
    // Debug: Print settings
    void PrintToStream() const {
        printf("AppSettings:\n");
        printf("  Window: %ldx%ld\n", fWindowWidth, fWindowHeight);
        printf("  Color: RGB(%d, %d, %d)\n", 
               fBackgroundColor.red, 
               fBackgroundColor.green, 
               fBackgroundColor.blue);
        printf("  Font Size: %ld\n", fFontSize);
        printf("  Auto-Save: %s\n", fAutoSave ? "Yes" : "No");
        printf("  Default Text: %s\n", fDefaultText.String());
    }

private:
    int32 fWindowWidth;
    int32 fWindowHeight;
    rgb_color fBackgroundColor;
    int32 fFontSize;
    bool fAutoSave;
    BString fDefaultText;
};

class SettingsView : public BView {
public:
    SettingsView(BRect frame)
        : BView(frame, "settings_view", B_FOLLOW_ALL, B_WILL_DRAW),
          fSettings(new AppSettings())
    {
        SetViewColor(245, 245, 245);
        
        BRect bounds = Bounds();
        float top = 10;
        float left = 10;
        float right = bounds.right - 10;
        
        // Title
        BStringView* title = new BStringView(
            BRect(left, top, right, top + 20),
            "title", "Settings Manager - BArchivable Demo",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        title->SetFont(be_bold_font);
        AddChild(title);
        top += 35;
        
        // Window width
        fWidthControl = new BTextControl(
            BRect(left, top, right, top + 25),
            "width", "Window Width:",
            "600",
            NULL,
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fWidthControl);
        top += 35;
        
        // Window height
        fHeightControl = new BTextControl(
            BRect(left, top, right, top + 25),
            "height", "Window Height:",
            "450",
            NULL,
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fHeightControl);
        top += 35;
        
        // Font size slider
        fFontSizeSlider = new BSlider(
            BRect(left, top, right, top + 50),
            "fontsize", "Font Size:",
            NULL,
            8, 24,
            B_BLOCK_THUMB,
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        fFontSizeSlider->SetValue(12);
        fFontSizeSlider->SetHashMarks(B_HASH_MARKS_BOTTOM);
        fFontSizeSlider->SetHashMarkCount(9);
        AddChild(fFontSizeSlider);
        top += 60;
        
        // Color control
        fColorControl = new BColorControl(
            BPoint(left, top),
            B_CELLS_32x8,
            4.0,
            "bgcolor",
            NULL);
        fColorControl->SetValue(make_color(245, 245, 245));
        AddChild(fColorControl);
        top += fColorControl->Bounds().Height() + 15;
        
        // Default text
        fTextControl = new BTextControl(
            BRect(left, top, right, top + 25),
            "text", "Default Text:",
            "Welcome to Settings Manager!",
            NULL,
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fTextControl);
        top += 35;
        
        // Auto-save checkbox
        fAutoSaveCheck = new BCheckBox(
            BRect(left, top, right, top + 25),
            "autosave", "Enable Auto-Save",
            NULL,
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        fAutoSaveCheck->SetValue(B_CONTROL_ON);
        AddChild(fAutoSaveCheck);
        top += 35;
        
        // Status text
        fStatusView = new BStringView(
            BRect(left, top, right, top + 20),
            "status", "Ready - Configure settings and save",
            B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
        AddChild(fStatusView);
        top += 30;
        
        // Buttons row 1
        fSaveButton = new BButton(
            BRect(left, top, left + 100, top + 30),
            "save", "Save",
            new BMessage(MSG_SAVE_SETTINGS),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fSaveButton);
        
        fLoadButton = new BButton(
            BRect(left + 110, top, left + 210, top + 30),
            "load", "Load",
            new BMessage(MSG_LOAD_SETTINGS),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fLoadButton);
        
        fApplyButton = new BButton(
            BRect(left + 220, top, left + 320, top + 30),
            "apply", "Apply",
            new BMessage(MSG_APPLY_SETTINGS),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fApplyButton);
        top += 40;
        
        // Buttons row 2
        fResetButton = new BButton(
            BRect(left, top, left + 100, top + 30),
            "reset", "Reset",
            new BMessage(MSG_RESET_SETTINGS),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fResetButton);
        
        fShowButton = new BButton(
            BRect(left + 110, top, left + 210, top + 30),
            "show", "Show Archive",
            new BMessage(MSG_SHOW_ARCHIVE),
            B_FOLLOW_LEFT | B_FOLLOW_TOP);
        AddChild(fShowButton);
    }
    
    ~SettingsView() {
        delete fSettings;
    }
    
    void AttachedToWindow() override {
        BView::AttachedToWindow();
        
        fSaveButton->SetTarget(this);
        fLoadButton->SetTarget(this);
        fResetButton->SetTarget(this);
        fApplyButton->SetTarget(this);
        fShowButton->SetTarget(this);
        
        // Try to load existing settings
        LoadSettings(false);
    }
    
    void MessageReceived(BMessage* message) override {
        switch (message->what) {
            case MSG_SAVE_SETTINGS:
                SaveSettings();
                break;
                
            case MSG_LOAD_SETTINGS:
                LoadSettings(true);
                break;
                
            case MSG_RESET_SETTINGS:
                ResetSettings();
                break;
                
            case MSG_APPLY_SETTINGS:
                ApplySettings();
                break;
                
            case MSG_SHOW_ARCHIVE:
                ShowArchive();
                break;
                
            default:
                BView::MessageReceived(message);
                break;
        }
    }
    
private:
    void UpdateUIFromSettings() {
        BString text;
        text << fSettings->WindowWidth();
        fWidthControl->SetText(text.String());
        
        text = "";
        text << fSettings->WindowHeight();
        fHeightControl->SetText(text.String());
        
        fFontSizeSlider->SetValue(fSettings->FontSize());
        fColorControl->SetValue(fSettings->BackgroundColor());
        fTextControl->SetText(fSettings->DefaultText());
        fAutoSaveCheck->SetValue(fSettings->AutoSave() ? B_CONTROL_ON : B_CONTROL_OFF);
    }
    
    void UpdateSettingsFromUI() {
        fSettings->SetWindowWidth(atoi(fWidthControl->Text()));
        fSettings->SetWindowHeight(atoi(fHeightControl->Text()));
        fSettings->SetFontSize(fFontSizeSlider->Value());
        fSettings->SetBackgroundColor(fColorControl->ValueAsColor());
        fSettings->SetDefaultText(fTextControl->Text());
        fSettings->SetAutoSave(fAutoSaveCheck->Value() == B_CONTROL_ON);
    }
    
    void SaveSettings() {
        // Update settings object from UI
        UpdateSettingsFromUI();
        
        // Create archive message
        BMessage archive;
        if (fSettings->Archive(&archive) != B_OK) {
            ShowError("Failed to archive settings!");
            return;
        }
        
        // Get settings file path
        BPath path;
        if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK) {
            ShowError("Failed to find settings directory!");
            return;
        }
        path.Append("SettingsManager_config");
        
        // Save to file
        BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
        if (file.InitCheck() != B_OK) {
            ShowError("Failed to create settings file!");
            return;
        }
        
        // Flatten the message to file
        if (archive.Flatten(&file) != B_OK) {
            ShowError("Failed to write settings!");
            return;
        }
        
        BString status;
        status << "Settings saved to: " << path.Path();
        fStatusView->SetText(status.String());
        
        // Debug output
        printf("\n=== Saved Settings ===\n");
        fSettings->PrintToStream();
        printf("Flattened size: %ld bytes\n", archive.FlattenedSize());
    }
    
    void LoadSettings(bool showMessage) {
        // Get settings file path
        BPath path;
        if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK) {
            if (showMessage)
                ShowError("Failed to find settings directory!");
            return;
        }
        path.Append("SettingsManager_config");
        
        // Open file
        BFile file(path.Path(), B_READ_ONLY);
        if (file.InitCheck() != B_OK) {
            if (showMessage) {
                fStatusView->SetText("No saved settings found - using defaults");
            }
            return;
        }
        
        // Unflatten message from file
        BMessage archive;
        if (archive.Unflatten(&file) != B_OK) {
            ShowError("Failed to read settings file!");
            return;
        }
        
        // Instantiate settings object from archive
        AppSettings* newSettings = (AppSettings*)AppSettings::Instantiate(&archive);
        if (!newSettings) {
            ShowError("Failed to unarchive settings!");
            return;
        }
        
        // Replace current settings
        delete fSettings;
        fSettings = newSettings;
        
        // Update UI
        UpdateUIFromSettings();
        
        BString status;
        status << "Settings loaded from: " << path.Path();
        fStatusView->SetText(status.String());
        
        // Debug output
        printf("\n=== Loaded Settings ===\n");
        fSettings->PrintToStream();
    }
    
    void ResetSettings() {
        delete fSettings;
        fSettings = new AppSettings();
        UpdateUIFromSettings();
        fStatusView->SetText("Settings reset to defaults");
    }
    
    void ApplySettings() {
        UpdateSettingsFromUI();
        
        // Apply to window
        BWindow* window = Window();
        if (window) {
            window->ResizeTo(fSettings->WindowWidth(), fSettings->WindowHeight());
            SetViewColor(fSettings->BackgroundColor());
            Invalidate();
        }
        
        fStatusView->SetText("Settings applied to window");
    }
    
    void ShowArchive() {
        UpdateSettingsFromUI();
        
        // Create archive
        BMessage archive;
        fSettings->Archive(&archive);
        
        // Build display string
        BString info;
        info << "Archived Message Contents:\n\n";
        
        char* name;
        type_code type;
        int32 count;
        
        for (int32 i = 0; archive.GetInfo(B_ANY_TYPE, i, &name, &type, &count) == B_OK; i++) {
            info << "Field: \"" << name << "\"\n";
            info << "  Type: 0x" << B_HOST_TO_BENDIAN_INT32(type) << "\n";
            
            // Show values
            if (type == B_INT32_TYPE) {
                int32 value;
                archive.FindInt32(name, &value);
                info << "  Value: " << value << "\n";
            } else if (type == B_BOOL_TYPE) {
                bool value;
                archive.FindBool(name, &value);
                info << "  Value: " << (value ? "true" : "false") << "\n";
            } else if (type == B_STRING_TYPE) {
                const char* value;
                archive.FindString(name, &value);
                info << "  Value: \"" << value << "\"\n";
            }
            info << "\n";
        }
        
        info << "Total flattened size: " << archive.FlattenedSize() << " bytes";
        
        BAlert* alert = new BAlert("Archive Contents",
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
    
    AppSettings* fSettings;
    
    BTextControl* fWidthControl;
    BTextControl* fHeightControl;
    BTextControl* fTextControl;
    BSlider* fFontSizeSlider;
    BColorControl* fColorControl;
    BCheckBox* fAutoSaveCheck;
    BStringView* fStatusView;
    
    BButton* fSaveButton;
    BButton* fLoadButton;
    BButton* fResetButton;
    BButton* fApplyButton;
    BButton* fShowButton;
};

class SettingsWindow : public BWindow {
public:
    SettingsWindow(BRect frame)
        : BWindow(frame, "Settings Manager",
                  B_TITLED_WINDOW,
                  B_QUIT_ON_WINDOW_CLOSE | B_AUTO_UPDATE_SIZE_LIMITS)
    {
        BRect viewRect = Bounds();
        SettingsView* view = new SettingsView(viewRect);
        AddChild(view);
    }
};

class SettingsApp : public BApplication {
public:
    SettingsApp()
        : BApplication("application/x-vnd.example-SettingsManager")
    {
        BRect rect(100, 100, 600, 550);
        SettingsWindow* window = new SettingsWindow(rect);
        window->Show();
    }
};

int main() {
    SettingsApp app;
    app.Run();
    return 0;
}
