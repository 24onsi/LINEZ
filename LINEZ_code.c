#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

// 맵 구조체
struct line                 // 행만들기
{
    char row[52];
};
struct map                  // 맵을 위한 행열 만들기
{
    struct line col[52];
};

// 위치 구조체             // 완료                   
struct location
{
    int floor;
    int y;
    int x;
};

// 장비 구조체              // 완료  - 수정
struct equip
{
    char name[19];
    int part;
    int tier;
    int enchantment;
    double power;
    int have;
};
struct equipments
{
    struct equip e[6][5];
};

// 아이템 구조체            // 완료
struct item
{
    int element;
    int have;
    char name[22];
    double power;
};
struct items
{
    struct item item[8];
};
struct inventory            // 완료 -- 수정
{
    struct item item[8];
    struct equip equip[6][5][11];
};

// 유저 정보 구조체         // 완료  -- 수정
struct user
{
    struct locations                  // 8개의 좌표. (floor, y, x)
    {
        struct location now;
        struct location place1;
        struct location place2;
        struct location place3;
        struct location place4;
        struct location place5;
        struct location place6;
        struct location place7;
    } fyx;
    char name[14];                           // 유저 이름(용사복이)
    double mhp;                              // 최대체력
    double hp;                               // 현재체력
    int gold;                                // 소지금
    struct equip eqt[6];                   // 장비창.장비부위.(부위, 등급, 인챈트)
    struct inventory inven;                          // 요소값, 소유량, 이름, 효과
    int flag[3];
};

// 적 정보 구조체
struct d_e
{
    int drop;
};
struct enemy
{
    char name[13];                           // 적 이름
    double mhp;                              // 최대체력
    double minhp;
    double atk_min;                          // 최소공격력
    double atk_max;                          // 최대공격력
    int gold_min;                                // 최소드랍골드
    int gold_max;                               // 최대드랍골드
    struct d_e drop_e[5];                         // 장비[부위][등급(0-4),드랍확률]
    int item[8];                          // 소비 아이템[종류(0-3: 물약, 4: 마을이동주문서, 5: 장비강화주문서, 6:순간이동주문서), 7: 엘릭서][드랍여부0 이면x, 100이면 o]
    double hpup;                             // 죽였을 경우 증가하는 체력 상승률
};
struct enemys
{
    struct enemy normal[5];
    struct enemy named;
    struct enemy boss_b[3];
};

void init_equip(struct equipments *init);                                                               // 장비 초기화
void init_item(struct items * init);                                                                    // 아이템 초기화
void init_user(struct user *init1, struct items init2, struct equipments init3);                        // 유저 초기화
void init_enemy_named(struct enemy *init1, struct user init2);                                          // 네임드 몬스터 초기화
void make_vil(struct map *vil1, struct user info);                                                      // 마을맵 전방선언
void show_vil(struct map vil1, struct user info);                                                       // 마을맵 출력
void mop_gen(struct map *m, struct user info);                                                          // 몹 리젠
void make_dungeon(struct map *m, struct user info);                                                     // 맵 만드는 함수
void show_dungeon(struct map *m, struct user info);                                                      // 맵 출력함수
void smelter(struct user *init1);                                                                    // 제련소
void normal(struct enemys *init1, struct user info);                                                    // 일반몬스터
void sanctum(struct user *effet);                                                                       // 성소
void init_enemys(struct enemys *init0, struct user info);                                               // 몬스터 전체 초기화
void pandora(struct items i, struct equipments e, struct user *u);                                      // 상점(판도라)
char movement(void);                                                                                    // 이동 키 입력 함수
char replace_movement(char move, struct map *map, struct user *user, struct items i, struct equipments e);  // 입력 키 처리 함수 및 만난 몬스터 리턴
int judge_monster (char M);                                                                                 // 입력 키 처리 함수에서 만난 몬스터 판단
void up_and_down(struct user *info);                                                                        // 던전 입구와 출구 시 유저 정보 변경
void remember_fyx(struct user *info);                                                                   // 순간이동 스크롤을 위한 위치 저장 함수                                                               
int normal_battle(struct enemys info,struct user *init1, int result, char jm);                         // 전투 일반몬스터
void normal_vic_reward(struct enemys info, struct user *init1, char jm);                               // 전투 일반몬스터 보상
void init_boss (struct enemys *init, struct user info);                                                     // 보스 몬스터 초기화
int boss_battle(struct enemys info,struct user *init1, int result, char jm);                               // 전투 보스몬스터
void boss_vic_reward(struct enemys info, struct user *init1, char jm);                                     // 보스 몬스터 보상
void elixir(struct user *init1, struct equipments info);                                                    // 엘릭서
void enchant(struct user *init1, struct equipments info);                                                   // 인첸트
int count_have(struct user *info);                                                                         // 가진 장비 숫자를 파악하는 함수
void dis_equip(struct user *info, struct equipments e);                                                   // 장비창 함수
int battle_logic(struct user * user, struct enemys enemys, char jm, int battle_result);                     // 전투로직(종합,보상)
int named_battle(struct enemys info, struct user *init1, int result, char jm);                              // named battle
void named_vic_reward(struct enemys info, struct user *init1, char jm);                                     // named reward
void opening ();
void ending ();


// 메인 함수
int main(void)
{
    char move;                              // 이동 변수
    char jm = 'T';                         // 몬스터 확인 변수
    int battle_result = 0;

    srand(time(NULL));                      //랜덤 랜덤하게 나오게 
                                                                            // 장비 선언 및 초기화
    struct equipments iequip;               // 장비 선언
    init_equip(&iequip);                    // 장비 초기화
                                                                            // 소모아이템 선언 및 초기화
    struct items iitems;                    // 아이템 선언
    init_item(&iitems);                     // 아이템 초기화
                                                                            // 유저 선언 및 초기화
    struct user bok;                        // 유저 선언
    init_user(&bok, iitems, iequip);        // 유저 초기화
                                                                            // 몬스터 선언 및 초기화
    struct enemys enemys;                       // 적 전체 선언
    init_enemys(&enemys, bok);                  // 적 전체 초기화
                                                                            // 마을 맵 생성
    struct map vilige;                           // 마을 선언
    struct map map;                              // 던전맵 선언
    make_vil(&vilige, bok);                      // 마을맵 생성

    // opening ();
    while(1)
    {
        init_enemys(&enemys, bok);                  // 적 전체 초기화
        if (bok.fyx.now.y == 50 && bok.fyx.now.x == 50)                     // 출구(50, 50)에 도달하였을 때,
        {
            if (bok.fyx.now.floor != 5)                                     // 단, 5층이 아니면 다음을 실행하라
            {
                up_and_down(&bok);          // 층수, 좌표 변동
                make_dungeon(&map, bok);    // 층수 맵 출력
                mop_gen(&map, bok);         // 몬스터 생성
            }
        }
        else if (bok.fyx.now.y == 1 && bok.fyx.now.x == 1)                      // 입구(1,1)에 도달하였을 때,
        {
            if(bok.fyx.now.floor != 0)                                          // 단, 마을이 아니고
            {
                if (bok.fyx.now.floor == 1)                                     // 1층 던전이면 다음을 실행하라
                {
                    up_and_down(&bok);          // 층수, 좌표 변동
                    make_vil(&vilige, bok);     //마을맵 생성
                }
                else                                                            // 그 외(던전2~5)면 다음을 실행하라
                {
                    up_and_down(&bok);          // 층수, 좌표 변동
                    make_dungeon(&map, bok);    // 층수 맵 출력
                    mop_gen(&map, bok);         // 몬스터 생성
                }
            }
        }

        // 이동키 입력 및 입력키 판단 후 처리, 만난 몬스터 판단
        if (bok.fyx.now.floor != 0)                                                         // 던전이면 다음을 실행하라.
        {
            show_dungeon(&map, bok);                                            // 던전 출력  
            move = movement();                                                 // 방향키 입력
            jm = replace_movement(move, &map, &bok, iitems, iequip);           // 방향키를 처리하는 함수, 만나는 몬스터를 반환 - 인벤토리 함수 추가 필요
            show_dungeon(&map, bok);                                            // 던전 출력  
        }
        else                                                                                // 마을이면 다음을 실행하라.
        {
            make_vil(&vilige, bok);     //마을맵 메인
            show_vil(vilige, bok);                                              // 마을 출력
            move = movement();                                                  // 방향키 입력
            jm = replace_movement(move, &vilige, &bok, iitems, iequip);         // 방향키를 처리하는 함수, 만나는 몬스터를 반환 - 인벤토리 함수 추가 필요
            show_vil(vilige, bok);                                              // 마을 출력
        }
        // 1-승리 2- 패배 3- 도망 4- 짠막보스 승리
        battle_logic(&bok, enemys, jm, battle_result);
    }
    return 0;
}


// 장비창 초기화 및 표시
void init_equip(struct equipments *init)
{
    char equipname[6][5][19] = {{{"맨손"}, {"기본검"}, {"장검"}, {"일본도"}, {"싸울아비장검"}}, 
                                  {{"맨몸"}, {"기본갑빠"}, {"반팔갑빠"}, {"후드갑빠"}, {"용갑빠"}}, 
                                  {{"맨발"}, {"기본장화"}, {"슬리퍼"}, {"운동화"}, {"에어조단"}}, 
                                  {{"빈손"}, {"기본장갑"}, {"고무장갑"}, {"면장갑"}, {"가죽장갑"}},
                                  {{"빈등"}, {"기본망토"}, {"면망토"}, {"비단망토"}, {"방탄망토"}},
                                  {{"맨얼굴"}, {"기본마스크"}, {"k80마스크"}, {"k94마스크"}, {"타이거마스크"}}};
    double equippower[6][5] = {{0, 2, 5, 10, 20},
                                {0, -1, -4, -8, -13},
                                {0, -1, -4, -8, -13},
                                {0, -1, -4, -8, -13},
                                {0, -1, -4, -8, -13},
                                {0, -1, -4, -8, -13}};
    for (int j = 0; j < 6; j++)
    {
        for (int i = 0; i < 5; i++)
        {
            strcpy(init->e[j][i].name, equipname[j][i]);
            init->e[j][i].part = j;
            init->e[j][i].tier = i;
            init->e[j][i].enchantment = 0;
            init->e[j][i].power = *(equippower[j]+i);
        }   
    }
}

// 아이템 초기화 및 표시
void init_item(struct items * init)
{
    char itemname[8][22] = {{"빨간물약"}, {"주항물약"}, {"맑은물약"}, {"고농도물약"}, {"마을이동주문서"}, {"장비강화주문서"}, {"순간이동주문서"}, {"엘릭서"}};
    double itempower[8] ={30, 50, 70, 150, 1, 20, 1, 100}; 
    for (int j = 0; j < 8; j++)
    {
        strcpy(init->item[j].name, *(itemname+j));
        init->item[j].element = j;
        init->item[j].have = 0;
        init->item[j].power = itempower[j];
    }
}

// 유저 초기화 및 표시              /* need to change before report it */
void init_user(struct user *init1, struct items init2, struct equipments init3)              // 유저 초기화
{

    init1->fyx.now.floor = 0;
    init1->fyx.now.y = 1;
    init1->fyx.now.x = 1;
    init1->fyx.place1.floor = 0;
    init1->fyx.place1.y = 1;
    init1->fyx.place1.x = 1;
    init1->fyx.place2.floor = 0;
    init1->fyx.place2.y = 1;
    init1->fyx.place2.x = 1;
    init1->fyx.place3.floor = 0;
    init1->fyx.place3.y = 1;
    init1->fyx.place3.x = 1;
    init1->fyx.place4.floor = 0;
    init1->fyx.place4.y = 1;
    init1->fyx.place4.x = 1;
    init1->fyx.place5.floor = 0;
    init1->fyx.place5.y = 1;
    init1->fyx.place5.x = 1;
    init1->fyx.place6.floor = 0;
    init1->fyx.place6.y = 1;
    init1->fyx.place6.x = 1;
    init1->fyx.place7.floor = 0;
    init1->fyx.place7.y = 1;
    init1->fyx.place7.x = 1;
    init1->mhp = 100;
    init1->hp = 100;
    init1->gold = 300;
    init1->flag[0] = 0;
    init1->flag[1] = 0;
    init1->flag[2] = 0;
    
    for (int j = 0; j < 8; j++)
    {
        strcpy(init1->inven.item[j].name, init2.item[j].name);
        init1->inven.item[j].element = init2.item[j].element;
        // if (j == 0)
        // {
            init1->inven.item[j].have = 3;

    //     }
    //     else
    //     {
    //     init1->inven.item[j].have = init2.item[j].have;              /* real code revive it */
    //     }
    //     init1->inven.item[j].power = init2.item[j].power;
    }

    for (int j = 0; j< 6; j++)
    {
        strcpy(init1->eqt[j].name, init3.e[j][1].name);
        init1->eqt[j].enchantment = init3.e[j][1].enchantment;
        init1->eqt[j].part = init3.e[j][1].part;
        init1->eqt[j].tier = 1;
        init1->eqt[j].power = init3.e[j][1].power;
    }
    init1->eqt[0].power = 9999;
    for (int p = 0; p < 11; p++)
    {
        for(int k = 0; k < 6; k++)
        {
            for(int l = 0; l < 5; l++)
            {
                init1->inven.equip[k][l][p].enchantment = p;
                strcpy(init1->inven.equip[k][l][p].name, init3.e[k][l].name);
                init1->inven.equip[k][l][p].part = k;
                init1->inven.equip[k][l][p].power = init3.e[k][l].power;
                init1->inven.equip[k][l][p].tier = l;
                init1->inven.equip[k][l][p].have = 0;
            }
        }
    }
}

