/*
===============================================================================
                    BANK ACCOUNT MANAGEMENT SYSTEM
                    FULLY CORRECTED GCC VERSION
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*=============================================================================
                                CONSTANTS
=============================================================================*/

#define FILE_NAME "credit.dat"

#define TEXT_FILE_NAME "accounts.txt"

#define MAX_RECORDS 100

#define TRUE 1

#define FALSE 0

/*=============================================================================
                            STRUCTURE DEFINITION
=============================================================================*/

struct clientData
{
    unsigned int acctNum;

    char lastName[15];

    char firstName[10];

    double balance;
};

/*=============================================================================
                            SAFE FILE MACROS
=============================================================================*/

/*
    sizeof() returns size_t (unsigned).

    GCC warning fixed by explicitly converting to signed long.
*/

#define RECORD_SIZE ((long)sizeof(struct clientData))

#define RECORD_POSITION(n) \
    ((long)(((long)((n) - 1)) * RECORD_SIZE))

/*=============================================================================
                            FUNCTION PROTOTYPES
=============================================================================*/

unsigned int enterChoice(void);

void initializeFile(void);

void textFile(FILE *readPtr);

void updateRecord(FILE *fPtr);

void newRecord(FILE *fPtr);

void deleteRecord(FILE *fPtr);

void displayAllRecords(FILE *fPtr);

void displayStatistics(FILE *fPtr);

void printRecord(struct clientData client);

void printHeader(void);

void printLine(void);

void clearInputBuffer(void);

void pauseScreen(void);

int validateAccountNumber(unsigned int account);

int fileExists(const char *filename);

void displayWelcomeScreen(void);

void displayExitMessage(void);

/*=============================================================================
                                MAIN FUNCTION
=============================================================================*/

int main(void)
{
    FILE *cfPtr = NULL;

    unsigned int choice = 0;

    initializeFile();

    cfPtr = fopen(FILE_NAME, "rb+");

    if (cfPtr == NULL)
    {
        fprintf(stderr,
                "\nERROR: Could not open \"%s\"\n",
                FILE_NAME);

        return EXIT_FAILURE;
    }

    displayWelcomeScreen();

    while ((choice = enterChoice()) != 6)
    {
        switch (choice)
        {
            case 1:

                textFile(cfPtr);

                break;

            case 2:

                updateRecord(cfPtr);

                break;

            case 3:

                newRecord(cfPtr);

                break;

            case 4:

                deleteRecord(cfPtr);

                break;

            case 5:

                displayAllRecords(cfPtr);

                break;

            default:

                printf("\nInvalid choice.\n");

                break;
        }

        pauseScreen();
    }

    fclose(cfPtr);

    displayExitMessage();

    return 0;
}

/*=============================================================================
                            INITIALIZE FILE
=============================================================================*/

void initializeFile(void)
{
    FILE *fPtr = NULL;

    int i = 0;

    struct clientData blankClient = {0, "", "", 0.0};

    if (fileExists(FILE_NAME))
    {
        return;
    }

    fPtr = fopen(FILE_NAME, "wb");

    if (fPtr == NULL)
    {
        fprintf(stderr,
                "\nERROR: Could not create \"%s\"\n",
                FILE_NAME);

        exit(EXIT_FAILURE);
    }

    for (i = 0; i < MAX_RECORDS; i++)
    {
        fwrite(&blankClient,
               sizeof(struct clientData),
               1,
               fPtr);
    }

    fclose(fPtr);
}

/*=============================================================================
                            CREATE TEXT FILE
=============================================================================*/

void textFile(FILE *readPtr)
{
    FILE *writePtr = NULL;

    struct clientData client = {0, "", "", 0.0};

    writePtr = fopen(TEXT_FILE_NAME, "w");

    if (writePtr == NULL)
    {
        printf("\nERROR: Could not create text file.\n");

        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-10s%-20s%-20s%-15s\n",
            "Account",
            "Last Name",
            "First Name",
            "Balance");

    printLine();

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-10u%-20s%-20s%-15.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("\naccounts.txt created successfully.\n");
}

/*=============================================================================
                            UPDATE RECORD
=============================================================================*/

void updateRecord(FILE *fPtr)
{
    unsigned int account = 0;

    double transaction = 0.0;

    struct clientData client = {0, "", "", 0.0};

    printf("\nEnter account number to update (1 - 100): ");

    if (scanf("%u", &account) != 1)
    {
        printf("\nInvalid input.\n");

        clearInputBuffer();

        return;
    }

    if (!validateAccountNumber(account))
    {
        printf("\nInvalid account number.\n");

        return;
    }

    fseek(fPtr,
          RECORD_POSITION(account),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("\nAccount #%u does not exist.\n",
               account);

        return;
    }

    printf("\nCurrent Record:\n");

    printLine();

    printRecord(client);

    printLine();

    printf("\nEnter charge (+) or payment (-): ");

    if (scanf("%lf", &transaction) != 1)
    {
        printf("\nInvalid amount.\n");

        clearInputBuffer();

        return;
    }

    client.balance += transaction;

    /*
        FULLY FIXED GCC WARNING HERE
    */

    fseek(fPtr,
          -RECORD_SIZE,
          SEEK_CUR);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("\nAccount updated successfully.\n");
}

/*=============================================================================
                            DELETE RECORD
=============================================================================*/

