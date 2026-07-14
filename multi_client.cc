/*
[구현한 기능들]
1. 서버와 연결
    실행: ./multi_client <서버IP> <포트> <닉네임> (예시: ./multi_client 127.0.0.1 9190 닉네임)
    먼저 socket()으로 TCP 소켓을 만들고, 접속할 서버의 주소를 구조체에 채운 뒤 connect()로 연결을 요청한다.
    이때 실제로는 3-way handshake가 일어난다.
    서버와 달리 클라이언트는 bind()나 listen()이 필요 없다. 클라이언트 쪽 포트 번호는 운영체제가 알아서 비어 있는 것을 하나 골라 붙여준다.
    연결이 되면 곧바로 'JOIN|닉네임' 형태의 등록 메시지를 보내준다.
    서버는 이걸 받고서야 이 소켓의 주인이 누구인지 알게 되고, 닉네임이 이미 사용 중이면 서버가 안내 메시지를 보내고 연결을 끊는다.
2. 메시지 송수신
    송신: 키보드로 입력한 내용을 write()로 서버에 보낸다.
    수신: 서버가 보내는 다른 사람의 채팅, 시스템 알림, 명령어 처리 결과 등을 read()로 받아 화면에 출력한다.

    이 둘은 각각 다른 스레드가 맡는다.
    하나의 흐름으로 처리해버리면 키보드 입력을 기다리느라 멈춰 있는 동안 다른 사람들의 채팅 메시지를 받을 수 없고,
    반대로 채팅 메시지를 기다리고 있으면 키보드 입력을 받을 수 없게 되기 때문이다.
    내가 채팅을 입력하는 동안에도 다른 사람들의 채팅이 계속 도착해서 보여야 하므로 두 가지가 동시에 되어야 한다.
    TCP는 전이중 통신을 지원하므로, 하나의 스레드가 같은 소켓에서 read()를 수행하는 동안 다른 스레드가 write()를 수행할 수 있다.
3. 종료
    입력줄에 q를 입력하면 채팅을 끝낸다.
    이때 소켓을 close()로 그냥 닫아버리지 않고, shutdown(sock, SHUT_WR)으로 보내는 방향만 먼저 닫는다.(half-close 방식)
    이렇게 하면 서버 쪽 read()가 0을 반환해서 서버가 이 사람이 나갔음을 알아채고 퇴장 알림을 보내고 정리 작업을 할 수 있다.
    그 다음 서버가 소켓을 닫으면 이번엔 클라이언트 수신 스레드의 read()가 0을 반환하면서 자연스럽게 끝이 난다.
    만약 close()로 소켓을 통째로 닫아버리게 되면, 아직 read() 중이던 수신 스레드가 닫힌 소켓을 읽게 되어 위험할 수 있다.
4. 화면 분리 UI
    내가 한창 타이핑하는 중에 다른 사람의 메시지가 도착하면 입력 중이던 글자 사이로 끼어들어 화면이 엉망이 되는 문제가 발생했다.
    그래서 화면을 위아래로 나눠서, 위쪽에는 주고받는 채팅 메시지만 쌓이고, 맨 아랫줄은 내 입력만을 위한 공간으로 구성했다.
    채팅 메시지가 몇 개가 오더라도 내가 치고 있던 채팅 메시지는 그대로 보여진다.
5. 메시지 종류별 색상 구분
    입퇴장과 같은 시스템 알림 → 청록색
    귓속말 → 자홍색
    멘션 → 노란색

    으로 구현했다.
6. 한글 입력 지원
    한글은 영어와 달리 글자 하나가 3바이트로 저장된다.
    그래서 처음에 백스페이스를 눌렀을 때 1바이트만 지우게 했더니 글자가 깨져버려 이상한 기호로 보였다.
    UTF-8은 어떤 바이트가 글자의 첫 바이트이고 어떤 바이트가 그 글자에 딸린 나머지 바이트인지 구분할 수 있게 되어 있어,
    백스페이스를 누르면 뒤에서부터 거슬러 올라가면서 글자의 첫 바이트를 만날 때까지 지우게 만들었다.
7. 터미널 창 크기 변경 대응
    채팅 도중 터미널 창을 줄였다 늘리면 메시지가 잘리고 구분선이 사라지는 문제를 겪었다.
    그래서 창 크기가 바뀌면 화면을 새 크기로 다시 짓고, 따로 보관해둔 최근 메시지들을 다시 그려주도록 했다.
8. 입력줄 가로 스크롤
    긴 채팅을 입력하면 화면 폭을 넘어간 뒤부터 내가 입력하는 채팅 내용이 안 보이는 문제가 있었다.
    그래서 한도를 넘어가면 앞쪽 글자들이 화면에서만 왼쪽으로 밀려나고, 항상 최신 입력이 보이도록 했다.
    한글은 화면에서 두 칸을 차지하므로 바이트 수가 아니라 화면 폭 기준으로 계산한다.

[설계]
- ncurses 사용
    터미널은 기본적으로 글자를 아래로 계속 이어 붙이기만 할 뿐, 화면의 이 위치에 무엇을 출력하라는 개념이 없다.
    그래서 입력줄에 채팅을 치고 있는 동안 다른 사람들의 채팅 메시지가 도착하면, 그 채팅이 내가 치던 줄 사이에 그대로 끼어들게 되는 문제가 있다.
    실제 채팅 프로그램처럼 쓰려면 채팅 메시지가 쌓이는 영역과 내가 입력하는 줄이 침범하지 않고 따로 있어야 한다.
    그러기 위해서는 화면을 영역별로 나누고 각 영역을 따로따로 다시 구성할 수 있어야 한다.
    이것을 ncurses가 제공하는데, ncurses는 터미널 화면의 원하는 위치를 직접 지정해서 그릴 수 있게 해준다.
    화면을 위 채팅 메시지 영역, 가운데 구분선, 아래 입력줄로 나누고, 메시지가 도착하면 위 채팅 메시지 영역만 갱신하고 입력줄은 그대로 둘 수 있다.
    채팅 메시지가 쌓이면 자동 스크롤, 백스페이스 같은 특수 키, 창 크기 변경과 같은 기능들도 ncurses 덕분에 직접 처리할 필요가 없다.
- 화면 mutex
    수신 스레드는 위쪽 영역에 채팅 메시지를 쓰고, 송신 스레드는 아래쪽 입력줄에 채팅 메시지를 쓴다.
    그런데 둘이 동시에 화면을 건드리면 출력이 뒤엉킬 수 있다.
    서버에서는 여러 스레드가 접속자 배열을 동시에 만지지 못하게 mutex로 막았는데, 여기서는 화면이 공유 자원이다.
    그래서 화면을 고치는 모든 순간을 mutex로 보호했다.
    또, 잠금을 걸어둔 채로 키 입력을 기다리면 안 된다.
    처음에 그렇게 만들었다가 내가 입력하는 채팅 메시지는 잘 보이는데 다른 사람이 보낸 채팅 메시지는 한참 뒤에 뜨는 문제가 있었다.
    화면을 실제로 고치는 아주 짧은 순간에만 잠금을 걸어야 한다.
- 서버가 보낸 색상 코드를 화면에 맞게 번역하기
    서버는 멘션 메시지에 특수 문자(ANSI 코드)를 함께 보낸다.
    이 특수 문자가 그대로 출력되면 이상한 기호로 깨져 보인다.
    그래서 클라이언트가 받은 줄에서 이 특수 문자들을 찾아내 걷어내고, 대신 ncurses 방식의 색상으로 바꿔서 출력한다.
- 내가 보낸 채팅을 내 화면에도 띄우기
    서버는 채팅을 보낸 사람에게는 그걸 다시 보내주지 않는다.
    채팅 메시지를 적고 엔터를 누르면 정작 내가 방금 무슨 말을 했는지 화면에 남지 않는 문제가 발생하게 된다.
    그래서 채팅 메시지를 보낸 직후 '나: 채팅 내용' 형태로 내 화면에 직접 찍어준다.
    단, /w 나 /list 같은 명령어는 서버가 답을 보내주므로 따로 찍지 않는다.
- 받을 때도 개행 단위로 잘라서 처리
    서버와 똑같이, 받은 데이터를 버퍼에 쌓아두고 개행이 나올 때마다 잘라서 한 줄씩 출력한다.

[테스트 환경]
    본인은 Windows 11 + WSL2(Ubuntu 24.04 LTS) 에서 테스트를 진행하였다.
    VS Code 터미널에서 창을 여러 개로 분할하여 서버 1개를 구동시키고
    클라이언트 여러 개를 동시에 띄워놓고 대화하며 구현한 기능들의 작동을 확인했다.
*/

