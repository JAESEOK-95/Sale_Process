#pragma once
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "LinkedList.h"

#define BUFSIZE 4096
#define IDSIZE 255
#define PWSIZE 255
#define NICKNAMESIZE 255
#define PRODUCT_NAMESIZE 255
#define MAX_AUCTION_COUNT 5
#define MAX_BID_COUNT 100

#define ID_ERROR_MSG "없는 아이디입니다\n"
#define PW_ERROR_MSG "패스워드가 틀렸습니다.\n"
#define LOGIN_SUCCESS_MSG "로그인에 성공했습니다.\n"
#define LOGOUT_MSG "로그아웃되었습니다.\n"
#define ID_EXIST_MSG "이미 있는 아이디 입니다.\n"
#define JOIN_SUCCESS_MSG "가입에 성공했습니다.\n"
#define AUCTION_BID_WAIT_MSG "경매에 인원이 차기를 기다립니다. ..\n"
#define AUCTION_ALL_AUCTION_COMPLETE_MSG "모든 경매가 종료되었습니다...\n"
#define AUCTION_BID_START_MSG "경매에 인원이 찼습니다.. 입찰해 주세요.\n"
#define AUCTION_FULL_ERROR_MSG "경매 인원수가 가득 찼습니다. 다른 경매를 선택해 주세요\n"
#define AUCTION_CODE_ERROR_MSG "경매 코드 에러입니다.\n"
#define AUCTION_MONEY_ERROR_MSG "보유 금액이 부족합니다.\n"
#define AUCTION_BID_MONEY_ERROR_MSG "최근 베팅 가격보다 낮은 금액을 베팅하셨습니다.\n"
#define USER_EXIT_INFO_MSG "최고가격을 배팅했던 유저가 포기했습니다. 이전가격부터 시작합니다.\n"
#define SEND 's'
#define RECV 'r'
#define PRE 'p'
#define NOMAL 'n'
static void err_quit(char* msg);

static void err_display(char* msg);
static CRITICAL_SECTION cs;
enum STATE
{
	INIT_STATE = -1,
	LOGIN_MENU_SELECT_STATE = 1,
	JOIN_RESULT_SEND_STATE,
	LOGIN_RESULT_SEND_STATE,
	USER_INFO_SEND_STATE,
	LOGOUT_RESULT_SEND_STATE,

	AUCION_MONEY_UPDATE_SEND_STATE,

	AUCTION_MENU_SELECT_STATE,
	AUCTION_LIST_SEND_STATE,
	AUCTION_BID_WAIT_SEND_STATE,
	AUCTION_BID_WAIT_STATE,
	AUCTION_BID_START_SEND_STATE,
	AUCTION_BID_STATE,
	AUCTION_BID_SEND_STATE,
	AUCTION_COUNT_SEND_STATE,
	AUCTION_COMPLETE_STATE,
	AUCTION_ERROR_SEND_STATE,
	CONNECT_END_SEND_STATE,
	USER_EXIT_INFO_SEND_STAE,
	DISCONNECTED_STATE
};

enum
{
	ERROR_DISCONNECTED = -2,
	DISCONNECTED = -1,
	SOC_FALSE,
	SOC_TRUE
};


enum PROTOCOL
{
	JOIN_INFO,
	LOGIN_INFO,
	JOIN_RESULT,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	CONNECT_END,
	USER_EXIT_INFO,
	DEPOSIT_INFO,

	AUCTION_MONEY_UPDATE_INFO,
	USER_INFO,
	REQ_AUCTION_LIST_INFO,
	ALL_AUCTION_COMPLETE,
	AUCTION_LIST_INFO,
	AUCTION_BID_WAIT,
	AUCTION_BID_START,
	AUCTION_BID_PRICE,
	AUCTION_SECOND_COUNT,
	AUCTION_SELECT_INFO,
	AUCTION_RESULT_INFO,
	AUCTION_ERROR_INFO
};

enum
{
	NODATA = -1,
	ID_EXIST = 1,
	ID_ERROR,
	PW_ERROR,
	JOIN_SUCCESS,
	LOGIN_SUCCESS,

	AUCTION_SUCCESS,
	AUCTION_FAIL,
	AUCTION_ONGOING,
	AUCTION_COMPLETE,

	AUCTION_GROUP_FULL_ERROR,
	AUCTION_CODE_ERROR,
	AUCTION_BID_MONEY_ERROR,
	AUCTION_SHORT_OF_MONEY_ERROR
};
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

struct _User_Info
{
	char	id[IDSIZE];
	char	pw[PWSIZE];
	char	nickname[NICKNAMESIZE];
	int		auction_money;
};

