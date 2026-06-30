#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>

int main(int argc, char *argv[]) {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");
    
    if (argc != 3) {
        fprintf(stderr, "Использование: %s <исходный_файл> <целевой_файл>\n", argv[0]);
        printf("\nНажмите Enter для выхода...");
        getchar();
        return 1;
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    FILE *src = fopen(source, "rb");
    if (!src) {
        perror("Не удалось открыть исходный файл");
        printf("\nНажмите Enter для выхода...");
        getchar();
        return 1;
    }

    FILE *dest = fopen(destination, "wb");
    if (!dest) {
        perror("Не удалось создать/открыть целевой файл");
        fclose(src);
        printf("\nНажмите Enter для выхода...");
        getchar();
        return 1;
    }

    int ch;
    long bytes_copied = 0;

    while ((ch = fgetc(src)) != EOF) {
        if (fputc(ch, dest) == EOF) {
            perror("Ошибка записи в целевой файл");
            fclose(src);
            fclose(dest);
            printf("\nНажмите Enter для выхода...");
            getchar();
            return 1;
        }
        bytes_copied++;
    }

    fclose(src);
    fclose(dest);

    printf("Файл успешно скопирован!\n");
    printf("Скопировано байт: %ld\n", bytes_copied);
    printf("Из: %s\n", source);
    printf("В:  %s\n", destination);

    printf("\n=== Копирование завершено ===\n");
    printf("Нажмите Enter для выхода...");
    getchar();

    return 0;
}
//cd <<ПУТЬ_К_ПАПКЕ_С_ФАЙЛОМ  >>
// task3.exe test.txt copy_test.txt