#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <ncursesw/ncurses.h>

#define BUFSIZE  1024
#define NICKSIZE 32

int sock; // 서버 연결 소켓 (송신/수신 스레드 공유)
volatile int running = 1;

WINDOW *msgwin, *sepwin, *inpwin; // 채팅 메시지 영역, 구분선, 입력줄
pthread_mutex_t scr_mutx = PTHREAD_MUTEX_INITIALIZER; // 화면 보호용 mutex

/*
화면을 다시 그리기 위한 최근 메시지 보관
ncurses의 창은 자기 크기만큼만 내용을 기억하기 때문에, 터미널 창을 줄이는 순간 잘려나간 메시지는 창 안에서 되살릴 방법이 없다.
그래서 화면에 출력한 메시지를 색 정보와 함께 여기에 따로 보관해뒀다가,
창 크기가 바뀌면 새 크기의 화면에 처음부터 다시 그려준다.
*/
#define SCROLLBACK 200 // 보관하는 최근 메시지 수
char sb_text[SCROLLBACK][BUFSIZE + 64];
int  sb_pair[SCROLLBACK];
int  sb_start = 0;
int  sb_count = 0;

/*
이모티콘 목록 (내 화면에 보여줄 때만 사용)
    원래 이모티콘 변환은 서버가 한다.
    그런데 내가 보낸 메시지만큼은 서버를 거치지 않고 내 화면에 바로 찍힌다.
    그래서 서버의 변환을 못 거쳐서 나 혼자만 ':smile:' 글자 그대로를 보게 된다.
    남들은 다 이모티콘으로 보는데 정작 보낸 나만 못 보는 셈이라, 내 화면에 찍을 때 쓸 목록을 똑같이 하나 뒀다.
*/
typedef struct { const char *code; const char *emoji; } emoji_t;
static const emoji_t EMOJI_MAP[] = {
    {":smile:", "\U0001F60A"}, {":laugh:", "\U0001F602"},
    {":cry:",   "\U0001F622"}, {":heart:", "\u2665"},
    {":star:",  "\u2605"},     {":thumbsup:", "\U0001F44D"},
    {":ok:",    "\U0001F44C"}, {":fire:",  "\U0001F525"},
    {":check:", "\u2714"},     {":angry:", "\U0001F620"},
};
static const int EMOJI_COUNT = sizeof(EMOJI_MAP) / sizeof(EMOJI_MAP[0]);

