#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>

// Подавляем предупреждения IDE
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

int main(int argc, char *argv[]) {

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    unsigned char bytes[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5};
    size_t num_bytes = sizeof(bytes);

    // Создание файла
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        perror("fopen write");
        return 1;
    }
    fwrite(bytes, 1, num_bytes, fp);
    fclose(fp);

    printf("Файл создан: %s\n\n", filename);

    // Побайтовое чтение + вывод FILE
    fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen read");
        return 1;
    }

    printf("------- Побайтовое чтение с состоянием FILE -------\n");
    int c;
    while ((c = fgetc(fp)) != EOF) {
        printf("Byte: %3d (0x%02X)  pos=%ld\n", c, c, ftell(fp)-1);

        {
            long cur_pos = ftell(fp);
            if (cur_pos < 0) cur_pos = -1;
            printf("  fileno         = %d\n", fileno(fp));
            printf("  pos_before_next= %ld\n", cur_pos - 1);
            printf("  feof           = %d\n", feof(fp));
            printf("  ferror         = %d\n", ferror(fp));
            printf("_______________________________________\n");
        }
    }
    fclose(fp);

    // fseek + fread
    fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen reread");
        return 1;
    }

    fseek(fp, 3, SEEK_SET);

    unsigned char buf[4] = {0};
    size_t n = fread(buf, 1, 4, fp);

    printf("\nПосле fseek(3, SEEK_SET) и fread(4):\n");
    printf("Буфер: ");
    for (size_t i = 0; i < n; i++) printf("%d ", buf[i]);
    printf("\n");

    fclose(fp);
    

    return 0;

}

//gcc task1.c -o task1
//task1.exe myfile.bin