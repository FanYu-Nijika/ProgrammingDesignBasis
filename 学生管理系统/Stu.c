#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>

#define MAX_NAME_LEN 20
#define MAX_ID_LEN 20
#define MAX_INPUT_LEN 128
#define MAX_COURSE_NUM 10
#define DATA_FILE "students.dat"

typedef struct Student {
    char id[MAX_ID_LEN];
    char name[MAX_NAME_LEN];
    float scores[MAX_COURSE_NUM];
    float total;
    float average;
} Student;

typedef struct Node {
    Student data;
    struct Node* next;
} Node;

typedef enum UserRole {
    ROLE_NONE = 0,
    ROLE_TEACHER,
    ROLE_STUDENT
} UserRole;

typedef struct LoginInfo {
    UserRole role;
    char username[MAX_ID_LEN];
    char studentId[MAX_ID_LEN];
} LoginInfo;

Node* gHead = NULL;
int gCourseNum = 0;
int gStudentCount = 0;

void Gotoxy(int x, int y);
void SetConsoleColor(int color);
void PrintTitle(const char* subTitle);
void WaitForUser(void);
void ClearInputBuffer(void);
void TrimRight(char* text);
int ReadLine(const char* prompt, char* buffer, int size);
int ReadInt(const char* prompt, int minValue, int maxValue);
float ReadScore(const char* prompt);
int Confirm(const char* prompt);

Node* CreateNode(Student stu);
void PushStudent(Student stu);
void FreeNodeChain(Node* head);
void FreeList(void);
int RefreshStudentCount(void);
Node* FindNodeById(const char* id);
Node* FindNodeByName(const char* name);
int IsIdExist(const char* id);

void UpdateStudentScore(Student* stu);
void UpdateAllStudentsScore(void);
void SwapStudent(Student* a, Student* b);
void SortList(int (*compare)(Student*, Student*));
int CompareById(Student* a, Student* b);
int CompareByName(Student* a, Student* b);
int CompareByTotalDesc(Student* a, Student* b);
int CompareByTotalAsc(Student* a, Student* b);

void PrintSeparatorLine(void);
void PrintTableHeader(void);
void PrintStudentRecord(Student* stu);
void PrintStudentTable(Node* start);

void SetCourseNumber(void);
void DoInputStudents(void);
void DoAddStudentRecord(void);
void DoDeleteStudentRecord(void);
void DoSearchById(void);
void DoSearchByName(void);
void DoModifyStudentInfo(void);
void DoCalculateStudentScore(void);
void DoCalculateCourseScore(void);
void DoSortById(void);
void DoSortByName(void);
void DoSortByTotalDesc(void);
void DoSortByTotalAsc(void);
void DoStatistics(void);
void DoPrintStudentRecords(void);
void DoSaveToFile(void);
int LoadFromFile(int askBeforeCover, int quiet);
void DoLoadFromFile(void);

int TeacherLogin(LoginInfo* user);
int StudentLogin(LoginInfo* user);
int Login(LoginInfo* user);
void TeacherMenu(LoginInfo* user);
void StudentMenu(LoginInfo* user);