void deleteRecord(FILE *fPtr)
{
    unsigned int accountNum = 0;

    struct clientData client;

    struct clientData blankClient = {0, "", "", 0.0};

    printf("\nEnter account number to delete (1 - 100): ");

    if (scanf("%u", &accountNum) != 1)
    {
        printf("\nInvalid input.\n");

        clearInputBuffer();

        return;
    }

    if (!validateAccountNumber(accountNum))
    {
        printf("\nInvalid account number.\n");

        return;
    }

    fseek(fPtr,
          RECORD_POSITION(accountNum),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("\nAccount does not exist.\n");

        return;
    }

    fseek(fPtr,
          RECORD_POSITION(accountNum),
          SEEK_SET);

    fwrite(&blankClient,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("\nAccount deleted successfully.\n");
}

/*=============================================================================
                            CREATE NEW RECORD
=============================================================================*/

void newRecord(FILE *fPtr)
{
    unsigned int accountNum = 0;

    struct clientData client = {0, "", "", 0.0};

    printf("\nEnter new account number (1 - 100): ");

    if (scanf("%u", &accountNum) != 1)
    {
        printf("\nInvalid input.\n");

        clearInputBuffer();

        return;
    }

    if (!validateAccountNumber(accountNum))
    {
        printf("\nInvalid account number.\n");

        return;
    }

    fseek(fPtr,
          RECORD_POSITION(accountNum),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum != 0)
    {
        printf("\nAccount already exists.\n");

        return;
    }

    printf("\nEnter lastname firstname balance\n");
    printf("? ");

    if (scanf("%14s %9s %lf",
              client.lastName,
              client.firstName,
              &client.balance) != 3)
    {
        printf("\nInvalid input.\n");

        clearInputBuffer();

        return;
    }

    client.acctNum = accountNum;

    fseek(fPtr,
          RECORD_POSITION(accountNum),
          SEEK_SET);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    fflush(fPtr);

    printf("\nNew account created successfully.\n");
}

/*=============================================================================
                            DISPLAY ALL RECORDS
=============================================================================*/

void displayAllRecords(FILE *fPtr)
{
    struct clientData client;

    int found = FALSE;

    rewind(fPtr);

    printHeader();

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            printRecord(client);

            found = TRUE;
        }
    }

    if (!found)
    {
        printf("\nNo records found.\n");
    }

    printLine();

    displayStatistics(fPtr);
}

/*=============================================================================
                            DISPLAY STATISTICS
=============================================================================*/

void displayStatistics(FILE *fPtr)
{
    struct clientData client;

    int totalAccounts = 0;

    double totalBalance = 0.0;

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            totalAccounts++;

            totalBalance += client.balance;
        }
    }

    printf("\nTotal Accounts : %d\n",
           totalAccounts);

    printf("Total Balance  : %.2f\n",
           totalBalance);

    if (totalAccounts > 0)
    {
        printf("Average Balance: %.2f\n",
               totalBalance / totalAccounts);
    }
}

/*=============================================================================
                            MENU FUNCTION
=============================================================================*/

unsigned int enterChoice(void)
{
    unsigned int menuChoice = 0;

    printLine();

    printf("\n");
    printf("1 - Create formatted text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Display all accounts\n");
    printf("6 - Exit program\n");

    printLine();

    printf("\nEnter your choice: ");

    if (scanf("%u", &menuChoice) != 1)
    {
        clearInputBuffer();

        return 0;
    }

    return menuChoice;
}

/*=============================================================================
                            PRINT RECORD
=============================================================================*/

void printRecord(struct clientData client)
{
    printf("%-10u%-20s%-20s%-15.2f\n",
           client.acctNum,
           client.lastName,
           client.firstName,
           client.balance);
}

/*=============================================================================
                            PRINT HEADER
=============================================================================*/

void printHeader(void)
{
    printLine();

    printf("%-10s%-20s%-20s%-15s\n",
           "Account",
           "Last Name",
           "First Name",
           "Balance");

    printLine();
}

/*=============================================================================
                            PRINT LINE
=============================================================================*/

void printLine(void)
{
    printf("=================================================================\n");
}

/*=============================================================================
                        VALIDATE ACCOUNT NUMBER
=============================================================================*/

int validateAccountNumber(unsigned int account)
{
    if (account < 1 || account > MAX_RECORDS)
    {
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================
                            CLEAR INPUT BUFFER
=============================================================================*/

void clearInputBuffer(void)
{
    int c = 0;

    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

/*=============================================================================
                            PAUSE SCREEN
=============================================================================*/

void pauseScreen(void)
{
    printf("\nPress ENTER to continue...");

    clearInputBuffer();

    getchar();
}

/*=============================================================================
                            FILE EXISTS
=============================================================================*/

int fileExists(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if (file != NULL)
    {
        fclose(file);

        return TRUE;
    }

    return FALSE;
}

/*=============================================================================
                            WELCOME SCREEN
=============================================================================*/

void displayWelcomeScreen(void)
{
    printLine();

    printf("            BANK ACCOUNT MANAGEMENT SYSTEM\n");

    printLine();

    printf("\nRandom Access File Processing Demonstration\n");
}

/*=============================================================================
                            EXIT MESSAGE
=============================================================================*/

void displayExitMessage(void)
{
    printLine();

    printf("Program terminated successfully.\n");

    printLine();
}

/*
===============================================================================
                                END OF PROGRAM
===============================================================================
*/