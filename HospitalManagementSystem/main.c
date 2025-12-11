#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define CLEAR_CMD "cls"
#define PAUSE_CMD "pause"
#else
#define CLEAR_CMD "clear"
#define PAUSE_CMD "" 
#endif

#define ADMIN_PASSWORD "admin123"  
#define RECEPTION_PASSWORD "rece123"  

struct Patient
{
    int id;
    char name[100];
    int age;
    char gender[20];
    char blood_group[5];  
    char mobile[15];      
};

struct Doctor
{
    int id;
    char name[100];
    char specialization[100];
    char department[100];
};

struct Staff
{
    int id;
    char name[100];
    char position[50];
    char department[100];
};

/* Function declarations */
int isPatientIdExists(int id);
int isDoctorIdExists(int id);
int isStaffIdExists(int id);
void savePatient(struct Patient p);
void showAllPatients();
void searchPatient();
void receptionPortal();
void patientPortal();
void viewPatientDetails();
void viewAppointments();
void viewLabReports();
void viewPrescriptions();
void doctorPortal();
void labPortal();
void adminPortal();
void findDoctorPortal();  
void contributionPortal();
void feedback();
void bookAppointment();
void writePrescription();
void enterLabReport();
void addStaff();
void removeStaff();
void updateMedicalRecord();
void printLabReport();
void editDatabase();
void addDoctor();
void removeDoctor();
void viewAllDoctors();
void viewAllStaff();
void editPatientRecord();
void deletePatientRecord();
void editAppointment();
void deleteAppointment();
void editLabReport();
void deleteLabReport();
void editPrescription();
void deletePrescription();
void editStaffRecord();
void deleteStaffRecord();
void editDoctorRecord();
void deleteDoctorRecord();
void viewMedicalRecords();
void viewAllAppointments();
void viewAllLabReports();
void viewAllPrescriptions();
void generateSystemReport();
int authenticateAdmin();
int authenticateReception();
void searchDoctorByName();
void searchDoctorBySpecialization();
char* getDoctorNameById(int id); 

void clearScreen()
{
    /* portable clear */
    system(CLEAR_CMD);
}

/* portable pause */
void pauseProg()
{
#ifdef _WIN32
    system(PAUSE_CMD);
#else
    printf("\nPress Enter to continue...");
    int c;
    /* consume leftover newline */
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
    getchar();
#endif
}

/* safe read line that trims trailing newline */
void read_line(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

/* helper to trim leading/trailing spaces (in place) */
void trim(char *s)
{
    /* trim leading */
    char *start = s;
    while (*start && (*start == ' ' || *start == '\t'))
        start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
    /* trim trailing */
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t'))
    {
        s[len - 1] = '\0';
        len--;
    }
}

/* Check if patient ID already exists */
int isPatientIdExists(int id)
{
    FILE *file = fopen("patients.txt", "r");
    if (!file)
        return 0;

    char line[512];
    while (fgets(line, sizeof(line), file))
    {
        char *tok = strtok(line, "|");
        if (tok && atoi(tok) == id)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Check if doctor ID already exists */
int isDoctorIdExists(int id)
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
        return 0;

    char line[512];
    while (fgets(line, sizeof(line), file))
    {
        char *tok = strtok(line, "|");
        if (tok && atoi(tok) == id)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Check if staff ID already exists */
int isStaffIdExists(int id)
{
    FILE *file = fopen("staff.txt", "r");
    if (!file)
        return 0;

    char line[512];
    while (fgets(line, sizeof(line), file))
    {
        char *tok = strtok(line, "|");
        if (tok && atoi(tok) == id)
        {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

/* Get doctor name by ID */
char* getDoctorNameById(int id)
{
    static char name[100] = "Unknown";
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
        return name;

    char line[512];
    while (fgets(line, sizeof(line), file))
    {
        char *tok = strtok(line, "|");
        if (!tok)
            continue;
        int doctorId = atoi(tok);
        if (doctorId == id)
        {
            tok = strtok(NULL, "|");
            if (tok)
            {
                strncpy(name, tok, sizeof(name) - 1);
                name[sizeof(name) - 1] = '\0';
                fclose(file);
                return name;
            }
        }
    }

    fclose(file);
    return name;
}

/* Admin authentication */
int authenticateAdmin()
{
    char password[50];
    int attempts = 3;
    
    while (attempts > 0)
    {
        printf("Enter admin password: ");
        read_line(password, sizeof(password));
        
        if (strcmp(password, ADMIN_PASSWORD) == 0)
        {
            return 1; // Authentication successful
        }
        else
        {
            attempts--;
            printf("Incorrect password. %d attempts remaining.\n", attempts);
        }
    }
    
    return 0; // Authentication failed
}

/* Reception authentication */
int authenticateReception()
{
    char password[50];
    int attempts = 3;
    
    while (attempts > 0)
    {
        printf("Enter reception password: ");
        read_line(password, sizeof(password));
        
        if (strcmp(password, RECEPTION_PASSWORD) == 0)
        {
            return 1; // Authentication successful
        }
        else
        {
            attempts--;
            printf("Incorrect password. %d attempts remaining.\n", attempts);
        }
    }
    
    return 0; // Authentication failed
}

/* Write patient to patients.txt as pipe-separated fields:
   id|name|age|gender|blood_group|mobile\n
*/
void savePatient(struct Patient p)
{
    /* Check if ID already exists */
    if (isPatientIdExists(p.id))
    {
        printf("\n\033[1;31mError: Patient ID %d already exists!\033[0m\n", p.id);
        pauseProg();
        return;
    }

    FILE *file = fopen("patients.txt", "a");
    if (!file)
    {
        perror("Error opening patients.txt for append");
        return;
    }
    /* Replace any '|' characters in fields to avoid corrupting delimiter */
    for (char *q = p.name; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = p.gender; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = p.blood_group; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = p.mobile; *q; ++q)
        if (*q == '|')
            *q = ' ';
    fprintf(file, "%d|%s|%d|%s|%s|%s\n", p.id, p.name, p.age, p.gender, p.blood_group, p.mobile);
    fclose(file);
}

void showAllPatients()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file)
    {
        printf("\nNo patient records found.\n");
        pauseProg();
        return;
    }

    char line[512];
    printf("\n==== Patient List ====\n\n");
    while (fgets(line, sizeof(line), file))
    {
        /* parse line: id|name|age|gender|blood_group|mobile */
        char *tok;
        int id;
        char name[100] = {0};
        int age;
        char gender[20] = {0};
        char blood_group[5] = {0};
        char mobile[15] = {0};

        /* get id */
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);

        tok = strtok(NULL, "|");
        if (tok)
            age = atoi(tok);
        else
            age = 0;

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(gender, tok, sizeof(gender) - 1);

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(blood_group, tok, sizeof(blood_group) - 1);

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(mobile, tok, sizeof(mobile) - 1);

        trim(name);
        trim(gender);
        trim(blood_group);
        trim(mobile);

        printf("ID: %d\nName: %s\nAge: %d\nGender: %s\nBlood Group: %s\nMobile: %s\n--------------\n",
               id, name, age, gender, blood_group, mobile);
    }

    fclose(file);
    pauseProg();
}

void searchPatient()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file)
    {
        printf("\nNo patient records found.\n");
        pauseProg();
        return;
    }

    int idToFind;
    printf("\nEnter Patient ID to search: ");
    if (scanf("%d", &idToFind) != 1)
    {
        while (getchar() != '\n')
        {
        }
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    } /* clear newline */

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0};
        int age;
        char gender[20] = {0};
        char blood_group[5] = {0};
        char mobile[15] = {0};

        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);

        if (id != idToFind)
            continue;

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);

        tok = strtok(NULL, "|");
        if (tok)
            age = atoi(tok);
        else
            age = 0;

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(gender, tok, sizeof(gender) - 1);

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(blood_group, tok, sizeof(blood_group) - 1);

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(mobile, tok, sizeof(mobile) - 1);

        trim(name);
        trim(gender);
        trim(blood_group);
        trim(mobile);

        printf("\nPatient Found!\n");
        printf("ID: %d\nName: %s\nAge: %d\nGender: %s\nBlood Group: %s\nMobile: %s\n", 
               id, name, age, gender, blood_group, mobile);
        found = 1;
        break;
    }

    if (!found)
        printf("\nPatient Not Found!\n");

    fclose(file);
    pauseProg();
}