// 네임드 몬스터 초기화 및 표시
void init_enemy_named(struct enemy *init1, struct user init2)
{
    char name [27][10] = 
    {
    {"유시온"},{"김승수"},{"권철민"},{"안광민"},{"강진영"},
    {"김영곤"},{"박선후"},{"김건"},{"이준호"},{"이철"},
    {"이동준"},{"황은비"},{"조세빈"},{"김성근"},{"이은승"},
    {"박희정"},{"박장미"},{"김민아"},{"조대정"},{"김재신"},
    {"박민건"},{"임석현"},{"황운하"},{"노주영"},{"김혜빈"},
    {"서훈"},{"오은지"}
    };
    int a = rand()%27;
    strcpy(init1->name, name[a]);
    init1->mhp = 2 * init2.mhp;
    init1->atk_min = 100;
    init1->atk_max = 300;       // 전투로직에서 랜덤값으로 정해야함. 사실상 의미는 입력(도감만들면 쓸지도?).
    for(int j = 0; j < 8; j++)
    {
        if (j == 6)
        {
            init1->item[j] = 30;        // 순간이동주문서드랍율, 1~3개 드랍은 따로 설정해야함. 현재는 30퍼센트만 적어놓음
        }
        else
        {
            init1->item[j] = 0;
        }
    }
    
    init1->gold_min = 5;
    init1->gold_max = 500;
    init1->hpup = 1.20;
    for(int i = 0; i < 5; i++)
    {
        if(i == 2 || i == 3)
        {
            init1->drop_e[i].drop = 20;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else
        {
            init1->drop_e[i].drop = 0;
        }
    }
}

// 보스(바포메트) 초기화 및 표시
void init_enemy_boss(struct enemy *init1, struct user init2)                               // 보스(바포케트) 초기화
{
    strcpy(init1->name, "바포메트");
    init1->mhp = 5 * init2.mhp;
    init1->atk_min = 180;
    init1->atk_max = 450;       // 전투로직에서 랜덤값으로 정해야함. 사실상 의미는 입력(도감만들면 쓸지도?).
    for(int j = 0; j < 8; j++)
    {
        if (j == 6)
        {
            init1->item[j] = 30;        // 순간이동주문서드랍율, 1~3개 드랍은 따로 설정해야함. 현재는 30퍼센트만 적어놓음
        }
        else
        {
            init1->item[j] = 0;
        }
    }
    
    init1->gold_min = 5;
    init1->gold_max = 700;
    init1->hpup = 30;
    for(int i = 0; i < 5; i++)
    {
        if(i == 3)
        {
            init1->drop_e[i].drop = 20;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else if(i == 4)
        {
            init1->drop_e[i].drop = 5;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else
        {
            init1->drop_e[i].drop = 0;
        }
    }
}

// 찐보스(이동녘) 초기화
void init_enemy_realboss(struct enemy *init1, struct user init2)                               // 찐보스(이동녘) 초기화
{
    strcpy(init1->name, "이동녘");
    init1->mhp = 7 * init2.mhp;
    init1->atk_min = 300;
    init1->atk_max = 550;       // 전투로직에서 랜덤값으로 정해야함. 사실상 의미는 입력(도감만들면 쓸지도?).
    for(int j = 0; j < 8; j++)
    {
        if (j == 6)
        {
            init1->item[j] = 30;        // 순간이동주문서드랍율, 1~3개 드랍은 따로 설정해야함. 현재는 30퍼센트만 적어놓음
        }
        else if(j == 7)
        {
            init1->item[j] = 10;
        }
        else
        {
            init1->item[j] = 0;
        }
    }
    
    init1->gold_min = 5;
    init1->gold_max = 1000;
    init1->hpup = 60;
    for(int i = 0; i < 5; i++)
    {
        if(i == 3)
        {
            init1->drop_e[i].drop = 20;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else if(i == 4)
        {
            init1->drop_e[i].drop = 10;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else
        {
            init1->drop_e[i].drop = 0;
        }
    }
}

// 찐막보스(류홍걸) 초기화
void init_enemy_ultboss(struct enemy *init1, struct user init2)                               // 찐막보스(류홍걸) 초기화
{
    strcpy(init1->name, "류홍걸");
    init1->mhp = 10 * init2.mhp;
    init1->atk_min = 500;
    init1->atk_max = 1300;       // 전투로직에서 랜덤값으로 정해야함. 사실상 의미는 입력(도감만들면 쓸지도?).
    for(int j = 0; j < 8; j++)
    {
        if (j == 6)
        {
            init1->item[j] = 30;        // 순간이동주문서드랍율, 1~3개 드랍은 따로 설정해야함. 현재는 30퍼센트만 적어놓음
        }
        else if(j == 7)
        {
            init1->item[j] = 20;
        }
        else
        {
            init1->item[j] = 0;
        }
    }
    
    init1->gold_min = 5;
    init1->gold_max = 3000;
    init1->hpup = 100;
    for(int i = 0; i < 5; i++)
    {
        if(i == 3)
        {
            init1->drop_e[i].drop = 30;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else if(i == 4)
        {
            init1->drop_e[i].drop = 20;     // 아이템 드랍율 j부위의 i등급을 확률만큼 드랍
        }
        else
        {
            init1->drop_e[i].drop = 0;
        }
    }
}

// 마을 생성 함수
void make_vil(struct map *vil1, struct user info)
{
    for (int i = 0; i < 52; i++)
    {
        for (int j = 0; j < 52; j++)
        {
            if (i == 0 || i == 51 || j == 0 || j == 51)
                vil1->col[i].row[j] = 'i';      //벽
            else if (i == 50 && j == 50)
                vil1->col[i].row[j] = '#';       //던전 포탈 밑에 함수
            else if (i == 10 && j == 25)
                vil1->col[i].row[j] = '$';       
            else if (i == 6 && j > 20 && j < 30)    //판도라 벽
                vil1->col[i].row[j] = 'i';
            else if (i > 5 && i < 15 && j == 21)
                vil1->col[i].row[j] = 'i';
            else if (i > 5 && i < 15 && j == 29)
                vil1->col[i].row[j] = 'i';
            else if (i == 25 && j == 25)
                vil1->col[i].row[j] = '^';       //성소 밑에 함수
                
            else if (i == 21 && j > 20 && j < 30)    //성소 벽
                vil1->col[i].row[j] = 'i';
            else if (i > 20 && i < 30 && j == 21)
                vil1->col[i].row[j] = 'i';
            else if (i > 20 && i < 30 && j == 29)
                vil1->col[i].row[j] = 'i';
            else if (i == 40 && j == 25)
                vil1->col[i].row[j] = '%';       //드워프 제련소 밑에 함수
            else if (i == 36 && j > 20 && j < 30)    //제련소 벽
                vil1->col[i].row[j] = 'i';
            else if (i > 35 && i < 45 && j == 21)
                vil1->col[i].row[j] = 'i';
            else if (i > 35 && i < 45 && j == 29)
                vil1->col[i].row[j] = 'i';
            else
                vil1->col[i].row[j] = 'o';      // 공백
            if((i == info.fyx.now.y) && (j == info.fyx.now.x))
                vil1->col[i].row[j] = 'p';
        }
    }
}
// 마을 출력 함수
void show_vil(struct map vil1, struct user info)        //마을맵 함수
{
    int i = 0;
    int j = 0;
    for (i = 0; i < 52 ; i++)
    {
        for(j = 0; j < 52; j++)
        {
            if (vil1.col[i].row[j] == 'i')
                printf("1");   
            else if(vil1.col[i].row[j] == 'p')
                printf("P");
            else if(vil1.col[i].row[j] =='#')
                printf("#");
            else if(vil1.col[i].row[j] =='$')
                printf("$");
            else if(vil1.col[i].row[j] =='^')
                printf("^");
            else if(vil1.col[i].row[j] =='%')
                printf("%%");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

// 던전에 몬스터 생성                                   
void mop_gen(struct map *m, struct user info)           // 몬스터 생성 확률 /* 검토 필요*/
{
    srand(time(NULL));
    int x, y, a, z;
    int b = info.fyx.now.floor;
    int pnamed = 30;
    int named = 1;
    int pbarpo = 20;
    int barpo = 1;
    int plee = 10;
    int lee = 1;
    int pryu = 5;
    int ryu = 1;

    if ((info.fyx.now.floor >= 1) && (info.fyx.now.floor <= 4))
    {
        for(a = 0; a < 10; a++)
        {
            while (1)
            {
                x = (rand() % 50) + 1;
                y = (rand() % 50) + 1;
                if(info.fyx.now.x != x && info.fyx.now.y != y)
                {
                    if (m->col[y].row[x] == 'o')
                    {
                        m->col[y].row[x] = 85 + b;
                        break;
                    }
                }
            }
            while (named == 1)
            {
                z = (rand()% 100) +1;
                if (z < pnamed)
                {
                    x = (rand() % 50) + 1;
                    y = (rand() % 50) + 1;
                    if(info.fyx.now.x != x && info.fyx.now.y != y)
                    {
                        if (m->col[y].row[x] == 'o')
                        {
                            m->col[y].row[x] = '!';
                            named--;
                            break;
                        }
                    }
                }
                break;
            }
            
        }
    }
    if (info.fyx.now.floor == 5)
    {
        for(a = 0; a < 10; a++)
        {
            while (1)
            {
                x = (rand() % 50) + 1;
                y = (rand() % 50) + 1;
                if(info.fyx.now.x != x && info.fyx.now.y != y)
                {
                    if (m->col[y].row[x] == 'o')
                    {
                        m->col[y].row[x] = 'Z';
                        break;
                    }
                }
            }
            while (named == 1)
            {
                z = (rand()% 100) +1;
            if (z < pnamed)
                {
                    x = (rand() % 50) + 1;
                    y = (rand() % 50) + 1;
                    if(info.fyx.now.x != x && info.fyx.now.y != y)
                    {
                        if (m->col[y].row[x] == 'o')
                        {
                            m->col[y].row[x] = '!';
                            named--;
                            break;
                        }
                    }
                }
                break;
            }
            while (barpo == 1)
            {
                z = (rand()%100) +1;
                if (z < pbarpo)
                {
                    x = (rand() % 50) + 1;
                    y = (rand() % 50) + 1;
                    if(info.fyx.now.x != x && info.fyx.now.y != y)
                        {
                            if (m->col[y].row[x] == 'o')
                            {
                                m->col[y].row[x] = 'B';
                                barpo--;
                                break;
                            }
                        }                    
                }
                break;
            }
            while (lee == 1)
            {
                z = (rand()%100) +1;
                if (z < plee)
                {
                    x = (rand() % 50) + 1;
                    y = (rand() % 50) + 1;
                    if(info.fyx.now.x != x && info.fyx.now.y != y)
                        {
                            if (m->col[y].row[x] == 'o')
                            {
                                m->col[y].row[x] = 'L';
                                lee--;
                                break;
                            }
                        }                    
                }
            break;    
            }
            while (ryu == 1)
            {
                z = (rand()%100) +1;
                if (z < pryu)
                {
                    x = (rand() % 50) + 1;
                    y = (rand() % 50) + 1;
                    if(info.fyx.now.x != x && info.fyx.now.y != y)
                        {
                            if (m->col[y].row[x] == 'o')
                            {
                                m->col[y].row[x] = 'R';
                                ryu--;
                                break;
                            }
                        }                    
                }
            break;
            }
        }
    }
}

// 던전 생성
void make_dungeon(struct map *m, struct user info)
{
    int i = 0;
    int j = 0;
    int u_y = info.fyx.now.y;
    int u_x = info.fyx.now.x;
    if (info.fyx.now.floor == 5)
    {
        for(i=0; i < 52; i++)
        {
            for(j = 0; j < 52; j++)
            {
                if (i == 0 || i == 51 || j == 0 || j == 51)
                {
                    m->col[i].row[j] = 'i';
                }
                else if((i == 1) && (j == 1))
                {
                    m->col[i].row[j] = '#';
                }
                else
                {
                    m->col[i].row[j] = 'o';
                }
                if((i == u_y) && (j == u_x))
                {
                    m->col[i].row[j] = 'p';
                }
            }
        }                
    }
    else
    {
        for(i=0; i < 52; i++)
        {
            for(j = 0; j < 52; j++)
            {
                if (i == 0 || i == 51 || j == 0 || j == 51)
                {
                    m->col[i].row[j] = 'i';
                }
                else if((i == 1) && (j == 1))
                {
                    m->col[i].row[j] = '#';
                }
                else if((i == 50) && (j == 50))
                {
                    m->col[i].row[j] = '#';
                }
                else
                {
                    m->col[i].row[j] = 'o';
                }
                if((i == u_y) && (j == u_x))
                {
                    m->col[i].row[j] = 'p';
                }
            }
        }
    }
}
// 던전 출력
void show_dungeon(struct map *m, struct user info)
{
    int i = 0;
    int j = 0;
    printf("던전 %d층\n", info.fyx.now.floor);

    if(info.fyx.now.floor == 5)
    {
        if ((info.fyx.now.y != 1) || (info.fyx.now.x != 1))
        {
            m->col[1].row[1] = '#';
        }
    }
    else if(info.fyx.now.floor == 0)
    {
        
    }
    else
    {
        if ((info.fyx.now.y != 1) || (info.fyx.now.x != 1))
        {
            m->col[1].row[1] = '#';
        }
        if ((info.fyx.now.y != 50) || (info.fyx.now.x != 50))
        {
            m->col[50].row[50] = '#';
        }
    }
    for (i = 0; i < 52 ; i++)
    {
        for(j = 0; j < 52; j++)
        {
            if (m->col[i].row[j] == 'i')
            {
                printf("1");    
            }
            else if(m->col[i].row[j] == 'p')
            {
                printf("P");
            }
            else if(m->col[i].row[j] =='#')
            {
                printf("#");
            }
            else if(m->col[i].row[j] == 'V')
            {
                printf("V");
            }
            else if(m->col[i].row[j] == 'W')
            {
                printf("W");
            }
            else if(m->col[i].row[j] == 'X')
            {
                printf("X");
            }
            else if(m->col[i].row[j] == 'Y')
            {
                printf("Y");
            }
            else if(m->col[i].row[j] == 'Z')
            {
                printf("Z");
            }
            else if(m->col[i].row[j] == 'B')
            {
                printf("B");
            }
            else if(m->col[i].row[j] == 'L')
            {
                printf("L");
            }
            else if(m->col[i].row[j] == 'R')
            {
                printf("R");
            }
            else if(m->col[i].row[j] == '!')
            {
                printf("!");
            }
            else
            {
                printf(" ");
            }
        }
        putchar('\n');
    }
    putchar('\n');
}
// 제련소
void smelter(struct user *init1)
{
    int choice;
    printf("스멜트 : 어서오세요. 제련소 입니다.\n         강화주문서 구매하시겠습니까?\n");
    printf("\n구매를 원하는 수량을 입력해주세요.(개 당 100) \n원하지 않으면 '0'을 입력해주세요.\n");
    printf("소지금 : %d 골드\n",init1->gold);
    scanf(" %d",&choice);
    if (choice > 0)
    {
        if((init1->gold)>=(choice*100))
        {
            init1->inven.item[5].have += choice;
            init1->gold -= (choice*100);
        }
        else
            printf("스멜트 : 금액이 부족합니다.\n         다음에 다시 방문해주세요.\n");
    }
    else
        printf("스멜트 : 안녕히가세요.\n");
}

// 성소
void sanctum(struct user *effet)
{
    char npc_priest[10] = {"성직자"};

    printf("성소에 입장했습니다.\n");
    printf("현재 체력: %.0lf\n", effet->hp);
    printf("%s: 용사 복이 님의 체력을 회복합니다.\n", npc_priest);

    int max;
    max = effet->mhp;
    effet->hp = max;   
    printf("%s: 최대 체력인 %.0lf까지 체력이 모두 회복 되었습니다.\n ", npc_priest,effet->hp);
    getchar();
}

// 노말 몬스터 구조체 초기화 함수
void normal(struct enemys *init, struct user info)
{
    // srand(time(NULL));
    char name[5][30] = {{"오크 전사"}, {"좀비"}, {"구울"}, {"해골"}, {"스파토이"}};
    double mhp[5] = {100, 180, 280, 260, 360};
    double minhp[5] = {50, 50, 120, 200, 260};
    double atk_max[5] = {15, 30, 45, 55, 75};
    double atk_min[5] = {10, 17, 20, 28, 32};
    int gold_max[5] = {30, 60, 100, 60, 200};
    int gold_min = 5;
    double hpup[5] = {1.01, 1.02, 1.03, 1.05, 1.07};
    // int atk = rand()% ((atk_max - atk_min) + 1) + atk_min;

    // strcpy(init1->name, *(name));

    for (int i = 0; i < 5; i++)
    {
        strcpy(init->normal[i].name, *(name + i));
        init->normal[i].mhp = *(mhp + i);
        init->normal[i].minhp = *(minhp + i);
        init->normal[i].atk_max = *(atk_max + i);
        init->normal[i].atk_min = *(atk_min + i);
        init->normal[i].gold_max = *(gold_max + i);
        init->normal[i].gold_min = gold_min;
        init->normal[i].hpup = *(hpup + i);
    }  
    for (int k = 0; k < 5; k++)
    {
        for (int i = 0; i < 5; i++)        
        {   
            if (i == 4)
                init->normal[k].item[i] = 20;
            else
                init->normal[k].item[i] = 0;
            if (k == 4 && i == 2)
                init->normal[k].drop_e[i].drop = 20;
        }
    }
}

// 모든 몬스터 생성 함수(전역변수 지정으로 구조체 선언도 여기로 뺄까 생각 중)
void init_enemys(struct enemys *init0, struct user info)
{
    normal(init0, info);
    init_enemy_named(&(init0->named), info);              // 네임드 몬스터 초기화
    init_boss(init0, info);

}

// 판도라 상점
void pandora(struct items i, struct equipments e, struct user *u)
{
    int choice1, choice2;
    char b;
    int input;
    printf("판도라 : 판도라의 잡화상에 어서 오세요\n");
    printf("GOLD: %d\n", u->gold);
    for (int i = 0; i < 50; i++)
    {
        printf("-");
    }
    putchar('\n');
    printf("%-30s\n%-27s%3d\n%-30s%3d\n%-30s\n","1. 장비", "2. 빨간물약 : ", 30, "3. 마을이동주문서 : ", 100, "4. 돌아가기");
    for (int i = 0; i < 50; i++)
    {
        printf("-");
    }
    putchar('\n');
    scanf("%d", &input);
    while(getchar() != '\n')
        continue;
    switch (input)
    {
        case 1:
        {
            printf("구매하려는 장비를 숫자로 입력하시오.\n(취소하려면 q)\n");
            for (int i = 0; i < 50; i++)
            {
                printf("-");
            }
            putchar('\n');
            printf("1. 기본검 : 50\n");
            printf("2. 기본갑빠 : 30\n");
            printf("3. 기본장화 : 30\n");
            printf("4. 기본장갑 : 30\n");
            printf("5. 기본망토 : 30\n");
            printf("6. 기본마스크 : 30\n");
            scanf("%d", &choice1);
            while(getchar() != '\n')
                continue;
            switch (choice1)                            // 반복문으로 코드를 줄일까 생각 중
            {
            case 1:
                if((u->gold) >= 50)
                {
                    printf("%s를 구매하였습니다.\n", e.e[0][1].name);
                    u->gold -= 50;
                    u->inven.equip[0][1][0].have += 1;
                    break;
                }
                else
                {
                    printf("%s를 구매하기에는 금액이 부족합니다.\n", e.e[0][1].name);
                    break;
                }
            case 2:
                if((u->gold) >= 30)
                {
                    printf("%s를 구매하였습니다.\n", e.e[1][1].name);
                    u->gold -= 30;
                    u->inven.equip[1][1][0].have += 1;
                    break;
                }
                else
                {
                    printf("%s를 구매하기에는 금액이 부족합니다.\n", e.e[1][1].name);
                    break;
                }                
            case 3:
                if((u->gold) >= 30)
                {
                    printf("%s를 구매하였습니다.\n", e.e[2][1].name);
                    u->gold -= 30;
                    u->inven.equip[2][1][0].have += 1;
                    break;
                }
                else
                {
                    printf("%s를 구매하기에는 금액이 부족합니다.\n", e.e[2][1].name);
                    break;
                }            
            case 4:
                if((u->gold) >= 30)
                {
                    printf("%s를 구매하였습니다.\n", e.e[3][1].name);
                    u->gold -= 30;
                    u->inven.equip[3][1][0].have += 1;
                    break;
                }
                else
                {
                    printf("%s를 구매하기에는 금액이 부족합니다.\n", e.e[3][1].name);
                    break;
                }
            case 5:
                if((u->gold) >= 30)
                {
                    printf("%s를 구매하였습니다.\n", e.e[4][1].name);
                    u->gold -= 30;
                    u->inven.equip[4][1][0].have += 1;
                    break;
                }
                else
                {
                    printf("%s를 구매하기에는 금액이 부족합니다.\n", e.e[4][1].name);
                    break;
                }                
            case 6:
                if((u->gold) >= 30)
                {
                    printf("%s를 구매하였습니다.\n", e.e[5][1].name);
                    u->gold -= 30;
                    u->inven.equip[5][1][0].have += 1;
                    break;
                }
                else
                {
                    printf("%s를 구매하기에는 금액이 부족합니다.\n", e.e[5][1].name);
                    break;
                }                
            default:
                break;
            }
            break;
        }
        case 2:
            {
                printf("%30s\n","구매하려 하는 빨간물약의 수량을 입력하시오.\n(취소하려면 q)\n");
                scanf("%d", &choice2);
                while(getchar() != '\n')
                    continue;
                printf("구매하려는 수량이 %d개가 맞습니까?\nY / N\n", choice2);
                scanf("%c", &b);
                while(getchar() != '\n')
                    continue;
                if (u->gold >= choice2 * 30)
                {
                    if (b == 'y' || b == 'Y')
                    {
                        printf("빨간물약 %d개를 구매했습니다.\n", choice2);
                        u->inven.item[0].have += choice2;
                        u->gold -= choice2*30;
                        break;
                    }
                    else if(b == 'n' || b == 'N')
                    {
                        printf("구매를 취소하였습니다.\n");
                        break;
                    }
                    else
                    {
                        printf("잘못된 입력입니다.\n");
                        break;
                    }
                }
                else
                {
                    printf("소지한 골드가 부족합니다\n");
                    break;
                }
            }
        case 3:
        {
            printf("%30s\n","구매하려 하는 마을이동주문서의 수량을 입력하시오.\n(취소하려면 q)\n");
            scanf("%d", &choice2);
            while(getchar() != '\n')
                continue;
            printf("구매하려는 수량이 %d개가 맞습니까?\nY / N\n", choice2);
            scanf("%c", &b);
            while(getchar() != '\n')
                continue;
            if (b == 'y' || b == 'Y')
            {
                if(u->gold >= choice2*100) 
                {
                    printf("마을이동주문서 %d개를 구매하였습니다.\n", choice2);
                    u->inven.item[4].have += choice2;
                    u->gold -= choice2*100;
                    break;
                }
            }
            else if(b == 'n' || b == 'N')
            {
                printf("구매를 취소하였습니다.\n");
                break;
            }
            else
            {
                printf("잘못된 입력입니다.\n");
                break;
            }
        }
        case 4:
        {
            break;
        }
        default:
           break;
    }
    getchar();
}

// 이동 로직
// 이동시 - 이동 명령을 받는 함수
char movement(void)     // 이동 명령을 받는 함수  - 아이템 창 - 장비창 필요
{
    struct termios old, current;
    char ch;
    tcgetattr(0,&old);
    current = old;
    current.c_lflag &= ~(ICANON|ECHO);
    current.c_cc[VMIN] = 1;
    current.c_cc[VTIME] = 1;
    tcsetattr(0, TCSAFLUSH, &current);
    printf("(이동:wasd  아이템창:i  상태창:c  위치저장:l  게임종료:esc)\n");
    getchar();
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &old);

    return ch;
}
// 이동시 - 입력 받은 후 그 결과를 처리하는 함수
char replace_movement(char move, struct map *map, struct user *user, struct items i, struct equipments e)        // 이동 키를 입력 받은 후 그 결과를 처리하는 함수 - 인벤토리 함수 추가 필요
{
    char jm = 'T';
    int u_y = user->fyx.now.y;
    int u_x = user->fyx.now.x;
    switch (move)
    {
        case 'w':
        {
            if(map->col[(u_y)-1].row[u_x] == 'i')                // 이동 불가 지역이라면 변화 없음.
            {
                break;
            }
            else if (map->col[(u_y)-1].row[u_x] == '#')        // 포탈에 해당하는 문자면 위치를 변화시킴
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'o';
                user->fyx.now.y -= 1;
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                break;
            }
            else if(map->col[(u_y)-1].row[u_x] == '%')          // 제련소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                smelter(user);
                break;
            }
            else if(map->col[(u_y)-1].row[u_x] == '^')          // 성소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                sanctum(user);
                break;
            }
            else if(map->col[(u_y)-1].row[u_x] == '$')          // 판도라 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                pandora(i, e, user);
                break;
            }
            else                                                                    // 그외의 문자면 몬스터 종류를 판단
            {
                jm = judge_monster(map->col[u_y-1].row[u_x]);   // 몬스터 판단 함수 (해당 문자로 반환)
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'o';
                user->fyx.now.y -= 1;
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                break;
            }
        }
        case 's':
        {
            if(map->col[(u_y)+1].row[u_x] == 'i')                // 이동 불가 지역이라면 변화 없음.
            {
                break;
            }
            else if(map->col[(u_y)+1].row[u_x] == '%')          // 제련소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                smelter(user);
                break;
            }
            else if(map->col[(u_y)+1].row[u_x] == '^')          // 성소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                sanctum(user);
                break;
            }
            else if(map->col[(u_y)+1].row[u_x] == '$')          // 판도라 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                pandora(i, e, user);
                break;
            }
            else                                                                    // 그외의 문자면 몬스터 종류를 판단
            {
                jm = judge_monster(map->col[u_y+1].row[u_x]);
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'o';
                user->fyx.now.y += 1;
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                break;
            }
        }
        case 'a':
        {
            if(map->col[u_y].row[u_x - 1] == 'i')                // 이동 불가 지역이라면 변화 없음.
            {
                break;
            }
            else if(map->col[(u_y)].row[u_x-1] == '%')          // 제련소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                smelter(user);
                break;
            }
            else if(map->col[(u_y)].row[u_x-1] == '^')          // 성소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                sanctum(user);
                break;
            }
            else if(map->col[(u_y)].row[u_x-1] == '$')          // 판도라 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                pandora(i, e, user);
                break;
            }
            else                                                                    // 그외의 문자면 몬스터 종류를 판단
            {
                jm = judge_monster(map->col[u_y].row[u_x-1]);
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'o';
                user->fyx.now.x -= 1;
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                break;
            }
        }
        case 'd':
        {
            if(map->col[u_y].row[u_x + 1] == 'i')                // 이동 불가 지역이라면 변화 없음.
            {
                break;
            }
            else if(map->col[(u_y)].row[u_x+1] == '%')          // 제련소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                smelter(user);
                break;
            }
            else if(map->col[(u_y)].row[u_x+1] == '^')          // 성소 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                sanctum(user);
                break;
            }
            else if(map->col[(u_y)].row[u_x+1] == '$')          // 판도라 판단
            {
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                pandora(i, e, user);
                break;
            }
            else                                                                    // 그외의 문자면 몬스터 종류를 판단
            {
                jm = judge_monster(map->col[u_y].row[u_x+1]);
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'o';
                user->fyx.now.x += 1;
                map->col[user->fyx.now.y].row[user->fyx.now.x] = 'p';
                break;
            }
        }
        case 'i':   // 아이템 창 로직
        {
            user->inven.item[0].have = 3;
            int itemnum;
            int place;
            double ramain = 0;
            double temp_hp = 0;

            printf("---------------------------------------아이템 창---------------------------------------\n");
            printf("1. 빨간 물약: (%d)  2. 주항 물약: (%d)  3. 맑은 물약: (%d)  ", user->inven.item[0].have, user->inven.item[1].have, user->inven.item[2].have);
            printf("4.고농도 물약: (%d)   5. 마을 이동 주문서: (%d)   \n6. 장비 강화 주문서 (%d)  ", user->inven.item[3].have, user->inven.item[4].have, user->inven.item[5].have);
            printf("7. 순간 이동 주문서 (%d)  8. 엘릭서: (%d)   9. 창 닫기\n", user->inven.item[6].have, user->inven.item[7].have);
            printf("---------------------------------------------------------------------------------------\n");
            printf("사용할 아이템 선택: ");
            scanf("%d", &itemnum);
            while(getchar() != '\n')
            {
                continue;
            }
            while((itemnum < 1) || (itemnum > 9))
            {
                printf("잘못된 입력입니다.\n");
                printf("저장 위치를 선택하세요\n(다시 저장하면 해당위치는 사라집니다.)\n");
                scanf("%d", &itemnum);
                while(getchar() != '\n')
                {
                    continue;
                }
            }    
            switch (itemnum)
                {
                    case 1:     //빨간 물약
                    {   
                        if (user->inven.item[0].have > 0)   //빨간 물약 소지 개수가 1개 이상이면
                        {
                            user->inven.item[0].have -= 1;  //소지 개수 -1하고
                            temp_hp = user->hp;             //유저 현재 체력을 변수에 저장
                            if (temp_hp + 30 >= user->mhp)  //유저 현재 체력 + 30 값이 유저 맥스 체력보다 크거나 같으면
                            {
                                temp_hp = user->mhp;        //현재 체력을 맥스 체력까지만 회복한다
                                user->hp = temp_hp;
                                printf("체력 회복: %lf\n", user->hp);
                            }
                            else                            //현재 체력 물약 +30 보다 작으면
                            {
                                user->hp = temp_hp + 30;        //현재 체력에 + 30을 한다
                            }
                            temp_hp = 0;    
                            break;
                        }
                        else
                        {
                            printf("\t\t\t\t\t<system>\n\t\t\t\t소지한 빨간 물약이 없습니다.\n\n");
                            break;
                        }
                    }
                    
                    case 2:     //주황 물약, 이하 물약 종류는 위와 같은 로직
                    {
                        if (user->inven.item[1].have > 0)
                        {
                            user->inven.item[1].have -= 1;
                            temp_hp = user->hp;
                            if (temp_hp + 50 >= user->mhp)
                            {
                                temp_hp = user->mhp;
                                user->hp = temp_hp;
                                printf("체력 회복: %lf\n", user->hp);
                            }
                            else
                            {
                            user->hp = temp_hp + 50;
                            }
                            temp_hp = 0;
                            break;
                        }
                        else
                        {
                            printf("\t\t\t\t\t<system>\n\t\t\t\t소지한 주황 물약이 없습니다.\n\n");
                            break;
                        }
                    }
                    
                    case 3:     //맑은 물약
                    {
                        if (user->inven.item[2].have > 0)
                        {
                            user->inven.item[2].have -= 1;
                            temp_hp = user->hp;
            
                            if (temp_hp + 70 >= user->mhp)
                            {
                                temp_hp = user->mhp;
                                user->hp = temp_hp;
                                printf("체력 회복: %lf\n", user->hp);
                            }
                            else
                            {
                                user->hp = temp_hp + 70;
                            }
                            temp_hp = 0;
                            break;
                        }
                        else
                        {
                            printf("\t\t\t\t\t<system>\n\t\t\t\t소지한 맑은 물약이 없습니다.\n\n");
                            break;
                        }
                    }
                    
                    case 4: //고농도 물약
                    {
                        if (user->inven.item[3].have > 0)
                        {
                            user->inven.item[3].have -= 1;
                            temp_hp = user->hp;
                            if (temp_hp + 150 >= user->mhp)
                            {
                                temp_hp = user->mhp;
                                user->hp = temp_hp;
                                printf("체력 회복: %lf\n", user->hp);
                            }
                            else
                            {
                                user->hp = temp_hp + 150;
                            }
                            temp_hp = 0;
                            break;
                        }
                        else
                        {
                            printf("\t\t\t\t\t<system>\n\t\t\t      소지한 고농도 물약이 없습니다.\n\n");
                            break;
                        }
                    }
                    
                    case 5:     //마을 이동 주문서
                    {
                        if (user->inven.item[4].have > 0)
                        {
                            user->inven.item[4].have -= 1;
                            user->fyx.now.floor = 0;                //맵 0층 마을의
                            user->fyx.now.x = 25;
                            user->fyx.now.y = 11;    //판도라 상점 앞
                            break;
                        }
                        else
                        {
                            printf("\t\t\t\t\t<system>\n\t\t\t   소지한 마을 이동 주문서가 없습니다.\n\n");
                            break;
                        }
                    }
                    
                    case 6:      //장비 강화 주문서, 장비창 만들고 해결하기 //인첸트 함수 선언
                    {
                        enchant(user, e);
                        break;
                    }   
                    case 7:     //순간 이동 주문서, 이동 후 맵이 안 나와요...
                    {
                        if (user->inven.item[6].have > 0)
                        {
                            printf("좌표 선택\n");  //1~7까지의 저장된 좌표로 이동
                            printf("1. %d %d %d\n", user->fyx.place1.floor, user->fyx.place1.x, user->fyx.place1.y);   
                            printf("2. %d %d %d\n", user->fyx.place2.floor, user->fyx.place2.x, user->fyx.place2.y);
                            printf("3. %d %d %d\n", user->fyx.place3.floor, user->fyx.place3.x, user->fyx.place3.y);
                            printf("4. %d %d %d\n", user->fyx.place4.floor, user->fyx.place4.x, user->fyx.place4.y);
                            printf("5. %d %d %d\n", user->fyx.place5.floor, user->fyx.place5.x, user->fyx.place5.y);
                            printf("6. %d %d %d\n", user->fyx.place6.floor, user->fyx.place6.x, user->fyx.place6.y);
                            printf("7. %d %d %d\n", user->fyx.place7.floor, user->fyx.place7.x, user->fyx.place7.y);
                            scanf("%d", &place);
                            printf("선택한 좌표 위치로 이동합니다.\n");
                            if (place == 1)
                            {
                                user->inven.item[6].have -= 1;                  //순간이동 주문서 차감
                                user->fyx.now.floor = user->fyx.place1.floor;   //현재 층을 1번 저장 층으로
                                user->fyx.now.x = user->fyx.place1.x;           //현재 x좌표를 1번 저장 x좌표로
                                user->fyx.now.y = user->fyx.place1.y;           //현재 y좌표를 1번 저장 y좌표로
                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;    
                            }
                            else if (place == 2)
                            {
                                user->inven.item[6].have -= 1;
                                user->fyx.now.floor = user->fyx.place2.floor;   
                                user->fyx.now.x = user->fyx.place2.x;   
                                user->fyx.now.y = user->fyx.place2.y;

                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;
                            }
                            else if (place == 3)
                            {
                                user->inven.item[6].have -= 1;
                                user->fyx.now.floor = user->fyx.place3.floor;   
                                user->fyx.now.x = user->fyx.place3.x;   
                                user->fyx.now.y = user->fyx.place3.y;

                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;
                            }
                            else if (place == 4)
                            {
                                user->inven.item[6].have -= 1;
                                user->fyx.now.floor = user->fyx.place4.floor;   
                                user->fyx.now.x = user->fyx.place4.x;   
                                user->fyx.now.y = user->fyx.place4.y;

                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;
                            }
                            else if (place == 5)
                            {
                                user->inven.item[6].have -= 1;
                                user->fyx.now.floor = user->fyx.place5.floor;   
                                user->fyx.now.x = user->fyx.place5.x;  
                                user->fyx.now.y = user->fyx.place5.y;

                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;
                            }
                            else if (place == 6)
                            {
                                user->inven.item[6].have -= 1;
                                user->fyx.now.floor = user->fyx.place6.floor;  
                                user->fyx.now.x = user->fyx.place6.x;  
                                user->fyx.now.y = user->fyx.place6.y;

                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;
                            }
                            else if (place == 7)
                            {
                                user->inven.item[6].have -= 1;
                                user->fyx.now.floor = user->fyx.place7.floor;   
                                user->fyx.now.x = user->fyx.place7.x;   
                                user->fyx.now.y = user->fyx.place7.y;

                                make_dungeon(map, *(user));
                                show_dungeon(map, *(user));
                                mop_gen(map, *(user));
                                break;
                            }
                        }
                        else
                        {
                            printf("\t\t\t\t\t<system>\n\t\t\t   소지한 순간 이동 주문서가 없습니다.\n\n");
                            break;
                        }
                    }
                    case 8:     //엘릭서
                        {
                            elixir(user, e);    //엘릭서 함수 추가
                                //enchant 확률 100% 
                            break;   
                        }            
                    case 9: 
                    {
                        printf("아이템 창 종료\n");    
                        break;
                    }
                }//아이템 스위치 종료
            break;  //아이템창 로직 브레이크
        }
        case 'c':
        {
            dis_equip(user, e);            // 상태창(장비창) 로직
            break;
        }
        case 'l':
        {
            remember_fyx(user);
            break;
        }
        case 27:
        {
            printf("게임을 종료 합니다.\n");
            getchar();
            exit(1);
        }
    }
