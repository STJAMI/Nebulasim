#ifndef AUTH_H
#define AUTH_H

#define USER_DB_FILE "users.db"
#define MAX_USER_LEN 64
#define MAX_PASS_LEN 128
#define MAX_LINE 512

/* Show authentication menu. Returns 1 if login success or guest; 0 to exit
 * program */
int auth_menu(void);

/* Get logged-in username (or NULL if none) */
const char* auth_get_current_user(void);

#endif  // AUTH_H