void bookAppointment()
{
    int pid, did;
    char date[64], time[64];
    
    printf("\nEnter Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Patient ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        pauseProg();
        return;
    }
    
    printf("Enter Doctor ID: ");
    if (scanf("%d", &did) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Doctor ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate doctor ID */
    if (!isDoctorIdExists(did))
    {
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", did);
        pauseProg();
        return;
    }
    
    printf("Enter Appointment Date (DD/MM/YYYY): ");
    read_line(date, sizeof(date));
    
    printf("Enter Appointment Time (HH:MM): ");
    read_line(time, sizeof(time));
    
    trim(date);
    trim(time);
    
    FILE *file = fopen("appointments.txt", "a");
    if (!file)
    {
        perror("Error opening appointments.txt");
        pauseProg();
        return;
    }
    
    fprintf(file, "%d|%d|%s|%s\n", pid, did, date, time);
    fclose(file);
    
    printf("\nAppointment booked successfully!\n");
    pauseProg();
}

void receptionPortal()
{
    /* Authenticate reception before accessing portal */
    if (!authenticateReception())
    {
        printf("\nAuthentication failed. Access denied.\n");
        pauseProg();
        return;
    }
    
    int choice;

    while (1)
    {
        clearScreen();
        printf("\n==========\033[1;32m RECEPTION PORTAL \033[0m==========\n\n");
        printf("1. Register New Patient\n");
        printf("2. Show All Registered Patients\n");
        printf("3. Search Patient by ID\n");
        printf("4. Book Appointment\n");
        printf("5. Edit Patient Details\n\n");  
        printf("6. Back to Main Menu\n");
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
            {
            }
            choice = 0;
        }
        while (getchar() != '\n')
        {
        } /* consume newline */

        switch (choice)
        {
        case 1:
        {
            struct Patient p;
            char buffer[200];

            printf("\nEnter Patient ID: ");
            if (scanf("%d", &p.id) != 1)
            {
                while (getchar() != '\n')
                    ;
                printf("Invalid ID.\n");
                pauseProg();
                break;
            }
            while (getchar() != '\n')
            {
            }

            /* Check if ID already exists */
            if (isPatientIdExists(p.id))
            {
                printf("\n\033[1;31mError: Patient ID %d already exists!\033[0m\n", p.id);
                pauseProg();
                break;
            }

            printf("Enter Patient Name: ");
            read_line(p.name, sizeof(p.name));

            printf("Enter Age: ");
            if (scanf("%d", &p.age) != 1)
            {
                while (getchar() != '\n')
                    ;
                printf("Invalid Age.\n");
                pauseProg();
                break;
            }
            while (getchar() != '\n')
            {
            }

            printf("Enter Gender: ");
            read_line(p.gender, sizeof(p.gender));

            printf("Enter Blood Group: ");
            read_line(p.blood_group, sizeof(p.blood_group));

            printf("Enter Mobile Number: ");
            read_line(p.mobile, sizeof(p.mobile));

            trim(p.name);
            trim(p.gender);
            trim(p.blood_group);
            trim(p.mobile);

            savePatient(p);
            printf("\nPatient Registered Successfully!\n");
            pauseProg();
            break;
        }
        case 2:
            showAllPatients();
            break;

        case 3:
            searchPatient();
            break;
            
        case 4:
            bookAppointment();
            break;
            
        case 5:
            editPatientRecord();  // New option
            break;

        case 6:
            return;

        default:
            printf("\nInvalid choice!\n");
            pauseProg();
        }
    }
}

void viewPatientDetails()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file)
    {
        printf("\nNo patient records found.\n");
        pauseProg();
        return;
    }

    int id;
    printf("\nEnter your Patient ID: ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }

    /* Validate patient ID */
    if (!isPatientIdExists(id))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id);
        fclose(file);
        pauseProg();
        return;
    }

    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int pid;
        char name[100] = {0};
        int age;
        char gender[20] = {0};
        char blood_group[5] = {0};
        char mobile[15] = {0};

        tok = strtok(line, "|");
        if (!tok)
            continue;
        pid = atoi(tok);
        if (pid != id)
            continue;

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);

        tok = strtok(NULL, "|");
        if (tok)
            age = atoi(tok);
        else
            age = 0;

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(gender, tok, sizeof(gender) - 1);

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(blood_group, tok, sizeof(blood_group) - 1);

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(mobile, tok, sizeof(mobile) - 1);

        trim(name);
        trim(gender);
        trim(blood_group);
        trim(mobile);

        printf("\n====== Your Details ======\n\n");
        printf("ID        : %d\n", pid);
        printf("Name      : %s\n", name);
        printf("Age       : %d\n", age);
        printf("Gender    : %s\n", gender);
        printf("Blood Grp : %s\n", blood_group);
        printf("Mobile    : %s\n", mobile);
        found = 1;
        break;
    }

    if (!found)
        printf("\nNo record found for this ID.\n");

    fclose(file);
    pauseProg();
}

void viewAppointments()
{
    FILE *file = fopen("appointments.txt", "r");
    if (!file)
    {
        printf("\nNo appointments found.\n");
        pauseProg();
        return;
    }

    int pid;
    printf("\nEnter your Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }

    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }

    char line[512];
    int found = 0;
    printf("\n====== Your Appointments ======\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        int did;
        char date[64] = {0}, time[64] = {0};

        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        if (id != pid)
            continue;

        tok = strtok(NULL, "|");
        if (tok)
            did = atoi(tok);

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(date, tok, sizeof(date) - 1);

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(time, tok, sizeof(time) - 1);

        trim(date);
        trim(time);

        char* doctorName = getDoctorNameById(did);
        printf("Date   : %s\n", date);
        printf("Time   : %s\n", time);
        printf("Doctor : %s (ID: %d)\n", doctorName, did);
        printf("-----------------------------\n");
        found = 1;
    }

    if (!found)
        printf("No appointment booked.\n");

    fclose(file);
    pauseProg();
}

void viewLabReports()
{
    FILE *file = fopen("labreports.txt", "r");
    if (!file)
    {
        printf("\nNo lab reports found.\n");
        pauseProg();
        return;
    }

    int pid;
    printf("\nEnter your Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }

    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }

    char line[1024];
    int found = 0;
    printf("\n====== Your Lab Reports ======\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char report[900] = {0};

        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        if (id != pid)
            continue;

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(report, tok, sizeof(report) - 1);
        trim(report);

        printf("Report: %s\n", report);
        printf("-----------------------------\n");
        found = 1;
    }

    if (!found)
        printf("No lab report available.\n");

    fclose(file);
    pauseProg();
}

void viewPrescriptions()
{
    FILE *file = fopen("prescriptions.txt", "r");
    if (!file)
    {
        printf("\nNo prescriptions found.\n");
        pauseProg();
        return;
    }

    int pid;
    printf("\nEnter your Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }

    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }

    char line[1024];
    int found = 0;
    printf("\n====== Your Prescriptions ======\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char doctorIdStr[20] = {0};
        char pres[900] = {0};

        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        if (id != pid)
            continue;

        tok = strtok(NULL, "|");
        if (tok)
            strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(pres, tok, sizeof(pres) - 1);
        trim(pres);

        int doctorId = atoi(doctorIdStr);
        char* doctorName = getDoctorNameById(doctorId);
        printf("Doctor: %s (ID: %d)\n", doctorName, doctorId);
        printf("Prescription: %s\n", pres);
        printf("-----------------------------\n");
        found = 1;
    }

    if (!found)
        printf("No prescription available.\n");

    fclose(file);
    pauseProg();
}

void patientPortal()
{
    int choice;
    while (1)
    {
        clearScreen();
        printf("\n===============\033[1;32m PATIENT PORTAL \033[0m===============\n\n");
        printf("1. View My Details\n");
        printf("2. View Appointments\n");
        printf("3. View Lab Reports\n");
        printf("4. View Prescriptions\n\n");
        printf("5. Back to Main Menu\n");

        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }

        switch (choice)
        {
        case 1:
            viewPatientDetails();
            break;
        case 2:
            viewAppointments();
            break;
        case 3:
            viewLabReports();
            break;
        case 4:
            viewPrescriptions();
            break;
        case 5:
            return;
        default:
            printf("\nInvalid Input!\033[0m\n");
            pauseProg();
        }
    }
}

