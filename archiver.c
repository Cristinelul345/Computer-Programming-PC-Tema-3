#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

union record {
        char charptr[512];
        struct header {
            char name[100];
            char mode[8];
            char uid[8];
            char gid[8];
            char size[12];
            char mtime[12];
            char chksum[8];
            char typeflag;
            char linkname[100];
            char magic[8];
            char uname[32];
            char gname[32];
            char devmajor[8];
            char devminor[8];
        } header;
    };


void create (char* nume_arh, char* nume_direct) {
    union record united;
    FILE* tar = fopen(nume_arh, "wb");
    unsigned int i;
    int j;
    char car;

    if(!tar) {
        printf("> Failed!\n");
        return;
    }
    FILE* open1 = fopen("files.txt", "r");
    if(!open1) {
        printf("> Failed!\n");
        return;
    }

    FILE* open2 = fopen("usermap.txt", "r");
    if(!open2) {
        printf("> Failed!\n");
        return;
    }

    char* line;
    line = malloc(100);

    if(line == NULL) {
        printf("> Failed!\n");
        return;
    }
    fgets(line, 100, open1);
    while(!feof(open1)) {

        int checksumm = 0;

        memset(&united, 0, sizeof(union record)); 
        char*  mode1;
        int mo = 0;

        mode1 = strtok(line, " \t\n");
        for(i = 1; i < 4; i++) {
            if(mode1[i] == 'r') {
                mo = mo + 400;
            } else {
                if(mode1[i] == 'w') {
                    mo = mo + 200;
                } else {
                    if(mode1[i] == 'x') {
                        mo = mo + 100;
                    }
                }
            }
        }

        for(i = 4; i < 7; i++) {
            if(mode1[i] == 'r') {
                mo = mo + 40;
            } else {
                if(mode1[i] =='w') {
                    mo = mo + 20;
                } else {
                    if(mode1[i] == 'x') {
                        mo = mo + 10;
                    }
                }
            }
        }

        for(i = 7; i < 10; i++) {
            if(mode1[i] == 'r') {
                mo = mo + 4;
            } else {
                if(mode1[i] == 'w') {
                    mo = mo + 2;
                } else {
                    if(mode1[i] == 'x') {
                        mo = mo + 1;
                    }
                }
            }
        }
        snprintf(united.header.mode, 9,  "%.08d", mo);
        strtok(NULL, " \t\n");
        char* owner_name = strtok(NULL, " \t\n");
        printf("owner_name: ");
        for(i = 0; i < strlen(owner_name); i++) {
            snprintf(united.header.uname + 3 * i, 4, "%03o", owner_name[i]);
        }
        char* owner_group = strtok(NULL, " \t\n");
        for(i = 0; i < strlen(owner_group); i++) {
            snprintf(united.header.gname + 3 * i, 4, "%03o", owner_group[i]);
        }
    
        char* size = strtok(NULL, " \t\n");
        snprintf(united.header.size, 13, "%.012o", atoi(size));
       
        char* an = strtok(NULL, "-");
        char* luna = strtok(NULL, "-");
        char* zi = strtok(NULL, " ");
        char* ora = strtok(NULL, ":");
        char* minut = strtok(NULL, ":");
        char* secunda = strtok(NULL, ".");
        struct tm timp;
        timp.tm_year = atoi(an) - 1900;
        timp.tm_mon = atoi(luna) - 1;
        timp.tm_mday = atoi(zi);
        timp.tm_hour = atoi(ora);
        timp.tm_min = atoi(minut);
        timp.tm_sec = atoi(secunda);
        timp.tm_isdst = -1;
        snprintf(united.header.mtime, 13, "%.012o", (int)mktime(&timp));

        strtok(NULL, " \t\n");
        strtok(NULL, " \t\n");
        char* fis_name = strtok(NULL, " \t\n");
        strcpy(united.header.name, fis_name);
        strcpy(united.header.linkname, fis_name);

        char* full_name = malloc(100);

        if(full_name == NULL) {
            printf("> Failed!\n");
            return;
        }



        strcpy(full_name, nume_direct);
        strcat(full_name, fis_name);


        united.header.typeflag = '0';
        strcpy(united.header.magic, "GNUtar ");
        memset(united.header.devminor, 0, 8);
        memset(united.header.devmajor, 0, 8);
        fseek(open2, 0, SEEK_SET);
        char* liner = malloc(100);
        fgets(liner, 100, open2);
        while(!feof(open2)){
            char* camp1 = strtok(liner, ":");
            if(!strcmp(united.header.uname, camp1)){
                strtok(NULL, ":");
                char* uid1 = strtok(NULL, ":");
                char* gid1 = strtok(NULL, ":");
                snprintf(united.header.uid, 9, "%.08o", atoi(uid1));
                snprintf(united.header.gid, 9, "%.08o", atoi(gid1));
                break;
            }
        fgets(liner, 100, open2);
        }
        strcpy(united.header.chksum, "        ");
        for(i = 0; i < 512; i++) {
            checksumm += united.charptr[i];
        }
        snprintf(united.header.chksum, 9, "%.08o", checksumm);

        fwrite(united.charptr, 1, 512, tar);

        FILE* tare = fopen(full_name, "rb");

        

        fread(&car, 1, 1, tare);
        while(!feof(tare)) {
            fwrite(&car, 1, 1, tar);
            fread(&car, 1, 1, tare);
        }

        car = 0;

        for(j = 0; j < 512 - (atoi(size) % 512); j++) {
            fwrite(&car, 1, 1, tar);
        }

        fclose(tare);

        free(liner);
        free(full_name);
        fgets(line, 100, open1);
       }
    car = 0;

    for(j = 0; j < 512; j++) {
       fwrite(&car, 1, 1, tar);
    }
    free(line);
    fclose(open1);
    fclose(open2);
    fclose(tar);
    printf("> Done!\n");
}


