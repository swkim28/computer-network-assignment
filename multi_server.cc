/*
[구현한 기능들]
1. 닉네임 등록 및 중복 닉네임은 거부
    소켓 번호만으로는 대화 상대를 구분할 수 없다. 누가 채팅을 쳤는지 알 수 있어야 한다.
2. 접속 및 퇴장 알림
    채팅을 하려면 지금 속해 있는 채팅방에 누가 접속하고 퇴장했는지 알아야 한다.
3. 일반 채팅 표시
    '닉네임: 내용' 형태로 표시해 준다.
4. 귓속말: /w 닉네임 내용 (대상에게만 전송, 보낸 사람에게는 보낸 내용 띄워주기)
    전체 공개되는 채팅 뿐만 아니라 1대1로 사적인 대화가 가능하기도 해야 한다.
    귓말을 보낸 사람에게도 누구에게 어떤 내용을 보냈는지 띄워주어 귓말이 올바로 전송됐음을 확인시켜준다.
5. 접속자 목록: /list
    이 명령어를 입력한 요청자에게만 응답해 준다. 본인은 '(나)'로 표시하여 보여준다.
    서버에 접속해 있는 사람들을 방별로 묶어서 보여준다.
    귓말을 보낼 대상의 닉네임을 단번에 알아내기에도 용이하다.
6. 최근 대화 불러오기(리플레이): 새로운 접속자에게 최근 20개의 대화를 보여준다.
    채팅 중간에 채팅방에 새로 입장한 사람은 이전에 어떤 대화들이 오갔는지 모른다.
    서버에 처음 접속했을 때뿐 아니라 /join으로 방을 옮겼을 때도 그 방의 최근 대화를 보여준다.
7. 멘션: 메시지에 '@닉네임'이 포함되면 그 사용자에게는 해당 줄을 노란색으로 하이라이트 시켜준다.
    @로 내 닉네임이 언급된 채팅만 노란색으로 강조한다.
8. 채팅방(채널)
    /join 방이름 으로 방 이동, 채팅/멘션/입퇴장 알림은 같은 방 안에서만 전달해 준다.
    채팅방에 최초 접속 시 기본 방인 로비에서 시작한다.
9. 타임스탬프: 채팅/귓속말 앞에 시각(오전/오후 몇 시 몇 분)을 표시
    채팅이 언제 쳐진 채팅인지 알 수 있도록 타임스탬프 기능도 추가했다.
10. 이모티콘 변환: 대화 내용의 :smile: 등 코드를 유니코드 이모지로 치환
    텍스트뿐만 아니라 이모티콘도 보낼 수 있어야 채팅 프로그램이라고 할 수 있으므로 간단한 이모티콘 10개만 구현했다.
11. 차단: /block 닉네임 으로 특정 사용자의 채팅/귓속말을 받지 않는다.
    누군가의 채팅을 보고 싶지 않을 수도 있으므로 차단 기능을 추가했다.
    /unblock 하면 해제하고, /block을 단독으로 입력하면 차단 목록을 확인한다.
12. 방장/강퇴: 빈 방에 처음 입장한 사람이 방장이 됨. /kick 닉네임 으로 같은 방 사용자를 로비로 추방, 방장 퇴장 시 방장은 자동 승계됨
    실제 채팅 프로그램에서는 문제를 일으키는 사용자를 강퇴할 수도 있어야 한다.
13. 잘못된 명령어/대상 없음 등 모든 실패 상황을, 입력한 본인에게만 안내함
    없는 닉네임, 내용 없는 귓속말, 존재하지 않는 명령어 등 모든 실패 상황에 안내 메시지가 출력되도록 했다.
    이를 따로 안내해주지 않으면, 사용자 입장에서는 입력을 했는데 아무런 피드백이 없을 때 자신이 잘못된 입력을 한 건지 단순 서버 에러인건지 판단할 방법이 없기 때문이다.
14. 방 목록: /rooms (존재하는 방들의 인원수, 방장, 공지를 한눈에 보여준다)
    /join으로 방을 만들 수는 있는데, 정작 다른 사람은 그 방이 존재하는지 알 방법이 없었다.
    귓속말을 만들었더니 대상 닉네임을 알기 위한 /list가 필요해졌던 것처럼, 방을 만들었더니 방 이름을 알기 위한 /rooms가 필요해졌다.
15. 방 공지: /topic 내용 (방장 전용, /topic 단독 입력 시 현재 방의 공지 확인)
    방 이름만으로는 무엇을 하는 방인지 알기 힘들다.
    방장이 방의 목적을 한 줄로 적어두면 방에 입장할 때 보여주고, /rooms 목록에서도 함께 보여줘서 어떤 방인지 판단할 수 있게 했다.
16. 방 나가기: /leave (현재 방에서 나가 기본 방인 로비로 돌아감)
    다른 방에 갔다가 로비로 돌아오려면 매번 '/join 로비' 를 입력해야 해서 번거로웠다.
    방을 나간다는 것은 곧 로비로 돌아간다는 것이므로, 이를 /leave 명령어 하나로 줄였다.
*/