void convert_emoji(const char *src, char *dst, int dstsize) {
    int di = 0;
    for (int i = 0; src[i] != '\0';) {
        int matched = 0;
        if (src[i] == ':') {
            for (int e = 0; e < EMOJI_COUNT; e++) {
                int clen = strlen(EMOJI_MAP[e].code);
                if (strncmp(&src[i], EMOJI_MAP[e].code, clen) == 0) {
                    int elen = strlen(EMOJI_MAP[e].emoji);
                    if (di + elen >= dstsize - 1) break;
                    memcpy(&dst[di], EMOJI_MAP[e].emoji, elen);
                    di += elen; i += clen; matched = 1;
                    break;
                }
            }
        }
        if (!matched) {
            if (di >= dstsize - 1) break;
            dst[di++] = src[i++];
        }
    }
    dst[di] = '\0';
}

void *send_msg(void *arg);
void *recv_msg(void *arg);

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(-1);
}

/*
서버가 보낸 색상 특수문자를 걷어내고, 멘션 메시지였는지 알려주는 함수
    서버는 나를 부른 메시지에 "여기부터 노란색"이라는 특수 문자를 함께 붙여서 보낸다.
    화면을 직접 그리는 방식인 ncurses에서는 이상한 기호로 깨져 보인다.
    그래서 이 함수가 받은 줄에서 그 특수 문자들을 전부 떼어내 순수한 글자만 남기고,
    대신 이건 멘션 메시지였다는 사실만 따로 알려준다.
    색칠은 호출한 쪽에서 ncurses 방식으로 다시 입혀준다.
*/
void strip_ansi(const char *in, char *out, int outsize, int *mentioned) {
    int j = 0;
    *mentioned = 0;

    for (int i = 0; in[i] != '\0' && j < outsize - 1; i++) {
        if (in[i] == '\a') {
            continue;
        }
        if (in[i] == '\033' && in[i + 1] == '[') {
            if (strncmp(&in[i], "\033[93m", 5) == 0)
                *mentioned = 1;
            i++;
            while (in[i + 1] != '\0' && in[i] != 'm')
                i++;
            continue;
        }
        out[j++] = in[i];
    }
    out[j] = '\0';
}