void list(char* nume_arh) {
    FILE* tar = fopen(nume_arh, "rb");
    if(tar == NULL) {
        printf("> File not found!\n");
    }
    union record caste;
    while(1) {
        int i;
        fread(&caste, 512, 1, tar);
        for(i = 0; i < 512; i++) {
            if(caste.charptr[i] != 0) {
                break;
            }
        }
        if(i == 512) {
            break;
        }

        int size = 0;
        int putere = 1;
        for(i = 11; i >= 0; i--) {
            if(caste.header.size[i] >= '0' && caste.header.size[i] <= '9') {
                size += putere * (caste.header.size[i] - '0');
                putere *= 8;
            }
        }
        fseek(tar, size, SEEK_CUR);

        fseek(tar, 512 - (size % 512), SEEK_CUR);

    }
    fclose(tar);
}

void extract(char* nume_fis, char* nume_arh) {
    int size = 0;
    int putere = 1;
    FILE* tar = fopen(nume_arh, "rb");
    if(tar == NULL) {
        printf("> File not found!\n");
    }
    union record caste;
    while(1) {
        int i;
        fread(&caste, 512, 1, tar);
        for(i = 0; i < 512; i++) {
            if(caste.charptr[i] != 0) {
                break;
            }
        }
        if(i == 512) {
            printf("> File not found!\n");
            break;
        }
        if(!strcmp(caste.header.name, nume_fis)) {
            char * extracted = malloc(100);
            strcpy(extracted, "extracted_");
            strcat(extracted, nume_fis);
            FILE* open0 = fopen(extracted, "wb");
            size = 0;
            putere = 1;
            for(i = 11; i >= 0; i--) {
                if(caste.header.size[i] >= '0' && caste.header.size[i] <= '9') {
                    size += putere * (caste.header.size[i] - '0');
                    putere *= 8;
                }
            }
            char c;
            for(int i  = 0; i < size; i++) {
                fread(&c, 1, 1, tar);
                fwrite(&c, 1, 1, open0);
            }
            free(extracted);
            fclose(open0);
            fclose(tar);
            break;

        } 
        size = 0;
        putere = 1;
        for(i = 11; i >= 0; i--) {
            if(caste.header.size[i] >= '0' && caste.header.size[i] <= '9') {
                size += putere * (caste.header.size[i] - '0');
                putere *= 8;
            }
        }
        fseek(tar, size, SEEK_CUR);

        fseek(tar, 512 - (size % 512), SEEK_CUR);

    }
}

int main() {
    char inputLine[200];

    while(1) {

        fgets(inputLine, sizeof(inputLine), stdin);

        char *token = strtok(inputLine, " \t\n");
        if(!strcmp(token, "exit")) {
            break;
        } 
        if (!strcmp(token, "create")) {
            char* nume_arh;
            nume_arh = strtok(NULL, " \t\n");
            if(nume_arh == NULL) {
                printf("> Wrong command!\n");
                continue;
            }
            char* nume_direct;
            nume_direct = strtok(NULL, " \t\n");
            if(nume_direct == NULL) {
                printf("> Wrong command!\n");
                continue;
            }
            create(nume_arh, nume_direct);
            continue;
        }
        if (!strcmp(token, "list")) {
            char* nume_arh;
            nume_arh = strtok(NULL, " \t\n");
            if(nume_arh == NULL) {
                printf("> Wrong command!\n");
                continue;
            }
            list(nume_arh);
            continue;
        }
        if (!strcmp(token, "extract")) {
            char* nume_fis;
            nume_fis = strtok(NULL, " \t\n");
            if(nume_fis == NULL) {
                printf("> Wrong command!\n");
                continue;
            }
            char* nume_arh;
            nume_arh = strtok(NULL, " \t\n");
            if(nume_arh == NULL) {
                printf("> Wrong command!\n");
                continue;
            }
            extract(nume_fis, nume_arh);
            continue;
        }

            
        printf("> Wrong command!\n");

    }


    return 0;
}
