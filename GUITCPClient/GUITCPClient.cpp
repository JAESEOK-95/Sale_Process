#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include "resource.h"
#include "NetWork.h"

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512

// 대화상자 프로시저
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

DWORD WINAPI RecvThread(LPVOID arg);

SOCKET sock; // 소켓
char buf[BUFSIZE+1]; // 스레드간 공용 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hOkButton, hJoin_Login, hDeposit, hJoin, hLogin, hBid; // 보내기 버튼
HWND hEdit1, hEdit2, hId, hPw, hNickname, hMoney; // 편집 컨트롤

char recv_buf[BUFSIZE];
char send_buf[BUFSIZE];

HINSTANCE g_Instance;
HANDLE hThread[2];
HWND hMainDg;
bool login = false;

enum  BID_STATE
{
	INIT_STATE,
	LOGIN_STATE,
	BID_SELECT,
	BID_PRICE


}Bid_State;

enum BUTTON_SELECT
{
	JOIN_BUTTON,
	LOGIN_BUTTON,
	LOGOUT_BUTTON,
	DEPOSIT_BUTTON,
	BID_BUTTON,
	OK_BUTTON,
	EXIT_BUTTON

}SelectButton;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
	// 이벤트 생성
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if(hReadEvent == NULL) return 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if(hWriteEvent == NULL) return 1;	

	g_Instance = hInstance;
	// 소켓 통신 스레드 생성
	hThread[0]=CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

	Bid_State = INIT_STATE;
	// 대화상자 생성
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, MainDlgProc);

	WaitForMultipleObjects(2, hThread, true, INFINITE);
	// 이벤트 제거
	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);
	
	closesocket(sock);
	// 윈속 종료
	WSACleanup();
	return 0;
}

