#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <locale.h>
#include <windows.h>

#define MAX_USERS 100
#define MAX_LOGIN 7

typedef struct {
    char login[MAX_LOGIN];
    int pin;
    int request_limit;     
    int requests_done;     
} User;

User users[MAX_USERS];
int user_count = 0;
int current_user = -1;

// ====================== Вспомогательные ======================

int is_valid_login(const char *login) {
    if (strlen(login) == 0 || strlen(login) > 6) return 0;
    for (int i = 0; login[i]; i++)
        if (!isalnum(login[i])) return 0;
    return 1;
}

int find_user(const char *login) {
    for (int i = 0; i < user_count; i++)
        if (strcmp(users[i].login, login) == 0) return i;
    return -1;
}

void save_users() {
    FILE *f = fopen("users.dat", "wb");
    if (f) {
        fwrite(&user_count, sizeof(int), 1, f);
        fwrite(users, sizeof(User), user_count, f);
        fclose(f);
    }
}

void load_users() {
    FILE *f = fopen("users.dat", "rb");
    if (f) {
        fread(&user_count, sizeof(int), 1, f);
        if (user_count > MAX_USERS) user_count = MAX_USERS;
        fread(users, sizeof(User), user_count, f);
        fclose(f);
    }
}

void print_time() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
}

void print_date() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    printf("%02d:%02d:%04d\n", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
}

// ====================== Howmuch ======================
void howmuch(const char *datetime, const char *flag) {
    struct tm input_tm = {0};
    int day, mon, year, hour, min, sec;

    if (sscanf(datetime, "%d:%d:%d %d:%d:%d", &day, &mon, &year, &hour, &min, &sec) != 6) {
        printf("Ошибка формата даты! Пример: 30:06:2026 15:00:00\n");
        return;
    }

    input_tm.tm_mday = day;
    input_tm.tm_mon = mon - 1;
    input_tm.tm_year = year - 1900;
    input_tm.tm_hour = hour;
    input_tm.tm_min = min;
    input_tm.tm_sec = sec;

    time_t input_time = mktime(&input_tm);
    if (input_time == -1) {
        printf("Ошибка в дате/времени!\n");
        return;
    }

    time_t now = time(NULL);
    double diff_seconds = difftime(now, input_time);

    if (strcmp(flag, "-s") == 0 || strcmp(flag, "s") == 0) {
        printf("%.0f секунд\n", diff_seconds);
    } else if (strcmp(flag, "-m") == 0 || strcmp(flag, "m") == 0) {
        printf("%.1f минут\n", diff_seconds / 60);
    } else if (strcmp(flag, "-h") == 0 || strcmp(flag, "h") == 0) {
        printf("%.2f часов\n", diff_seconds / 3600);
    } else if (strcmp(flag, "-y") == 0 || strcmp(flag, "y") == 0) {
        printf("%.3f лет\n", diff_seconds / (3600.0 * 24 * 365.25));
    } else {
        printf("Неизвестный флаг! Используйте -s, -m, -h или -y\n");
    }
}

// ====================== MAIN ======================