void Gotoxy(int x, int y) {
    COORD coord;
    coord.X = (SHORT)x;
    coord.Y = (SHORT)y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void SetConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void PrintTitle(const char* subTitle) {
    system("cls");
    SetConsoleColor(14);
    Gotoxy(30, 1);
    printf("========================================\n");
    Gotoxy(30, 2);
    printf("          学生成绩管理系统\n");
    Gotoxy(30, 3);
    printf("========================================\n");
    SetConsoleColor(7);
    if (subTitle && subTitle[0] != '\0') {
        printf("\n  【%s】\n\n", subTitle);
    } else {
        printf("\n");
    }
}

void WaitForUser(void) {
    printf("\n  按回车键继续...");
    fflush(stdout);
    getchar();
}

void ClearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void TrimRight(char* text) {
    int len;
    if (!text) return;

    len = (int)strlen(text);
    while (len > 0 && (text[len - 1] == '\n' || text[len - 1] == '\r' ||
                       isspace((unsigned char)text[len - 1]))) {
        text[len - 1] = '\0';
        len--;
    }
}

int ReadLine(const char* prompt, char* buffer, int size) {
    if (prompt) {
        printf("%s", prompt);
    }
    if (!fgets(buffer, size, stdin)) {
        buffer[0] = '\0';
        return 0;
    }
    if (strchr(buffer, '\n') == NULL) {
        ClearInputBuffer();
    }
    TrimRight(buffer);
    return buffer[0] != '\0';
}

int ReadInt(const char* prompt, int minValue, int maxValue) {
    char input[MAX_INPUT_LEN];
    char* endPtr;
    long value;

    while (1) {
        ReadLine(prompt, input, sizeof(input));
        value = strtol(input, &endPtr, 10);
        while (isspace((unsigned char)*endPtr)) endPtr++;

        if (input[0] != '\0' && *endPtr == '\0' &&
            value >= minValue && value <= maxValue) {
            return (int)value;
        }
        printf("  输入不合适，请输入 %d 到 %d 之间的整数。\n", minValue, maxValue);
    }
}

float ReadScore(const char* prompt) {
    char input[MAX_INPUT_LEN];
    char* endPtr;
    float value;

    while (1) {
        ReadLine(prompt, input, sizeof(input));
        value = strtof(input, &endPtr);
        while (isspace((unsigned char)*endPtr)) endPtr++;

        if (input[0] != '\0' && *endPtr == '\0' && value >= 0 && value <= 100) {
            return value;
        }
        printf("  成绩范围是 0 到 100，请重新输入。\n");
    }
}

int Confirm(const char* prompt) {
    char input[MAX_INPUT_LEN];

    while (1) {
        ReadLine(prompt, input, sizeof(input));
        if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0) return 1;
        if (strcmp(input, "n") == 0 || strcmp(input, "N") == 0) return 0;
        printf("  请输入 y 或 n。\n");
    }
}

Node* CreateNode(Student stu) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) {
        printf("  内存不够，程序只能先退出。\n");
        exit(1);
    }
    node->data = stu;
    node->next = NULL;
    return node;
}

void PushStudent(Student stu) {
    Node* node = CreateNode(stu);
    node->next = gHead;
    gHead = node;
    gStudentCount++;
}

void FreeNodeChain(Node* head) {
    Node* current = head;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
}

void FreeList(void) {
    FreeNodeChain(gHead);
    gHead = NULL;
    gStudentCount = 0;
}

int RefreshStudentCount(void) {
    int count = 0;
    Node* p = gHead;
    while (p) {
        count++;
        p = p->next;
    }
    gStudentCount = count;
    return count;
}