/*
[설계]
- 얇은 클라이언트
    클라이언트를 최대한 단순하게 만들고, 판단이 필요한 일은 모두 서버가 하도록 했다.
    클라이언트가 하는 일은 세 가지뿐인데,
    사용자로부터 키보드 입력을 받고, 그걸 그대로 서버에 보내고, 다시 서버가 보내준 글자를 화면에 출력하는 것이다.
    이렇게 만들면, 일단 기능을 추가할 때 서버만 고치면 된다.
    실제로 귓속말, 접속자 목록 list, 차단, 강퇴 등을 구현할 때 클라이언트는 안내 문구만 건드리고 고치지 않았다.
    그리고 또 채팅을 누가 보냈는지 판단을 소켓을 가지고 있는 서버가 하기 때문에, 실제 채팅 프로그램처럼 사칭을 막을 수도 있다.
    또 서버가 규칙을 정하므로 채팅방의 모든 참여자가 똑같은 화면을 본다.
- 소켓 번호만 담은 int 배열을 client_t 구조체 배열로 바꿨다.
    원래는 클라이언트를 소켓 번호만 담은 배열로 관리했는데,
    이러면 서버는 몇 번 소켓이 연결되어 있다는 것만 알고 누가 누군지를 모르게 된다.
    닉네임을 붙이려면 이 소켓의 주인이 누구인지를 알아야 하고, 귓속말을 보내려면 이 닉네임을 가진 사람의 소켓이 몇 번인지도 역으로 알아낼 수 있어야 한다.
    그래서 소켓 번호 하나만 저장하던 것을 그 사람에 대한 정보를 통째로 담는 구조체로 바꿨다.
    그 구조체에는 소켓 번호, 닉네임, 지금 속한 방, 방장인지 여부, 차단한 사람 목록 등의 정보가 다 들어가 있다.
    기능을 추가할 때마다 여기에 필요한 정보를 하나씩 추가하는 식으로 확장했다.
    퇴장 처리도 방식을 바꿨는데, 원래는 누군가 나가면 배열에서 그 자리를 빼고 뒤에 있는 사람들을 한 칸씩 앞으로 당겨왔다.
    하지만 사람들이 동시에 들어오고 나가는 상황에서 원소를 옮기는 건 위험하다.
    옮기는 도중에 다른 스레드가 그 배열을 읽으면 엉뚱한 사람을 가리키게 되기 때문이다.
    그래서 아예 옮기지 않고 active라는 표시만 꺼두면 그 자리는 비어 있는 것으로 여기고, 나중에 새로 접속한 사람이 그 자리를 다시 쓰는 방식으로 바꿨다.
- clients 배열처럼 여러 스레드가 같이 쓰는 데이터는 전부 mutex로 보호했다.
    특히 닉네임 등록과 귓속말을 조심해야 했는데,
    닉네임 등록은 이미 닉네임이 있는지 검사하는 과정과 닉네임이 없으면 등록하는 과정을 따로 잠그면 안 된다.
    두 사람이 동시에 같은 닉네임으로 들어오면 둘 다 검사를 통과하고 둘 다 등록될 수 있기 때문이다.
    귓속말도 대상을 찾고 잠금을 풀면, 보내려는 찰나에 그 사람이 나가버릴 수가 있다.
    확인하고 나서 행동하기까지를 한 덩어리로 묶어 race condition을 방지했다.
- 스레드에는 소켓 번호의 malloc 복사본을 전달한다.
    clnt_sock라는 지역 변수의 주소를 넘기면, 다음 accept()가 값을 덮어쓴다면 두 스레드가 같은 소켓을 읽을 수 있기 때문이다.
- '/'로 시작하는 입력은 전체 채팅으로 내보내지 않는다. 귓속말 내용이나 명령어 입력이 전체 채팅에 보이면 안 되기 때문이다.
- 개행 문자 \n 까지를 메시지 하나로 정했다. 받은 데이터를 버퍼에 쌓아두다가 개행이 나올 때마다 거기서 잘라 처리하도록 했다.
    개행이 아직 안 온 조각은 버퍼에 남겨뒀다가 다음에 이어붙인다.
- 최근 대화 리플레이: 채팅방에 들어가면 그 방의 최근 대화를 보여준다.
    저장 대상은 일반 채팅만이다. 귓속말 같은 건 저장하지 않는다.
    입퇴장 알림 같은 내용도 새로운 접속자가 알 필요는 없으므로 제외한다.
- 대화 기록은 방마다 따로 저장한다.
    방에 새로 들어온 사람에게 최근 대화를 보여줄 때, 그 방에서 오간 대화만 보여줘야 한다.
    그리고 처음에는 모든 방이 하나의 저장 공간을 같이 쓰는 식으로 만들었다.
    방 이름만 같이 적어두고 골라서 보여주는 식이었는데, 이렇게 해버리면 채팅이 활발한 방 하나가 최근 대화 20개를 다 차지해 버려서 조용한 방의 채팅 기록이 밀려나 사라졌다.
    조용한 방에 들어가면 보여줄 채팅 내역이 아무것도 없게 되어 버리는 것이다.
    그래서 방마다 저장 공간을 따로 주었다. 어느 방이든 그 방의 최근 대화 20개는 확실하게 남게 된다.
    그리고 방의 마지막 사람이 나가서 방이 사라지면 대화 기록을 모두 지운다.
- 멘션: 일반 채팅 메시지는 전원이 동일한 문자열을 받지만,
    @ 멘션은 수신자가 누구냐에 따라 하이라이트 처리된 메시지를 보내는 수신자 맞춤 전송이다.
    판별 기준이 수신자의 닉네임이므로 닉네임 목록을 가진 서버만이 처리할 수 있다.
    서버가 멘션 여부를 ANSI 코드로 표시해 전송하고, 클라이언트는 해당 코드를 제거한 뒤 ncurses 색상으로 변환한다.
    히스토리에는 색상 없는 일반 버전을 저장한다.

    멘션 구현에는 문제가 하나 있었는데, 처음에는 그냥 '@닉네임'이라는 글자가 들어있는지만 확인했다가
    예를 들어 '승우'와 '승우2'가 같이 접속해 있을 때 누가 '@승우2'라고 채팅을 입력하면,
    그 안에 '@승우'도 들어있기 때문에 승우까지 멘션으로 걸려버리는 문제가 있었다.
    그래서 닉네임 바로 뒤에 오는 글자까지 확인하도록 하는 is_mentioned 함수를 구현했다.
    뒤에 영어나 숫자, 밑줄이 이어지면 그건 다른 사람의 닉네임을 부른 것으로 보고 멘션이 아니라고 여기기로 했다.
    @승우2와 @승우를 분리시킨 것이다.
    반면 공백이나 문장부호, 한글이 오거나 거기서 문장이 끝나면 그 사람을 부른 것으로 판단한다.
    @승우 안녕, @승우님, @승우, 처럼 조사나 부호가 붙는 경우도 정상적으로 멘션 처리된다.
- 채팅방 목록을 따로 관리하지 않고 client_t의 room 필드가 같은 사람들을 같은 방으로 정의한다.
    첫 사용자가 입장하면 방이 생성되는 것이고 마지막 사용자가 퇴장하면 방이 소멸되는 것이다.
    별도의 방 생성, 방 삭제, 빈 방 관리 설계가 필요 없다.
- 범위는 메시지별로 다르다. 일반채팅/멘션/입퇴장 알림은 같은 방 안에서만 전달된다.
    귓속말은 개인 간의 메시지이므로 방과 무관하게 전달되게 했다. 
- block(차단)은 서버가 전달 시점에 필터링하며 차단 사실은 상대에게 노출하지 않는다.
    차단당한 사람의 귓속말은, 보낸 사람 시점에선 발신 확인까지 정상으로 보이지만 실제로는 전달되지 않는다.
    리플레이에서도 차단한 사람의 메시지를 걸러내기 위해 히스토리에 발신자 닉네임을 함께 저장한다.
    하지만 차단은 닉네임 기준이라 상대가 닉네임을 바꿔 재접속하면 풀린다는 한계가 있기도 하다.
- 방장은 빈 방에 처음 입장한 사람이다.
    방을 만드는 별도의 명령어를 두지 않고, 아무도 없는 이름의 방에 /join으로 들어가면 그 순간 방이 생기고 처음 들어간 사람이 자연스럽게 방장이 된다.
    기본 방인 로비는 예외이다. 로비는 방장이 없고 강퇴도 불가능하다.
    방장이 방을 떠나면 남은 사람 중 한 명에게 방장을 넘겨 방장 공백을 막는다.
    강퇴는 채팅 프로그램과의 연결을 종료시키는 것이 아니라 로비로 추방하는 것이다.
- 공지는 사람이 아니라 방에 딸린 정보라서 따로 저장해야 했다.
    지금까지 저장한 정보들은 사람에 대한 정보라 모두 사용자 정보 안에 넣으면 됐지만 공지는 방과 관련된 것이므로
    방 이름과 그 방의 공지를 짝지어서 저장하는 표를 따로 만들었다.
    그리고 방의 마지막 사람이 나가서 그 방이 사라지면 공지도 같이 지운다.

[컴파일 방법]
    gcc -o multi_server multi_server.cc -lpthread
    gcc -o multi_client multi_client.cc -lncursesw -lpthread
    없으면 설치: sudo apt install libncursesw5-dev

[실행 방법]
    ./multi_server [포트번호] (예: ./multi_server 9190)
    ./multi_client 127.0.0.1 9190 닉네임

- ./multi_client 127.0.0.1 9190 닉네임
    으로 클라이언트가 서버에 접속을 하면, 접속 직후 'JOIN|닉네임' 형태로 보내준다.
    서버는 접속 후 첫 메시지가 JOIN| 으로 시작하면 그 뒤를 닉네임으로 받아들인다.
    등록 이후에 보내는 것들은 모두 일반 채팅이나 명령어로 처리된다.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUFSIZE     1024
#define NICKSIZE    32
#define ROOMSIZE    32
#define MAX_CLIENT  64
#define MAX_BLOCK   8    // 한 명당 차단 가능 인원
#define TOPICSIZE   128  // 방 공지 최대 길이
#define MAX_TOPIC   64   // 공지를 저장할 수 있는 방의 수
#define DEFAULT_ROOM "로비" // 접속 직후 맨 처음에 속하는 기본 로비 방

typedef struct {
    int  sock;
    char nick[NICKSIZE];
    char room[ROOMSIZE]; // 현재 속해있는 방
    int  room_owner;     // 이 값이 1이면 현재 방의 방장 (로비에서는 항상 0)
    char blocked[MAX_BLOCK][NICKSIZE]; // 내가 차단한 닉네임 목록
    int  n_blocked;
    int  active;
} client_t;

client_t clients[MAX_CLIENT];
pthread_mutex_t mutx;

void send_to(int sock, const char *msg);

// 이모티콘 매핑 테이블
// :code: 형태의 코드를 유니코드 이모지로 치환 -> 치환은 서버가 메시지를 내보내기 전에 수행
// 매핑에 없는 코드나 일반 콜론은 건드리지 않는다.
typedef struct { const char *code; const char *emoji; } emoji_t;
static const emoji_t EMOJI_MAP[] = {
    {":smile:", "\U0001F60A"}, {":laugh:", "\U0001F602"},
    {":cry:",   "\U0001F622"}, {":heart:", "\u2665"},
    {":star:",  "\u2605"},     {":thumbsup:", "\U0001F44D"},
    {":ok:",    "\U0001F44C"}, {":fire:",  "\U0001F525"},
    {":check:", "\u2714"},     {":angry:", "\U0001F620"},
};
static const int EMOJI_COUNT = sizeof(EMOJI_MAP) / sizeof(EMOJI_MAP[0]);

// src의 이모티콘 코드를 이모지로 치환해서 dst에 쓴다.
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
                    di += elen;
                    i += clen;
                    matched = 1;
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

// 메시지를 보낸 현재 시각을 '오전 1:02' / '오후 5:07' 이런 형식으로 out에 채운다.
void timestamp(char *out, int size) {
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);

    const char *ampm = (tm_now.tm_hour < 12) ? "오전" : "오후";
    int h12 = tm_now.tm_hour % 12;
    if (h12 == 0) h12 = 12;
    snprintf(out, size, "%s %d:%02d", ampm, h12, tm_now.tm_min);
}

// 멘션(@)용 ANSI 코드
#define ANSI_MENTION "\033[93m" // 밝은 노란색
#define ANSI_RESET   "\033[0m"

// 특정 방의 구성원들에게 메시지 전송 (입퇴장/이동 알림용)
void broadcast_room(const char *room, const char *msg, int except_sock) {
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clients[i].active && strcmp(clients[i].room, room) == 0 &&
            clients[i].sock != except_sock)
            write(clients[i].sock, msg, strlen(msg));
    }
    pthread_mutex_unlock(&mutx);
}

void send_to(int sock, const char *msg) {
    write(sock, msg, strlen(msg));
}

// recipient_idx 사용자가 sender 닉네임을 차단했는지 검사하는 함수
int is_blocked(int recipient_idx, const char *sender) {
    for (int b = 0; b < clients[recipient_idx].n_blocked; b++)
        if (strcmp(clients[recipient_idx].blocked[b], sender) == 0)
            return 1;
    return 0;
}


// 채팅 내용에 특정 닉네임이 멘션되었는지 검사하는 함수
int is_mentioned(const char *content, const char *nick) {
    char pattern[NICKSIZE + 2];
    snprintf(pattern, sizeof(pattern), "@%s", nick);
    int plen = strlen(pattern);
 
    const char *p = content;
    while ((p = strstr(p, pattern)) != NULL) {
        char next = *(p + plen); // 닉네임 바로 뒤에 오는 글자

        // 뒤에 닉네임에 쓰일 만한 문자가 이어지면 다른 사람을 부른 것
        int is_nick_char = (next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ||
                           (next >= '0' && next <= '9') || (next == '_');
        if (!is_nick_char)
            return 1;
        p += plen;
    }
    return 0;
}

// 채팅 메시지를 같은 방 사람들에게 뿌린다 (보낸 사람 본인은 제외)
// 메시지에 '@닉네임'(멘션)이 들어있는 사람에게는 노란색 채팅을, 나머지에게는 평범한 버전의 채팅을 보낸다.
// 이렇게 받는 사람마다 다르게 처리하려면 누가 어떤 방에 있고 누구를 차단했는지 알아야 하는데, 그 정보는 서버만 갖고 있으므로 서버가 할 일이다.
// 마지막으로 발신자가 있던 방 이름을 room_out에 담아 돌려준다. 호출한 쪽에서 이 메시지를 어느 방 대화 기록에 저장할지 알아야 하기 때문이다.
void broadcast_chat(const char *sender, const char *content, int sender_sock,
                    char *room_out, int room_out_size) {
    char plain[BUFSIZE + NICKSIZE + 32];
    char highlighted[BUFSIZE + NICKSIZE + 64];
    char ts[24];

    timestamp(ts, sizeof(ts));
    snprintf(plain, sizeof(plain), "%s %s: %s\n", ts, sender, content);
    snprintf(highlighted, sizeof(highlighted), ANSI_MENTION "%s %s: %s" ANSI_RESET "\n",
             ts, sender, content);

    pthread_mutex_lock(&mutx);

    room_out[0] = '\0';
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clients[i].active && clients[i].sock == sender_sock) {
            strncpy(room_out, clients[i].room, room_out_size - 1);
            room_out[room_out_size - 1] = '\0';
            break;
        }
    }
    
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!clients[i].active || clients[i].sock == sender_sock ||
            strcmp(clients[i].room, room_out) != 0)
            continue;
        if (is_blocked(i, sender))
            continue;
        if (is_mentioned(content, clients[i].nick))
            write(clients[i].sock, highlighted, strlen(highlighted));
        else
            write(clients[i].sock, plain, strlen(plain));
    }
    pthread_mutex_unlock(&mutx);
}
/*
방별 최근 대화 내역 저장용 히스토리 버퍼.
처음에는 모든 방이 링 버퍼 하나를 공유하는 구조였는데, 채팅이 많은 방이 채팅이 적은 방의 채팅 히스토리를 밀어내 버려서
채팅이 적은 방은 채팅 리플레이가 불가능한 문제가 있었다.
그래서 방마다 독립된 버퍼를 두기로 했다.
최근 20개만 남기려면, 21번째 메시지가 오는 순간 가장 오래된 걸 버려야 한다.
배열 원소를 매번 한 칸씩 앞으로 당기면 비효율적이므로 시작 위치만 한 칸 옮기는 링 버퍼를 사용한다.
가장 오래된 자리에 새 채팅 메시지가 그냥 덮어써진다.
*/
#define HISTORY_MAX 20
#define HISTMSG_LEN (BUFSIZE + NICKSIZE + 16)
#define MAX_ROOMHIST (MAX_CLIENT + 1)