return jm;
}
// 이동시 - 몬스터 판단 함수    // 확인중
int judge_monster (char M)  // 몬스터 판단 함수
{
    char jm = 'T';
    if (M == 'V')
        jm = 'V';
    else if (M == 'W')
        jm = 'W';
    else if (M == 'X')
        jm = 'X';
    else if (M == 'Y')
        jm = 'Y';
    else if (M == 'Z')
        jm = 'Z';
    else if (M =='!')
        jm = '!';
    else if (M =='B')
        jm = 'B';        
    else if (M =='L')
        jm = 'L';
    else if (M =='R')
        jm = 'R';
    else
    return jm;
}
// 이동시 - floor 변화 함수
void up_and_down(struct user *info) // 포탈위치(1또는 50)에 도달 시 위치 변화
{
    switch (info->fyx.now.floor)
    {
    case 0:
        if((info->fyx.now.y == 50) && (info->fyx.now.y == 50))
        {
            info->fyx.now.floor = 1;
            info->fyx.now.x = 1;
            info->fyx.now.y = 1;
            break;
        }
    case 1:
        if((info->fyx.now.y == 50) && (info->fyx.now.y == 50))
        {
            info->fyx.now.floor = 2;
            info->fyx.now.x = 1;
            info->fyx.now.y = 1;
            break;
        }   
        else if((info->fyx.now.y == 1) && (info->fyx.now.y == 1))
        {
            info->fyx.now.floor = 0;
            info->fyx.now.x = 50;
            info->fyx.now.y = 50;
            break;
        }
    case 2:
        if((info->fyx.now.y == 50) && (info->fyx.now.y == 50))
        {
            info->fyx.now.floor = 3;
            info->fyx.now.x = 1;
            info->fyx.now.y = 1;
            break;
        }   
        else if((info->fyx.now.y == 1) && (info->fyx.now.y == 1))
        {
            info->fyx.now.floor = 1;
            info->fyx.now.x = 50;
            info->fyx.now.y = 50;
            break;
        }
    case 3:
        if((info->fyx.now.y == 50) && (info->fyx.now.y == 50))
        {
            info->fyx.now.floor = 4;
            info->fyx.now.x = 1;
            info->fyx.now.y = 1;
            break;
        }   
        else if((info->fyx.now.y == 1) && (info->fyx.now.y == 1))
        {
            info->fyx.now.floor = 2;
            info->fyx.now.x = 50;
            info->fyx.now.y = 50;
            break;
        }
    case 4:
        if((info->fyx.now.y == 50) && (info->fyx.now.y == 50))
        {
            info->fyx.now.floor = 5;
            info->fyx.now.x = 1;
            info->fyx.now.y = 1;
            break;
        }   
        else if((info->fyx.now.y == 1) && (info->fyx.now.y == 1))
        {
            info->fyx.now.floor = 3;
            info->fyx.now.x = 50;
            info->fyx.now.y = 50;
            break;
        }
    case 5:
        if((info->fyx.now.y == 1) && (info->fyx.now.y == 1))
        {
            info->fyx.now.floor = 4;
            info->fyx.now.x = 50;
            info->fyx.now.y = 50;
            break;
        }
    default:
        break;
    }
}