// 대화상자 프로시저
BOOL CALLBACK MainDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg){
	case WM_INITDIALOG:
		hMainDg = hDlg;
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hJoin_Login = GetDlgItem(hDlg, ID_LOGIN);
		hDeposit = GetDlgItem(hDlg, ID_DEPOSIT);
		hBid= GetDlgItem(hDlg, ID_BID);
		hOkButton = GetDlgItem(hDlg, IDOK);
		SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
		
		EnableWindow(hDeposit, FALSE);
		EnableWindow(hBid, FALSE);
		EnableWindow(hOkButton, FALSE);

		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EnableWindow(hOkButton, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE+1);
			SelectButton = OK_BUTTON;
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			SetWindowText(hEdit1, TEXT(""));
			SetFocus(hEdit1);
			
			return TRUE;
		case ID_LOGIN:
			if (!login)
			{
				DialogBox(g_Instance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, LoginDlgProc);
			}
			else
			{
				login = false;
				SelectButton = LOGOUT_BUTTON;	
				SetEvent(hWriteEvent);
			}
			
			return TRUE;
		case ID_DEPOSIT:
			EnableWindow(hDeposit, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);
			GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
			SelectButton = DEPOSIT_BUTTON;
			SetWindowText(hEdit1, TEXT(""));
			SetFocus(hEdit1);
			SetEvent(hWriteEvent);
			return TRUE;
		case ID_BID:
			EnableWindow(hBid, FALSE);
			WaitForSingleObject(hReadEvent, INFINITE);		
			SelectButton = BID_BUTTON;
			SetEvent(hWriteEvent);
			return TRUE;
		case IDCANCEL:	
			SelectButton = EXIT_BUTTON;
			SetEvent(hWriteEvent);
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CALLBACK LoginDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		hId = GetDlgItem(hDlg, IDC_ID);
		hPw = GetDlgItem(hDlg, IDC_PW);
		hNickname = GetDlgItem(hDlg, IDC_NICKNAME);
		hJoin = GetDlgItem(hDlg, ID_JOIN);
		hLogin = GetDlgItem(hDlg, ID_LOGIN);
		hMoney= GetDlgItem(hDlg, IDC_MONEY);
		memset(&UserInfo, 0, sizeof(UserInfo));
		SendMessage(hId, EM_SETLIMITTEXT, BUFSIZE, 0);
		SendMessage(hPw, EM_SETLIMITTEXT, BUFSIZE, 0);
		SendMessage(hNickname, EM_SETLIMITTEXT, BUFSIZE, 0);
		SendMessage(hMoney, EM_SETLIMITTEXT, BUFSIZE, 0);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_JOIN:
			EnableWindow(hJoin, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_ID, UserInfo.id, BUFSIZE + 1);
			GetDlgItemText(hDlg, IDC_PW, UserInfo.pw, BUFSIZE + 1);
			GetDlgItemText(hDlg, IDC_NICKNAME, UserInfo.nickname, BUFSIZE + 1);
			GetDlgItemText(hDlg, IDC_MONEY,buf, BUFSIZE + 1);
			UserInfo.auction_money=atoi(buf);
			SelectButton = JOIN_BUTTON;
			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case ID_LOGIN:
			EnableWindow(hLogin, FALSE); // 보내기 버튼 비활성화
			WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
			GetDlgItemText(hDlg, IDC_ID, UserInfo.id, BUFSIZE + 1);
			GetDlgItemText(hDlg, IDC_PW, UserInfo.pw, BUFSIZE + 1);

			SelectButton = LOGIN_BUTTON;

			SetEvent(hWriteEvent); // 쓰기 완료 알리기
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		case IDCANCEL:			
			EndDialog(hDlg, IDCANCEL);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	char cbuf[BUFSIZE+256];
	vsprintf(cbuf, fmt, arg);

	int nLength = GetWindowTextLength(hEdit2);
	SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

// 소켓 함수 오류 출력 후 종료


// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if(retval == SOCKET_ERROR) err_quit("connect()");
	

	hThread[1]=CreateThread(NULL, 0, RecvThread, (LPVOID)sock, 0, NULL);

	bool end_flag = false;
	int size, code, price;
	// 서버와 데이터 통신
	while(1)
	{		
		WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

		switch (SelectButton)
		{
		case JOIN_BUTTON:
			if (strlen(UserInfo.id) == 0 || 
				strlen(UserInfo.pw) == 0 || 
				strlen(UserInfo.nickname) == 0||
				UserInfo.auction_money==0)
			{
				EnableWindow(hJoin, TRUE); // 보내기 버튼 활성화
				SetEvent(hReadEvent); // 읽기 완료 알리기
				continue;
			}
		
			PackPacket(send_buf, JOIN_INFO, UserInfo.id, UserInfo.pw, UserInfo.nickname,UserInfo.auction_money, size);

			retval = send(sock, send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
			SetWindowText(hId, TEXT(""));
			SetWindowText(hPw, TEXT(""));
			SetWindowText(hNickname, TEXT(""));
			EnableWindow(hJoin, TRUE);
			SetEvent(hReadEvent);
			break;
		case LOGIN_BUTTON:
			if (strlen(UserInfo.id) == 0 || strlen(UserInfo.pw) == 0)
			{
				EnableWindow(hLogin, TRUE); // 보내기 버튼 활성화
				SetEvent(hReadEvent); // 읽기 완료 알리기
				continue;
			}

			PackPacket(send_buf, LOGIN_INFO, UserInfo.id, UserInfo.pw, size);

			retval = send(sock, send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}
			SetWindowText(hId, TEXT(""));
			SetWindowText(hPw, TEXT(""));	
			EnableWindow(hLogin, TRUE);
			SetEvent(hReadEvent);

			break;
		case LOGOUT_BUTTON:
			PackPacket(send_buf, LOGOUT, size);

			retval = send(sock, send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send()");
				break;
			}

			EnableWindow(hDeposit, FALSE);
			EnableWindow(hBid, FALSE);
			EnableWindow(hOkButton, FALSE);
			EnableWindow(hLogin, TRUE);		
			SetEvent(hReadEvent);
			break;
		case BID_BUTTON:
			if (Bid_State != INIT_STATE)
			{
				PackPacket(send_buf, REQ_AUCTION_LIST_INFO, size);
				retval = send(sock, send_buf, size, 0);
				if (retval == SOCKET_ERROR)
				{
					err_quit("send()");
				}

				SetEvent(hReadEvent);
				break;
			}	
			SetEvent(hReadEvent);
			EnableWindow(hBid, TRUE);
			break;
		case DEPOSIT_BUTTON:
			price = atoi(buf);
			PackPacket(send_buf, DEPOSIT_INFO, price, size);
			retval = send(sock, send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_quit("send()");
			}
			EnableWindow(hDeposit, TRUE);
			SetEvent(hReadEvent);
			break;
		case OK_BUTTON:
			if (strlen(buf) == 0)
			{
				EnableWindow(hOkButton, TRUE); // 보내기 버튼 활성화
				SetEvent(hReadEvent); // 읽기 완료 알리기
				continue;
			}

			switch (Bid_State)
			{
			case BID_SELECT:
				EnableWindow(hDeposit, FALSE);
				code=atoi(buf);
				PackPacket(send_buf, AUCTION_SELECT_INFO, code, size);
				retval = send(sock, send_buf, size, 0);
				if (retval == SOCKET_ERROR)
				{
					err_quit("send()");
				}		
				break;
			case BID_PRICE:
				price = atoi(buf);
				PackPacket(send_buf, AUCTION_BID_PRICE, price, size);
				retval = send(sock, send_buf, size, 0);
				if (retval == SOCKET_ERROR)
				{
					err_quit("send()");
				}
				break;
			}

			EnableWindow(hOkButton, TRUE); // 보내기 버튼 활성화
			SetEvent(hReadEvent);

			break;
		case EXIT_BUTTON:

			PackPacket(send_buf, CONNECT_END, size);
			retval = send(sock, send_buf, size, 0);
			if (retval == SOCKET_ERROR)
			{
				err_quit("send()");
			}
			
			end_flag = true;
			break;
			
		}

		if (end_flag)
		{
			break;
		}

	}

	return 0;
}

DWORD WINAPI RecvThread(LPVOID _ptr)
{
	SOCKET sock = (SOCKET)_ptr;

	int count;
	int code;
	int price;
	char product_name[PRODUCT_NAMESIZE];
	char* ptr;
	bool endflag = false;

	while (1)
	{	

		if (!PacketRecv(sock, recv_buf))
		{
			break;
		}

		PROTOCOL protocol;

		GetProtocol(recv_buf, protocol);

		int result;
		char msg[BUFSIZE];

		switch (protocol)
		{
		case JOIN_RESULT:
		case LOGIN_RESULT:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, result, msg);
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);
			if (result == LOGIN_SUCCESS)
			{
				//EnableWindow(hJoin_Login, FALSE);
				EnableWindow(hDeposit, TRUE);
				EnableWindow(hBid, TRUE);
				EnableWindow(hOkButton, TRUE);
				SetWindowText(hJoin_Login, TEXT("로그아웃"));
				Bid_State = LOGIN_STATE;
				login = true;

			}
			break;
		case LOGOUT_RESULT:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);	
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);				
			SetWindowText(hJoin_Login, TEXT("회원가입/로그인"));
			Bid_State = INIT_STATE;
			break;
		case AUCTION_LIST_INFO:
				SetWindowText(hEdit2, TEXT(""));
				UnPackPacket(recv_buf, count);

				ptr = recv_buf + sizeof(PROTOCOL) + sizeof(count);

				for (int i = 0; i < count; i++)
				{
					memset(product_name, 0, sizeof(product_name));

					PartUnPackPacket(&ptr, code, product_name, price);
				DisplayText("코드:%d   제품이름:%s   시작가:%d\r\n", code, product_name, price);
			}

			Bid_State = BID_SELECT;

			break;	
		case USER_INFO:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf,UserInfo.auction_money, UserInfo.nickname);
			sprintf(msg, "닉네임:%s  보유금액:%d", UserInfo.nickname, UserInfo.auction_money);
			SetWindowText(hMainDg, msg);
			break;
		case AUCTION_MONEY_UPDATE_INFO:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, UserInfo.auction_money);
			sprintf(msg, "닉네임:%s  보유금액:%d", UserInfo.nickname, UserInfo.auction_money);
			SetWindowText(hMainDg, msg);
			break;
		case AUCTION_BID_START:
			Bid_State = BID_PRICE;
			EnableWindow(hOkButton, TRUE);
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);
			break;
		
		case AUCTION_BID_WAIT:			
			EnableWindow(hOkButton, FALSE);
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);
			break;
		case AUCTION_RESULT_INFO:
			Bid_State = LOGIN_STATE;
			EnableWindow(hBid, TRUE);
			EnableWindow(hDeposit, TRUE);
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);
			break;
		case AUCTION_SECOND_COUNT:
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);			
			DisplayText("%s\r\n", msg);
			break;
		case USER_EXIT_INFO:
		case ALL_AUCTION_COMPLETE:
		case AUCTION_BID_PRICE:	
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);
			break;
		case AUCTION_ERROR_INFO:
			EnableWindow(hBid, TRUE);
			memset(msg, 0, sizeof(msg));
			UnPackPacket(recv_buf, msg);
			SetWindowText(hEdit2, TEXT(""));
			DisplayText("%s\r\n", msg);
			break;

		case CONNECT_END:
			endflag = true;
			break;			
		}

		if (endflag)
		{
			break;
		}

	}

	return 0;
}