typedef struct {
    char sender[NICKSIZE]; // 리플레이 시 수신자의 차단 목록과 대조하기 위함
    char text[HISTMSG_LEN];
} hist_entry_t;

typedef struct {
    char room[ROOMSIZE];   // 이 버퍼가 담당하는 방
    int  used;             // 이 칸이 사용 중인지
    int  start;            // 가장 오래된 메시지
    int  count;            // 현재 저장된 메시지 수
    hist_entry_t entry[HISTORY_MAX];
} room_hist_t;
room_hist_t room_hists[MAX_ROOMHIST];

// 일반 채팅 메시지 하나를 그 방의 전용 버퍼에 저장
void add_history(const char *room, const char *sender, const char *msg) {
    pthread_mutex_lock(&mutx);
    int slot = -1, empty = -1;
    for (int i = 0; i < MAX_ROOMHIST; i++) {
        if (room_hists[i].used && strcmp(room_hists[i].room, room) == 0) { slot = i; break; }
        if (!room_hists[i].used && empty < 0)
            empty = i;
    }
    if (slot < 0) {
        if (empty < 0) {
            pthread_mutex_unlock(&mutx);
            return;
        }
        slot = empty;
        strncpy(room_hists[slot].room, room, ROOMSIZE - 1);
        room_hists[slot].room[ROOMSIZE - 1] = '\0';
        room_hists[slot].used = 1;
        room_hists[slot].start = 0;
        room_hists[slot].count = 0;
    }
    room_hist_t *h = &room_hists[slot];
    int idx = (h->start + h->count) % HISTORY_MAX;
    strncpy(h->entry[idx].sender, sender, NICKSIZE - 1);
    h->entry[idx].sender[NICKSIZE - 1] = '\0';
    strncpy(h->entry[idx].text, msg, HISTMSG_LEN - 1);
    h->entry[idx].text[HISTMSG_LEN - 1] = '\0';
    if (h->count < HISTORY_MAX)
        h->count++;
    else
        h->start = (h->start + 1) % HISTORY_MAX;
    pthread_mutex_unlock(&mutx);
}