/*
메시지 영역(화면 위쪽)에 한 줄을 출력하는 함수
    다른 사람이 보낸 메시지를 받았을 때도, 내가 보낸 메시지를 내 화면에 찍을 때도 모두 이 함수를 거친다.
    메시지 종류를 보고 색을 정한 뒤 화면에 출력하고, 마지막으로 커서를 다시 아래쪽 입력줄로 돌려놓는다.
    이 단계들은 도중에 끊기면 안 되므로(다른 스레드가 중간에 끼어들어 화면을 건드리면 안 됨) 잠금을 걸고 한 번에 처리한다.
*/
void print_line(const char *line) {
    char clean[BUFSIZE + 64];
    int mentioned = 0;

    strip_ansi(line, clean, sizeof(clean), &mentioned);

    int pair = 0;
    if (mentioned)                                            pair = 3; // 노랑
    else if (strncmp(clean, "[알림]", strlen("[알림]")) == 0)   pair = 1; // 청록
    else if (strstr(clean, "[귓속말]") != NULL)                 pair = 2; // 자홍

    pthread_mutex_lock(&scr_mutx);
    // 창 크기가 바뀔 때 다시 출력할 수 있도록 보관함에도 저장해둔다.
    int idx = (sb_start + sb_count) % SCROLLBACK;
    strncpy(sb_text[idx], clean, sizeof(sb_text[idx]) - 1);
    sb_text[idx][sizeof(sb_text[idx]) - 1] = '\0';
    sb_pair[idx] = pair;
    if (sb_count < SCROLLBACK)
        sb_count++;
    else
        sb_start = (sb_start + 1) % SCROLLBACK; // 가득 차면 가장 오래된 것 폐기

    if (pair) wattron(msgwin, COLOR_PAIR(pair));
    wprintw(msgwin, "%s\n", clean);
    if (pair) wattroff(msgwin, COLOR_PAIR(pair));
    wrefresh(msgwin);
    wrefresh(inpwin);
    pthread_mutex_unlock(&scr_mutx);
}

/*
맨 아래 입력줄을 지우고 지금까지 친 글자를 다시 그려주는 함수
    글자를 하나 칠 때마다, 지울 때마다 호출해서 입력줄을 최신 상태로 유지한다.
    여기서도 화면을 건드리므로, 다른 스레드가 동시에 화면을 그리지 못하도록 잠금을 걸고 처리한다.
*/
static int char_width(const char *s, int *cols) {
    wchar_t wc;
    int n = mbtowc(&wc, s, MB_CUR_MAX);
    if (n <= 0) {
        *cols = 1;
        return 1;
    }
    int w = wcwidth(wc);
    *cols = (w > 0) ? w : 1;
    return n;
}

void redraw_input(const char *buf) {
    pthread_mutex_lock(&scr_mutx);

    // 입력이 화면 폭을 넘어가면 글자가 잘려서 지금 치는 내용이 안 보이는 문제가 있었다.
    // 그래서 한도를 넘으면 앞쪽 글자들을 왼쪽으로 밀어내고(화면에서만, 실제 내용은 그대로)
    // 항상 최신에 입력한 내용이 보이도록 뒤쪽만 잘라서 보여주도록 햇다.
    int avail = COLS - 3;
    if (avail < 1) avail = 1;

    int total = 0;
    for (const char *p = buf; *p != '\0'; ) {
        int w;
        p += char_width(p, &w);
        total += w;
    }
    const char *start = buf;
    while (total > avail && *start != '\0') {
        int w;
        start += char_width(start, &w);
        total -= w;
    }

    werase(inpwin);
    mvwprintw(inpwin, 0, 0, "> %s", start);
    wrefresh(inpwin);
    pthread_mutex_unlock(&scr_mutx);
}

