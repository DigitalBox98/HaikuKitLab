#include <NodeInfo.h>
#include <Path.h>
#include <TranslatorAddOn.h>
#include <TranslationKit.h>
#include <TranslatorFormats.h>
#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Alert.h>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <syslog.h>
#include <cstring>

#define MARKDOWN_TYPE 'MDWN'

// Translator info
char translatorName[] = "MarkdownTranslator";
char translatorInfo[] = "Markdown -> Plain Text Translator";
int32 translatorVersion = 100;

// Input and output formats
translation_format inputFormats[] = {
    { MARKDOWN_TYPE, B_TRANSLATOR_TEXT, 0.5, 0.8, "text/markdown", "Markdown text" },
    { 0,0,0,0,"\0","\0" }
};

translation_format outputFormats[] = {
    { B_TRANSLATOR_TEXT, B_TRANSLATOR_TEXT, 0.5, 0.8, "text/plain", "Plain text" },
    { 0,0,0,0,"\0","\0" }
};

// ---------------------------
// Check file extension
// ---------------------------
bool hasMarkdownExtension(const char* filename) {
    if (!filename) return false;
    std::string name(filename);
    auto pos = name.rfind('.');
    if (pos == std::string::npos) return false;
    std::string ext = name.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "md" || ext == "markdown";
}

// ---------------------------
// Check MIME type
// ---------------------------
bool hasMarkdownMime(const char* filename) {
    if (!filename) return false;
    BPath path(filename);
    BNode node(path.Path());
    if (node.InitCheck() != B_OK) return false;

    BNodeInfo ninfo(&node);
    char type[B_MIME_TYPE_LENGTH];
    if (ninfo.GetType(type) != B_OK) return false;

    return strcmp(type, "text/markdown") == 0 || strcmp(type, "text/x-markdown") == 0;
}

// ---------------------------
// Identify function
// ---------------------------
status_t Identify(BPositionIO*, const translation_format*, BMessage* ioExtension,
                  translator_info* outInfo, uint32) 
{
    syslog(LOG_INFO, "MarkdownTranslator Identify called");

    const char* filename = nullptr;
    if (ioExtension)
        ioExtension->FindString("name", &filename);

    if (!filename || (!hasMarkdownExtension(filename) && !hasMarkdownMime(filename))) {
        return B_NO_TRANSLATOR; 
    }

    // Fill output info
    outInfo->group = B_TRANSLATOR_TEXT;
    outInfo->type = B_TRANSLATOR_TEXT;
    outInfo->quality = 0.9;
    outInfo->capability = 1.0;

    std::strncpy(outInfo->name, translatorName, sizeof(outInfo->name)-1);
    outInfo->name[sizeof(outInfo->name)-1] = '\0';

    std::strncpy(outInfo->MIME, "text/markdown", sizeof(outInfo->MIME)-1);
    outInfo->MIME[sizeof(outInfo->MIME)-1] = '\0';

    return B_OK;
}

// ---------------------------
// Translate Markdown -> Plain Text
// ---------------------------
status_t Translate(BPositionIO* inSource, const translator_info*, BMessage*, uint32 outType, BPositionIO* outDestination) {
    if (!outType) outType = B_TRANSLATOR_TEXT;

    inSource->Seek(0, SEEK_SET);
    std::string line;
    char c;

    while (inSource->Read(&c, 1) == 1) {
        if (c == '\n') {
            // Remove Markdown headers
            if (line.rfind("## ", 0) == 0) line = line.substr(3);
            else if (line.rfind("# ", 0) == 0) line = line.substr(2);

            // Remove bold and italic marks
            size_t pos;
            while ((pos = line.find("**")) != std::string::npos) line.erase(pos, 2);
            while ((pos = line.find("*")) != std::string::npos) line.erase(pos, 1);

            line += "\n";
            outDestination->Write(line.c_str(), line.size());
            line.clear();
        } else {
            line += c;
        }
    }

    if (!line.empty()) {
        outDestination->Write(line.c_str(), line.size());
    }

    return B_OK;
}

// ---------------------------
// Minimal config window
// ---------------------------
class MDWindow : public BWindow {
public:
    MDWindow(BRect frame) 
        : BWindow(frame, "Markdown Translator", B_TITLED_WINDOW, B_NOT_RESIZABLE|B_NOT_ZOOMABLE) {}
    ~MDWindow() { be_app->PostMessage(B_QUIT_REQUESTED); }
};

class MDView : public BView {
public:
    MDView(BRect frame) 
        : BView(frame, "MDView", B_FOLLOW_ALL, B_WILL_DRAW) 
    { SetViewColor(220,220,220); }

    void Draw(BRect) override {
        std::ostringstream text;
        double versionFloat = translatorVersion / 100.0;
        text << "Markdown -> Plain Text " << std::fixed << std::setprecision(2) << versionFloat;
        DrawString(text.str().c_str(), BPoint(5,20));
    }
};

// ---------------------------
// Create config view
// ---------------------------
status_t MakeConfig(BMessage*, BView** outView, BRect* outExtent) {
    MDView* v = new MDView(BRect(0,0,300,50));
    *outView = v;
    *outExtent = v->Bounds();
    return B_OK;
}

// ---------------------------
// Main application
// ---------------------------
int main() {
    BApplication app("application/x-vnd.md-translator");

    BView* v = nullptr;
    BRect r;
    if (MakeConfig(nullptr, &v, &r) != B_OK) {
        BAlert* err = new BAlert("Error", "Cannot create configuration view!", "OK");
        err->Go();
        return 1;
    }

    MDWindow* w = new MDWindow(r);
    v->ResizeTo(r.Width(), r.Height());
    w->AddChild(v);
    w->MoveTo(BPoint(100,100));
    w->Show();

    app.Run();
    return 0;
}