// 특정 방의 최근 대화를 오래된 순서대로 전송 (접속/방 이동 시 호출)
// 수신자가 차단한 발신자의 메시지는 리플레이에서도 걸러낸다.
void send_history(int sock, const char *room) {
    char header[96];
    int matched = 0, me = -1, slot = -1;

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_CLIENT; i++)
        if (clients[i].active && clients[i].sock == sock) { me = i; break; }
    for (int i = 0; i < MAX_ROOMHIST; i++)
        if (room_hists[i].used && strcmp(room_hists[i].room, room) == 0) { slot = i; break; }
    if (slot < 0) {
        pthread_mutex_unlock(&mutx);
        return;
    }
    room_hist_t *h = &room_hists[slot];

    for (int i = 0; i < h->count; i++) {
        int idx = (h->start + i) % HISTORY_MAX;
        if (me < 0 || !is_blocked(me, h->entry[idx].sender))
            matched++;
    }
    if (matched == 0) {
        pthread_mutex_unlock(&mutx);
        return;
    }
    snprintf(header, sizeof(header), "[알림] ── '%s' 방의 최근 대화 %d개 ──\n", room, matched);
    write(sock, header, strlen(header));
    for (int i = 0; i < h->count; i++) {
        int idx = (h->start + i) % HISTORY_MAX;
        if (me < 0 || !is_blocked(me, h->entry[idx].sender))
            write(sock, h->entry[idx].text, strlen(h->entry[idx].text));
    }
    pthread_mutex_unlock(&mutx);

    send_to(sock, "[알림] ── 여기서부터 실시간 대화 ──\n");
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(-1);
}

// 귓속말: 반환값이 1이면 성공, 0이면 대상 없음
// 대상 찾기와 전송을 한 임계 구역에서 처리한다.
// 귓속말은 발신자와 수신자가 다른 방에 있더라도 전달된다.
int send_whisper(const char *from, const char *to, const char *content, int from_sock) {
    char sendbuf[BUFSIZE + NICKSIZE * 2 + 48];
    char ts[24];
    int found = 0;

    timestamp(ts, sizeof(ts));

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clients[i].active && strcmp(clients[i].nick, to) == 0) {
            // 대상이 발신자를 차단했다면 전달하지 않되, found는 1로 유지해서 발신자에게는 정상 전송된 것처럼 보이게 한다.
            // 차단 사실을 상대에게 노출하지 않기 위함이다.
            if (!is_blocked(i, from)) {
                snprintf(sendbuf, sizeof(sendbuf), "%s [귓속말] %s -> 나: %s\n", ts, from, content);
                write(clients[i].sock, sendbuf, strlen(sendbuf));
            }
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutx);

    if (found) { // 발신자에게도 귓말이 올바르게 전송됐음을 화면에 띄워 확인시켜준다.
        snprintf(sendbuf, sizeof(sendbuf), "%s [귓속말] 나 -> %s: %s\n", ts, to, content);
        send_to(from_sock, sendbuf);
    }
    return found;
}

