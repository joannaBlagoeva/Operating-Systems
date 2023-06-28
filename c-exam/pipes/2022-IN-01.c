// Зад. 88 2022-IN-01 Ваши колеги - асистенти по ОС имат нужда от демострационна програма на C, която да служи като пример за конкурентност и синхронизация на процеси. 
// Напишете такава програма, приемаща два задължителни позиционни параметъра – едноцифрени числа. Примерно извикване: ./main N D 
// Общ алгоритъм на програмата:
// • началният (родителски) процес създава процес-наследник
// • N на брой пъти се изпълнява:
// – родителският процес извежда на stdout низа “ DING ”
// – процесът-наследник извежда на stdout низа “DONG”
// – родителският процес изчаква D секунди
// Гарантирайте, че:
// • процесът-наследник винаги извежда “ DONG” след като родителския процес е извел “DING ”
// • родителският процес винаги започва изчакването след като процеса-наследник е извел “ DONG”
// Забележка: За изчакването погледнете sleep(3).

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>

int main(int argc, char** argv) 
{
    if (argc != 3) 
    {
        errx(1, "Arg count");
    }

    int pd1[2], pd2[2];
    if (pipe(pd1) < 0 || pipe(pd2) < 0 )
    {
        err(2, "Error with pipe");
    }

    pid_t process_id = fork();
    if (process_id < 0)
    {
        err(3, "Error with fork");
    }

    int n = atoi(argv[1]);   
    int d = atoi(argv[2]);
    //argv[1] = '0'; for digits
    //strtoi  

    if (process_id == 0)
    {
        close(pd1[1]);
        close(pd2[0]);
    }
    else
    {
        close(pd1[0]);
        close(pd2[1]);
    }

    for (int i = 0; i < n; ++i)
    {
        if (process_id > 0)
        {
            if (write(1, "DING ", 5) < 0)
            {
                err(4, "Error while writing.");
            }

            if (write(pd1[1], "D", 1) < 0)
            {
                err(4, "Error while writing.");
            }

            char buf;
            if (read(pd2[0], &buf, 1) < 0)
            {
                err(4, "Error while reading");
            }

            sleep(d);
        }
        else
        {
            char buf;
            if (read(pd1[0], &buf, 1) < 0)
            {
                err(4, "Error while reading");
            }

            if (write(1, "DONG\n", 5) < 0)
            {
                err(4, "Error while writing.");
            }

            if (write(pd2[1], "D", 1) < 0)
            {
                err(4, "Error while writing.");
            }
        }
    }

    if (process_id == 0)
    {
        close(pd1[0]);
        close(pd2[1]);
    }
    else
    {
        close(pd1[1]);
        close(pd2[0]);
    }

    return 0;
}
