#include "MainWindow.h"
#include "sxEditor.h"
#include "MainEditor.h"
#include "EditorSettings.h"

str256				MainWin::s_Title(L"SeganX Editor");
sx::sys::Window		MainWin::s_Win(L"SeganX Editor");

void MainWin::InitWindow( void )
{	
	int dw = sx::sys::GetDesktopWidth();
	int dh = sx::sys::GetDesktopHeight();
	int ww = EditorSettings::GetInitWindowSize().x;
	int wh = EditorSettings::GetInitWindowSize().y;
	int lf = GetSystemMetrics(SM_CXSIZEFRAME);
	s_Win.SetBorder(WBT_ORDINARY_RESIZABLE);
	s_Win.SetRect(dw/2 - ww/2 - lf, dh/2 - wh/2 - 50, ww, wh);
	s_Win.SetIcon(LoadIcon(sx::sys::Application::Get_Instance(), MAKEINTRESOURCE(IDI_SXEDITOR)));
	s_Win.SetBackground(NULL);
	s_Win.SetOnRect(OnRect);
	s_Win.SetOnClose(OnClose);
	s_Win.SetVisible(true);

	sx::sys::Application::Create_Window(&MainWin::s_Win);
	sxLog::SetWindow( MainWin::s_Win.GetHandle() );
}

void MainWin::OnRect( sx::sys::Window* Sender, WindowRect& newRect )
{
	if(newRect.Width>0 && newRect.Height>0)
	{
		SEGAN_CLAMP(newRect.Width,	640, 0xFFFF);
		SEGAN_CLAMP(newRect.Height, 480, 0xFFFF);
	}

	WindowRect curRect;
	s_Win.GetRect(curRect);
	if (newRect.Width != curRect.Width || newRect.Height != curRect.Height)
		Editor::Resize(newRect.Width, newRect.Height);
}

bool MainWin::OnClose( sx::sys::Window* Sender )
{
	return true;
}