// /list 명령에 보여줄 접속자 목록 문자열을 만든다.
// 목록을 만드는 도중에 누가 들어오거나 나가면, 결과가 이상한 목록이 나올 수 있으므로 목록을 다 만들 때까지 잠금을 걸어두고 진행한다.
void build_user_list(char *buf, int bufsize, const char *my_nick) {
    char rooms[MAX_CLIENT][ROOMSIZE];
    int nrooms = 0, total = 0;

    pthread_mutex_lock(&mutx);

    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!clients[i].active)
            continue;
        total++;
        int seen = 0;
        for (int r = 0; r < nrooms; r++)
            if (strcmp(rooms[r], clients[i].room) == 0) { seen = 1; break; }
        if (!seen) {
            strncpy(rooms[nrooms], clients[i].room, ROOMSIZE - 1);
            rooms[nrooms][ROOMSIZE - 1] = '\0';
            nrooms++;
        }
    }

    int off = snprintf(buf, bufsize, "[접속자 목록] 총 %d명\n", total);
    for (int r = 0; r < nrooms && off < bufsize - 1; r++) {
        int members = 0;
        for (int i = 0; i < MAX_CLIENT; i++)
            if (clients[i].active && strcmp(clients[i].room, rooms[r]) == 0)
                members++;
        off += snprintf(buf + off, bufsize - off, " - %s(%d): ", rooms[r], members);
        int first = 1;
        for (int i = 0; i < MAX_CLIENT && off < bufsize - 1; i++) {
            if (!clients[i].active || strcmp(clients[i].room, rooms[r]) != 0)
                continue;
            off += snprintf(buf + off, bufsize - off, "%s%s%s%s",
                            first ? "" : ", ",
                            clients[i].nick,
                            clients[i].room_owner ? "(방장)" : "",
                            strcmp(clients[i].nick, my_nick) == 0 ? "(나)" : "");
            first = 0;
        }
        if (off < bufsize - 1)
            off += snprintf(buf + off, bufsize - off, "\n");
    }
    pthread_mutex_unlock(&mutx);
}

// 방 공지 저장소
typedef struct {
    char room[ROOMSIZE];
    char topic[TOPICSIZE];
    int  used;
} topic_t;
topic_t topics[MAX_TOPIC];

// 방의 공지를 설정
int set_topic(const char *room, const char *text) {
    int empty = -1;
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_TOPIC; i++) {
        if (topics[i].used && strcmp(topics[i].room, room) == 0) {
            strncpy(topics[i].topic, text, TOPICSIZE - 1);
            topics[i].topic[TOPICSIZE - 1] = '\0';
            pthread_mutex_unlock(&mutx);
            return 1;
        }
        if (!topics[i].used && empty < 0)
            empty = i;
    }
    if (empty < 0) {
        pthread_mutex_unlock(&mutx);
        return 0;
    }
    strncpy(topics[empty].room, room, ROOMSIZE - 1);
    topics[empty].room[ROOMSIZE - 1] = '\0';
    strncpy(topics[empty].topic, text, TOPICSIZE - 1);
    topics[empty].topic[TOPICSIZE - 1] = '\0';
    topics[empty].used = 1;
    pthread_mutex_unlock(&mutx);
    return 1;
}

// 방의 공지를 out에 복사
int get_topic(const char *room, char *out, int outsize) {
    int found = 0;
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_TOPIC; i++) {
        if (topics[i].used && strcmp(topics[i].room, room) == 0) {
            strncpy(out, topics[i].topic, outsize - 1);
            out[outsize - 1] = '\0';
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&mutx);
    return found;
}

// 방이 비게 되면 즉 마지막 사용자가 떠나면. 그 방의 공지와 대화 기록을 함께 지운다.
// 로비는 항상 유지되는 기본 채팅방이기 때문에 잠시 비어도 방의 소멸로 보지 않고 기록을 남겨둔다.
void clear_room_if_empty(const char *room) {
    if (strcmp(room, DEFAULT_ROOM) == 0)
        return;
    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clients[i].active && strcmp(clients[i].room, room) == 0) {
            pthread_mutex_unlock(&mutx);
            return;
        }
    }
    for (int i = 0; i < MAX_TOPIC; i++) {
        if (topics[i].used && strcmp(topics[i].room, room) == 0) {
            topics[i].used = 0;
            break;
        }
    }
    for (int i = 0; i < MAX_ROOMHIST; i++) {
        if (room_hists[i].used && strcmp(room_hists[i].room, room) == 0) {
            room_hists[i].used = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutx);
}

// /rooms 명령에 보여줄 방 목록 문자열을 만든다.
// 방마다 인원수, 방장, 공지를 함께 보여줘서 들어가기 전에 어떤 방인지 판단할 수 있게 했다.
// 접속자 목록과 마찬가지로, 목록을 다 만들 때까지 잠금을 걸어둔 채 진행한다.
void build_room_list(char *buf, int bufsize) {
    char rooms[MAX_CLIENT][ROOMSIZE];
    int nrooms = 0;

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!clients[i].active)
            continue;
        int seen = 0;
        for (int r = 0; r < nrooms; r++)
            if (strcmp(rooms[r], clients[i].room) == 0) { seen = 1; break; }
        if (!seen) {
            strncpy(rooms[nrooms], clients[i].room, ROOMSIZE - 1);
            rooms[nrooms][ROOMSIZE - 1] = '\0';
            nrooms++;
        }
    }
    int off = snprintf(buf, bufsize, "[방 목록] 총 %d개\n", nrooms);
    for (int r = 0; r < nrooms && off < bufsize - 1; r++) {
        int members = 0;
        const char *owner = NULL;
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (clients[i].active && strcmp(clients[i].room, rooms[r]) == 0) {
                members++;
                if (clients[i].room_owner)
                    owner = clients[i].nick;
            }
        }
        if (owner != NULL)
            off += snprintf(buf + off, bufsize - off, " - %s (%d명, 방장: %s)\n", rooms[r], members, owner);
        else
            off += snprintf(buf + off, bufsize - off, " - %s (%d명)\n", rooms[r], members);
        for (int t = 0; t < MAX_TOPIC && off < bufsize - 1; t++) {
            if (topics[t].used && strcmp(topics[t].room, rooms[r]) == 0) {
                off += snprintf(buf + off, bufsize - off, "     공지: %s\n", topics[t].topic);
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutx);
}

// 사용자를 등록해준다.
// 반환값: 슬롯 인덱스, -1이면 닉네임 중복, -2이면 정원 초과
// 중복 검사와 등록을 한 임계 구역으로 묶어 동시에 똑같은 사용자를 등록하는 race를 방지한다.
int register_client(int sock, const char *nick) {
    int slot = -2;

    pthread_mutex_lock(&mutx);
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (clients[i].active && strcmp(clients[i].nick, nick) == 0) {
            pthread_mutex_unlock(&mutx);
            return -1;
        }
    }
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!clients[i].active) {
            clients[i].sock = sock;
            strncpy(clients[i].nick, nick, NICKSIZE - 1);
            clients[i].nick[NICKSIZE - 1] = '\0';
            strncpy(clients[i].room, DEFAULT_ROOM, ROOMSIZE - 1); // 기본 방 로비에서 시작
            clients[i].room[ROOMSIZE - 1] = '\0';
            clients[i].room_owner = 0; // 로비는 방장이 따로 없음
            clients[i].n_blocked = 0;  // 슬롯 재사용 시 이전 사용자의 차단 목록/방장 플래그가 남지 않도록 초기화
            clients[i].active = 1;
            slot = i;
            break;
        }
    }
    pthread_mutex_unlock(&mutx);
    return slot;
}

void remove_client(int slot) {
    pthread_mutex_lock(&mutx);
    clients[slot].active = 0;
    pthread_mutex_unlock(&mutx);
}

// slot의 현재 방 이름을 out에 복사
void get_room(int slot, char *out, int outsize) {
    pthread_mutex_lock(&mutx);
    strncpy(out, clients[slot].room, outsize - 1);
    out[outsize - 1] = '\0';
    pthread_mutex_unlock(&mutx);
}