// 순간이동스크롤에 필요한 좌표 저장 함수
void remember_fyx(struct user *info)
{
    int choice;
    int f = info->fyx.now.floor;
    int y = info->fyx.now.y;
    int x = info->fyx.now.x;
    printf("0. 취소\n");
    printf("1.( floor: %d y: %d x: %d )\n", info->fyx.place1.floor, info->fyx.place1.y, info->fyx.place1.x);
    printf("2.( floor: %d y: %d x: %d )\n", info->fyx.place2.floor, info->fyx.place2.y, info->fyx.place2.x);
    printf("3.( floor: %d y: %d x: %d )\n", info->fyx.place3.floor, info->fyx.place3.y, info->fyx.place3.x);
    printf("4.( floor: %d y: %d x: %d )\n", info->fyx.place4.floor, info->fyx.place4.y, info->fyx.place4.x);
    printf("5.( floor: %d y: %d x: %d )\n", info->fyx.place5.floor, info->fyx.place5.y, info->fyx.place5.x);
    printf("6.( floor: %d y: %d x: %d )\n", info->fyx.place6.floor, info->fyx.place6.y, info->fyx.place6.x);
    printf("7.( floor: %d y: %d x: %d )\n", info->fyx.place7.floor, info->fyx.place7.y, info->fyx.place7.x);
    
    printf("저장 위치를 선택하세요\n(다시 저장하면 해당위치는 사라집니다.)\n");
    scanf("%d", &choice);
    while(getchar() != '\n')
    {
        continue;
    }
    while((choice < 0) || (choice > 7))
    {
        printf("잘못된 입력입니다.\n");
        printf("저장 위치를 선택하세요\n(다시 저장하면 해당위치는 사라집니다.)\n");
        scanf("%d", &choice);
        while(getchar() != '\n')
        {
            continue;
        }
    }

    switch(choice)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            info->fyx.place1.floor = f;
            info->fyx.place1.y = y;
            info->fyx.place1.x = x;
            break;
        }
        case 2:
        {
            info->fyx.place2.floor = f;
            info->fyx.place2.y = y;
            info->fyx.place2.x = x;
            break;
        }
        case 3:
        {
            info->fyx.place3.floor = f;
            info->fyx.place3.y = y;
            info->fyx.place3.x = x;
            break;
        }
        case 4:
        {
            info->fyx.place4.floor = f;
            info->fyx.place4.y = y;
            info->fyx.place4.x = x;
            break;
        }
        case 5:
        {
            info->fyx.place5.floor = f;
            info->fyx.place5.y = y;
            info->fyx.place5.x = x;
            break;
        }
        case 6:
        {
            info->fyx.place6.floor = f;
            info->fyx.place6.y = y;
            info->fyx.place6.x = x;
            break;
        }
        case 7:
        {
            info->fyx.place7.floor = f;
            info->fyx.place7.y = y;
            info->fyx.place7.x = x;
            break;
        }
    }    
}