void writePrescription()
{
    int pid, did;
    char prescription[900];
    
    printf("\nEnter Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Patient ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        pauseProg();
        return;
    }
    
    printf("Enter Doctor ID: ");
    if (scanf("%d", &did) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Doctor ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate doctor ID */
    if (!isDoctorIdExists(did))
    {
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", did);
        pauseProg();
        return;
    }
    
    printf("Enter Prescription Details: ");
    read_line(prescription, sizeof(prescription));
    trim(prescription);
    
    FILE *file = fopen("prescriptions.txt", "a");
    if (!file)
    {
        perror("Error opening prescriptions.txt");
        pauseProg();
        return;
    }
    
    /* Replace any '|' characters to avoid corrupting delimiter */
    for (char *q = prescription; *q; ++q)
        if (*q == '|')
            *q = ' ';
    
    fprintf(file, "%d|%d|%s\n", pid, did, prescription);
    fclose(file);
    
    printf("\nPrescription saved successfully!\n");
    pauseProg();
}

void updateMedicalRecord()
{
    int pid, did;
    char record[900];
    
    printf("\nEnter Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Patient ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        pauseProg();
        return;
    }
    
    printf("Enter Doctor ID: ");
    if (scanf("%d", &did) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Doctor ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate doctor ID */
    if (!isDoctorIdExists(did))
    {
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", did);
        pauseProg();
        return;
    }
    
    printf("Enter Medical Record Details: ");
    read_line(record, sizeof(record));
    trim(record);
    
    FILE *file = fopen("medicalrecords.txt", "a");
    if (!file)
    {
        perror("Error opening medicalrecords.txt");
        pauseProg();
        return;
    }
    
    /* Replace any '|' characters to avoid corrupting delimiter */
    for (char *q = record; *q; ++q)
        if (*q == '|')
            *q = ' ';
    
    fprintf(file, "%d|%d|%s\n", pid, did, record);
    fclose(file);
    
    printf("\nMedical record updated successfully!\n");
    pauseProg();
}

void doctorPortal()
{
    int choice;
    
    while (1)
    {
        clearScreen();
        printf("\n========== \033[1;32m DOCTOR PORTAL \033[0m ==========\n\n");
        printf("1. View Patient List\n");
        printf("2. Write Prescription\n");
        printf("3. Update Medical Record\n");
        printf("4. View All Appointments\n");
        printf("5. View Medical Records\n\n");
        printf("6. Back to Main Menu\n");
        
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }
        
        switch (choice)
        {
        case 1:
            showAllPatients();
            break;
        case 2:
            writePrescription();
            break;
        case 3:
            updateMedicalRecord();
            break;
        case 4:
            viewAllAppointments();
            break;
        case 5:
            viewMedicalRecords();
            break;
        case 6:
            return;
        default:
            printf("\nInvalid choice!\n");
            pauseProg();
        }
    }
}

void enterLabReport()
{
    int pid;
    char report[900];
    
    printf("\nEnter Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Patient ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        pauseProg();
        return;
    }
    
    printf("Enter Lab Report Details: ");
    read_line(report, sizeof(report));
    trim(report);
    
    FILE *file = fopen("labreports.txt", "a");
    if (!file)
    {
        perror("Error opening labreports.txt");
        pauseProg();
        return;
    }
    
    /* Replace any '|' characters to avoid corrupting delimiter */
    for (char *q = report; *q; ++q)
        if (*q == '|')
            *q = ' ';
    
    fprintf(file, "%d|%s\n", pid, report);
    fclose(file);
    
    printf("\nLab report saved successfully!\n");
    pauseProg();
}

void printLabReport()
{
    int pid;
    char filename[100];
    FILE *srcFile, *destFile;
    char line[1024];
    int found = 0;
    
    printf("\nEnter Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid Patient ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        pauseProg();
        return;
    }
    
    sprintf(filename, "patient_%d_report.txt", pid);
    
    srcFile = fopen("labreports.txt", "r");
    if (!srcFile)
    {
        printf("\nNo lab reports found.\n");
        pauseProg();
        return;
    }
    
    destFile = fopen(filename, "w");
    if (!destFile)
    {
        perror("Error creating report file");
        fclose(srcFile);
        pauseProg();
        return;
    }
    
    fprintf(destFile, "LAB REPORT FOR PATIENT ID: %d\n", pid);
    fprintf(destFile, "=====================================\n\n");
    
    while (fgets(line, sizeof(line), srcFile))
    {
        char *tok;
        int id;
        char report[900] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        if (id != pid)
            continue;
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(report, tok, sizeof(report) - 1);
        trim(report);
        
        fprintf(destFile, "Report: %s\n", report);
        fprintf(destFile, "-------------------------------------\n");
        found = 1;
    }
    
    fclose(srcFile);
    fclose(destFile);
    
    if (found)
    {
        printf("\nLab report printed to file: %s\n", filename);
        
        // Display the report on screen
        printf("\n====== Lab Report Preview ======\n");
        srcFile = fopen(filename, "r");
        if (srcFile)
        {
            while (fgets(line, sizeof(line), srcFile))
            {
                printf("%s", line);
            }
            fclose(srcFile);
        }
    }
    else
    {
        printf("\nNo lab report found for this patient.\n");
        remove(filename);
    }
    
    pauseProg();
}

void labPortal()
{
    int choice;
    
    while (1)
    {
        clearScreen();
        printf("\n==========\033[1;32m LAB PORTAL\033[0m ==========\n\n");
        printf("1. Enter Test Report\n");
        printf("2. Print Report\n");
        printf("3. View All Lab Reports\n\n");
        printf("4. Back to Main Menu\n");
        
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }
        
        switch (choice)
        {
        case 1:
            enterLabReport();
            break;
        case 2:
            printLabReport();
            break;
        case 3:
            viewAllLabReports();
            break;
        case 4:
            return;
        default:
            printf("\nInvalid choice!\n");
            pauseProg();
        }
    }
}

