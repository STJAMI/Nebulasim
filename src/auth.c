// auth.c -- very simple auth: register / login / forgot (requires current
// password)
#include "auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char current_user[MAX_USER_LEN] = {0};

/* File format: each line contains "username password\n" (space-separated).
   Example:
     jami 1234
     rahim pass567
*/

/* Check whether username exists */
static int user_exists(const char* username) {
  FILE* f = fopen(USER_DB_FILE, "r");
  if (!f) return 0;
  char line[MAX_LINE];
  char u[MAX_USER_LEN], p[MAX_PASS_LEN];
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%63s %127s", u, p) == 2) {
      if (strcmp(u, username) == 0) {
        fclose(f);
        return 1;
      }
    }
  }
  fclose(f);
  return 0;
}

/* Append a new username/password to DB */
static int append_user(const char* username, const char* password) {
  FILE* f = fopen(USER_DB_FILE, "a");
  if (!f) return 0;
  fprintf(f, "%s %s\n", username, password);
  fclose(f);
  return 1;
}

/* Register flow */
static void do_register(void) {
  char username[MAX_USER_LEN];
  char pass[MAX_PASS_LEN];
  char pass2[MAX_PASS_LEN];

  printf("=== Register ===\n");
  printf("Choose username: ");
  if (!fgets(username, sizeof(username), stdin)) return;
  username[strcspn(username, "\r\n")] = 0;
  if (strlen(username) == 0) {
    printf("Empty username.\n");
    return;
  }
  if (user_exists(username)) {
    printf("Username already exists.\n");
    return;
  }

  printf("Choose password: ");
  if (!fgets(pass, sizeof(pass), stdin)) return;
  pass[strcspn(pass, "\r\n")] = 0;
  printf("Confirm password: ");
  if (!fgets(pass2, sizeof(pass2), stdin)) return;
  pass2[strcspn(pass2, "\r\n")] = 0;
  if (strcmp(pass, pass2) != 0) {
    printf("Passwords do not match.\n");
    return;
  }

  if (!append_user(username, pass)) {
    printf("Failed to save user (file error).\n");
  } else {
    printf("User '%s' registered successfully.\n", username);
  }
}

/* Login flow */
static int do_login(char* out_user, size_t outlen) {
  char username[MAX_USER_LEN];
  char pass[MAX_PASS_LEN];

  printf("=== Login ===\n");
  printf("Username: ");
  if (!fgets(username, sizeof(username), stdin)) return 0;
  username[strcspn(username, "\r\n")] = 0;
  printf("Password: ");
  if (!fgets(pass, sizeof(pass), stdin)) return 0;
  pass[strcspn(pass, "\r\n")] = 0;

  FILE* f = fopen(USER_DB_FILE, "r");
  if (!f) {
    printf("No users registered yet.\n");
    return 0;
  }

  char line[MAX_LINE], u[MAX_USER_LEN], p[MAX_PASS_LEN];
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%63s %127s", u, p) == 2) {
      if (strcmp(u, username) == 0) {
        fclose(f);
        if (strcmp(p, pass) == 0) {
          if (out_user) {
            strncpy(out_user, username, outlen - 1);
            out_user[outlen - 1] = 0;
          }
          printf("Login successful. Welcome, %s!\n", username);
          return 1;
        } else {
          printf("Wrong password.\n");
          return 0;
        }
      }
    }
  }
  fclose(f);
  printf("User not found.\n");
  return 0;
}

/* Forgot password: requires current password to set a new password.
   This keeps a minimal verification step without security questions. */
static void do_forgot(void) {
  char username[MAX_USER_LEN];
  char curpass[MAX_PASS_LEN];
  char newp[MAX_PASS_LEN], newp2[MAX_PASS_LEN];

  printf("=== Reset Password (requires current password) ===\n");
  printf("Username: ");
  if (!fgets(username, sizeof(username), stdin)) return;
  username[strcspn(username, "\r\n")] = 0;

  printf("Current password: ");
  if (!fgets(curpass, sizeof(curpass), stdin)) return;
  curpass[strcspn(curpass, "\r\n")] = 0;

  /* Verify current credentials */
  FILE* f = fopen(USER_DB_FILE, "r");
  if (!f) {
    printf("No users.\n");
    return;
  }
  char line[MAX_LINE], u[MAX_USER_LEN], p[MAX_PASS_LEN];
  int found = 0, ok = 0;
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "%63s %127s", u, p) == 2) {
      if (strcmp(u, username) == 0) {
        found = 1;
        if (strcmp(p, curpass) == 0) ok = 1;
        break;
      }
    }
  }
  fclose(f);

  if (!found) {
    printf("User not found.\n");
    return;
  }
  if (!ok) {
    printf("Current password is incorrect.\n");
    return;
  }

  /* Ask new password */
  printf("Enter new password: ");
  if (!fgets(newp, sizeof(newp), stdin)) return;
  newp[strcspn(newp, "\r\n")] = 0;
  printf("Confirm new password: ");
  if (!fgets(newp2, sizeof(newp2), stdin)) return;
  newp2[strcspn(newp2, "\r\n")] = 0;
  if (strcmp(newp, newp2) != 0) {
    printf("Passwords do not match.\n");
    return;
  }

  /* Rewrite file with updated password for the user */
  FILE* rf = fopen(USER_DB_FILE, "r");
  FILE* wf = fopen("users.tmp", "w");
  if (!rf || !wf) {
    if (rf) fclose(rf);
    if (wf) fclose(wf);
    printf("File error.\n");
    return;
  }

  while (fgets(line, sizeof(line), rf)) {
    if (sscanf(line, "%63s %127s", u, p) == 2) {
      if (strcmp(u, username) == 0) {
        fprintf(wf, "%s %s\n", u, newp);
      } else {
        fputs(line, wf);
      }
    } else {
      fputs(line, wf);
    }
  }

  fclose(rf);
  fclose(wf);
  remove(USER_DB_FILE);
  rename("users.tmp", USER_DB_FILE);
  printf("Password updated successfully.\n");
}

/* Authentication menu (minimal) */
int auth_menu(void) {
  while (1) {
    printf("\n=== Authentication ===\n");
    printf(
        "1) Login\n2) Register\n3) Forgot password (requires current "
        "password)\n4) Continue as guest\n5) Exit\n");
    printf("Choose: ");

    int choice = 0;
    if (scanf("%d", &choice) != 1) {
      while (getchar() != '\n'); /* clear bad input */
      continue;
    }
    while (getchar() != '\n'); /* clear newline */

    if (choice == 1) {
      char uname[MAX_USER_LEN];
      if (do_login(uname, sizeof(uname))) {
        strncpy(current_user, uname, sizeof(current_user) - 1);
        current_user[sizeof(current_user) - 1] = 0;
        return 1;
      }
    } else if (choice == 2) {
      do_register();
    } else if (choice == 3) {
      do_forgot();
    } else if (choice == 4) {
      strncpy(current_user, "guest", sizeof(current_user) - 1);
      current_user[sizeof(current_user) - 1] = 0;
      return 1;
    } else if (choice == 5) {
      return 0;
    }
  }
}

/* Return current logged-in user (or NULL) */
const char* auth_get_current_user(void) {
  return current_user[0] ? current_user : NULL;
}
