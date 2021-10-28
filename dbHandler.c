#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "typedefs.h"
#define MAX_STATEMENT_LEN 511

sqlite3 *database;
char *errMsg;

bool openDatabase()
{

    int result = sqlite3_open("pandr.db", &database);

    if (result != SQLITE_OK)
    {
        printf("Error opening database!\n");
        return false;
    }

    char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS MISSING_PERSONS("
                           "IMAGE_PATH TEXT PRIMARY KEY NOT NULL,"
                           "FIRSTNAME TEXT NOT NULL,"
                           "LASTNAME TEXT NOT NULL,"
                           "LOCATION TEXT NOT NULL,"
                           "DESCRIPTION TEXT);";

    result = sqlite3_exec(database, sqlCreateTable, 0, 0, &errMsg);

    if (result != SQLITE_OK)
    {
        printf("Error creating table!\n");
        sqlite3_free(errMsg);
        sqlite3_close(database);

        return false;
    }

    return true;
}

bool closeDatabase()
{
    int result = sqlite3_close(database);
    return (result == SQLITE_OK);
}

bool add_to_db(char *firstname, char *lastname, char *location, char *description, char *imagePath)
{
    sqlite3_stmt *stmt;
    char *sql = "INSERT INTO MISSING_PERSONS (FIRSTNAME, LASTNAME, LOCATION, DESCRIPTION, IMAGE_PATH) VALUES (@fstname,@lstname,@loc,@desc,@path);";
    int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        printf("Error executing sql statement!\n");
        return false;
    }

    int id = sqlite3_bind_parameter_index(stmt, "@fstname");
    sqlite3_bind_text(stmt, id, firstname, strlen(firstname), SQLITE_TRANSIENT);
    id = sqlite3_bind_parameter_index(stmt, "@lstname");
    sqlite3_bind_text(stmt, id, lastname, strlen(lastname), SQLITE_TRANSIENT);
    id = sqlite3_bind_parameter_index(stmt, "@loc");
    sqlite3_bind_text(stmt, id, location, strlen(location), SQLITE_TRANSIENT);
    id = sqlite3_bind_parameter_index(stmt, "@desc");
    sqlite3_bind_text(stmt, id, description, strlen(description), SQLITE_TRANSIENT);
    id = sqlite3_bind_parameter_index(stmt, "@path");
    sqlite3_bind_text(stmt, id, imagePath, strlen(imagePath), SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE)
    {
        printf("Error adding to db\n");
        return false;
    }

    sqlite3_finalize(stmt);

    return true;
}

bool get_from_db(person *results, char *firstname, char *lastname)
{
    char *sql = malloc(512);
    strcpy(sql, "SELECT * FROM MISSING_PERSONS WHERE ");

    if (firstname)
    {
        strcat(sql, "FIRSTNAME = @fstname");
    }
    if (lastname)
    {
        strcat(sql, " AND LASTNAME = @lstname;");
    }
    else
    {
        strcat(sql, ";");
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        printf("Cannot prepare get stmt\n");
        return 0;
    }

    int id = -1;

    if (firstname)
    {
        id = sqlite3_bind_parameter_index(stmt, "@fstname");
        sqlite3_bind_text(stmt, id, firstname, strlen(firstname), SQLITE_TRANSIENT);
    }
    if (lastname)
    {
        id = sqlite3_bind_parameter_index(stmt, "@lstname");
        sqlite3_bind_text(stmt, id, lastname, strlen(lastname), SQLITE_TRANSIENT);
    }
    int resultsCount = 0;
    while (true)
    {
        rc = sqlite3_step(stmt);

        if (rc == SQLITE_DONE)
        {
            break;
        }

        if (rc != SQLITE_ROW)
        {
            printf("Error getting from db\n");
            return false;
        }

        if (resultsCount <= 200)
        {
            results[resultsCount].firstname = malloc(511 * sizeof(char));
            results[resultsCount].secondname  = malloc(511 * sizeof(char));
            results[resultsCount].location = malloc(511 * sizeof(char));
            results[resultsCount].description = malloc(511 * sizeof(char));
            results[resultsCount].imagePath = malloc(511 * sizeof(char));
            sprintf(results[resultsCount].firstname, "%s", sqlite3_column_text(stmt, 1));
            sprintf(results[resultsCount].secondname, "%s", sqlite3_column_text(stmt, 2));
            sprintf(results[resultsCount].location, "%s", sqlite3_column_text(stmt, 3));
            sprintf(results[resultsCount].description, "%s", sqlite3_column_text(stmt, 4));
            sprintf(results[resultsCount].imagePath, "%s", sqlite3_column_text(stmt, 0));
            resultsCount++;
        }
        else
        {
            break;
        }
    }
    sqlite3_finalize(stmt);

    return results;
}

bool delete_from_db(char *imagePath)
{
    char *sql = malloc(512);
    strcpy(sql, "DELETE FROM MISSING_PERSONS WHERE IMAGE_PATH = @path;");

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(database, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK)
    {
        printf("Cannot prepare delete stmt\n");
        return 0;
    }

    int id = sqlite3_bind_parameter_index(stmt, "@path");
    sqlite3_bind_text(stmt, id, imagePath, strlen(imagePath), SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return true;
}