//전투 일반몬스터
int normal_battle(struct enemys info,struct user *init1, int result, char jm)
{
    /* result : 1-몬스터 처치, 2-유저체력 0, 3-도망 */
    
    int choice;                                 // 행동입력
    int randNum;                                // 랜덤숫자
    int num = 10;
    char mon_name[5] = {'V', 'W', 'X', 'Y', 'Z'};
    
    for (int i =0 ; i < 5 ; i++)
    {
        if(jm == mon_name[i])
            num = i;
    }

    int max_hp = info.normal[num].mhp;
    int min_hp = info.normal[num].minhp;


    printf("몬스터 \"%s\"나타났습니다.\n\n",info.normal[num].name);
    double m_hp = (double)(rand()% (max_hp-min_hp+1)+min_hp);
            // 몬스터 체력
    double protect;                         // 복이 데미지 감소(장비착용으로 인해)
    for(int i=1; i<6 ;i++)
        protect += init1->eqt[i].power;
    protect *= (-1);


    while((init1->hp)>0)        // 복이 체력이 0이 아니면 계속
    {
        double m_atk = rand()% (int)((info.normal[num].atk_max-info.normal[num].atk_min+1)+(info.normal[num].atk_min));
              //몬스터 공격력
        double atk = init1->eqt[0].power;       // 복이 공격력

        printf("----------------------------------------------\n");
        printf(" [ 복 이 ]                 HP : %.1lf\n\n", init1->hp);
        printf("  HP : %.1lf              [ %s ]\n\n", m_hp ,info.normal[num].name);
        printf("----------------------------------------------\n");
        printf("1) 싸우기       2) 도망       3) 아이템 사용\n");
        printf("----------------------------------------------\n");
        printf("::행동을 입력하시오.\n");

        scanf("%d",&choice);
        while(getchar() != '\n')
            continue;
        while (choice < 1 && choice > 3)            //1~3번 이 외 선택시 다시 입력 받기
        {               
            printf("다시 입력해주시길 바랍니다.\n");
            scanf("%d",&choice);
            while(getchar() != '\n')
                continue;
        }

        switch (choice)
        {
            case 1 : //싸우다
                    m_hp -= atk;
                    printf("----------------------------------------------\n");
                    //printf("공격력 : %.2f\n",atk); //복이 공격력 확인용////////

                    printf("\"복이\"가 \"%s\"를 공격했습니다.\n",info.normal[num].name);
                    printf("\"%s\"(은)는 %.0lf 데미지를 입었습니다.\n",info.normal[num].name, atk);

                    break;
            case 2 : //도망
                    randNum = rand()%100+1;
                    if (randNum<=30){
                        printf("도망에 성공했습니다.\n");
                        result = 3;              // 전투 결과 3 :도망
                        break;
                    }
                    else{
                        printf("도망에 실패했습니다.\n");
                    }
                    break;

            case 3 : //아이템
                    int i_choice;

                    printf("----------------------------------------------\n");//  돌아가기  해줘>>?? 도망실패면 .. 그냥 맞는ㄴ디???? 오쩌지???
                    printf("    1) 빨간 물약 : 30회복  (%d 개 소유) \n    2) 주황 물약 : 50회복  (%d 개 소유)\n",
                            init1->inven.item[0].have, init1->inven.item[1].have);
                    printf("    3) 맑은 물약 : 70회복  (%d 개 소유) \n    4) 고동도 물약 : 150회복  (%d 개 소유)\n",
                            init1->inven.item[2].have, init1->inven.item[3].have);
                    printf("    5) 돌아가기\n");
                    printf("----------------------------------------------\n");
                    printf("::현재 HP : %.0lf / 최대 HP : %.0lf\n",init1->hp, init1->mhp);
                    printf("::행동을 입력하시오.\n");

                    scanf("%d",&i_choice);
                    while(getchar() != '\n')
                        continue;
                    while ((i_choice < 1) || (i_choice > 5))    //1~5번 이 외 선택시 다시 입력 받기(왜 안 먹히지으,,,,,,,,,,,,,,,,,,)
                        {               
                            printf("다시 입력해주시길 바랍니다.\n");
                            scanf("%d",&i_choice);
                            while(getchar() != '\n')
                                continue;
                        }
                    if(i_choice == 5)
                        continue;
                    
                    i_choice -= 1;
                    if((init1->inven.item[i_choice].have) > 0)
                        {
                            init1->inven.item[i_choice].have -= 1;
                            init1->hp += init1->inven.item[i_choice].power;
                            if(init1->hp > init1->mhp)
                                init1->hp = init1->mhp;
                            break;
                        }
                    else
                        {
                            printf("선택한 물약을 소지하고 있지 않습니다.\n신중하게 선택 해주세요.\n");
                            break;
                        }
                    
        }

        if(result == 3)      // 전투결과 3: 도망
            break;

        //printf("몬스터 공격력 : %.2f\n", m_atk);      // 몬스터 공격력 확인용///////
        //printf("방어력 : %.2f\n",protect); //복이 방어력 확인용/////////////
        if(m_hp > 0)
        {
            printf("\"%s\"(이))가 \"복이\"를 공격했습니다.\n",info.normal[num].name);
            if(protect > m_atk)
            {            // 방어력이 몬스터 공격력보다 큰 경우
                printf("\"복이\"는 방어구로 인해 데미지를 입지 않았습니다.\n");
            }
            else
            {
                init1->hp -= (m_atk-protect);
                printf("\"복이\"는 %.0lf 데미지를 입었습니다.\n",m_atk-protect);
            }
        }
       
        if(m_hp <= 0)
        {
            printf("\"%s\"(이)가 쓰러졌습니다.\n",info.normal[num].name);
            normal_vic_reward(info, init1, jm);
            result = 1;      // 전투결과 1: 몬스터 처치
            break;
        }
        if((init1->hp)<=0){
            double tempmhp = init1->mhp;
            init1->hp = 0.1 * tempmhp;
            init1->fyx.now.floor = 0;
            init1->fyx.now.y = 1;
            init1->fyx.now.x = 1;
            printf("\"복이\"가 쓰러졌습니다.\n");
            result = 2;      // 전투결과 2: 유저 체력 0
            break;
        }
        
    }
    return result;
}

