#ifndef DBHANDLER_H_
#define DBHANDLER_H_
#include "typedefs.h"

bool openDatabase();

bool closeDatabase();

bool add_to_db(char *firstname, char *lastname, char *location, char *description, char *imagePath);

bool get_from_db(person *results, char *firstname, char *lastname);

bool delete_from_db(char *imagePath);

#endif