void addStaff()
{
    struct Staff s;
    
    printf("\nEnter Staff ID: ");
    if (scanf("%d", &s.id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Check if ID already exists */
    if (isStaffIdExists(s.id))
    {
        printf("\n\033[1;31mError: Staff ID %d already exists!\033[0m\n", s.id);
        pauseProg();
        return;
    }
    
    printf("Enter Staff Name: ");
    read_line(s.name, sizeof(s.name));
    
    printf("Enter Staff Position: ");
    read_line(s.position, sizeof(s.position));
    
    printf("Enter Department: ");
    read_line(s.department, sizeof(s.department));
    
    trim(s.name);
    trim(s.position);
    trim(s.department);
    
    FILE *file = fopen("staff.txt", "a");
    if (!file)
    {
        perror("Error opening staff.txt");
        pauseProg();
        return;
    }
    
    /* Replace any '|' characters to avoid corrupting delimiter */
    for (char *q = s.name; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = s.position; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = s.department; *q; ++q)
        if (*q == '|')
            *q = ' ';
    
    fprintf(file, "%d|%s|%s|%s\n", s.id, s.name, s.position, s.department);
    fclose(file);
    
    printf("\nStaff member added successfully!\n");
    pauseProg();
}

void removeStaff()
{
    FILE *file = fopen("staff.txt", "r");
    if (!file)
    {
        printf("\nNo staff records found.\n");
        pauseProg();
        return;
    }
    
    int id;
    printf("\nEnter Staff ID to remove: ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate staff ID */
    if (!isStaffIdExists(id))
    {
        printf("\n\033[1;31mError: Staff ID %d does not exist!\033[0m\n", id);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display staff details for confirmation */
    char line[512];
    int found = 0;
    char name[100] = {0}, position[50] = {0}, department[100] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int sid;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        sid = atoi(tok);
        
        if (sid == id)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(name, tok, sizeof(name) - 1);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(position, tok, sizeof(position) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(department, tok, sizeof(department) - 1);
            
            trim(name);
            trim(position);
            trim(department);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nStaff member not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    printf("\nStaff Member Details:\n");
    printf("ID: %d\nName: %s\nPosition: %s\nDepartment: %s\n", id, name, position, department);
    
    printf("\nAre you sure you want to delete this staff member? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int sid;
        char sname[100] = {0}, sposition[50] = {0}, sdepartment[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        sid = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(sname, tok, sizeof(sname) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(sposition, tok, sizeof(sposition) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(sdepartment, tok, sizeof(sdepartment) - 1);
        
        trim(sname);
        trim(sposition);
        trim(sdepartment);
        
        if (sid == id)
        {
            found = 1;
            continue; /* Skip writing this line to effectively remove it */
        }
        
        fprintf(temp, "%d|%s|%s|%s\n", sid, sname, sposition, sdepartment);
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("staff.txt");
        rename("temp.txt", "staff.txt");
        printf("\nStaff member removed successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nStaff member not found!\n");
    }
    
    pauseProg();
}

void addDoctor()
{
    struct Doctor d;
    
    printf("\nEnter Doctor ID: ");
    if (scanf("%d", &d.id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid ID.\n");
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Check if ID already exists */
    if (isDoctorIdExists(d.id))
    {
        printf("\n\033[1;31mError: Doctor ID %d already exists!\033[0m\n", d.id);
        pauseProg();
        return;
    }
    
    printf("Enter Doctor Name: ");
    read_line(d.name, sizeof(d.name));
    
    printf("Enter Specialization: ");
    read_line(d.specialization, sizeof(d.specialization));
    
    printf("Enter Department: ");
    read_line(d.department, sizeof(d.department));
    
    trim(d.name);
    trim(d.specialization);
    trim(d.department);
    
    FILE *file = fopen("doctors.txt", "a");
    if (!file)
    {
        perror("Error opening doctors.txt");
        pauseProg();
        return;
    }
    
    /* Replace any '|' characters to avoid corrupting delimiter */
    for (char *q = d.name; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = d.specialization; *q; ++q)
        if (*q == '|')
            *q = ' ';
    for (char *q = d.department; *q; ++q)
        if (*q == '|')
            *q = ' ';
    
    fprintf(file, "%d|%s|%s|%s\n", d.id, d.name, d.specialization, d.department);
    fclose(file);
    
    printf("\nDoctor added successfully!\n");
    pauseProg();
}

void removeDoctor()
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
    {
        printf("\nNo doctor records found.\n");
        pauseProg();
        return;
    }
    
    int id;
    printf("\nEnter Doctor ID to remove: ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate doctor ID */
    if (!isDoctorIdExists(id))
    {
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", id);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display doctor details for confirmation */
    char line[512];
    int found = 0;
    char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int did;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        did = atoi(tok);
        
        if (did == id)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(name, tok, sizeof(name) - 1);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(specialization, tok, sizeof(specialization) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(department, tok, sizeof(department) - 1);
            
            trim(name);
            trim(specialization);
            trim(department);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nDoctor not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    printf("\nDoctor Details:\n");
    printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n", id, name, specialization, department);
    
    printf("\nAre you sure you want to delete this doctor? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int did;
        char dname[100] = {0}, dspecialization[100] = {0}, ddepartment[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        did = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(dname, tok, sizeof(dname) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(dspecialization, tok, sizeof(dspecialization) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(ddepartment, tok, sizeof(ddepartment) - 1);
        
        trim(dname);
        trim(dspecialization);
        trim(ddepartment);
        
        if (did == id)
        {
            found = 1;
            continue; /* Skip writing this line to effectively remove it */
        }
        
        fprintf(temp, "%d|%s|%s|%s\n", did, dname, dspecialization, ddepartment);
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("doctors.txt");
        rename("temp.txt", "doctors.txt");
        printf("\nDoctor removed successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nDoctor not found!\n");
    }
    
    pauseProg();
}

void viewAllDoctors()
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
    {
        printf("\nNo doctor records found.\n");
        pauseProg();
        return;
    }
    
    char line[512];
    printf("\n==== Doctor List ====\n\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(specialization, tok, sizeof(specialization) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(department, tok, sizeof(department) - 1);
        
        trim(name);
        trim(specialization);
        trim(department);
        
        printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n--------------\n",
               id, name, specialization, department);
    }
    
    fclose(file);
    pauseProg();
}

void viewAllStaff()
{
    FILE *file = fopen("staff.txt", "r");
    if (!file)
    {
        printf("\nNo staff records found.\n");
        pauseProg();
        return;
    }
    
    char line[512];
    printf("\n==== Staff List ====\n\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0}, position[50] = {0}, department[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(position, tok, sizeof(position) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(department, tok, sizeof(department) - 1);
        
        trim(name);
        trim(position);
        trim(department);
        
        printf("ID: %d\nName: %s\nPosition: %s\nDepartment: %s\n--------------\n",
               id, name, position, department);
    }
    
    fclose(file);
    pauseProg();
}

void viewAllAppointments()
{
    FILE *file = fopen("appointments.txt", "r");
    if (!file)
    {
        printf("\nNo appointments found.\n");
        pauseProg();
        return;
    }
    
    char line[512];
    printf("\n==== All Appointments ====\n\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int pid;
        int did;
        char date[64] = {0}, time[64] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        pid = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            did = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(date, tok, sizeof(date) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(time, tok, sizeof(time) - 1);
        
        trim(date);
        trim(time);
        
        char* doctorName = getDoctorNameById(did);
        printf("Patient ID: %d\n", pid);
        printf("Doctor: %s (ID: %d)\n", doctorName, did);
        printf("Date: %s\n", date);
        printf("Time: %s\n", time);
        printf("--------------\n");
    }
    
    fclose(file);
    pauseProg();
}

void viewAllLabReports()
{
    FILE *file = fopen("labreports.txt", "r");
    if (!file)
    {
        printf("\nNo lab reports found.\n");
        pauseProg();
        return;
    }
    
    char line[1024];
    printf("\n==== All Lab Reports ====\n\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int pid;
        char report[900] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        pid = atoi(tok);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(report, tok, sizeof(report) - 1);
        trim(report);
        
        printf("Patient ID: %d\n", pid);
        printf("Report: %s\n", report);
        printf("--------------\n");
    }
    
    fclose(file);
    pauseProg();
}

void viewAllPrescriptions()
{
    FILE *file = fopen("prescriptions.txt", "r");
    if (!file)
    {
        printf("\nNo prescriptions found.\n");
        pauseProg();
        return;
    }
    
    char line[1024];
    printf("\n==== All Prescriptions ====\n\n");
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int pid;
        char doctorIdStr[20] = {0};
        char pres[900] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        pid = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(pres, tok, sizeof(pres) - 1);
        trim(pres);
        
        int doctorId = atoi(doctorIdStr);
        char* doctorName = getDoctorNameById(doctorId);
        printf("Patient ID: %d\n", pid);
        printf("Doctor: %s (ID: %d)\n", doctorName, doctorId);
        printf("Prescription: %s\n", pres);
        printf("--------------\n");
    }
    
    fclose(file);
    pauseProg();
}

void viewMedicalRecords()
{
    FILE *file = fopen("medicalrecords.txt", "r");
    if (!file)
    {
        printf("\nNo medical records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[1024];
    int found = 0;
    printf("\n====== Medical Records for Patient ID: %d ======\n", pid);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char doctorIdStr[20] = {0};
        char record[900] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        if (id != pid)
            continue;
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(record, tok, sizeof(record) - 1);
        trim(record);
        
        int doctorId = atoi(doctorIdStr);
        char* doctorName = getDoctorNameById(doctorId);
        printf("Doctor: %s (ID: %d)\n", doctorName, doctorId);
        printf("Record: %s\n", record);
        printf("-----------------------------\n");
        found = 1;
    }
    
    if (!found)
        printf("No medical records available for this patient.\n");
    
    fclose(file);
    pauseProg();
}

void editPatientRecord()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file)
    {
        printf("\nNo patient records found.\n");
        pauseProg();
        return;
    }
    
    int idToEdit;
    printf("\nEnter Patient ID to edit: ");
    if (scanf("%d", &idToEdit) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(idToEdit))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", idToEdit);
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0};
        int age;
        char gender[20] = {0};
        char blood_group[5] = {0};
        char mobile[15] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            age = atoi(tok);
        else
            age = 0;
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(gender, tok, sizeof(gender) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(blood_group, tok, sizeof(blood_group) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(mobile, tok, sizeof(mobile) - 1);
        
        trim(name);
        trim(gender);
        trim(blood_group);
        trim(mobile);
        
        if (id == idToEdit)
        {
            found = 1;
            printf("\nCurrent Details:\n");
            printf("ID: %d\nName: %s\nAge: %d\nGender: %s\nBlood Group: %s\nMobile: %s\n", 
                   id, name, age, gender, blood_group, mobile);
            
            printf("\nEnter new Name (leave blank to keep current): ");
            char newName[100];
            read_line(newName, sizeof(newName));
            trim(newName);
            if (strlen(newName) > 0)
                strcpy(name, newName);
            
            printf("Enter new Age (0 to keep current): ");
            int newAge;
            if (scanf("%d", &newAge) != 1)
            {
                while (getchar() != '\n')
                    ;
                newAge = age; // keep current
            }
            else
            {
                while (getchar() != '\n')
                    ;
                if (newAge > 0)
                    age = newAge;
            }
            
            printf("Enter new Gender (leave blank to keep current): ");
            char newGender[20];
            read_line(newGender, sizeof(newGender));
            trim(newGender);
            if (strlen(newGender) > 0)
                strcpy(gender, newGender);
            
            printf("Enter new Blood Group (leave blank to keep current): ");
            char newBloodGroup[5];
            read_line(newBloodGroup, sizeof(newBloodGroup));
            trim(newBloodGroup);
            if (strlen(newBloodGroup) > 0)
                strcpy(blood_group, newBloodGroup);
            
            printf("Enter new Mobile Number (leave blank to keep current): ");
            char newMobile[15];
            read_line(newMobile, sizeof(newMobile));
            trim(newMobile);
            if (strlen(newMobile) > 0)
                strcpy(mobile, newMobile);
            
            // Write the updated record
            fprintf(temp, "%d|%s|%d|%s|%s|%s\n", id, name, age, gender, blood_group, mobile);
        }
        else
        {
            // Write the original line
            fprintf(temp, "%d|%s|%d|%s|%s|%s\n", id, name, age, gender, blood_group, mobile);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("patients.txt");
        rename("temp.txt", "patients.txt");
        printf("\nPatient record updated successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nPatient not found!\n");
    }
    
    pauseProg();
}

void deletePatientRecord()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file)
    {
        printf("\nNo patient records found.\n");
        pauseProg();
        return;
    }
    
    int idToDelete;
    printf("\nEnter Patient ID to delete: ");
    if (scanf("%d", &idToDelete) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(idToDelete))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", idToDelete);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display patient details for confirmation */
    char line[512];
    int found = 0;
    char name[100] = {0};
    int age = 0;
    char gender[20] = {0};
    char blood_group[5] = {0};
    char mobile[15] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == idToDelete)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(name, tok, sizeof(name) - 1);
            
            tok = strtok(NULL, "|");
            if (tok)
                age = atoi(tok);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(gender, tok, sizeof(gender) - 1);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(blood_group, tok, sizeof(blood_group) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(mobile, tok, sizeof(mobile) - 1);
            
            trim(name);
            trim(gender);
            trim(blood_group);
            trim(mobile);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nPatient not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    printf("\nPatient Details:\n");
    printf("ID: %d\nName: %s\nAge: %d\nGender: %s\nBlood Group: %s\nMobile: %s\n", 
           idToDelete, name, age, gender, blood_group, mobile);
    
    printf("\nAre you sure you want to delete this patient record? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == idToDelete)
        {
            found = 1;
            continue; // skip writing this line
        }
        else
        {
            fputs(line, temp);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("patients.txt");
        rename("temp.txt", "patients.txt");
        printf("\nPatient record deleted successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nPatient not found!\n");
    }
    
    pauseProg();
}

void editAppointment()
{
    FILE *file = fopen("appointments.txt", "r");
    if (!file)
    {
        printf("\nNo appointment records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID for the appointment to edit: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        int did;
        char date[64] = {0}, time[64] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            did = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(date, tok, sizeof(date) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(time, tok, sizeof(time) - 1);
        
        trim(date);
        trim(time);
        
        if (id == pid)
        {
            found = 1;
            char* doctorName = getDoctorNameById(did);
            printf("\nCurrent Appointment Details:\n");
            printf("Patient ID: %d\nDoctor: %s (ID: %d)\nDate: %s\nTime: %s\n", id, doctorName, did, date, time);
            
            printf("\nEnter new Doctor ID (0 to keep current): ");
            int newDid;
            if (scanf("%d", &newDid) != 1)
            {
                while (getchar() != '\n')
                    ;
                newDid = did; // keep current
            }
            else
            {
                while (getchar() != '\n')
                    ;
                if (newDid > 0 && isDoctorIdExists(newDid))
                    did = newDid;
                else if (newDid > 0)
                    printf("\nWarning: Doctor ID %d does not exist. Keeping current doctor ID.\n", newDid);
            }
            
            printf("Enter new Date (DD/MM/YYYY) (leave blank to keep current): ");
            char newDate[64];
            read_line(newDate, sizeof(newDate));
            trim(newDate);
            if (strlen(newDate) > 0)
                strcpy(date, newDate);
            
            printf("Enter new Time (HH:MM) (leave blank to keep current): ");
            char newTime[64];
            read_line(newTime, sizeof(newTime));
            trim(newTime);
            if (strlen(newTime) > 0)
                strcpy(time, newTime);
            
            // Write the updated record
            fprintf(temp, "%d|%d|%s|%s\n", id, did, date, time);
        }
        else
        {
            // Write the original line
            fprintf(temp, "%d|%d|%s|%s\n", id, did, date, time);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("appointments.txt");
        rename("temp.txt", "appointments.txt");
        printf("\nAppointment updated successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nAppointment not found!\n");
    }
    
    pauseProg();
}

void deleteAppointment()
{
    FILE *file = fopen("appointments.txt", "r");
    if (!file)
    {
        printf("\nNo appointment records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID for the appointment to delete: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display appointment details for confirmation */
    char line[512];
    int found = 0;
    int did;
    char date[64] = {0}, time[64] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == pid)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                did = atoi(tok);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(date, tok, sizeof(date) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(time, tok, sizeof(time) - 1);
            
            trim(date);
            trim(time);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nAppointment not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    char* doctorName = getDoctorNameById(did);
    printf("\nAppointment Details:\n");
    printf("Patient ID: %d\nDoctor: %s (ID: %d)\nDate: %s\nTime: %s\n", pid, doctorName, did, date, time);
    
    printf("\nAre you sure you want to delete this appointment? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == pid)
        {
            found = 1;
            continue; // skip writing this line
        }
        else
        {
            fputs(line, temp);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("appointments.txt");
        rename("temp.txt", "appointments.txt");
        printf("\nAppointment deleted successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nAppointment not found!\n");
    }
    
    pauseProg();
}

void editLabReport()
{
    FILE *file = fopen("labreports.txt", "r");
    if (!file)
    {
        printf("\nNo lab report records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID for the lab report to edit: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char report[900] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(report, tok, sizeof(report) - 1);
        trim(report);
        
        if (id == pid)
        {
            found = 1;
            printf("\nCurrent Lab Report:\n");
            printf("Patient ID: %d\nReport: %s\n", id, report);
            
            printf("\nEnter new Report Details (leave blank to keep current): ");
            char newReport[900];
            read_line(newReport, sizeof(newReport));
            trim(newReport);
            if (strlen(newReport) > 0)
                strcpy(report, newReport);
            
            // Write the updated record
            fprintf(temp, "%d|%s\n", id, report);
        }
        else
        {
            // Write the original line
            fprintf(temp, "%d|%s\n", id, report);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("labreports.txt");
        rename("temp.txt", "labreports.txt");
        printf("\nLab report updated successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nLab report not found!\n");
    }
    
    pauseProg();
}

void deleteLabReport()
{
    FILE *file = fopen("labreports.txt", "r");
    if (!file)
    {
        printf("\nNo lab report records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID for the lab report to delete: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display lab report details for confirmation */
    char line[1024];
    int found = 0;
    char report[900] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == pid)
        {
            found = 1;
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(report, tok, sizeof(report) - 1);
            
            trim(report);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nLab report not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    printf("\nLab Report Details:\n");
    printf("Patient ID: %d\nReport: %s\n", pid, report);
    
    printf("\nAre you sure you want to delete this lab report? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == pid)
        {
            found = 1;
            continue; // skip writing this line
        }
        else
        {
            fputs(line, temp);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("labreports.txt");
        rename("temp.txt", "labreports.txt");
        printf("\nLab report deleted successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nLab report not found!\n");
    }
    
    pauseProg();
}

void editPrescription()
{
    FILE *file = fopen("prescriptions.txt", "r");
    if (!file)
    {
        printf("\nNo prescription records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID for the prescription to edit: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[1024];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char doctorIdStr[20] = {0};
        char prescription[900] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(prescription, tok, sizeof(prescription) - 1);
        trim(prescription);
        
        if (id == pid)
        {
            found = 1;
            int doctorId = atoi(doctorIdStr);
            char* doctorName = getDoctorNameById(doctorId);
            printf("\nCurrent Prescription:\n");
            printf("Patient ID: %d\nDoctor: %s (ID: %d)\nPrescription: %s\n", id, doctorName, doctorId, prescription);
            
            printf("\nEnter new Doctor ID (0 to keep current): ");
            int newDoctorId;
            if (scanf("%d", &newDoctorId) != 1)
            {
                while (getchar() != '\n')
                    ;
                newDoctorId = doctorId; // keep current
            }
            else
            {
                while (getchar() != '\n')
                    ;
                if (newDoctorId > 0 && isDoctorIdExists(newDoctorId))
                    doctorId = newDoctorId;
                else if (newDoctorId > 0)
                    printf("\nWarning: Doctor ID %d does not exist. Keeping current doctor ID.\n", newDoctorId);
            }
            
            printf("Enter new Prescription Details (leave blank to keep current): ");
            char newPrescription[900];
            read_line(newPrescription, sizeof(newPrescription));
            trim(newPrescription);
            if (strlen(newPrescription) > 0)
                strcpy(prescription, newPrescription);
            
            // Write the updated record
            fprintf(temp, "%d|%d|%s\n", id, doctorId, prescription);
        }
        else
        {
            // Write the original line
            fprintf(temp, "%d|%s|%s\n", id, doctorIdStr, prescription);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("prescriptions.txt");
        rename("temp.txt", "prescriptions.txt");
        printf("\nPrescription updated successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nPrescription not found!\n");
    }
    
    pauseProg();
}

void deletePrescription()
{
    FILE *file = fopen("prescriptions.txt", "r");
    if (!file)
    {
        printf("\nNo prescription records found.\n");
        pauseProg();
        return;
    }
    
    int pid;
    printf("\nEnter Patient ID for the prescription to delete: ");
    if (scanf("%d", &pid) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate patient ID */
    if (!isPatientIdExists(pid))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display prescription details for confirmation */
    char line[1024];
    int found = 0;
    char doctorIdStr[20] = {0};
    char prescription[900] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == pid)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(prescription, tok, sizeof(prescription) - 1);
            
            trim(prescription);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nPrescription not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    int doctorId = atoi(doctorIdStr);
    char* doctorName = getDoctorNameById(doctorId);
    printf("\nPrescription Details:\n");
    printf("Patient ID: %d\nDoctor: %s (ID: %d)\nPrescription: %s\n", pid, doctorName, doctorId, prescription);
    
    printf("\nAre you sure you want to delete this prescription? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == pid)
        {
            found = 1;
            continue; // skip writing this line
        }
        else
        {
            fputs(line, temp);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("prescriptions.txt");
        rename("temp.txt", "prescriptions.txt");
        printf("\nPrescription deleted successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nPrescription not found!\n");
    }
    
    pauseProg();
}

void editStaffRecord()
{
    FILE *file = fopen("staff.txt", "r");
    if (!file)
    {
        printf("\nNo staff records found.\n");
        pauseProg();
        return;
    }
    
    int idToEdit;
    printf("\nEnter Staff ID to edit: ");
    if (scanf("%d", &idToEdit) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate staff ID */
    if (!isStaffIdExists(idToEdit))
    {
        printf("\n\033[1;31mError: Staff ID %d does not exist!\033[0m\n", idToEdit);
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0}, position[50] = {0}, department[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(position, tok, sizeof(position) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(department, tok, sizeof(department) - 1);
        
        trim(name);
        trim(position);
        trim(department);
        
        if (id == idToEdit)
        {
            found = 1;
            printf("\nCurrent Details:\n");
            printf("ID: %d\nName: %s\nPosition: %s\nDepartment: %s\n", id, name, position, department);
            
            printf("\nEnter new Name (leave blank to keep current): ");
            char newName[100];
            read_line(newName, sizeof(newName));
            trim(newName);
            if (strlen(newName) > 0)
                strcpy(name, newName);
            
            printf("Enter new Position (leave blank to keep current): ");
            char newPosition[50];
            read_line(newPosition, sizeof(newPosition));
            trim(newPosition);
            if (strlen(newPosition) > 0)
                strcpy(position, newPosition);
            
            printf("Enter new Department (leave blank to keep current): ");
            char newDepartment[100];
            read_line(newDepartment, sizeof(newDepartment));
            trim(newDepartment);
            if (strlen(newDepartment) > 0)
                strcpy(department, newDepartment);
            
            // Write the updated record
            fprintf(temp, "%d|%s|%s|%s\n", id, name, position, department);
        }
        else
        {
            // Write the original line
            fprintf(temp, "%d|%s|%s|%s\n", id, name, position, department);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("staff.txt");
        rename("temp.txt", "staff.txt");
        printf("\nStaff record updated successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nStaff not found!\n");
    }
    
    pauseProg();
}

void deleteStaffRecord()
{
    FILE *file = fopen("staff.txt", "r");
    if (!file)
    {
        printf("\nNo staff records found.\n");
        pauseProg();
        return;
    }
    
    int idToDelete;
    printf("\nEnter Staff ID to delete: ");
    if (scanf("%d", &idToDelete) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate staff ID */
    if (!isStaffIdExists(idToDelete))
    {
        printf("\n\033[1;31mError: Staff ID %d does not exist!\033[0m\n", idToDelete);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display staff details for confirmation */
    char line[512];
    int found = 0;
    char name[100] = {0}, position[50] = {0}, department[100] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == idToDelete)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(name, tok, sizeof(name) - 1);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(position, tok, sizeof(position) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(department, tok, sizeof(department) - 1);
            
            trim(name);
            trim(position);
            trim(department);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nStaff member not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    printf("\nStaff Member Details:\n");
    printf("ID: %d\nName: %s\nPosition: %s\nDepartment: %s\n", idToDelete, name, position, department);
    
    printf("\nAre you sure you want to delete this staff member? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char sname[100] = {0}, sposition[50] = {0}, sdepartment[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(sname, tok, sizeof(sname) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(sposition, tok, sizeof(sposition) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(sdepartment, tok, sizeof(sdepartment) - 1);
        
        trim(sname);
        trim(sposition);
        trim(sdepartment);
        
        if (id == idToDelete)
        {
            found = 1;
            continue; /* Skip writing this line to effectively remove it */
        }
        
        fprintf(temp, "%d|%s|%s|%s\n", id, sname, sposition, sdepartment);
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("staff.txt");
        rename("temp.txt", "staff.txt");
        printf("\nStaff member removed successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nStaff member not found!\n");
    }
    
    pauseProg();
}

void editDoctorRecord()
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
    {
        printf("\nNo doctor records found.\n");
        pauseProg();
        return;
    }
    
    int idToEdit;
    printf("\nEnter Doctor ID to edit: ");
    if (scanf("%d", &idToEdit) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate doctor ID */
    if (!isDoctorIdExists(idToEdit))
    {
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", idToEdit);
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(specialization, tok, sizeof(specialization) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(department, tok, sizeof(department) - 1);
        
        trim(name);
        trim(specialization);
        trim(department);
        
        if (id == idToEdit)
        {
            found = 1;
            printf("\nCurrent Details:\n");
            printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n", id, name, specialization, department);
            
            printf("\nEnter new Name (leave blank to keep current): ");
            char newName[100];
            read_line(newName, sizeof(newName));
            trim(newName);
            if (strlen(newName) > 0)
                strcpy(name, newName);
            
            printf("Enter new Specialization (leave blank to keep current): ");
            char newSpecialization[100];
            read_line(newSpecialization, sizeof(newSpecialization));
            trim(newSpecialization);
            if (strlen(newSpecialization) > 0)
                strcpy(specialization, newSpecialization);
            
            printf("Enter new Department (leave blank to keep current): ");
            char newDepartment[100];
            read_line(newDepartment, sizeof(newDepartment));
            trim(newDepartment);
            if (strlen(newDepartment) > 0)
                strcpy(department, newDepartment);
            
            // Write the updated record
            fprintf(temp, "%d|%s|%s|%s\n", id, name, specialization, department);
        }
        else
        {
            // Write the original line
            fprintf(temp, "%d|%s|%s|%s\n", id, name, specialization, department);
        }
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("doctors.txt");
        rename("temp.txt", "doctors.txt");
        printf("\nDoctor record updated successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nDoctor not found!\n");
    }
    
    pauseProg();
}

void deleteDoctorRecord()
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
    {
        printf("\nNo doctor records found.\n");
        pauseProg();
        return;
    }
    
    int idToDelete;
    printf("\nEnter Doctor ID to delete: ");
    if (scanf("%d", &idToDelete) != 1)
    {
        while (getchar() != '\n')
            ;
        printf("Invalid input.\n");
        fclose(file);
        pauseProg();
        return;
    }
    while (getchar() != '\n')
    {
    }
    
    /* Validate doctor ID */
    if (!isDoctorIdExists(idToDelete))
    {
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", idToDelete);
        fclose(file);
        pauseProg();
        return;
    }
    
    /* Display doctor details for confirmation */
    char line[512];
    int found = 0;
    char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
    
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        if (id == idToDelete)
        {
            found = 1;
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(name, tok, sizeof(name) - 1);
            
            tok = strtok(NULL, "|");
            if (tok)
                strncpy(specialization, tok, sizeof(specialization) - 1);
            
            tok = strtok(NULL, "|\n");
            if (tok)
                strncpy(department, tok, sizeof(department) - 1);
            
            trim(name);
            trim(specialization);
            trim(department);
            
            break;
        }
    }
    
    if (!found)
    {
        printf("\nDoctor not found!\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    printf("\nDoctor Details:\n");
    printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n", idToDelete, name, specialization, department);
    
    printf("\nAre you sure you want to delete this doctor? (y/n): ");
    char confirm;
    scanf(" %c", &confirm);
    while (getchar() != '\n')
        ;
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("\nDeletion cancelled.\n");
        fclose(file);
        pauseProg();
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (!temp)
    {
        perror("Error creating temporary file");
        fclose(file);
        pauseProg();
        return;
    }
    
    rewind(file);
    found = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char dname[100] = {0}, dspecialization[100] = {0}, ddepartment[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(dname, tok, sizeof(dname) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(dspecialization, tok, sizeof(dspecialization) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(ddepartment, tok, sizeof(ddepartment) - 1);
        
        trim(dname);
        trim(dspecialization);
        trim(ddepartment);
        
        if (id == idToDelete)
        {
            found = 1;
            continue; /* Skip writing this line to effectively remove it */
        }
        
        fprintf(temp, "%d|%s|%s|%s\n", id, dname, dspecialization, ddepartment);
    }
    
    fclose(file);
    fclose(temp);
    
    if (found)
    {
        remove("doctors.txt");
        rename("temp.txt", "doctors.txt");
        printf("\nDoctor removed successfully!\n");
    }
    else
    {
        remove("temp.txt");
        printf("\nDoctor not found!\n");
    }
    
    pauseProg();
}

void generateSystemReport()
{
    FILE *patients = fopen("patients.txt", "r");
    FILE *doctors = fopen("doctors.txt", "r");
    FILE *staff = fopen("staff.txt", "r");
    FILE *appointments = fopen("appointments.txt", "r");
    FILE *labreports = fopen("labreports.txt", "r");
    FILE *prescriptions = fopen("prescriptions.txt", "r");
    
    int patientCount = 0, doctorCount = 0, staffCount = 0;
    int appointmentCount = 0, labReportCount = 0, prescriptionCount = 0;
    
    char line[1024];
    
    // Count patients
    if (patients)
    {
        while (fgets(line, sizeof(line), patients))
            patientCount++;
        fclose(patients);
    }
    
    // Count doctors
    if (doctors)
    {
        while (fgets(line, sizeof(line), doctors))
            doctorCount++;
        fclose(doctors);
    }
    
    // Count staff
    if (staff)
    {
        while (fgets(line, sizeof(line), staff))
            staffCount++;
        fclose(staff);
    }
    
    // Count appointments
    if (appointments)
    {
        while (fgets(line, sizeof(line), appointments))
            appointmentCount++;
        fclose(appointments);
    }
    
    // Count lab reports
    if (labreports)
    {
        while (fgets(line, sizeof(line), labreports))
            labReportCount++;
        fclose(labreports);
    }
    
    // Count prescriptions
    if (prescriptions)
    {
        while (fgets(line, sizeof(line), prescriptions))
            prescriptionCount++;
        fclose(prescriptions);
    }
    
    clearScreen();
    printf("\n========== SYSTEM REPORT ==========\n\n");
    printf("Total Patients: %d\n", patientCount);
    printf("Total Doctors: %d\n", doctorCount);
    printf("Total Staff: %d\n", staffCount);
    printf("Total Appointments: %d\n", appointmentCount);
    printf("Total Lab Reports: %d\n", labReportCount);
    printf("Total Prescriptions: %d\n", prescriptionCount);
    
    pauseProg();
}

void viewFeedback()
{
    FILE *file = fopen("feedback.txt", "r");
    if (!file)
    {
        printf("\nNo feedback records found.\n");
        pauseProg();
        return;
    }

    char line[1024];
    int found = 0;
    int feedbackCount = 0;
    printf("\n==== All Feedback ====\n\n");
    
    // First, count the total number of feedback entries
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        feedbackCount++;
    }
    
    printf("Total Feedback Entries: %d\n\n", feedbackCount);
    
    // Now display each feedback entry
    rewind(file);
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char feedback[900] = {0};

        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);

        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(feedback, tok, sizeof(feedback) - 1);
        trim(feedback);

        printf("Feedback Entry:\n");
        if (id == 0)
        {
            printf("From: Anonymous User\n");
        }
        else
        {
            // Check if patient exists
            if (isPatientIdExists(id))
            {
                printf("From: Patient ID %d\n", id);
            }
            else
            {
                printf("From: Patient ID %d (No longer in system)\n", id);
            }
        }
        printf("Feedback: %s\n", feedback);
        printf("-----------------------------\n");
        found = 1;
    }

    fclose(file);
    
    if (!found)
    {
        printf("No feedback available.\n");
    }
    
    pauseProg();
    return;
}

void editDatabase()
{
    int choice;
    
    while (1)
    {
        clearScreen();
        printf("==========\033[1;32m EDIT DATABASE \033[0m==========\n\n");

        printf("1. View All Patients\n");
        printf("2. Edit Patient Record\n");
        printf("3. Delete Patient Record\n\n");

        printf("4. View All Appointments\n");
        printf("5. Edit Appointment\n");
        printf("6. Delete Appointment\n\n");

        printf("7. View All Lab Reports\n");
        printf("8. Edit Lab Report\n");
        printf("9. Delete Lab Report\n\n");
        printf("10. View All Prescriptions\n");
        
        printf("11. Edit Prescription\n");
        printf("12. Delete Prescription\n\n");
        printf("13. View All Staff\n");
        printf("14. Edit Staff Record\n");
        printf("15. Delete Staff Record\n\n");

        printf("16. View All Doctors\n");
        printf("17. Edit Doctor Record\n");
        printf("18. Delete Doctor Record\n\n");

        printf("19. Generate System Report\n\n");

        printf("20. Back to Admin Portal\n");
        
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }
        
        switch (choice)
        {
        case 1:
            showAllPatients();
            break;
        case 2:
            editPatientRecord();
            break;
        case 3:
            deletePatientRecord();
            break;
        case 4:
            viewAllAppointments();
            break;
        case 5:
            editAppointment();
            break;
        case 6:
            deleteAppointment();
            break;
        case 7:
            viewAllLabReports();
            break;
        case 8:
            editLabReport();
            break;
        case 9:
            deleteLabReport();
            break;
        case 10:
            viewAllPrescriptions();
            break;
        case 11:
            editPrescription();
            break;
        case 12:
            deletePrescription();
            break;
        case 13:
            viewAllStaff();
            break;
        case 14:
            editStaffRecord();
            break;
        case 15:
            deleteStaffRecord();
            break;
        case 16:
            viewAllDoctors();
            break;
        case 17:
            editDoctorRecord();
            break;
        case 18:
            deleteDoctorRecord();
            break;
        case 19:
            generateSystemReport();
            break;
        case 20:
            return;
        default:
            printf("\nInvalid choice!\n");
            pauseProg();
        }
    }
}

void adminPortal()
{
    /* Authenticate admin before accessing portal */
    if (!authenticateAdmin())
    {
        printf("\nAuthentication failed. Access denied.\n");
        pauseProg();
        return;
    }
    
    int choice;
    
    while (1)
    {
        clearScreen();
        printf("\n==========\033[1;32m ADMIN PORTAL \033[0m==========\n\n");
        printf("1. Add Staff\n");
        printf("2. Remove Staff\n\n");
        printf("3. Add Doctor\n");
        printf("4. Remove Doctor\n\n");
        printf("5. Edit Database\n");
        printf("6. View Feedbacks\n\n");
        printf("7. Back to Main Menu\n");
        
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }
        
        switch (choice)
        {
        case 1:
            addStaff();
            break;
        case 2:
            removeStaff();
            break;
        case 3:
            addDoctor();
            break;
        case 4:
            removeDoctor();
            break;
        case 5:
            editDatabase();
            break;
        case 6:
            viewFeedback();
            break;
        case 7:
            return;
        default:
            printf("\nInvalid choice!\n");
            pauseProg();
        }
    }
}

void searchDoctorByName()
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
    {
        printf("\nNo doctor records found.\n");
        pauseProg();
        return;
    }
    
    char searchName[100];
    printf("\nEnter Doctor Name to search: ");
    read_line(searchName, sizeof(searchName));
    trim(searchName);
    
    // Convert search name to lowercase for case-insensitive comparison
    for (char *p = searchName; *p; ++p)
        *p = tolower(*p);
    
    char line[512];
    int found = 0;
    printf("\n==== Search Results ====\n\n");
    
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(specialization, tok, sizeof(specialization) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(department, tok, sizeof(department) - 1);
        
        trim(name);
        trim(specialization);
        trim(department);
        
        // Convert name to lowercase for case-insensitive comparison
        char lowerName[100];
        strcpy(lowerName, name);
        for (char *p = lowerName; *p; ++p)
            *p = tolower(*p);
        
        // Check if search name is part of doctor name
        if (strstr(lowerName, searchName) != NULL)
        {
            printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n--------------\n",
                   id, name, specialization, department);
            found = 1;
        }
    }
    
    if (!found)
        printf("No doctors found with name containing '%s'.\n", searchName);
    
    fclose(file);
    pauseProg();
}

void searchDoctorBySpecialization()
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file)
    {
        printf("\nNo doctor records found.\n");
        pauseProg();
        return;
    }
    
    char searchSpec[100];
    printf("\nEnter Specialization to search: ");
    read_line(searchSpec, sizeof(searchSpec));
    trim(searchSpec);
    
    // Convert search specialization to lowercase for case-insensitive comparison
    for (char *p = searchSpec; *p; ++p)
        *p = tolower(*p);
    
    char line[512];
    int found = 0;
    printf("\n==== Search Results ====\n\n");
    
    while (fgets(line, sizeof(line), file))
    {
        char *tok;
        int id;
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        
        tok = strtok(line, "|");
        if (!tok)
            continue;
        id = atoi(tok);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(name, tok, sizeof(name) - 1);
        
        tok = strtok(NULL, "|");
        if (tok)
            strncpy(specialization, tok, sizeof(specialization) - 1);
        
        tok = strtok(NULL, "|\n");
        if (tok)
            strncpy(department, tok, sizeof(department) - 1);
        
        trim(name);
        trim(specialization);
        trim(department);
        
        // Convert specialization to lowercase for case-insensitive comparison
        char lowerSpec[100];
        strcpy(lowerSpec, specialization);
        for (char *p = lowerSpec; *p; ++p)
            *p = tolower(*p);
        
        // Check if search specialization is part of doctor specialization
        if (strstr(lowerSpec, searchSpec) != NULL)
        {
            printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n--------------\n",
                   id, name, specialization, department);
            found = 1;
        }
    }
    
    if (!found)
        printf("No doctors found with specialization containing '%s'.\n", searchSpec);
    
    fclose(file);
    pauseProg();
}

void findDoctorPortal()
{
    int choice;
    
    while (1)
    {
        clearScreen();
        printf("\n========== \033[1;32m FIND DOCTOR PORTAL \033[0m ==========\n\n");
        printf("1. Search Doctor by Name\n");
        printf("2. Search Doctor by Specialization\n");
        printf("3. View All Doctors\n");
        printf("4. Back to Main Menu\n");
        
        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
                ;
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }
        
        switch (choice)
        {
        case 1:
            searchDoctorByName();
            break;
        case 2:
            searchDoctorBySpecialization();
            break;
        case 3:
            viewAllDoctors();
            break;
        case 4:
            return;
        default:
            printf("\nInvalid choice!\n");
            pauseProg();
        }
    }
}

void contributionPortal()
{
  
    int choice;

    while (1){

          clearScreen();

 printf("\n|-------------------------------------------------|\n");
printf("|                                                |\n");
printf("|                 \033[1;36mTeam UpSideDown\033[0m                |\n");  
printf("|                                                |\n");
printf("|   Project : \033[1;33mHospital Management System (HMS)\033[0m   |\n"); 
printf("|                                                |\n");
printf("|------------------------------------------------|\n\n\n");

printf(" # Team Members:\n\n");

printf("   - Member 1\033[1;32m: Naim Hossain\033[0m (ID: 252-15-178)\n");  
printf("                           -Project Coordinator and developer\n\n");

printf("   - Member 2\033[1;32m: Md Ajmine Adil Sadik\033[0m (ID: 252-15-172)\n");
printf("                           -Project Structurer and developer\n\n");

printf("   - Member 3\033[1;32m: Sadman Sakib Mahi\033[0m (ID: 252-15-102)\n");
printf("                           - Developer\n\n");

printf("   - Member 4\033[1;32m: Md Zihad Hasan\033[0m (ID: 252-15-436)\n");
printf("                           - Developer\n\n");

printf("   - Member 5\033[1;32m: Shahriar Ahmmed Limon\033[0m (ID: 252-15-351)\n");
printf("                           - Developer\n\n");

printf("\n____________________________Thank You ! ____________________________\n");


         printf("\n1.Back to main.\n");
        printf("\nYour Choice : ");
           
     if (scanf("%d", &choice) == 1)
        {

         return ;
        }

        else{
            pauseProg();
        }

    }

   
}

/* Collect feedback from user and append to feedback.txt */
void feedback()
{
    clearScreen();
    int id;
    char fb[1000];

    printf("Enter your Patient ID (or 0 to remain anonymous): ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n')
            ;
        id = 0;
    }
    while (getchar() != '\n')
    {
    }

    /* Validate patient ID if not anonymous */
    if (id != 0 && !isPatientIdExists(id))
    {
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id);
        pauseProg();
        return;
    }

    printf("Enter your feedback (single line):\n");
    read_line(fb, sizeof(fb));
    trim(fb);

    FILE *f = fopen("feedback.txt", "a");
    if (!f)
    {
        perror("Unable to open feedback.txt");
        pauseProg();
        return;
    }
    /* store as: id|feedback\n */
    fprintf(f, "%d|%s\n", id, fb);
    fclose(f);

    printf("\nThank you for your feedback!\n");
    pauseProg();
}

int main()
{
    int choice;

    while (1)
    {
        clearScreen();
       

  
   
    printf("\n+------------------------------------------------------------+\n");
    printf("|                                                            |\n");
    printf("| \033[1;33m           WELCOME TO HOSPITAL MANAGEMENT SYSTEM           \033[0m|\n");
    printf("| \033[1;36m                  TEAM: UPSIDEDOWN                         \033[0m|\n");
    printf("|                                                            |\n");
    printf("+------------------------------------------------------------+\n\n");

    printf("+------------------------------------------------------------+\n");
    printf("|                       MAIN MENU                            |\n");
    printf("+------------------------------------------------------------+\n");
    printf("|                                                            |\n");
    printf("| 1. Reception Portal                                        |\n");
    printf("| 2. Patient Portal                                          |\n");
    printf("|                                                            |\n");
    printf("| 3. Doctor Portal                                           |\n");
    printf("| 4. Lab Portal                                              |\n");
    printf("| 5. Admin Portal                                            |\n");
    printf("|                                                            |\n");
    printf("| 6. Find Doctor                                             |\n");
    printf("| 7. Feedback                                                |\n");
    printf("|                                                            |\n");
    printf("| 8. Team Details                                            |\n");
    printf("| 9. Exit System                                             |\n");
    printf("|                                                            |\n");
    printf("+------------------------------------------------------------+\n\n");

 


        printf("\nEnter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n')
            {
            }
            choice = 0;
        }
        while (getchar() != '\n')
        {
        }

        switch (choice)
        {
        case 1:
            receptionPortal();
            break;
        case 2:
            patientPortal();
            break;
        case 3:
            doctorPortal();
            break;
        case 4:
            labPortal();
            break;
        case 5:
            adminPortal();
            break;
        case 6:
            findDoctorPortal();  
            break;
        case 7:
            feedback();
            break;
        case 8:
             contributionPortal();
             break;
           
        case 9:
            printf("\nThank you for using the system!\n");
            exit(0);
        default:
            printf("\nInvalid choice! Try again.\n");
            pauseProg();
            break;
        }
    }

    return 0;
}
