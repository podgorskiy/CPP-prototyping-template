#include <string>
#include <stdarg.h>
#include "assertion.h"


#ifdef _WIN32
#include <windows.h>
__declspec(thread) HHOOK DebugMessageBoxHook;

enum {
	Debug = IDYES,
	Ignore = IDNO,
	Continue = IDCANCEL
};

static LRESULT CALLBACK CustomMessageBoxProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_ACTIVATE) {
		HWND hChildWnd = (HWND)wParam;
		UINT result;
		if (GetDlgItem(hChildWnd, IDYES))
		{
			result = SetDlgItemTextA(hChildWnd, IDYES, "Debug");
		}
		if (GetDlgItem(hChildWnd, IDNO))
		{
			result = SetDlgItemTextA(hChildWnd, IDNO, "Ignore");
		}
		if (GetDlgItem(hChildWnd, IDCANCEL))
		{
			result = SetDlgItemTextA(hChildWnd, IDCANCEL, "Continue");
		}
		UnhookWindowsHookEx(DebugMessageBoxHook);
	}
	else
	{
		CallNextHookEx(DebugMessageBoxHook, nCode, wParam, lParam);
	}
	return 0;
}
int DebugMessageBox(HWND hwnd, const char* lpText, const char* lpCaption, UINT uType)
{
	DebugMessageBoxHook = SetWindowsHookEx(WH_CBT, &CustomMessageBoxProc, 0, GetCurrentThreadId());
	return MessageBoxA(hwnd, lpText, lpCaption, uType);
}
#elif __linux__
enum {
	Debug = 1,
	Ignore = 2,
	Continue = 3
};


#ifdef GTK_ASSERT
#include <gtk/gtk.h>

int MessageBox(const char* text, const char* caption)
{
    GtkWidget *dialog;

    gtk_init(nullptr, nullptr);

    dialog = gtk_message_dialog_new(nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_NONE, "%s", text);

    gtk_dialog_add_button(GTK_DIALOG(dialog), "Debug", Debug);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Ignore", Ignore);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Continue", Continue);

    gtk_window_set_title(GTK_WINDOW(dialog), caption);
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy( GTK_WIDGET(dialog) );

    while(gtk_events_pending())
    {
    	gtk_main_iteration();
    }

    if (result != Debug && result != Ignore && result != Continue)
    {
	    return Debug;
    }
    return result;
}
#else
#include <stdlib.h>

int MessageBox(const char* text, const char* caption)
{
	std::string command = utils::format("zenity --question --title \"%s\" --text \"%s\" --no-wrap --ok-label \"Debug\" --cancel-label \"Ignore\"", caption, text);
	int result = system(command.c_str());

    if (result == 0)
    {
	    return Debug;
    }
    return Ignore;
}
#endif

#endif


static Assert::result ShowMessageBox(const char *file, int line, const char *condition, const char *fmt, va_list ap)
{
	char buf[2048];
	va_list apCopy;
	va_copy(apCopy, ap);
	vsnprintf(&buf[0], 2048, fmt, apCopy);
	char caption[256];
	sprintf(caption, "Assert failed at: %s(%d)", file, line);
	std::string message = caption;
	message += "\n\n";
	message += buf;
	message += "\n";
	if (condition != nullptr)
	{
		message += condition;
	}
#ifdef _WIN32
	int value = DebugMessageBox(0, message.c_str(), caption, MB_ICONWARNING | MB_YESNOCANCEL | MB_SYSTEMMODAL);
#elif __linux__
	int value = MessageBox(message.c_str(), caption);
	if (value == Debug)
	{
		return Assert::result_break;
	}
	else if (value == Ignore)
	{
		return Assert::result_ignore_always;
	}
	else if (value == Continue)
	{
		return Assert::result_ignore_once;
	}
	return Assert::result_break;
#endif
}

Assert::result Assert::message(const char *file, int line, const char *condition, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	result ab = ShowMessageBox(file, line, condition, fmt, ap);
	va_end(ap);
	return ab;
}
