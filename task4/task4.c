#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <windows.h>

void xor8(const char *filename) {
    FILE *f = fopen(filename, "r+b");
    if (!f) { perror("Ошибка открытия файла"); return; }

    int byte;
    while ((byte = fgetc(f)) != EOF) {
        fseek(f, -1, SEEK_CUR);
        fputc(byte ^ 0xFF, f);
    }
    fclose(f);
    printf("Операция xor8 успешно выполнена.\n");
}

void xorodd(const char *filename) {
    FILE *f = fopen(filename, "rb");
    FILE *temp = fopen("temp.bin", "wb");
    if (!f || !temp) {
        if (f) fclose(f);
        if (temp) fclose(temp);
        perror("Ошибка открытия файла");
        return;
    }

    unsigned char buf[4];
    while (fread(buf, 1, 4, f) == 4) {
        int has_prime = 0;
        for (int i = 0; i < 4; i++) {
            unsigned char b = buf[i];
            if (b > 1) {
                int prime = 1;
                for (int j = 2; j*j <= b; j++) if (b % j == 0) prime = 0;
                if (prime) has_prime = 1;
            }
        }
        if (has_prime) {
            for (int i = 0; i < 4; i++) buf[i] ^= 0xFF;
        }
        fwrite(buf, 1, 4, temp);
    }
    fclose(f);
    fclose(temp);
    remove(filename);
    rename("temp.bin", filename);
    printf("Операция xorodd успешно выполнена.\n");
}

void mask(const char *filename, unsigned int hex_mask) {
    FILE *f = fopen(filename, "rb");
    if (!f) { perror("Ошибка открытия файла"); return; }

    unsigned int count = 0;
    unsigned char buf[4];
    while (fread(buf, 1, 4, f) == 4) {
        unsigned int val = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
        if ((val & hex_mask) == hex_mask) count++;
    }
    fclose(f);
    printf("Найдено %u четырёхбайтовых чисел, соответствующих маске 0x%X\n", count, hex_mask);
}

int main(int argc, char *argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");

    if (argc < 3) {
        printf("=== Задание 4: Побайтовая обработка файлов ===\n\n");
        printf("Использование:\n");
        printf("  %s <файл> xor8\n", argv[0]);
        printf("  %s <файл> xorodd\n", argv[0]);
        printf("  %s <файл> mask <hex>\n\n", argv[0]);
        printf("Пример: %s test.bin mask FF\n", argv[0]);
        printf("\nНажмите Enter для выхода...");
        getchar();
        return 1;
    }

    const char *filename = argv[1];
    const char *flag = argv[2];

    printf("=== Обработка файла: %s ===\n\n", filename);

    if (strcmp(flag, "xor8") == 0) {
        xor8(filename);
    }
    else if (strcmp(flag, "xorodd") == 0) {
        xorodd(filename);
    }
    else if (strcmp(flag, "mask") == 0 && argc == 4) {
        unsigned int hex = (unsigned int)strtol(argv[3], NULL, 16);
        mask(filename, hex);
    }
    else {
        printf("Неизвестный флаг или неверные параметры!\n");
        printf("Доступные флаги: xor8, xorodd, mask <hex>\n");
    }

    return 0;
}
//gcc task4.c -o task4.exe
//task4.exe test.bin xor8