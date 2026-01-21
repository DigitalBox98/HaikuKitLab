#include <Application.h>
#include <Locale.h>
#include <LocaleRoster.h>
#include <DateFormat.h>
#include <TimeFormat.h>
#include <NumberFormat.h>
#include <String.h>
#include <iostream>
#include <ctime>

class LocaleDemoApp : public BApplication {
public:
    LocaleDemoApp()
        : BApplication("application/x-vnd.kitlab-localeexample")
    {
        ShowLocaleInfo();
        PostMessage(B_QUIT_REQUESTED);
    }

private:
    void ShowLocaleInfo()
    {
        BLocale locale;

        // Get the current language
        BLanguage language;
        if (locale.GetLanguage(&language) == B_OK) {
            BString langName;
            language.GetName(langName);
            std::cout << "Language: " << langName.String() << std::endl;
        } else {
            std::cout << "Language: (unknown)" << std::endl;
        }

        // Get preferred languages via LocaleRoster
        BLocaleRoster* roster = BLocaleRoster::Default();
        if (roster != nullptr) {
            BMessage langs;
            if (roster->GetPreferredLanguages(&langs) == B_OK) {
                std::cout << "Preferred languages: ";
                const char* langCode = nullptr;
                bool first = true;
                int32 index = 0;
                while (langs.FindString("language", index, &langCode) == B_OK) {
                    if (!first)
                        std::cout << ", ";
                    std::cout << langCode;
                    first = false;
                    index++;
                }
                std::cout << std::endl;
            }
        }

        // Format current date/time using the current locale
        time_t now = time(nullptr);
        BDateFormat dateFormat;
        BTimeFormat timeFormat;

        BString dateString;
        BString timeString;
        dateFormat.Format(dateString, now, B_SHORT_DATE_FORMAT);
        timeFormat.Format(timeString, now, B_SHORT_TIME_FORMAT);

        std::cout << "\nLocalized date/time:" << std::endl;
        std::cout << "  Date: " << dateString.String() << std::endl;
        std::cout << "  Time: " << timeString.String() << std::endl;

        // Localized number formatting
        BNumberFormat numberFormat;
        double value = 12345.6789;
        BString numberString;
        numberFormat.Format(numberString, value);

        std::cout << "\nLocalized number:" << std::endl;
        std::cout << "  " << numberString.String() << std::endl;

        std::cout << "\n(Change your locale in Preferences → Locale and re-run to see differences.)" << std::endl;
    }
};

int main()
{
    LocaleDemoApp app;
    app.Run();
    return 0;
}
