
/* c headers */
#include <getopt.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/signal.h>
#include <unistd.h>

/* class headers */
#include "logtype.hpp"
#include "logtype_timefolder.hpp"

/* config header */
#include "../config.h"

static LogType *g_logtype = NULL;

static void handle_sigterm(int sig)
{
	printf("interrupted by SIGTERM\n");
    if (g_logtype != NULL)
        g_logtype->terminate();
}

static void handle_sigint(int sig)
{
	printf("interrupted by SIGINT\n");
    if (g_logtype != NULL)
        g_logtype->terminate();
}

void print_help()
{
    printf("%s version %s\n\n", PACKAGE, VERSION);

    printf("Usage: %s [-h] [-c FILE] [-l FILE]\n\n", PACKAGE);

    printf(" -c, --conf=FILE    read config from FILE\n");
    printf(" -l, --log=FILE     log to the specified FILE\n");
    printf(" -v, --verbose      increase verbosity\n\n");

    printf("(-h) --help         show this help (-h is --help only if used alone)\n\n");
}

void parse_options(int argc, char **argv, LogType* logType)
{
    static struct option long_options[] =
    {
        /* These options don't set a flag.
          We distinguish them by their indices. */
        {"help",    no_argument,       0, 'h'},
        {"verbose", no_argument,       0, 'v'},
        {"conf",    required_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

    int opt;

    while(1) {

        /* getopt_long stores the option index here. */
        int option_index = 0;

        opt = getopt_long (argc, argv, "hvc:", long_options, &option_index);

        /* Detect the end of the options. */
        if (opt == -1)
            break;

        switch (opt)
        {
            case 0:
                /* If this option set a flag, do nothing else now. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;

            case 'c':
                logType->setConfigFile(optarg);
                break;

            case 'v':
                logType->setVerbose(true);
                break;

            case 'h':   /* fall-through is intentional */
            case '?':
                print_help();
                exit(EXIT_SUCCESS);

            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char **argv)
{
    LogType *logType = new LogType();
    if (!logType)
        goto fail;

    parse_options(argc, argv, logType);

    /* check config */
    if (!logType->validate())
        goto fail;

    /* check storage type */
    switch (logType->getStorageType())
    {
        case LogType::LOG_STORAGETYPE_TIMEFOLDER:
            {
                LogTypeTimeFolder *logType_tf = new LogTypeTimeFolder(*logType);
                delete logType;
                logType = logType_tf;
            }
            break;
        default:
            /* wrong storage type */
            goto fail;
    }

    /* check config */
    if (!logType->validate())
        goto fail;

    signal(SIGTERM, handle_sigterm);
    signal(SIGINT, handle_sigint);

    // assign on global variable for stop threads
    g_logtype = logType;

    logType->run();

    if (logType != NULL)
        delete logType;
    return EXIT_SUCCESS;

fail:
    if (logType != NULL)
        delete logType;
    return EXIT_FAILURE;
}