/*
터미널 창 크기가 바뀌었을 때 화면을 새 크기로 다시 짓는 함수
    채팅 도중 터미널 창을 줄였다 늘리면 메시지가 잘리고 구분선이 사라지고
    입력한 글자가 안 보이는 문제가 있었다. 창 크기가 바뀌었는데 화면은
    여전히 옛날 크기 기준으로 그려지고 있었기 때문이다.
    창 크기가 바뀌면 ncurses가 KEY_RESIZE라는 특수 키 입력으로 알려주므로,
    그때 기존 세 창을 지우고 새 크기에 맞춰 다시 만든 뒤 보관함에 저장해둔
    최근 메시지들을 처음부터 다시 그려준다.
*/
void ui_resize(void) {
    pthread_mutex_lock(&scr_mutx);
    endwin();
    refresh();

    delwin(msgwin); delwin(sepwin); delwin(inpwin);
    msgwin = newwin(LINES - 2, COLS, 0, 0);
    sepwin = newwin(1, COLS, LINES - 2, 0);
    inpwin = newwin(1, COLS, LINES - 1, 0);
    scrollok(msgwin, TRUE);
    keypad(inpwin, TRUE);
    wtimeout(inpwin, 20);

    mvwhline(sepwin, 0, 0, ACS_HLINE, COLS);
    wrefresh(sepwin);

    for (int i = 0; i < sb_count; i++) {
        int idx = (sb_start + i) % SCROLLBACK;
        if (sb_pair[idx]) wattron(msgwin, COLOR_PAIR(sb_pair[idx]));
        wprintw(msgwin, "%s\n", sb_text[idx]);
        if (sb_pair[idx]) wattroff(msgwin, COLOR_PAIR(sb_pair[idx]));
    }
    wrefresh(msgwin);
    pthread_mutex_unlock(&scr_mutx);
}

/*
송신 스레드: 키보드 입력을 한 글자씩 받아서 서버로 보냄
    키 입력을 기다리는 동안에는 화면 잠금을 쥐고 있으면 안 된다는 것이 중요하다.
    키 입력을 기다리는 시간 내내 잠금 상태로 있으면 메시지를 받은 수신 스레드가 화면에 메시지를 출력하려 해도 잠금이 안 풀려서 계속 기다려야 한다.
    키를 기다리는 일은 잠금 밖에서 하고, 실제로 화면을 고치는 짧은 순간에만 잠그도록 했다.
*/
void *send_msg(void *arg) {
    char inbuf[BUFSIZE] = {0,};
    int len = 0;

    redraw_input("");

    while (running) {
        wint_t wch;
        int rc = wget_wch(inpwin, &wch);

        if (rc == ERR)
            continue;

        if (rc == KEY_CODE_YES && wch == KEY_RESIZE) {
            ui_resize();
            redraw_input(inbuf);
            continue;
        }

        int is_backspace = (rc == KEY_CODE_YES && wch == KEY_BACKSPACE) ||
                           (rc == OK && (wch == 127 || wch == 8));

        if (is_backspace) {
            while (len > 0) {
                len--;
                if ((inbuf[len] & 0xC0) != 0x80)
                    break;
            }
            inbuf[len] = '\0';
            redraw_input(inbuf);
        }
        else if (rc == OK && (wch == L'\n' || wch == L'\r')) {
            if (len == 0)
                continue;
            if (strcmp(inbuf, "q") == 0 || strcmp(inbuf, "Q") == 0)
                break;

            write(sock, inbuf, len);
            write(sock, "\n", 1);

            /*
            내가 방금 보낸 메시지를 내 화면에도 찍어준다.
            다른 사람들이 보낸 채팅에는 서버가 시각을 붙여주므로,
            내 것에만 시각이 안 찍히면 가독성이 나빠진다. 그래서 여기서도 시각을 붙인다.
            이 시각은 서버가 아니라 내 컴퓨터의 시계를 읽은 것이라 서버가 찍는 시각과 아주 조금 차이날 수 있지만,
            내 화면에만 보이는 것이라 다른 사람들이 보는 대화 순서에는 아무 영향이 없다.
            /w, /list 같은 명령어는 서버가 알아서 답을 보내주므로 여기서는 따로 건드리지 않는다.
            */
            if (inbuf[0] != '/') {
                char echo[BUFSIZE + 32];
                char conv[BUFSIZE];
                char ts[24];
                time_t now = time(NULL);
                struct tm tm_now;
                localtime_r(&now, &tm_now);
                const char *ampm = (tm_now.tm_hour < 12) ? "오전" : "오후";
                int h12 = tm_now.tm_hour % 12;
                if (h12 == 0) h12 = 12;
                snprintf(ts, sizeof(ts), "%s %d:%02d", ampm, h12, tm_now.tm_min);
                convert_emoji(inbuf, conv, sizeof(conv));
                snprintf(echo, sizeof(echo), "%s 나: %s", ts, conv);
                print_line(echo);
            }
            len = 0;
            inbuf[0] = '\0';
            redraw_input(inbuf);
        }
        else if (rc == OK && wch >= 32) {
            char mb[MB_CUR_MAX];
            int n = wcrtomb(mb, (wchar_t)wch, NULL);
            if (n > 0 && len + n < BUFSIZE - 1) {
                memcpy(inbuf + len, mb, n);
                len += n;
                inbuf[len] = '\0';
                redraw_input(inbuf);
            }
        }
    }

    shutdown(sock, SHUT_WR);
    running = 0;
    return NULL;
}

