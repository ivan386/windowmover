#define _WIN32_WINNT 0x0500
#include <stdio.h>
#include <windows.h>

HWND console_window;

HWND active_window;
RECT active_window_rect;

HWND desktop_window;
RECT desktop_window_rect;
int desktop_window_width;
int desktop_window_height;


void enum_windows(WNDENUMPROC enum_function)
{
	EnumWindows(enum_function, 0);
}

void move_window(HWND window, int window_x, int window_y, int window_width, int window_height)
{
	MoveWindow(window, window_x, window_y, window_width, window_height, 1);
}

void get_window_rect(HWND window, LPRECT window_rect)
{
	GetWindowRect(window, window_rect);
}

void get_cursor_pos(LPPOINT cursor_point)
{
	GetCursorPos(cursor_point);
}

void set_cursor_pos(int x, int y)
{
	SetCursorPos(x, y);
}
void sleep(DWORD dwMsecs)
{
	Sleep(dwMsecs);
}



CALLBACK move_by_cursor(HWND window, LPARAM lParam)
{
	
	
	if (window == desktop_window) return 1;
	if (IsWindowVisible(window) == 0) return 1;
	if (IsIconic(window) != 0) return 1;
	if (IsZoomed(window) != 0) return 1;


	POINT cursor_pos;
	
	get_cursor_pos(&cursor_pos);

	if 	( (cursor_pos.x == 0) 
		|| (cursor_pos.x == desktop_window_rect.right - 1) 
		|| (cursor_pos.y == 0 )
		|| (cursor_pos.y == desktop_window_rect.bottom - 1)
		)
	{
	
		RECT window_rect;
		get_window_rect(window, &window_rect);
		
		int window_x = window_rect.left;
		int window_y = window_rect.top;
		int window_width = window_rect.right - window_rect.left;
		int window_height = window_rect.bottom - window_rect.top;
		
		if ( (window_width > 0) && (window_height > 0) ){
			int do_move_window = 0;
			if ( cursor_pos.x == desktop_window_rect.left ){
				window_x += 20;
				do_move_window = 1;
			}else if ( cursor_pos.x == desktop_window_rect.right - 1 ){
				window_x -= 20;
				do_move_window = 1;
			}
			
			if ( cursor_pos.y == desktop_window_rect.top ){
				window_y += 20;
				do_move_window = 1;
			}else if ( cursor_pos.y == desktop_window_rect.bottom - 1 ){
				window_y -= 20;
				do_move_window = 1;
			}
			if (do_move_window = 1) move_window(window, window_x, window_y, window_width, window_height);
		}
	}

    return 1;
}

CALLBACK move_by_active_window(HWND window, LPARAM lParam)
{
	if (window == desktop_window) return 1;
	if (IsWindowVisible(window) == 0) return 1;
	if (IsIconic(window) != 0) return 1;
	if (IsZoomed(window) != 0) return 1;

	
	
	RECT window_rect;
	get_window_rect(window, &window_rect);
	
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;
	
	if ( (window_width > 0) && (window_height > 0) ){
		
		int active_window_width = active_window_rect.right - active_window_rect.left;
		int active_window_height = active_window_rect.bottom - active_window_rect.top;
		
		int active_window_x = desktop_window_rect.left 
				+ (desktop_window_width) / 2
				- (active_window_width) / 2;
		int active_window_y = desktop_window_rect.top
				+ (desktop_window_height) / 2
				- (active_window_height) / 2;
		
		int window_x = active_window_x;
		int window_y = active_window_y;
		
		if (window != active_window) {
			window_x = window_rect.left + (active_window_x - active_window_rect.left);
			window_y = window_rect.top + (active_window_y - active_window_rect.top);
		}
		
		move_window(window, window_x, window_y, window_width, window_height);
	}

	return 1;
}


int main(int argc, char *argv[]){
	console_window = GetConsoleWindow();
	ShowWindow(console_window, SW_MINIMIZE);
	active_window =  GetForegroundWindow();
	desktop_window = GetDesktopWindow();
	get_window_rect(desktop_window, &desktop_window_rect);
	desktop_window_width = desktop_window_rect.right - desktop_window_rect.left;
	desktop_window_height = desktop_window_rect.bottom - desktop_window_rect.top;
	printf("Windows Mover By Ivan386");
	for(;;){
		
		POINT cursor_pos;
		get_cursor_pos(&cursor_pos);
		
		sleep(200); //Задержка перед началом движения
		
		HWND new_active_window =  GetForegroundWindow();
		
		
		while(     (cursor_pos.x == 0) 
				|| (cursor_pos.x == desktop_window_rect.right - 1) 
				|| (cursor_pos.y == 0 )
				|| (cursor_pos.y == desktop_window_rect.bottom - 1)
			)
		{
			//Двигаем окна
			enum_windows(move_by_cursor);
			get_cursor_pos(&cursor_pos);
			int move_cursor;
			
			//Отодвигаем курсор от края если надо
			if (cursor_pos.x == 0 ){
				cursor_pos.x += 1;
				move_cursor = 1;
			}else if (cursor_pos.x == desktop_window_rect.right - 1){
				cursor_pos.x -= 1;
				move_cursor = 1;
			}
			
			if (cursor_pos.y == 0 ){
				cursor_pos.y += 1;
				move_cursor = 1;
			}else if (cursor_pos.y == desktop_window_rect.bottom - 1){
				cursor_pos.y -= 1;
				move_cursor = 1;
			}
			
			//if (move_cursor) set_cursor_pos(cursor_pos.x, cursor_pos.y);
			sleep(30);
			get_cursor_pos(&cursor_pos);
		}
		
		

		// Перемещение к новому активному окну если оно вне экрана.
		if ( new_active_window != active_window )
		{

			active_window = new_active_window;
			
			get_window_rect(active_window, &active_window_rect);
			
			if	( 
				active_window != desktop_window
				&& (IsWindowVisible(active_window) != 0)
				&& (IsIconic(active_window) == 0)
				&& (
					desktop_window_rect.left > active_window_rect.right
					|| desktop_window_rect.right < active_window_rect.left
					|| desktop_window_rect.top > active_window_rect.bottom
					|| desktop_window_rect.bottom < active_window_rect.top
					)
			)
			{
				enum_windows(move_by_active_window);
			}
		}
	}
}