Node* FindNodeById(const char* id) {
    Node* p = gHead;
    while (p) {
        if (strcmp(p->data.id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

Node* FindNodeByName(const char* name) {
    Node* p = gHead;
    while (p) {
        if (strcmp(p->data.name, name) == 0) return p;
        p = p->next;
    }
    return NULL;
}

int IsIdExist(const char* id) {
    return FindNodeById(id) != NULL;
}

void UpdateStudentScore(Student* stu) {
    int i;
    if (!stu) return;

    stu->total = 0;
    for (i = 0; i < gCourseNum; i++) {
        stu->total += stu->scores[i];
    }
    stu->average = (gCourseNum > 0) ? stu->total / gCourseNum : 0;
}

void UpdateAllStudentsScore(void) {
    Node* p = gHead;
    while (p) {
        UpdateStudentScore(&p->data);
        p = p->next;
    }
}

void SwapStudent(Student* a, Student* b) {
    Student temp = *a;
    *a = *b;
    *b = temp;
}

void SortList(int (*compare)(Student*, Student*)) {
    int swapped;
    Node* p;
    Node* last = NULL;

    if (!gHead || !gHead->next) return;

    do {
        swapped = 0;
        p = gHead;
        while (p->next != last) {
            if (compare(&p->data, &p->next->data) > 0) {
                SwapStudent(&p->data, &p->next->data);
                swapped = 1;
            }
            p = p->next;
        }
        last = p;
    } while (swapped);
}

int CompareById(Student* a, Student* b) {
    return strcmp(a->id, b->id);
}

int CompareByName(Student* a, Student* b) {
    return strcmp(a->name, b->name);
}

int CompareByTotalDesc(Student* a, Student* b) {
    if (a->total < b->total) return 1;
    if (a->total > b->total) return -1;
    return 0;
}

int CompareByTotalAsc(Student* a, Student* b) {
    if (a->total > b->total) return 1;
    if (a->total < b->total) return -1;
    return 0;
}

void PrintSeparatorLine(void) {
    int i;
    printf("+------------+------------+");
    for (i = 0; i < gCourseNum; i++) printf("--------+");
    printf("--------+--------+\n");
}

void PrintTableHeader(void) {
    int i;
    PrintSeparatorLine();
    printf("| %-10s | %-10s |", "学号", "姓名");
    for (i = 0; i < gCourseNum; i++) {
        printf(" 课%-4d|", i + 1);
    }
    printf(" 总分   | 平均分 |\n");
    PrintSeparatorLine();
}

void PrintStudentRecord(Student* stu) {
    int i;
    printf("| %-10s | %-10s |", stu->id, stu->name);
    for (i = 0; i < gCourseNum; i++) {
        printf(" %6.1f |", stu->scores[i]);
    }
    printf(" %6.1f | %6.1f |\n", stu->total, stu->average);
}

void PrintStudentTable(Node* start) {
    Node* p = start;
    PrintTableHeader();
    while (p) {
        PrintStudentRecord(&p->data);
        p = p->next;
    }
    PrintSeparatorLine();
}

void SetCourseNumber(void) {
    PrintTitle("设置科目数量");
    if (gHead) {
        printf("  已经有学生记录了，改科目数量会让已有成绩对不上。\n");
        if (!Confirm("  确定清空当前学生记录并重新设置吗？(y/n): ")) {
            WaitForUser();
            return;
        }
        FreeList();
    }
    gCourseNum = ReadInt("  请输入科目数量(1-10): ", 1, MAX_COURSE_NUM);
    printf("  科目数量已设为 %d。\n", gCourseNum);
    WaitForUser();
}

void DoInputStudents(void) {
    int n, i, j;

    PrintTitle("批量输入学生信息");
    if (gCourseNum == 0) {
        printf("  请先设置科目数量。\n");
        WaitForUser();
        return;
    }

    n = ReadInt("  请输入学生人数: ", 1, 1000);
    for (i = 0; i < n; i++) {
        Student stu;
        char prompt[64];
        memset(&stu, 0, sizeof(stu));

        printf("\n  第 %d 个学生\n", i + 1);
        while (1) {
            if (!ReadLine("    学号: ", stu.id, sizeof(stu.id))) {
                printf("    学号不能为空。\n");
                continue;
            }
            if (IsIdExist(stu.id)) {
                printf("    这个学号已经存在，请换一个。\n");
                continue;
            }
            break;
        }
        while (!ReadLine("    姓名: ", stu.name, sizeof(stu.name))) {
            printf("    姓名不能为空。\n");
        }

        for (j = 0; j < gCourseNum; j++) {
            sprintf(prompt, "    课程%d成绩: ", j + 1);
            stu.scores[j] = ReadScore(prompt);
        }
        UpdateStudentScore(&stu);
        PushStudent(stu);
    }
    printf("\n  录入完成。当前共有 %d 名学生。\n", gStudentCount);
    WaitForUser();
}

void DoAddStudentRecord(void) {
    Student stu;
    char prompt[64];
    int i;

    PrintTitle("增加学生记录");
    if (gCourseNum == 0) {
        printf("  请先设置科目数量。\n");
        WaitForUser();
        return;
    }

    memset(&stu, 0, sizeof(stu));
    while (1) {
        if (!ReadLine("  学号: ", stu.id, sizeof(stu.id))) {
            printf("  学号不能为空。\n");
            continue;
        }
        if (IsIdExist(stu.id)) {
            printf("  学号已存在，不能重复添加。\n");
            continue;
        }
        break;
    }
    while (!ReadLine("  姓名: ", stu.name, sizeof(stu.name))) {
        printf("  姓名不能为空。\n");
    }

    for (i = 0; i < gCourseNum; i++) {
        sprintf(prompt, "  课程%d成绩: ", i + 1);
        stu.scores[i] = ReadScore(prompt);
    }
    UpdateStudentScore(&stu);
    PushStudent(stu);
    printf("  添加成功。\n");
    WaitForUser();
}

void DoDeleteStudentRecord(void) {
    char id[MAX_ID_LEN];
    Node* p;
    Node* prev = NULL;

    PrintTitle("删除学生信息");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    if (!ReadLine("  请输入要删除的学号: ", id, sizeof(id))) {
        printf("  学号不能为空。\n");
        WaitForUser();
        return;
    }

    p = gHead;
    while (p) {
        if (strcmp(p->data.id, id) == 0) {
            if (prev) {
                prev->next = p->next;
            } else {
                gHead = p->next;
            }
            free(p);
            RefreshStudentCount();
            printf("  已删除学号 %s。\n", id);
            WaitForUser();
            return;
        }
        prev = p;
        p = p->next;
    }
    printf("  没找到这个学号。\n");
    WaitForUser();
}

void DoSearchById(void) {
    char id[MAX_ID_LEN];
    Node* node;

    PrintTitle("按学号查找");
    if (!ReadLine("  请输入学号: ", id, sizeof(id))) {
        printf("  学号不能为空。\n");
        WaitForUser();
        return;
    }
    node = FindNodeById(id);
    if (!node) {
        printf("  没找到该学生。\n");
    } else {
        PrintTableHeader();
        PrintStudentRecord(&node->data);
        PrintSeparatorLine();
    }
    WaitForUser();
}

void DoSearchByName(void) {
    char name[MAX_NAME_LEN];
    Node* node;

    PrintTitle("按姓名查找");
    if (!ReadLine("  请输入姓名: ", name, sizeof(name))) {
        printf("  姓名不能为空。\n");
        WaitForUser();
        return;
    }
    node = FindNodeByName(name);
    if (!node) {
        printf("  没找到该学生。\n");
    } else {
        PrintTableHeader();
        PrintStudentRecord(&node->data);
        PrintSeparatorLine();
    }
    WaitForUser();
}

void DoModifyStudentInfo(void) {
    char id[MAX_ID_LEN];
    char prompt[64];
    Node* node;
    int i;

    PrintTitle("修改学生信息");
    if (!ReadLine("  请输入要修改的学号: ", id, sizeof(id))) {
        printf("  学号不能为空。\n");
        WaitForUser();
        return;
    }
    node = FindNodeById(id);
    if (!node) {
        printf("  没找到该学生。\n");
        WaitForUser();
        return;
    }

    printf("  原来的记录:\n");
    PrintTableHeader();
    PrintStudentRecord(&node->data);
    PrintSeparatorLine();

    while (!ReadLine("\n  新姓名: ", node->data.name, sizeof(node->data.name))) {
        printf("  姓名不能为空。\n");
    }
    for (i = 0; i < gCourseNum; i++) {
        sprintf(prompt, "  新课程%d成绩: ", i + 1);
        node->data.scores[i] = ReadScore(prompt);
    }
    UpdateStudentScore(&node->data);
    printf("  修改完成。\n");
    WaitForUser();
}

void DoCalculateStudentScore(void) {
    PrintTitle("计算学生成绩");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    UpdateAllStudentsScore();
    printf("  已重新计算每名学生的总分和平均分。\n");
    WaitForUser();
}

void DoCalculateCourseScore(void) {
    float sum[MAX_COURSE_NUM] = {0};
    Node* p;
    int i;

    PrintTitle("计算课程成绩");
    if (!gHead || gStudentCount == 0) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }

    p = gHead;
    while (p) {
        for (i = 0; i < gCourseNum; i++) {
            sum[i] += p->data.scores[i];
        }
        p = p->next;
    }

    for (i = 0; i < gCourseNum; i++) {
        printf("  课程%d：总分 %.2f，平均分 %.2f\n",
               i + 1, sum[i], sum[i] / gStudentCount);
    }
    WaitForUser();
}

void DoSortById(void) {
    PrintTitle("按学号排序");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    SortList(CompareById);
    printf("  排序完成。\n\n");
    PrintStudentTable(gHead);
    WaitForUser();
}

void DoSortByName(void) {
    PrintTitle("按姓名排序");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    SortList(CompareByName);
    printf("  排序完成。\n\n");
    PrintStudentTable(gHead);
    WaitForUser();
}

void DoSortByTotalDesc(void) {
    PrintTitle("按总成绩降序排序");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    UpdateAllStudentsScore();
    SortList(CompareByTotalDesc);
    printf("  排序完成。\n\n");
    PrintStudentTable(gHead);
    WaitForUser();
}

void DoSortByTotalAsc(void) {
    PrintTitle("按总成绩升序排序");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    UpdateAllStudentsScore();
    SortList(CompareByTotalAsc);
    printf("  排序完成。\n\n");
    PrintStudentTable(gHead);
    WaitForUser();
}

void DoStatistics(void) {
    int level[6] = {0};
    Node* p;

    PrintTitle("学生成绩统计");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    UpdateAllStudentsScore();

    p = gHead;
    while (p) {
        float avg = p->data.average;
        if (avg < 60) level[0]++;
        else if (avg < 70) level[1]++;
        else if (avg < 80) level[2]++;
        else if (avg < 90) level[3]++;
        else if (avg < 100) level[4]++;
        else level[5]++;
        p = p->next;
    }

    printf("  0-59分：   %d 人\n", level[0]);
    printf("  60-69分：  %d 人\n", level[1]);
    printf("  70-79分：  %d 人\n", level[2]);
    printf("  80-89分：  %d 人\n", level[3]);
    printf("  90-99分：  %d 人\n", level[4]);
    printf("  100分：    %d 人\n", level[5]);
    WaitForUser();
}

void DoPrintStudentRecords(void) {
    PrintTitle("学生成绩列表");
    if (!gHead) {
        printf("  现在还没有学生记录。\n");
        WaitForUser();
        return;
    }
    PrintStudentTable(gHead);
    printf("  当前共 %d 名学生。\n", gStudentCount);
    WaitForUser();
}

void DoSaveToFile(void) {
    FILE* fp;
    Node* p;
    int i;

    PrintTitle("保存数据到文件");
    if (gCourseNum == 0) {
        printf("  科目数量还没有设置，不能保存。\n");
        WaitForUser();
        return;
    }

    fp = fopen(DATA_FILE, "w");
    if (!fp) {
        printf("  文件打不开，保存失败。\n");
        WaitForUser();
        return;
    }

    fprintf(fp, "%d\n%d\n", gCourseNum, gStudentCount);
    p = gHead;
    while (p) {
        fprintf(fp, "%s\n%s\n", p->data.id, p->data.name);
        for (i = 0; i < gCourseNum; i++) {
            fprintf(fp, "%.2f ", p->data.scores[i]);
        }
        fprintf(fp, "\n");
        p = p->next;
    }
    fclose(fp);
    printf("  已保存到 %s。\n", DATA_FILE);
    WaitForUser();
}

int LoadFromFile(int askBeforeCover, int quiet) {
    FILE* fp;
    Node* newHead = NULL;
    Node* newTail = NULL;
    int savedCourseNum;
    int savedCount;
    int oldCourseNum;
    int loadedCount = 0;
    int i, j;

    fp = fopen(DATA_FILE, "r");
    if (!fp) {
        if (!quiet) printf("  没找到 %s。\n", DATA_FILE);
        return 0;
    }

    if (fscanf(fp, "%d", &savedCourseNum) != 1 ||
        fscanf(fp, "%d", &savedCount) != 1 ||
        savedCourseNum < 1 || savedCourseNum > MAX_COURSE_NUM ||
        savedCount < 0) {
        fclose(fp);
        if (!quiet) printf("  数据文件格式不对，读取失败。\n");
        return 0;
    }

    if (gHead && askBeforeCover) {
        if (!Confirm("  当前内存中已有数据，读取文件会覆盖它，继续吗？(y/n): ")) {
            fclose(fp);
            return 0;
        }
    }

    oldCourseNum = gCourseNum;
    gCourseNum = savedCourseNum;

    for (i = 0; i < savedCount; i++) {
        Student stu;
        Node* node;
        memset(&stu, 0, sizeof(stu));
        if (fscanf(fp, "%19s", stu.id) != 1 ||
            fscanf(fp, "%19s", stu.name) != 1) {
            fclose(fp);
            FreeNodeChain(newHead);
            gCourseNum = oldCourseNum;
            if (!quiet) printf("  读取到第 %d 名学生时出错。\n", i + 1);
            return 0;
        }
        for (j = 0; j < gCourseNum; j++) {
            if (fscanf(fp, "%f", &stu.scores[j]) != 1 ||
                stu.scores[j] < 0 || stu.scores[j] > 100) {
                fclose(fp);
                FreeNodeChain(newHead);
                gCourseNum = oldCourseNum;
                if (!quiet) printf("  第 %d 名学生的成绩格式不对。\n", i + 1);
                return 0;
            }
        }
        UpdateStudentScore(&stu);
        node = CreateNode(stu);
        if (!newHead) {
            newHead = node;
            newTail = node;
        } else {
            newTail->next = node;
            newTail = node;
        }
        loadedCount++;
    }
    fclose(fp);
    FreeList();
    gHead = newHead;
    gStudentCount = loadedCount;
    gCourseNum = savedCourseNum;
    if (!quiet) {
        printf("  已读取 %d 条学生记录，科目数量为 %d。\n", gStudentCount, gCourseNum);
    }
    return 1;
}

void DoLoadFromFile(void) {
    PrintTitle("从文件加载数据");
    LoadFromFile(1, 0);
    WaitForUser();
}

int TeacherLogin(LoginInfo* user) {
    char name[MAX_INPUT_LEN];
    char password[MAX_INPUT_LEN];

    PrintTitle("教师登录");
    ReadLine("  教师账号: ", name, sizeof(name));
    ReadLine("  密码: ", password, sizeof(password));

    if (strcmp(name, "teacher") == 0 && strcmp(password, "123456") == 0) {
        user->role = ROLE_TEACHER;
        strcpy(user->username, "teacher");
        user->studentId[0] = '\0';
        return 1;
    }
    printf("  账号或密码不对。\n");
    WaitForUser();
    return 0;
}

int StudentLogin(LoginInfo* user) {
    char id[MAX_ID_LEN];
    char password[MAX_INPUT_LEN];
    Node* node;

    PrintTitle("学生登录");
    if (!gHead) {
        LoadFromFile(0, 1);
    }
    if (!gHead) {
        printf("  还没有学生数据，学生暂时不能登录。\n");
        WaitForUser();
        return 0;
    }

    ReadLine("  学号: ", id, sizeof(id));
    ReadLine("  密码(默认与学号相同): ", password, sizeof(password));

    node = FindNodeById(id);
    if (node && strcmp(password, id) == 0) {
        user->role = ROLE_STUDENT;
        strcpy(user->username, id);
        strcpy(user->studentId, id);
        return 1;
    }
    printf("  学号不存在，或者密码不对。\n");
    WaitForUser();
    return 0;
}

int Login(LoginInfo* user) {
    int choice;

    while (1) {
        PrintTitle("登录");
        printf("  1. 教师登录\n");
        printf("  2. 学生登录\n");
        printf("  0. 退出系统\n");
        printf("\n  教师默认账号：teacher，密码：123456\n");
        printf("  学生账号为学号，默认密码也为学号。\n\n");

        choice = ReadInt("  请选择: ", 0, 2);
        if (choice == 0) return 0;
        if (choice == 1 && TeacherLogin(user)) return 1;
        if (choice == 2 && StudentLogin(user)) return 1;
    }
}

void TeacherMenu(LoginInfo* user) {
    int choice;
    (void)user;

    while (1) {
        PrintTitle("教师菜单");
        SetConsoleColor(11);
        printf("  当前学生人数: %d    科目数量: %d\n\n", gStudentCount, gCourseNum);
        SetConsoleColor(7);
        printf("  1. 设置科目数量        2. 批量输入学生信息\n");
        printf("  3. 增加学生记录        4. 删除学生信息\n");
        printf("  5. 按学号查找          6. 按姓名查找\n");
        printf("  7. 修改学生信息        8. 计算学生成绩\n");
        printf("  9. 计算课程成绩       10. 按学号排序\n");
        printf(" 11. 按姓名排序         12. 按总成绩降序排序\n");
        printf(" 13. 按总成绩升序排序   14. 学生成绩统计\n");
        printf(" 15. 打印所有学生记录   16. 学生记录存盘\n");
        printf(" 17. 从磁盘读取记录      0. 退出登录\n\n");

        choice = ReadInt("  请选择: ", 0, 17);
        switch (choice) {
            case 1: SetCourseNumber(); break;
            case 2: DoInputStudents(); break;
            case 3: DoAddStudentRecord(); break;
            case 4: DoDeleteStudentRecord(); break;
            case 5: DoSearchById(); break;
            case 6: DoSearchByName(); break;
            case 7: DoModifyStudentInfo(); break;
            case 8: DoCalculateStudentScore(); break;
            case 9: DoCalculateCourseScore(); break;
            case 10: DoSortById(); break;
            case 11: DoSortByName(); break;
            case 12: DoSortByTotalDesc(); break;
            case 13: DoSortByTotalAsc(); break;
            case 14: DoStatistics(); break;
            case 15: DoPrintStudentRecords(); break;
            case 16: DoSaveToFile(); break;
            case 17: DoLoadFromFile(); break;
            case 0: return;
        }
    }
}

void StudentMenu(LoginInfo* user) {
    int choice;
    Node* node;

    while (1) {
        PrintTitle("学生菜单");
        printf("  登录学号: %s\n\n", user->studentId);
        printf("  1. 查看我的成绩\n");
        printf("  2. 重新读取成绩文件\n");
        printf("  0. 退出登录\n\n");

        choice = ReadInt("  请选择: ", 0, 2);
        if (choice == 0) return;

        if (choice == 2) {
            LoadFromFile(0, 0);
            WaitForUser();
            continue;
        }

        PrintTitle("我的成绩");
        node = FindNodeById(user->studentId);
        if (!node) {
            printf("  没找到你的记录，可能教师还没有录入或文件已变化。\n");
        } else {
            PrintTableHeader();
            PrintStudentRecord(&node->data);
            PrintSeparatorLine();
        }
        WaitForUser();
    }
}

int main(void) {
    LoginInfo user;

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    system("title 学生成绩管理系统");
    LoadFromFile(0, 1);

    while (1) {
        memset(&user, 0, sizeof(user));
        if (!Login(&user)) break;

        if (user.role == ROLE_TEACHER) {
            TeacherMenu(&user);
        } else if (user.role == ROLE_STUDENT) {
            StudentMenu(&user);
        }
    }

    FreeList();
    printf("  已退出系统。\n");
    return 0;
}