// 수신 스레드: 받은 메시지를 버퍼에 쌓아뒀다가 개행(\n)이 나올 때마다 거기서 잘라 한 줄씩 출력한다.
void *recv_msg(void *arg) {
    char buf[BUFSIZE];
    char acc[BUFSIZE * 2] = {0,};
    int acc_len = 0, n;

    while ((n = read(sock, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';

        if (acc_len + n >= (int)sizeof(acc))
            acc_len = 0;
        memcpy(acc + acc_len, buf, n + 1);
        acc_len += n;

        char *start = acc;
        char *nl;
        while ((nl = strchr(start, '\n')) != NULL) {
            *nl = '\0';
            print_line(start);
            start = nl + 1;
        }
        acc_len = strlen(start);
        memmove(acc, start, acc_len + 1);
    }

    print_line("[알림] 서버와의 연결이 종료되었습니다. 잠시 후 종료됩니다.");
    running = 0;
    return NULL;
}

int main(int argc, char *argv[]) {
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void *thread_return;
    char join_msg[NICKSIZE + 8];

    if (argc != 4) {
        printf("Usage : %s <IP> <port> <nickname>\n", argv[0]);
        exit(1);
    }
    if (strlen(argv[3]) >= NICKSIZE) {
        printf("닉네임은 %d자 이내로 입력해주세요.\n", NICKSIZE - 1);
        exit(1);
    }

    char *loc = setlocale(LC_ALL, "");
    if (loc == NULL || (strstr(loc, "UTF-8") == NULL && strstr(loc, "utf8") == NULL))
        setlocale(LC_ALL, "C.UTF-8");

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error");

    snprintf(join_msg, sizeof(join_msg), "JOIN|%s\n", argv[3]);
    write(sock, join_msg, strlen(join_msg));

    // 화면을 세 부분으로 나눠서 만든다.
    // 위쪽: 주고받는 채팅 메시지가 쌓이는 영역 / 가운데: 가로 구분선 / 맨 아래: 내 입력줄
    initscr();
    cbreak();
    noecho(); // 키를 누를 때 화면에 자동으로 찍히지 않게 한다.
              // 한글 처리를 위해 글자를 어디에 어떻게 그릴지 직접 관리해야 하므로, 입력줄에 글자를 출력하는 일은 redraw_input 함수가 맡는다.
    start_color();
    use_default_colors();
    init_pair(1, COLOR_CYAN, -1);    // 알림
    init_pair(2, COLOR_MAGENTA, -1); // 귓속말
    init_pair(3, COLOR_YELLOW, -1);  // 멘션

    // 화면 전체 높이 LINES 에서 구분선 1줄, 입력줄 1줄을 빼고 나머지를 채팅 메시지 영역으로
    msgwin = newwin(LINES - 2, COLS, 0, 0);
    sepwin = newwin(1, COLS, LINES - 2, 0);
    inpwin = newwin(1, COLS, LINES - 1, 0);
    scrollok(msgwin, TRUE); // 메시지가 가득 차면 위로 자동 스크롤
    keypad(inpwin, TRUE);
    wtimeout(inpwin, 20);

    mvwhline(sepwin, 0, 0, ACS_HLINE, COLS); // 메시지 영역과 입력줄 사이에 가로줄 긋기
    wrefresh(sepwin);

    char welcome[128];
    snprintf(welcome, sizeof(welcome), "[알림] '%s' 닉네임으로 접속했습니다.", argv[3]);
    print_line(welcome);
    print_line("[알림] 명령어: /w 귓속말, /list 목록, /rooms 방목록, /join 방이동, /leave 방나가기, /topic 공지(방장), /block 차단, /kick 강퇴(방장), @멘션, q 종료");

    // 입력 담당 스레드와 수신 담당 스레드를 각각 띄운다.
    pthread_create(&snd_thread, NULL, send_msg, NULL);
    pthread_create(&rcv_thread, NULL, recv_msg, NULL);

    // 두 스레드가 모두 끝날 때까지 여기서 기다린다.
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);

    endwin(); // ncurses 종료: 터미널을 원래 상태로 복원
    close(sock);
    printf("채팅을 종료했습니다.\n");
    return 0;
}