void normal_vic_reward(struct enemys info, struct user *init1,char jm)
{
    int randNum;                                // 랜덤숫자
    int num = 0;
    char mon_name[5] = {'V', 'W', 'X', 'Y', 'Z'};

    for (int i =0 ; i < 5 ; i++ ){
        if(jm == mon_name[i])
            num = i;
    }
       
    randNum = rand ()%(info.normal[num].gold_max-info.normal[num].gold_min+1) + info.normal[num].gold_min;
    init1->gold += randNum;
    printf("    보상 : %d  골드\n",randNum);

    printf("    총 체력 %.2lf 에서 ", init1->mhp);
    init1->mhp *= info.normal[num].hpup; 
    printf("%.2lf 으로 증가 \n", init1->mhp);
    

    randNum = rand()%100+1;
    if(randNum <= 20){           // 20% 확률로 마을이동주문서 드랍
        init1->inven.item[4].have += 1;
        printf("     %s 획득",init1->inven.item[4].name);
    }
            
    if(num>=3){                           // 4층 부터 장비 드랍
        randNum = rand()%100 +1;
        if(randNum <= 20){
            randNum = rand()%6;
            init1->inven.equip[randNum][2][0].have += 1;
            printf("    %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[randNum][2][0].tier,init1->inven.equip[randNum][2][0].name);
        }
    }
}

int named_battle(struct enemys info, struct user *init1, int result, char jm)
{
    /* result : 1-몬스터 처치, 2-유저체력 0, 3-도망 */
    
    int choice;                                 // 행동입력
    int randNum;                                // 랜덤숫자
    
    int max_hp = info.named.mhp;
    int min_hp = info.named.minhp;


    printf("몬스터 \"%s\"나타났습니다.\n\n",info.named.name);
    double m_hp = info.named.mhp;
            // 몬스터 체력
    double protect;                         // 복이 데미지 감소(장비착용으로 인해)
    for(int i=1; i<6 ;i++)
        protect += init1->eqt[i].power;
    protect *= (-1);


    while((init1->hp)>0)        // 복이 체력이 0이 아니면 계속
    {
        double m_atk = rand()% (int)((info.named.atk_max-info.named.atk_min+1)+(info.named.atk_min));
              //몬스터 공격력
        double atk = init1->eqt[0].power;       // 복이 공격력

        printf("----------------------------------------------\n");
        printf(" [ 복 이 ]                 HP : %.1lf\n\n", init1->hp);
        printf("  HP : %.1lf              [ %s ]\n\n", m_hp ,info.named.name);
        printf("----------------------------------------------\n");
        printf("1) 싸우기       2) 도망       3) 아이템 사용\n");
        printf("----------------------------------------------\n");
        printf("::행동을 입력하시오.\n");

        scanf("%d",&choice);
        while(getchar() != '\n')
            continue;
        while (choice < 1 && choice > 3)            //1~3번 이 외 선택시 다시 입력 받기
        {               
            printf("다시 입력해주시길 바랍니다.\n");
            scanf("%d",&choice);
            while(getchar() != '\n')
                continue;
        }

        switch (choice)
        {
            case 1 : //싸우다
                    m_hp -= atk;
                    printf("----------------------------------------------\n");
                    //printf("공격력 : %.2f\n",atk); //복이 공격력 확인용////////

                    printf("\"복이\"가 \"%s\"를 공격했습니다.\n",info.named.name);
                    printf("\"%s\"(은)는 %.0lf 데미지를 입었습니다.\n",info.named.name, atk);

                    break;
            case 2 : //도망
                    randNum = rand()%100+1;
                    if (randNum<=30){
                        printf("도망에 성공했습니다.\n");
                        result = 3;              // 전투 결과 3 :도망
                        break;
                    }
                    else{
                        printf("도망에 실패했습니다.\n");
                    }
                    break;

            case 3 : //아이템
                    int i_choice;

                    printf("----------------------------------------------\n");//  돌아가기  해줘>>?? 도망실패면 .. 그냥 맞는ㄴ디???? 오쩌지???
                    printf("    1) 빨간 물약 : 30회복  (%d 개 소유) \n    2) 주황 물약 : 50회복  (%d 개 소유)\n",
                            init1->inven.item[0].have, init1->inven.item[1].have);
                    printf("    3) 맑은 물약 : 70회복  (%d 개 소유) \n    4) 고동도 물약 : 150회복  (%d 개 소유)\n",
                            init1->inven.item[2].have, init1->inven.item[3].have);
                    printf("    5) 돌아가기\n");
                    printf("----------------------------------------------\n");
                    printf("::현재 HP : %.0lf / 최대 HP : %.0lf\n",init1->hp, init1->mhp);
                    printf("::행동을 입력하시오.\n");

                    scanf("%d",&i_choice);
                    while(getchar() != '\n')
                        continue;
                    while ((i_choice < 1) || (i_choice > 5))    //1~5번 이 외 선택시 다시 입력 받기(왜 안 먹히지으,,,,,,,,,,,,,,,,,,)
                        {               
                            printf("다시 입력해주시길 바랍니다.\n");
                            scanf("%d",&i_choice);
                            while(getchar() != '\n')
                                continue;
                        }
                    if(i_choice == 5)
                        continue;
                    
                    i_choice -= 1;
                    if((init1->inven.item[i_choice].have) > 0)
                        {
                            init1->inven.item[i_choice].have -= 1;
                            init1->hp += init1->inven.item[i_choice].power;
                            if(init1->hp > init1->mhp)
                                init1->hp = init1->mhp;
                            break;
                        }
                    else
                        {
                            printf("선택한 물약을 소지하고 있지 않습니다.\n신중하게 선택 해주세요.\n");
                            break;
                        }
                    
        }

        if(result == 3)      // 전투결과 3: 도망
            break;

        //printf("몬스터 공격력 : %.2f\n", m_atk);      // 몬스터 공격력 확인용///////
        //printf("방어력 : %.2f\n",protect); //복이 방어력 확인용/////////////
        if(m_hp <= 0)
        {
            printf("\"%s\"(이)가 쓰러졌습니다.\n",info.named.name);
            named_vic_reward(info, init1, jm);
            result = 1;      // 전투결과 1: 몬스터 처치
            break;
        }
        if(m_hp > 0)
        {
            printf("\"%s\"(이))가 \"복이\"를 공격했습니다.\n",info.named.name);
            if(protect > m_atk)
            {            // 방어력이 몬스터 공격력보다 큰 경우
                printf("\"복이\"는 방어구로 인해 데미지를 입지 않았습니다.\n");
            }
            else
            {
                init1->hp -= (m_atk-protect);
                printf("\"복이\"는 %.0lf 데미지를 입었습니다.\n",m_atk-protect);
            }
        }

        if((init1->hp)<=0){
            double tempmhp = init1->mhp;
            init1->hp = 0.1 * tempmhp;
            init1->fyx.now.floor = 0;
            init1->fyx.now.y = 1;
            init1->fyx.now.x = 1;
            printf("\"복이\"가 쓰러졌습니다.\n");
            result = 2;      // 전투결과 2: 유저 체력 0
            break;
        }
    }
    return result;
}
void named_vic_reward(struct enemys info, struct user *init1, char jm)
{
    int randNum;                                // 랜덤숫자
    int part_rand;
    int num = 0;

    randNum = rand ()%(info.named.gold_max-info.named.gold_min+1) + info.named.gold_min;
    init1->gold += randNum; // 골드 랜덤 드랍
    printf("----------------------------------------------\n");
    printf("    보상 : %d  골드\n",randNum);
    
    randNum = rand()%100+1;
    if(randNum <= 20){           // 30% 확률로 townportar주문서 (1~3개)드랍
        init1->inven.item[4].have += 1;
        printf("     %s %d 개 획득",init1->inven.item[7].name, 1);
    }
    randNum = rand()%100+1;
    if(randNum <= 30){           // 30% 확률로 순간이동주문서 (1~3개)드랍
        randNum = rand()%3+1;
        init1->inven.item[7].have += randNum;
        printf("     %s %d 개 획득",init1->inven.item[7].name,randNum);
    }
    printf("    총 체력 %.0lf 에서 ", init1->mhp);        
    init1->mhp *= info.named.hpup;      // 총 체력 % 상승
    printf("%.2lf 으로 증가 \n", init1->mhp);

    randNum = rand()%100+1;
    part_rand = rand()%6;
    if((randNum<=20)){            // 20 확률로 3티어 장비 named
        init1->inven.equip[part_rand][3][0].have += 1;
        printf("     %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][3][0].tier,init1->inven.equip[part_rand][3][0].name);
    }
    randNum = rand()%100+1;
    part_rand = rand()%6;
    if((randNum<=20)){       // 30 확률로 2티어 장비 named
        init1->inven.equip[part_rand][2][0].have += 1;
        printf("    %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][2][0].tier,init1->inven.equip[part_rand][2][0].name);
    }
}

void init_boss (struct enemys *init, struct user info) // 보스 몬스터 초기화(test ver?)
{
    char name[3][30] = {{"바포메트"}, {"이동녘"}, {"류홍걸"}};
    double mhp[3] = {5, 7, 10};
    double atk_max[3] = {450, 550, 1300};
    double atk_min[3] = {180, 300, 500};
    int gold_max[3] = {700, 1000, 3000}; 
    int gold_min = 5;
    double hpup[3] = {1.2, 1.3, 1.6};

    for (int i = 0; i < 3; i++)
    {
        strcpy(init->boss_b[i].name, *(name + i));
        init->boss_b[i].atk_max = *(atk_max + i);
        init->boss_b[i].atk_min = *(atk_min + i);
        init->boss_b[i].gold_max = *(gold_max + i);
        init->boss_b[i].gold_min = gold_min;
        init->boss_b[i].hpup = *(hpup + i);
        init->boss_b[i].mhp = mhp[i] * info.mhp;
    }  

}

int boss_battle(struct enemys info,struct user *init1, int result, char jm)
{
    /* result : 1-몬스터 처치, 2-유저체력 0, 3-도망 */

    char name [27][10] = 
    {
    {"유시온"},{"김승수"},{"권철민"},{"안광민"},{"강진영"},
    {"김영곤"},{"박선후"},{"김건"},{"이준호"},{"이철"},
    {"이동준"},{"황은비"},{"조세빈"},{"김성근"},{"이은승"},
    {"박희정"},{"박장미"},{"김민아"},{"조대정"},{"김재신"},
    {"박민건"},{"임석현"},{"황운하"},{"노주영"},{"김혜빈"},
    {"서훈"},{"오은지"}
    };
    char bossname[13];
    char mon_name[3] = {'B', 'L', 'R'};
    int num = 0;
    int choice;                                 // 행동입력
    int randNum;                                // 랜덤숫자
    for (int i =0 ; i < 3 ; i++ )
    {
        if(jm == mon_name[i])
        {
            if(init1->flag[i] == 0)
            {
                strcpy(bossname, info.boss_b[i].name);
                num = i;

            }
            else
            {
                randNum = rand()%27;
                strcpy(bossname, name[randNum]);
                num = i;
            }
        }
    }
    printf("몬스터 \"%s\"나타났습니다.\n\n",bossname);
    double m_hp = info.boss_b[num].mhp;
            // 몬스터 체력
    double protect;                         // 복이 데미지 감소(장비착용으로 인해)
    for(int i=1; i<6 ;i++)
        protect += init1->eqt[i].power;
    protect *= (-1);


    while((init1->hp)>0)        // 복이 체력이 0이 아니면 계속
    {
        double m_atk = rand()% (int)((info.boss_b[num].atk_max-info.boss_b[num].atk_min+1)+info.boss_b[num].atk_min);
              //몬스터 공격력
        double atk = init1->eqt[0].power;       // 복이 공격력

        printf("----------------------------------------------\n");
        printf(" [ 복 이 ]                 HP : %.2lf\n\n", init1->hp);
        printf("  HP : %.2lf              [ %s ]\n\n",m_hp, bossname);
        printf("----------------------------------------------\n");
        printf("1) 싸우기       2) 도망       3) 아이템 사용\n");
        printf("----------------------------------------------\n");
        printf("::행동을 입력하시오.\n");

        scanf("%d",&choice);
        while(getchar() != '\n')
            continue;
        while (choice<1 && choice>3){               //1~3번 이 외 선택시 다시 입력 받기
            printf("다시 입력해주시길 바랍니다.\n");
            scanf("%d",&choice);
            while(getchar() != '\n')
                continue;
        }
        switch (choice)
        {
            case 1 : //싸우다
                    m_hp -= atk;
                    printf("----------------------------------------------\n");
                    //printf("공격력 : %.2f\n",atk); //복이 공격력 확인용////////

                    printf("\"복이\"가 \"%s\"를 공격했습니다.\n",bossname);
                    printf("\"%s\"(은)는 %.0lf 데미지를 입었습니다.\n",bossname, atk);
                    break;
            case 2 : //도망
                    randNum = rand()%100+1;
                    if (randNum<=30){
                        printf("도망에 성공했습니다.\n");
                        result = 3;              // 전투 결과 3 :도망
                        break;

                    }
                    else{
                        printf("도망에 실패했습니다.\n");
                    }
                    break;

            case 3 : //아이템
                    int i_choice;

                    printf("----------------------------------------------\n");//  돌아가기  해줘>>?? 도망실패면 .. 그냥 맞는ㄴ디???? 오쩌지???
                    printf("    1) 빨간 물약 : 30회복  (%d 개 소유) \n    2) 주황 물약 : 50회복  (%d 개 소유)\n",
                            init1->inven.item[0].have, init1->inven.item[1].have);
                    printf("    3) 맑은 물약 : 70회복  (%d 개 소유) \n    4) 고농도 물약 : 150회복  (%d 개 소유)\n",
                            init1->inven.item[2].have, init1->inven.item[3].have);
                    printf("    5) 돌아가기\n");
                    printf("----------------------------------------------\n");
                    printf("::현재 HP : %.0lf / 최대 HP : %.0lf\n",init1->hp, init1->mhp);
                    printf("::행동을 입력하시오.\n");
                    scanf("%d",&i_choice);
                    while(getchar() != '\n')
                        continue;
                    while ((scanf("%d",&i_choice))!=1 && i_choice > 5){               //1~4번 이 외 선택시 다시 입력 받기(왜 안 먹히지으,,,,,,,,,,,,,,,,,,)
                        printf("다시 입력해주시길 바랍니다.\n");
                        scanf("%d",&i_choice);
                    }
                    if(i_choice == 5)
                        continue;
                    i_choice -= 1;

                    if((init1->inven.item[i_choice].have) > 0){
                        init1->inven.item[i_choice].have -= 1;
                        init1->hp += init1->inven.item[i_choice].power;
                        if(init1->hp > init1->mhp)
                            init1->hp = init1->mhp;
                    //     else
                    //         continue;
                    }
                    else{
                        printf("선택한 물약을 소지하고 있지 않습니다.\n신중하게 선택 해주세요.\n");
                    }
                    break;
        }
        if(result == 3)      // 전투결과 3: 도망 좌표값 추가(no need it)
        {
            break;
        }

        //printf("몬스터 공격력 : %.2f\n", m_atk);      // 몬스터 공격력 확인용///////
        //printf("방어력 : %.2f\n",protect); //복이 방어력 확인용/////////////
        if(m_hp < 0){
            boss_vic_reward(info, init1, jm);
            result = 1;      // 전투결과 1: 몬스터 처치
            init1->flag[num] += 1; 
            if(num == 3)
                result = 4;
            break;
        }
        
        printf("\"%s\"(이))가 \"복이\"를 공격했습니다.\n",bossname);
        if(protect > m_atk){            // 방어력이 몬스터 공격력보다 큰 경우
            printf("\"복이\"는 방어구로 인해 데미지를 입지 않았습니다.\n");
        }
        else{
            init1->hp -= (m_atk-protect);
            printf("\"복이\"는 %.0lf 데미지를 입었습니다.\n",m_atk-protect);
        }

        double tempmhp = init1->mhp;
        if((init1->hp)<0){
            printf("\"복이\"가 쓰러졌습니다.\n");
            result = 2;      // 전투결과 2: 유저 체력 0 
            init1->hp = 0.1 * tempmhp;
            init1->fyx.now.floor = 0;
            init1->fyx.now.y = 1;
            init1->fyx.now.x = 1;
            break;
        }
    }
    return result;
}

void boss_vic_reward(struct enemys info, struct user *init1, char jm) //전투 보상 보스몬스터
{
    int randNum;                                // 랜덤숫자
    int part_rand;
    int num = 0;
    
    char mon_name[3] = {'B', 'L', 'R'};
    for (int i =0 ; i < 3 ; i++ ){
        if(jm == mon_name[i])
            num = i;
    }
    // 골드 랜덤 드랍
    randNum = rand ()%(info.boss_b[num].gold_max-info.boss_b[num].gold_min+1) + info.boss_b[num].gold_min; 
    init1->gold += randNum;                                              
    printf("----------------------------------------------\n");
    printf("    보상 : %d  골드\n",randNum);
    /*30% 확률로 순간이동주문서 (1~3개)드랍*/
    randNum = rand()%100+1;
    if(randNum <= 30){                                                  
        randNum = rand()%3+1;
        init1->inven.item[6].have += randNum;
        printf("         %s %d 개 획득",init1->inven.item[6].name,randNum);
    }
    /*총 체력 상승*/
    printf("    총 체력 %.0lf 에서 ", init1->mhp);        
    init1->mhp *= info.boss_b[num].hpup;               
    printf("%.2lf 으로 증가 \n", init1->mhp);
    /*3티어 장비 랜덤드랍*/
    randNum = rand()%100+1;
    part_rand = rand()%6;
    if((num<=1)&&(randNum<=20))     // 보스, 찐보스                           
    {                                         
        init1->inven.equip[part_rand][3][0].have += 1;
        printf("         %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][3][0].tier, init1->inven.equip[part_rand][3][0].name);
    }
    else if((num==2)&&(randNum<=30)) // 찐막보스                          
    {                                    
        init1->inven.equip[part_rand][3][0].have += 1;
        printf("         %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][3][0].tier, init1->inven.equip[part_rand][3][0].name);
    }
    /*4티어 장비 랜덤 드랍*/
    randNum = rand()%100+1;
    part_rand = rand()%6;
    if((num==0)&&(randNum<=5))      // 보스
    {             
        init1->inven.equip[part_rand][4][0].have += 1;
        printf("         %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][4][0].tier, init1->inven.equip[part_rand][4][0].name);
    }
    else if((num==1)&&(randNum<=10))    // 찐 보스
    {         
        init1->inven.equip[part_rand][4][0].have += 1;
        printf("         %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][4][0].tier, init1->inven.equip[part_rand][4][0].name);
    }
    else if((num==2)&&(randNum<=20))    // 찐막 보스
    {   
        init1->inven.equip[part_rand][4][0].have += 1;
        printf("         %d티어 %s (을/를) 획득했습니다.\n ", init1->inven.equip[part_rand][4][0].tier, init1->inven.equip[part_rand][4][0].name);
    }
    /*엘릭서 랜덤 드랍*/
    randNum = rand()%100+1;
    if((num==1)&&(randNum<=10)) // 찐 보스
    {           
        randNum = rand()%3+1;
        init1->inven.item[7].have += randNum;
        printf("         %s %d 개 획득 \n ",init1->inven.item[7].name,randNum);
    }
    else if((num==2)&&(randNum<=30))    // 찐막 보스
    {          
        randNum = rand()%3+1;
        init1->inven.item[7].have += randNum;
        printf("         %s %d 개 획득 \n ",init1->inven.item[7].name,randNum); 
    }
    if(num == 2 && init1->flag[2] == 0)
    {
        init1->fyx.now.floor = 0;
        init1->fyx.now.x = 1;
        init1->fyx.now.y = 1;
        ending ();

    }

}

//엘릭서
void elixir(struct user *init1, struct equipments info)
{
    float step[10] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};        // 강화 단계 별 % 배열 ㅎㅎ
    int choice;
    int ect;

    while((init1->inven.item[7].have)>0)
    {
        printf("[  엘릭서 사용  ]\n");
        for (int i=0; i<6; i++)
            printf("%d) %s\n",i+1, init1->eqt[i].name);         // 장착 장비 출력 (보기)
        printf("7) 나가기\n\n");                                 // 나가기
        
        printf("엘릭서 사용할 장비를 선택해주세요.\n");
        scanf("%d",&choice);

        while(getchar() != '\n')
        {
            continue;
        }

        while (choice<1 && choice>7){                               //1~7번 이외 선택시 다시 입력 받기
            printf("다시 입력해주시길 바랍니다.\n");
            scanf("%d",&choice);
        }
        if(choice == 7)                                             // 7번 선택 나가기
            break;       
        if((init1->eqt[choice-1].enchantment)<10 && (init1->eqt[choice-1].tier)!=0){ // 강화단계 10미만이거나 티어가 0이 아닌경우
            init1->inven.item[7].have -= 1;                                          // 엘릭서 차감

            ect = init1->eqt[choice-1].enchantment;
            init1->eqt[choice-1].power += ((init1->eqt[choice-1].power)*(step[ect]+1));   // 파워에 강화값 곱해서 더하기
            init1->eqt[choice-1].enchantment += 1;                                   // 강화단계 +1                              
                printf("엘릭서 사용하여 %s 아이템 성능이 %.2f가 되었습니다.\n",
                        init1->eqt[choice-1].name,init1->eqt[choice-1].power);
        }
        else
            printf("선택한 장비에 엘릭서를 사용할 수 없습니다.\n");     
    }
    printf("[ 엘릭서 사용 창 종료 ]\n");
}
//인첸트
void enchant(struct user *init1, struct equipments info)                  
{
    float step[10] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0};        // 강화 단계 별 % 배열 ㅎㅎ
    int choice;
    int randNum;
    int ect;

    while((init1->inven.item[5].have)>0)
    {
        

        printf("[  장비 강화  ]\n");
        for (int i=0; i<6; i++)
            printf("%d) %s\n",i+1, init1->eqt[i].name);         // 장착 장비 출력 (보기)
        printf("7) 나가기\n\n");                                   // 나가기
        
        printf("강화할 장비를 선택해주세요.\n");
        scanf("%d",&choice);

        while (getchar() != '\n')
        {
            continue;
        }

        while (choice<1 && choice>7){                               //1~7번 이외 선택시 다시 입력 받기
            printf("다시 입력해주시길 바랍니다.\n");
            scanf("%d",&choice);
        }
        if(choice == 7)                                             // 7번 선택 나가기
            break;       
        if((init1->eqt[choice-1].enchantment)<10 && (init1->eqt[choice-1].tier)!=0){ // 강화단계 10미만이거나 티어가 0이 아닌경우
            randNum = rand()%100 + 1;
            init1->inven.item[5].have -= 1;                                         // 강화주문서 차감

            if(randNum<=30){
                ect = init1->eqt[choice-1].enchantment;
                init1->eqt[choice-1].power += ((init1->eqt[choice-1].power)*(step[ect]+1));   // 파워에 강화값 곱해서 더하기
                init1->eqt[choice-1].enchantment += 1;                                        // 강화단계 +1                              
                printf("강화에 성공하여 %s 아이템 성능이 %.1f가 되었습니다.\n",
                        init1->eqt[choice-1].name,init1->eqt[choice-1].power);
            }
            else{
                printf("강화에 실패하여 %s 아이템이 파괴 되었습니다.\n",init1->eqt[choice-1].name); 
                strcpy(init1->eqt[choice-1].name, info.e[choice-1][0].name);               // 장비이름 변경
                init1->eqt[choice-1].power = 0;                                         // 파워 0으로
                init1->eqt[choice-1].tier = 0;                                          // 티어 0으로 변경
                init1->eqt[choice-1].enchantment = 0;                                   // 강화단계 0으로 변경
            }
        }
        else
            printf("선택한 장비는 강화할 수 없습니다.\n");     
    }

    printf("[ 장비 강화 종료 ]\n");
}

void dis_equip(struct user *info, struct equipments e)
{
    int count1 = 1;
    int have_num;
    int choice;
    int temp_i, temp_j, temp_k;
    int choice2;
    // print status
    printf("-------------------------------\n");
    printf("복이의 상태창\n");
    printf("위치: %d층 y축 %d x축 %d\n", info->fyx.now.floor, info->fyx.now.y, info->fyx.now.x);
    printf("이름: %s\n", info->name);
    printf("최대체력: %.0lf\n", info->mhp);
    printf("현재체력: %.0lf\n", info->hp);
    printf("공격력: %.0lf\n", info->eqt[0].power);
    printf("방어력: %.0lf\n", info->eqt[1].power + info->eqt[2].power + info->eqt[3].power + info->eqt[4].power + info->eqt[5].power);
    printf("소지금: %d\n", info->gold);
    printf("장비\n");
    printf("무기: %s %d등급 +%d\n", info->eqt[0].name, info->eqt[0].tier, info->eqt[0].enchantment);
    printf("갑옷: %s %d등급 +%d\n", info->eqt[1].name, info->eqt[1].tier, info->eqt[1].enchantment);
    printf("신발: %s %d등급 +%d\n", info->eqt[2].name, info->eqt[2].tier, info->eqt[2].enchantment);
    printf("장갑: %s %d등급 +%d\n", info->eqt[3].name, info->eqt[3].tier, info->eqt[3].enchantment);
    printf("망토: %s %d등급 +%d\n", info->eqt[4].name, info->eqt[4].tier, info->eqt[4].enchantment);
    printf("마스크: %s %d등급 +%d\n", info->eqt[5].name, info->eqt[5].tier, info->eqt[5].enchantment);
    printf("-------------------------------\n");
    printf("1. return    2. change equipment\n");
    scanf("%d", &choice);
    while (choice > 2 || choice < 0)
    {
        printf("잘못된 입력입니다.\n");
        printf("-------------------------------\n");
        printf("1. return    2. change equipment\n");
        scanf("%d", &choice);
        while (getchar() != '\n')
        {
            continue;
        }
    }
    if (choice == 1)
    {
        return;
    }
    else
    {
        have_num = count_have(info);
        int *arr_have[have_num];
        int arr_part[have_num];
        int arr_tier[have_num];
        int arr_enchant[have_num];

        printf("장비창\n");
        printf(" 무기: %s %d등급 +%d\n", info->eqt[0].name, info->eqt[0].tier, info->eqt[0].enchantment);
        printf(" 갑옷: %s %d등급 +%d\n", info->eqt[1].name, info->eqt[1].tier, info->eqt[1].enchantment);
        printf(" 신발: %s %d등급 +%d\n", info->eqt[2].name, info->eqt[2].tier, info->eqt[2].enchantment);
        printf(" 장갑: %s %d등급 +%d\n", info->eqt[3].name, info->eqt[3].tier, info->eqt[3].enchantment);
        printf(" 망토: %s %d등급 +%d\n", info->eqt[4].name, info->eqt[4].tier, info->eqt[4].enchantment);
        printf(" 마스크: %s %d등급 +%d\n", info->eqt[5].name, info->eqt[5].tier, info->eqt[5].enchantment);

        printf("0: 돌아가기\n");
        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                for (int k = 0; k < 11; k++)
                {
                    if (info->inven.equip[i][j][k].have > 0)
                    {
                        printf("%d: %s %d\n", count1, info->inven.equip[i][j][k].name, info->inven.equip[i][j][k].enchantment);
                        arr_have[count1 - 1] = &(info->inven.equip[i][j][k].have);
                        arr_part[count1 - 1] = i;
                        arr_tier[count1 - 1] = j;
                        arr_enchant[count1 - 1] = k;
                        count1++;
                    }
                }
            }
        }
        printf("장비하고 싶은 아이템을 선택하시오: ");

        scanf("%d", &choice);
        while (choice > have_num || choice < 0)
        {
            printf("잘못된 입력입니다.\n");
            printf("장비하고 싶은 아이템을 선택하시오: ");
            scanf("%d", &choice);
            while (getchar() != '\n')
            {
                continue;
            }
        }
        if (choice == 0)
        {
            printf("장비창을 종료합니다.\n");
            return;
        }
        else
        {
            // 기존 아이템 주소복사
            temp_i = info->eqt[choice - 1].part;
            temp_j = info->eqt[choice - 1].tier;
            temp_k = info->eqt[choice - 1].enchantment;
            // 기존 아이템 해제
            info->inven.equip[temp_i][temp_j][temp_k].have += 1;
            // 새로운 아이템 장비
            info->eqt[arr_part[choice - 1]].part = arr_part[choice - 1];
            info->eqt[arr_part[choice - 1]].tier = arr_tier[choice - 1];
            info->eqt[arr_part[choice - 1]].enchantment = arr_enchant[choice - 1];
            info->eqt[arr_part[choice - 1]].power = e.e[arr_part[choice - 1]][arr_tier[choice - 1]].power;
            strcpy(info->eqt[arr_part[choice - 1]].name, e.e[arr_part[choice - 1]][arr_tier[choice - 1]].name);
            info->inven.equip[arr_part[choice - 1]][arr_tier[choice - 1]][arr_enchant[choice - 1]].have -= 1;
        }
    }
}
int count_have(struct user *info) // 소유한 장비의 종류를 세어주는 함수
{
    int count = 0;

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            for (int k = 0; k < 11; k++)
            {
                if (info->inven.equip[i][j][k].have > 0)
                {
                    count++;
                }
            }
        }
    }
    return count;
}

int battle_logic(struct user * user, struct enemys enemys, char jm, int battle_result)
{
    if (jm > 85 && jm < 91)
    {
        battle_result = normal_battle(enemys, user, battle_result, jm);

    }
    else if(jm == '!')
    {
        battle_result = named_battle(enemys, user, battle_result, jm);
    }
    else if((jm == 'L') || (jm == 'R') || (jm == 'B'))
    {
        battle_result = boss_battle(enemys, user, battle_result, jm);
    }

    return battle_result;
}   

void opening ()
{
    printf("전설의 용사 복이는\n");
    sleep(1);
    printf("말하는 섬의 몬스터 바포메트를 처치해 달라는 의뢰를 받고\n");
    sleep(1);
    printf("섬의 마을에 도착했다.\n");
    sleep(3);
    printf("\n【 성직자 】\n어서오세요 용사님\n");
    sleep(1);
    printf("최근 마을 근처 던전에 있는 몬스터 수가 증가하여 주민들이 힘들어 하고 있습니다.\n"
           "던전 5층의 우두머리인 바포메트의 영향일 거라 추측은 하고 있지만,\n");
    sleep(4);
    printf("\n저희는 던전 내부로 들어갈 수 없어 정확한 원인 분석은 어렵습니다.\n"
           "부디 5층의 바포메트를 처치하고 마을에 안정을 가져다 주세요.\n");
    sleep(4);      
    printf("\n【 스멜트 】\n 던전은 마을 끝에 위치한 포탈을 통해 입장하실 수 있습니다.\n");
    sleep(2);
    printf("\n【 스멜트 】\n이후 장비 강화 주문서가 필요하면 스멜트 제련소를 찾아주세요.\n");
    sleep(3);
    printf("\n【 판도라 】\n약소하지만 빨간 물약 3개를 드리겠습니다.\n");
    sleep(3);
    printf("\n【 판도라 】\n용사님의 모험에 도움이 되기를..\n");
    sleep(3);
    printf("\n【 판도라 】\n빨간 물약과 기본 장비를 판도라의 잡화상에서 판매하고 있으니 필요한 아이템이 있으면 잡화상에 방문해주세요!\n");
    sleep(3);
    printf("\n【 성직자 】\n성소에서도 용사님의 체력 회복을 도와드릴 수 있습니다.\n");
    sleep(3);
    printf("\n【 성직자 】\n용사님의 모험에 행운이 함께하길..\n");
    sleep(3);
    printf("\n\n\n복이는 마을 사람들의 기대와 걱정을 뒤로하고 발걸음을 옮겼다.\n");
    sleep(3);
    system("clear");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣉ ⠉ ⢉ ⣉ ⣿ ⣿ ⣿ ⣿ ⣿ ⠛ ⢻ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣟ ⠛ ⢛ ⠛ ⣛ ⣛ ⢻ ⣿ ⠛ ⣉ ⣋ ⡉ ⠉ ⢻ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⡇ ⢸ ⣿ ⣿ ⣿ ⣿ ⡿ ⠿ ⠶ ⢾ ⣿ ⣿ ⣿ ⠿ ⠿ ⡿ ⠟ ⠻ ⢿ ⣿ ⣿ ⡇ ⣿ ⣿ ⢻ ⣿ ⣈ ⣿ ⣇ ⣿ ⡿ ⠁ ⣠ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⡇ ⢸ ⣿ ⣿ ⣿ ⣿ ⣷ ⣶ ⣶ ⠀ ⣿ ⣿ ⣿ ⣷ ⠀ ⣤ ⣾ ⣷ ⠘ ⣿ ⣿ ⡇ ⣋ ⣉ ⢸ ⣿ ⣿ ⣿ ⣿ ⡟ ⢁ ⣼ ⣿ ⠿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⡇ ⢸ ⣿ ⣿ ⡇ ⢸ ⣿ ⣿ ⣿ ⠀ ⣿ ⣿ ⣿ ⣿ ⠀ ⣿ ⣿ ⣿ ⢺ ⣿ ⣿ ⡇ ⣿ ⣿ ⣾ ⣿ ⢀ ⣿ ⠋ ⠠ ⠾ ⢿ ⠿ ⠀ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣧ ⣤ ⣤ ⣤ ⣤ ⣤ ⣾ ⣍ ⣉ ⣍ ⣤ ⣴ ⣤ ⣹ ⣍ ⣀ ⣩ ⣯ ⣍ ⣠ ⣽ ⣯ ⣤ ⣤ ⣤ ⣤ ⣤ ⣼ ⣿ ⣧ ⣤ ⣤ ⣤ ⣤ ⣴ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    printf("⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿ ⣿\n");
    sleep(4);
    system("clear");
}

void ending ()
{
    char name [27][10] = 
    {
    {"김승수"},{"권철민"},{"안광민"},{"강진영"},{"유시온"},
    {"김영곤"},{"박선후"},{"김건"},{"이준호"},{"이철"},
    {"이동준"},{"황은비"},{"조세빈"},{"김성근"},{"이은승"},
    {"박희정"},{"박장미"},{"김민아"},{"조대정"},{"김재신"},
    {"박민건"},{"임석현"},{"황운하"},{"노주영"},{"김혜빈"},
    {"서훈"},{"오은지"}
    };

    system("clear");
    printf("용사 복이는 찐막보스 \"류홍걸\"을 무찔렀다.\n\n");
    sleep(2);
    printf("단순히 바포메트만 처리하면 될 줄 알았지만\n");
    sleep(2);
    printf("던전의 5층에는 다른 보스인 \"이동녁\" \"류홍걸\" 이  있었고\n");
    sleep(2);
    printf("그들이 C언어를 전파해 몬스터들이 날뛰고 있었던 것이었다.\n\n");
    sleep(2);
    printf("용사 복이는 무사히 마을에 평화를 가져다 줄 수 있었다.\n");
    sleep(3);
    printf("\n\nLinEz continue ...\n\n");
    sleep(4);
    system("clear");

    for (int i = 0; i < 26; i++)
    {
            
        printf("%s, ", name[i]);

    }
    printf("%s\n", name[26]);

    printf("\n\n 나도 살려줘...\t\t                                                 나도 갇혔어...\n"
           "\n                              \t\t 나도 살려줘...                \t\t\t         나도 데려가...\n"
           "\n       살려주세요...       \t\t                    나도 구해줘...\n");
    sleep(5);
    system("clear");
}    