// 방 이동 처리 ("/join 방이름")
// 방 변경 -> 새 방의 최근 대화 리플레이 -> 새 방에 입장을 알림 -> 이전 방에는 방을 이동했음을 알린다.
// 채팅 참여자 모두가 편히 상황을 알 수 있도록 했다.
void do_join(int clnt_sock, int slot, const char *nick, const char *newroom) {
    char oldroom[ROOMSIZE];
    char heir[NICKSIZE] = {0,};
    char notice[ROOMSIZE * 2 + NICKSIZE + 96];
    int became_owner = 0;

    pthread_mutex_lock(&mutx);
    strncpy(oldroom, clients[slot].room, ROOMSIZE - 1);
    oldroom[ROOMSIZE - 1] = '\0';
    if (strcmp(oldroom, newroom) == 0) {
        pthread_mutex_unlock(&mutx);
        snprintf(notice, sizeof(notice), "[알림] 이미 '%s' 방에 있습니다.\n", newroom);
        send_to(clnt_sock, notice);
        return;
    }

    int was_owner = clients[slot].room_owner;

    // 입장하는 새 방이 비어 있으면(첫 입장) 그 사용자가 방장이 된다. 로비는 제외
    int has_members = 0;
    for (int i = 0; i < MAX_CLIENT; i++)
        if (clients[i].active && i != slot && strcmp(clients[i].room, newroom) == 0)
            { has_members = 1; break; }
    became_owner = (strcmp(newroom, DEFAULT_ROOM) != 0 && !has_members);

    strncpy(clients[slot].room, newroom, ROOMSIZE - 1);
    clients[slot].room[ROOMSIZE - 1] = '\0';
    clients[slot].room_owner = became_owner;

    // 내가 이전 방의 방장이었다면, 방에 남은 구성원 중 한 명에게 방장을 승계해줌
    if (was_owner && strcmp(oldroom, DEFAULT_ROOM) != 0) {
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (clients[i].active && strcmp(clients[i].room, oldroom) == 0) {
                clients[i].room_owner = 1;
                strncpy(heir, clients[i].nick, NICKSIZE - 1);
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutx);

    clear_room_if_empty(oldroom); // 내가 이전 방의 마지막 사용자였다면 그 방의 공지와 대화 기록도 정리

    send_history(clnt_sock, newroom); // 새 방의 최근 대화부터 보여줌

    snprintf(notice, sizeof(notice), "[알림] %s님이 '%s' 방에 입장했습니다.%s\n",
             nick, newroom, became_owner ? " (방장)" : "");
    broadcast_room(newroom, notice, -1);

    // 새 방에 공지가 설정되어 있으면 입장 알림 바로 아래에 보여준다.
    char topic[TOPICSIZE];
    if (get_topic(newroom, topic, sizeof(topic))) {
        snprintf(notice, sizeof(notice), "[알림] 공지: %s\n", topic);
        send_to(clnt_sock, notice);
    }

    snprintf(notice, sizeof(notice), "[알림] %s님이 '%s' 방으로 이동했습니다.\n", nick, newroom);
    broadcast_room(oldroom, notice, -1);

    if (heir[0] != '\0') {
        snprintf(notice, sizeof(notice), "[알림] %s님이 새 방장이 되었습니다.\n", heir);
        broadcast_room(oldroom, notice, -1);
    }
}

// 완성된 한 줄을 명령어 또는 일반 채팅으로 처리
void handle_line(int clnt_sock, int slot, const char *nick, char *msg) {
    char sendbuf[BUFSIZE + NICKSIZE + 16];

    if (msg[0] == '\0')
        return;

    if (msg[0] == '/') { // 입력한 채팅이 명령어라면
        if (strncmp(msg, "/w ", 3) == 0) {
            char target[NICKSIZE] = {0,};
            char *content = strchr(msg + 3, ' ');

            if (content == NULL || *(content + 1) == '\0') {
                send_to(clnt_sock, "[알림] 사용법: /w 닉네임 내용\n");
                return;
            }
            int nick_len = content - (msg + 3);
            if (nick_len >= NICKSIZE) nick_len = NICKSIZE - 1;
            strncpy(target, msg + 3, nick_len);
            target[nick_len] = '\0';
            content++;

            if (strcmp(target, nick) == 0) {
                send_to(clnt_sock, "[알림] 자기 자신에게는 귓속말을 보낼 수 없습니다.\n");
                return;
            }
            char wcontent[BUFSIZE];
            convert_emoji(content, wcontent, sizeof(wcontent));
            if (!send_whisper(nick, target, wcontent, clnt_sock)) {
                snprintf(sendbuf, sizeof(sendbuf),
                         "[알림] '%s' 닉네임의 사용자를 찾을 수 없습니다.\n", target);
                send_to(clnt_sock, sendbuf);
            }
        }
        else if (strncmp(msg, "/join ", 6) == 0) { // 방 이동
            const char *newroom = msg + 6;
            if (strlen(newroom) == 0 || strchr(newroom, ' ') != NULL) {
                send_to(clnt_sock, "[알림] 사용법: /join 방이름 (공백 없이)\n");
                return;
            }
            if (strlen(newroom) >= ROOMSIZE) {
                send_to(clnt_sock, "[알림] 방 이름이 너무 깁니다.\n");
                return;
            }
            do_join(clnt_sock, slot, nick, newroom);
        }
        else if (strcmp(msg, "/join") == 0) { // 인자 없이 입력 시에는 현재 방을 안내
            char cur[ROOMSIZE];
            get_room(slot, cur, sizeof(cur));
            snprintf(sendbuf, sizeof(sendbuf),
                     "[알림] 현재 '%s' 방에 있습니다. 이동: /join 방이름\n", cur);
            send_to(clnt_sock, sendbuf);
        }
        else if (strcmp(msg, "/block") == 0) { // 내 차단 목록 확인
            pthread_mutex_lock(&mutx);
            if (clients[slot].n_blocked == 0) {
                pthread_mutex_unlock(&mutx);
                send_to(clnt_sock, "[알림] 차단한 사용자가 없습니다.\n");
                return;
            }
            char listmsg[MAX_BLOCK * NICKSIZE + 64];
            int off = snprintf(listmsg, sizeof(listmsg), "[알림] 차단 목록: ");
            for (int b = 0; b < clients[slot].n_blocked; b++)
                off += snprintf(listmsg + off, sizeof(listmsg) - off, "%s%s",
                                b > 0 ? ", " : "", clients[slot].blocked[b]);
            snprintf(listmsg + off, sizeof(listmsg) - off, "\n");
            pthread_mutex_unlock(&mutx);
            send_to(clnt_sock, listmsg);
        }
        else if (strncmp(msg, "/block ", 7) == 0) { // 차단
            const char *target = msg + 7;
            if (strcmp(target, nick) == 0) {
                send_to(clnt_sock, "[알림] 자기 자신은 차단할 수 없습니다.\n");
                return;
            }
            if (strlen(target) >= NICKSIZE) {
                send_to(clnt_sock, "[알림] 닉네임이 너무 깁니다.\n");
                return;
            }
            pthread_mutex_lock(&mutx);
            int dup = 0;
            for (int b = 0; b < clients[slot].n_blocked; b++)
                if (strcmp(clients[slot].blocked[b], target) == 0) { dup = 1; break; }
            if (dup) {
                pthread_mutex_unlock(&mutx);
                send_to(clnt_sock, "[알림] 이미 차단한 사용자입니다.\n");
                return;
            }
            if (clients[slot].n_blocked >= MAX_BLOCK) {
                pthread_mutex_unlock(&mutx);
                send_to(clnt_sock, "[알림] 차단 목록이 가득 찼습니다. (/unblock 으로 정리)\n");
                return;
            }
            strncpy(clients[slot].blocked[clients[slot].n_blocked], target, NICKSIZE - 1);
            clients[slot].blocked[clients[slot].n_blocked][NICKSIZE - 1] = '\0';
            clients[slot].n_blocked++;
            pthread_mutex_unlock(&mutx);
            snprintf(sendbuf, sizeof(sendbuf),
                     "[알림] '%s'님을 차단했습니다. 해제: /unblock %s\n", target, target);
            send_to(clnt_sock, sendbuf);
        }
        else if (strncmp(msg, "/unblock ", 9) == 0) { // 차단 해제
            const char *target = msg + 9;
            int found_b = 0;
            pthread_mutex_lock(&mutx);
            for (int b = 0; b < clients[slot].n_blocked; b++) {
                if (strcmp(clients[slot].blocked[b], target) == 0) {
                    clients[slot].n_blocked--;
                    strcpy(clients[slot].blocked[b],
                           clients[slot].blocked[clients[slot].n_blocked]);
                    found_b = 1;
                    break;
                }
            }
            pthread_mutex_unlock(&mutx);
            if (found_b) {
                snprintf(sendbuf, sizeof(sendbuf), "[알림] '%s'님 차단을 해제했습니다.\n", target);
                send_to(clnt_sock, sendbuf);
            } else {
                snprintf(sendbuf, sizeof(sendbuf), "[알림] '%s'님은 차단 목록에 없습니다.\n", target);
                send_to(clnt_sock, sendbuf);
            }
        }
        else if (strncmp(msg, "/kick ", 6) == 0) { // 강퇴
            const char *target = msg + 6;
            char myroom[ROOMSIZE];
            char tnick[NICKSIZE] = {0,};
            int tsock = -1, tslot = -1, err = 0; // err 1: 권한 없음, 2: 로비임, 3: 대상 없음, 4: 본인임

            pthread_mutex_lock(&mutx);
            strncpy(myroom, clients[slot].room, ROOMSIZE - 1);
            myroom[ROOMSIZE - 1] = '\0';
            if (strcmp(myroom, DEFAULT_ROOM) == 0)      err = 2;
            else if (!clients[slot].room_owner)          err = 1;
            else if (strcmp(target, nick) == 0)          err = 4;
            else {
                for (int i = 0; i < MAX_CLIENT; i++) {
                    if (clients[i].active && strcmp(clients[i].nick, target) == 0 &&
                        strcmp(clients[i].room, myroom) == 0) {
                        tslot = i; tsock = clients[i].sock;
                        strncpy(tnick, clients[i].nick, NICKSIZE - 1);
                        break;
                    }
                }
                if (tslot < 0) err = 3;
                else {
                    strncpy(clients[tslot].room, DEFAULT_ROOM, ROOMSIZE - 1);
                    clients[tslot].room[ROOMSIZE - 1] = '\0';
                    clients[tslot].room_owner = 0;
                }
            }
            pthread_mutex_unlock(&mutx);

            if (err == 2) { send_to(clnt_sock, "[알림] 로비에서는 강퇴할 수 없습니다.\n"); return; }
            if (err == 1) { send_to(clnt_sock, "[알림] 방장만 강퇴할 수 있습니다.\n"); return; }
            if (err == 4) { send_to(clnt_sock, "[알림] 자기 자신은 강퇴할 수 없습니다.\n"); return; }
            if (err == 3) {
                snprintf(sendbuf, sizeof(sendbuf),
                         "[알림] '%s'님은 이 방에 없습니다.\n", target);
                send_to(clnt_sock, sendbuf);
                return;
            }

            // 강퇴당한 본인에게 안내 + 로비 히스토리, 각 방에 알림
            snprintf(sendbuf, sizeof(sendbuf),
                     "[알림] 방장에 의해 '%s' 방에서 강퇴되어 로비로 이동되었습니다.\n", myroom);
            send_to(tsock, sendbuf);
            send_history(tsock, DEFAULT_ROOM);
            snprintf(sendbuf, sizeof(sendbuf), "[알림] %s님이 강퇴되었습니다.\n", tnick);
            broadcast_room(myroom, sendbuf, -1);
            snprintf(sendbuf, sizeof(sendbuf), "[알림] %s님이 입장했습니다.\n", tnick);
            broadcast_room(DEFAULT_ROOM, sendbuf, tsock);
        }
        else if (strcmp(msg, "/leave") == 0) { // 현재 방에서 나가 기본 방인 로비로 돌아갈 때
            // 방장 승계, 빈 방 공지 정리, 양쪽 방 알림까지 do_join이 다 처리해준다.
            // 이미 로비에 있는 상태로 입력하면 do_join의 같은 방 검사에 걸려 안내 메시지가 나간다.
            do_join(clnt_sock, slot, nick, DEFAULT_ROOM);
        }
        else if (strcmp(msg, "/rooms") == 0) {
            char roombuf[BUFSIZE * 2];
            build_room_list(roombuf, sizeof(roombuf));
            send_to(clnt_sock, roombuf);
        }
        else if (strncmp(msg, "/topic ", 7) == 0) {
            const char *text = msg + 7;
            char myroom[ROOMSIZE];
            int am_owner;

            pthread_mutex_lock(&mutx);
            strncpy(myroom, clients[slot].room, ROOMSIZE - 1);
            myroom[ROOMSIZE - 1] = '\0';
            am_owner = clients[slot].room_owner;
            pthread_mutex_unlock(&mutx);

            if (strcmp(myroom, DEFAULT_ROOM) == 0) {
                send_to(clnt_sock, "[알림] 로비에는 공지를 설정할 수 없습니다.\n");
                return;
            }
            if (!am_owner) {
                send_to(clnt_sock, "[알림] 방장만 공지를 설정할 수 있습니다.\n");
                return;
            }
            if (strlen(text) >= TOPICSIZE) {
                send_to(clnt_sock, "[알림] 공지가 너무 깁니다.\n");
                return;
            }
            if (!set_topic(myroom, text)) {
                send_to(clnt_sock, "[알림] 더 이상 공지를 저장할 공간이 없습니다.\n");
                return;
            }
            
            snprintf(sendbuf, sizeof(sendbuf), "[알림] 방장이 공지를 설정했습니다: %s\n", text);
            broadcast_room(myroom, sendbuf, -1);
        }
        else if (strcmp(msg, "/topic") == 0) {
            char myroom[ROOMSIZE];
            char topic[TOPICSIZE];
            get_room(slot, myroom, sizeof(myroom));
            if (get_topic(myroom, topic, sizeof(topic))) {
                snprintf(sendbuf, sizeof(sendbuf), "[알림] '%s' 방 공지: %s\n", myroom, topic);
                send_to(clnt_sock, sendbuf);
            } else {
                send_to(clnt_sock, "[알림] 이 방에는 설정된 공지가 없습니다. (설정: /topic 내용, 방장만 가능)\n");
            }
        }
        else if (strcmp(msg, "/list") == 0) {
            char listbuf[BUFSIZE * 2];
            build_user_list(listbuf, sizeof(listbuf), nick);
            send_to(clnt_sock, listbuf);
        }
        else {
            send_to(clnt_sock, "[알림] 알 수 없는 명령어입니다. (/w, /list, /rooms, /join, /leave, /topic, /block, /unblock, /kick)\n");
        }
        return;
    }

    // 일반 채팅: 채팅 발신자 식별은 서버가 담당
    // 이모티콘 코드를 이모지로 치환한 뒤, broadcast_chat이 발신자의 방을 찾아 같은 방에만 전송하고,
    // 그 방 이름을 돌려주면 해당 방의 전용 히스토리 버퍼에 저장한다.
    char room[ROOMSIZE];
    char ts[24];
    char converted[BUFSIZE];
    convert_emoji(msg, converted, sizeof(converted));
    broadcast_chat(nick, converted, clnt_sock, room, sizeof(room));
    timestamp(ts, sizeof(ts));
    snprintf(sendbuf, sizeof(sendbuf), "%s %s: %s\n", ts, nick, converted);
    add_history(room, nick, sendbuf);
}

// 클라이언트 1명 담당 스레드: 닉네임 등록 -> 채팅/명령어 처리 -> 퇴장 정리
void* handle_clnt(void *arg) {
    int clnt_sock = *((int *)arg);
    free(arg);

    char msg[BUFSIZE] = {0,};
    char sendbuf[BUFSIZE + NICKSIZE + 16];
    char nick[NICKSIZE] = {0,};
    int str_len, slot;

    char acc[BUFSIZE * 2] = {0,};
    int acc_len = 0;

    // 닉네임 등록: 첫 번째 완성된 줄이 'JOIN|닉네임'이어야 한다.
    while (strchr(acc, '\n') == NULL) {
        str_len = read(clnt_sock, msg, sizeof(msg) - 1);
        if (str_len <= 0 || acc_len + str_len >= (int)sizeof(acc)) {
            close(clnt_sock);
            return NULL;
        }
        msg[str_len] = '\0';
        memcpy(acc + acc_len, msg, str_len + 1);
        acc_len += str_len;
    }

    char *nl = strchr(acc, '\n');
    *nl = '\0';

    if (strncmp(acc, "JOIN|", 5) != 0 || strlen(acc + 5) == 0) {
        send_to(clnt_sock, "[알림] 잘못된 접속 요청입니다.\n");
        close(clnt_sock);
        return NULL;
    }
    strncpy(nick, acc + 5, NICKSIZE - 1);

    acc_len = acc_len - (int)(nl + 1 - acc);
    memmove(acc, nl + 1, acc_len + 1);

    slot = register_client(clnt_sock, nick);
    if (slot == -1) {
        send_to(clnt_sock, "[알림] 이미 사용 중인 닉네임입니다. 다른 닉네임으로 다시 접속해주세요.\n");
        close(clnt_sock);
        return NULL;
    }
    if (slot == -2) {
        send_to(clnt_sock, "[알림] 채팅방 정원이 가득 찼습니다.\n");
        close(clnt_sock);
        return NULL;
    }

    // 로비의 최근 대화 리플레이
    send_history(clnt_sock, DEFAULT_ROOM);

    // 그 뒤 입장 알림 (같은 방 사람들에게만 알려줌)
    snprintf(sendbuf, sizeof(sendbuf), "[알림] %s님이 입장했습니다.\n", nick);
    printf("%s", sendbuf); fflush(stdout);
    broadcast_room(DEFAULT_ROOM, sendbuf, -1);

    // 채팅/명령어 처리
    str_len = 0;
    do {
        if (str_len > 0) {
            msg[str_len] = '\0';
            if (acc_len + str_len >= (int)sizeof(acc))
                acc_len = 0;
            memcpy(acc + acc_len, msg, str_len + 1);
            acc_len += str_len;
        }

        char *start = acc;
        while ((nl = strchr(start, '\n')) != NULL) {
            *nl = '\0';
            handle_line(clnt_sock, slot, nick, start);
            start = nl + 1;
        }
        acc_len = strlen(start);
        memmove(acc, start, acc_len + 1);
    } while ((str_len = read(clnt_sock, msg, sizeof(msg) - 1)) > 0);

    // 퇴장 정리와 알림
    char lastroom[ROOMSIZE];
    char heir[NICKSIZE] = {0,};
    pthread_mutex_lock(&mutx);
    strncpy(lastroom, clients[slot].room, ROOMSIZE - 1);
    lastroom[ROOMSIZE - 1] = '\0';
    int was_owner = clients[slot].room_owner;
    clients[slot].active = 0;
    if (was_owner && strcmp(lastroom, DEFAULT_ROOM) != 0) {
        for (int i = 0; i < MAX_CLIENT; i++) {
            if (clients[i].active && strcmp(clients[i].room, lastroom) == 0) {
                clients[i].room_owner = 1;
                strncpy(heir, clients[i].nick, NICKSIZE - 1);
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutx);

    clear_room_if_empty(lastroom);

    snprintf(sendbuf, sizeof(sendbuf), "[알림] %s님이 퇴장했습니다.\n", nick);
    printf("%s", sendbuf); fflush(stdout);
    broadcast_room(lastroom, sendbuf, -1);
    if (heir[0] != '\0') {
        snprintf(sendbuf, sizeof(sendbuf), "[알림] %s님이 새 방장이 되었습니다.\n", heir);
        broadcast_room(lastroom, sendbuf, -1);
    }

    close(clnt_sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    pthread_t t_id;

    struct sockaddr_in serv_addr, clnt_addr;
    unsigned int clnt_addr_size;

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutx, NULL);
    memset(clients, 0, sizeof(clients));

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    // 서버 재시작 시 TIME_WAIT 포트 점유로 bind()가 실패하는 문제 방지
    int option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind() error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen() error");

    printf("채팅 서버가 포트 %s에서 시작되었습니다.\n", argv[1]);

    clnt_addr_size = sizeof(clnt_addr);

    while (1) {
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept() error");

        int *sock_ptr = (int *)malloc(sizeof(int));
        *sock_ptr = clnt_sock;

        pthread_create(&t_id, NULL, handle_clnt, (void *)sock_ptr);
        pthread_detach(t_id);
    }

    return 0;
}