int main() {
    load_users();

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");

    char input[256];
    char login[MAX_LOGIN];
    int pin;

    printf("=== Примитивная оболочка ===\n");

    while (1) {
        if (current_user == -1) {
            printf("\n1. Войти\n2. Зарегистрироваться\n0. Выход\n> ");
            fgets(input, sizeof(input), stdin);
            int choice = atoi(input);

            if (choice == 0) break;

            if (choice == 1) {
                printf("Login: ");
                fgets(login, sizeof(login), stdin);
                login[strcspn(login, "\n")] = 0;

                printf("PIN: ");
                scanf("%d", &pin);
                getchar();

                int idx = find_user(login);
                if (idx != -1 && users[idx].pin == pin) {
                    current_user = idx;
                    users[idx].requests_done = 0;
                    printf("Добро пожаловать, %s!\n", login);
                    printf("Доступные команды:\n");
                    printf("  Time                  - текущее время\n");
                    printf("  Date                  - текущая дата\n");
                    printf("  Howmuch <дд:ММ:гггг чч:мм:сс> -flag - прошедшее время\n");
                    printf("  Sanctions <login>     - наложить санкции\n");
                    printf("  RemoveSanctions <login> - снять санкции\n");
                    printf("  Logout                - выйти из аккаунта\n");
                } else {
                    printf("Неверный логин или PIN!\n");
                }
            } 
            else if (choice == 2) {
                printf("Новый login: ");
                fgets(login, sizeof(login), stdin);
                login[strcspn(login, "\n")] = 0;

                if (!is_valid_login(login) || find_user(login) != -1) {
                    printf("Некорректный или занятый логин!\n");
                    continue;
                }

                printf("PIN: ");
                scanf("%d", &pin);
                getchar();

                strcpy(users[user_count].login, login);
                users[user_count].pin = pin;
                users[user_count].request_limit = -1;
                users[user_count].requests_done = 0;
                user_count++;
                save_users();
                printf("Регистрация успешна!\n");
            }
        } 
        else {
            printf("\n[%s] > ", users[current_user].login);
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            if (strlen(input) == 0) continue;

            if (strcmp(input, "Logout") == 0) {
                printf("Вы вышли из аккаунта.\n");
                current_user = -1;
                continue;
            }

            if (users[current_user].request_limit != -1 && 
                users[current_user].requests_done >= users[current_user].request_limit) {
                printf("Ваш лимит запросов исчерпан! Доступна только команда Logout.\n");
                continue;
            }

            int command_executed = 0;

            if (strcmp(input, "Time") == 0) {
                print_time();
                command_executed = 1;
            }
            else if (strcmp(input, "Date") == 0) {
                print_date();
                command_executed = 1;
            }
            else if (strncmp(input, "Howmuch", 7) == 0) {
                char datetime[100] = {0};
                char flag[10] = {0};

                char *flag_pos = strstr(input, " -");
                if (flag_pos) {
                    *flag_pos = '\0';
                    strcpy(datetime, input + 8);
                    strcpy(flag, flag_pos + 2);
                    howmuch(datetime, flag);
                    command_executed = 1;
                } else {
                    printf("Формат: Howmuch дд:ММ:гггг чч:мм:сс -s\n");
                    printf("Пример: Howmuch 30:06:2026 15:00:00 -h\n");
                }
            }
            else if (strncmp(input, "Sanctions", 9) == 0) {
                char target[20] = {0};
                sscanf(input, "Sanctions %s", target);

                printf("Введите 52 для подтверждения: ");
                int code;
                scanf("%d", &code);
                getchar();

                if (code == 52) {
                    int t_idx = find_user(target);
                    if (t_idx != -1) {
                        users[t_idx].request_limit = 5;
                        save_users();
                        printf("Ограничение в 5 запросов применено для %s\n", target);
                    } else {
                        printf("Пользователь %s не найден.\n", target);
                    }
                } else {
                    printf("Подтверждение отменено.\n");
                }
                command_executed = 1;
            }
            else if (strncmp(input, "RemoveSanctions", 15) == 0) {
                char target[20] = {0};
                sscanf(input, "RemoveSanctions %s", target);

                printf("Введите 52 для подтверждения снятия санкций: ");
                int code;
                scanf("%d", &code);
                getchar();

                if (code == 52) {
                    int t_idx = find_user(target);
                    if (t_idx != -1) {
                        users[t_idx].request_limit = -1;
                        save_users();
                        printf("Санкции с пользователя %s успешно сняты!\n", target);
                    } else {
                        printf("Пользователь %s не найден.\n", target);
                    }
                } else {
                    printf("Подтверждение отменено.\n");
                }
                command_executed = 1;
            }
            else {
                printf("Неизвестная команда.\n");
            }

            if (command_executed && current_user != -1) {
                users[current_user].requests_done++;
            }
        }
    }

    save_users();
    printf("До свидания!\n");
    return 0;
}