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

// --- Refactoring: Generic Record Modification ---
typedef enum { ACTION_EDIT, ACTION_DELETE } ModifyAction;
void modifyRecordInFile(const char* filename, int targetId, ModifyAction action, void (*editFunc)(char** tokens, int numFields), int numFields);

// --- Refactoring: Specific Edit Logic Functions ---
void editPatientLogic(char** tokens, int numFields);
void editDoctorLogic(char** tokens, int numFields);
void editStaffLogic(char** tokens, int numFields);
void editAppointmentLogic(char** tokens, int numFields);
void editLabReportLogic(char** tokens, int numFields);
void editPrescriptionLogic(char** tokens, int numFields);

void clearScreen()
{
    system(CLEAR_CMD);
}

void pauseProg()
{
#ifdef _WIN32
    system(PAUSE_CMD);
#else
    printf("\nPress Enter to continue...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    getchar();
#endif
}

void read_line(char *buffer, size_t size)
{
    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

void trim(char *s)
{
    char *start = s;
    while (*start && (*start == ' ' || *start == '\t')) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t')) {
        s[len - 1] = '\0';
        len--;
    }
}

int isPatientIdExists(int id)
{
    FILE *file = fopen("patients.txt", "r");
    if (!file) return 0;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char *tok = strtok(line, "|");
        if (tok && atoi(tok) == id) { 
            fclose(file); 
            return 1; 
        }
    }
    fclose(file);
    return 0;
}

int isDoctorIdExists(int id)
{
    FILE *file = fopen("doctors.txt", "r");
    if (!file) return 0;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char *tok = strtok(line, "|");
        if (tok && atoi(tok) == id) { 
            fclose(file); 
            return 1; 
        }
    }
    fclose(file);
    return 0;
}

int isStaffIdExists(int id)
{
    FILE *file = fopen("staff.txt", "r");
    if (!file) return 0;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char *tok = strtok(line, "|");
        if (tok && atoi(tok) == id) { 
            fclose(file); 
            return 1; 
        }
    }
    fclose(file);
    return 0;
}

char* getDoctorNameById(int id)
{
    static char name[100] = "Unknown";
    FILE *file = fopen("doctors.txt", "r");
    if (!file) return name;
    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char *tok = strtok(line, "|");
        if (!tok) continue;
        int doctorId = atoi(tok);
        if (doctorId == id) {
            tok = strtok(NULL, "|");
            if (tok) {
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

int authenticateAdmin()
{
    char password[50];
    int attempts = 3;
    while (attempts > 0) {
        printf("Enter admin password: ");
        read_line(password, sizeof(password));
        if (strcmp(password, ADMIN_PASSWORD) == 0) return 1;
        else {
            attempts--;
            printf("Incorrect password. %d attempts remaining.\n", attempts);
        }
    }
    return 0;
}

int authenticateReception()
{
    char password[50];
    int attempts = 3;
    while (attempts > 0) {
        printf("Enter reception password: ");
        read_line(password, sizeof(password));
        if (strcmp(password, RECEPTION_PASSWORD) == 0) return 1;
        else {
            attempts--;
            printf("Incorrect password. %d attempts remaining.\n", attempts);
        }
    }
    return 0;
}

void savePatient(struct Patient p)
{
    if (isPatientIdExists(p.id)) {
        printf("\n\033[1;31mError: Patient ID %d already exists!\033[0m\n", p.id);
        pauseProg();
        return;
    }
    FILE *file = fopen("patients.txt", "a");
    if (!file) { 
        perror("Error opening patients.txt for append"); 
        return; 
    }
  
    for (char *q = p.name; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = p.gender; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = p.blood_group; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = p.mobile; *q; ++q) if (*q == '|') *q = ' ';
    fprintf(file, "%d|%s|%d|%s|%s|%s\n", p.id, p.name, p.age, p.gender, p.blood_group, p.mobile);
    fclose(file);
}

void showAllPatients()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file) { 
        printf("\nNo patient records found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[512];
    printf("\n==== Patient List ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok;
        int id, age = 0;
        char name[100] = {0}, gender[20] = {0}, blood_group[5] = {0}, mobile[15] = {0};
        tok = strtok(line, "|"); if (!tok) continue; id = atoi(tok);
        tok = strtok(NULL, "|"); if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); if (tok) age = atoi(tok);
        tok = strtok(NULL, "|"); if (tok) strncpy(gender, tok, sizeof(gender) - 1);
        tok = strtok(NULL, "|"); if (tok) strncpy(blood_group, tok, sizeof(blood_group) - 1);
        tok = strtok(NULL, "|\n"); if (tok) strncpy(mobile, tok, sizeof(mobile) - 1);
        trim(name); trim(gender); trim(blood_group); trim(mobile);
        printf("ID: %d\nName: %s\nAge: %d\nGender: %s\nBlood Group: %s\nMobile: %s\n--------------\n", 
               id, name, age, gender, blood_group, mobile);
    }
    fclose(file);
    pauseProg();
}

void searchPatient()
{
    FILE *file = fopen("patients.txt", "r");
    if (!file) { 
        printf("\nNo patient records found.\n"); 
        pauseProg(); 
        return; 
    }
    int idToFind;
    printf("\nEnter Patient ID to search: ");
    if (scanf("%d", &idToFind) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    while (getchar() != '\n') {}
    char line[512];
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char *tok; int id, age = 0;
        char name[100] = {0}, gender[20] = {0}, blood_group[5] = {0}, mobile[15] = {0};
        tok = strtok(line, "|"); if (!tok) continue; id = atoi(tok);
        if (id != idToFind) continue;
        tok = strtok(NULL, "|"); if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); if (tok) age = atoi(tok);
        tok = strtok(NULL, "|"); if (tok) strncpy(gender, tok, sizeof(gender) - 1);
        tok = strtok(NULL, "|"); if (tok) strncpy(blood_group, tok, sizeof(blood_group) - 1);
        tok = strtok(NULL, "|\n"); if (tok) strncpy(mobile, tok, sizeof(mobile) - 1);
        trim(name); trim(gender); trim(blood_group); trim(mobile);
        printf("\nPatient Found!\nID: %d\nName: %s\nAge: %d\nGender: %s\nBlood Group: %s\nMobile: %s\n", 
               id, name, age, gender, blood_group, mobile);
        found = 1; break;
    }
    if (!found) printf("\nPatient Not Found!\n");
    fclose(file);
    pauseProg();
}

void bookAppointment()
{
    int pid, did; 
    char date[64], time[64];
    printf("\nEnter Patient ID: "); 
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Patient ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        pauseProg(); 
        return; 
    }
    printf("Enter Doctor ID: "); 
    if (scanf("%d", &did) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Doctor ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isDoctorIdExists(did)) { 
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", did); 
        pauseProg(); 
        return; 
    }
    printf("Enter Appointment Date (DD/MM/YYYY): "); 
    read_line(date, sizeof(date));
    printf("Enter Appointment Time (HH:MM): "); 
    read_line(time, sizeof(time));
    trim(date); trim(time);
    FILE *file = fopen("appointments.txt", "a"); 
    if (!file) { 
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
    if (!authenticateReception()) { 
        printf("\nAuthentication failed. Access denied.\n"); 
        pauseProg(); 
        return; 
    }
    int choice;
    while (1) {
        clearScreen();
        printf("\n==========\033[1;32m RECEPTION PORTAL \033[0m==========\n\n");
        printf("1. Register New Patient\n"); 
        printf("2. Show All Registered Patients\n"); 
        printf("3. Search Patient by ID\n");
        printf("4. Book Appointment\n"); 
        printf("5. Edit Patient Details\n\n"); 
        printf("6. Back to Main Menu\n");
        printf("\nEnter your choice: "); 
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
            case 1: {
                struct Patient p;
                printf("\nEnter Patient ID: "); 
                if (scanf("%d", &p.id) != 1) { 
                    while (getchar() != '\n') {} 
                    printf("Invalid ID.\n"); 
                    pauseProg(); 
                    break; 
                } 
                while (getchar() != '\n') {}
                if (isPatientIdExists(p.id)) { 
                    printf("\n\033[1;31mError: Patient ID %d already exists!\033[0m\n", p.id); 
                    pauseProg(); 
                    break; 
                }
                printf("Enter Patient Name: "); 
                read_line(p.name, sizeof(p.name));
                printf("Enter Age: "); 
                if (scanf("%d", &p.age) != 1) { 
                    while (getchar() != '\n') {} 
                    printf("Invalid Age.\n"); 
                    pauseProg(); 
                    break; 
                } 
                while (getchar() != '\n') {}
                printf("Enter Gender: "); 
                read_line(p.gender, sizeof(p.gender));
                printf("Enter Blood Group: "); 
                read_line(p.blood_group, sizeof(p.blood_group));
                printf("Enter Mobile Number: "); 
                read_line(p.mobile, sizeof(p.mobile));
                trim(p.name); trim(p.gender); trim(p.blood_group); trim(p.mobile);
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
                editPatientRecord(); 
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
    if (!file) { 
        printf("\nNo patient records found.\n"); 
        pauseProg(); 
        return; 
    }
    int id; 
    printf("\nEnter your Patient ID: "); 
    if (scanf("%d", &id) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        fclose(file); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(id)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    char line[512]; 
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int pid, age = 0; 
        char name[100] = {0}, gender[20] = {0}, blood_group[5] = {0}, mobile[15] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        pid = atoi(tok); 
        if (pid != id) continue;
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); 
        if (tok) age = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(gender, tok, sizeof(gender) - 1);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(blood_group, tok, sizeof(blood_group) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(mobile, tok, sizeof(mobile) - 1);
        trim(name); trim(gender); trim(blood_group); trim(mobile);
        printf("\n====== Your Details ======\n\nID : %d\nName : %s\nAge : %d\nGender : %s\nBlood Grp : %s\nMobile : %s\n", 
               pid, name, age, gender, blood_group, mobile); 
        found = 1; 
        break;
    }
    if (!found) printf("\nNo record found for this ID.\n"); 
    fclose(file); 
    pauseProg();
}

void viewAppointments()
{
    FILE *file = fopen("appointments.txt", "r"); 
    if (!file) { 
        printf("\nNo appointments found.\n"); 
        pauseProg(); 
        return; 
    }
    int pid; 
    printf("\nEnter your Patient ID: "); 
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        fclose(file); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    char line[512]; 
    int found = 0; 
    printf("\n====== Your Appointments ======\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id, did; 
        char date[64] = {0}, time[64] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok); 
        if (id != pid) continue;
        tok = strtok(NULL, "|"); 
        if (tok) did = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(date, tok, sizeof(date) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(time, tok, sizeof(time) - 1);
        trim(date); trim(time); 
        char* doctorName = getDoctorNameById(did);
        printf("Date : %s\nTime : %s\nDoctor : %s (ID: %d)\n-----------------------------\n", 
               date, time, doctorName, did); 
        found = 1;
    }
    if (!found) printf("No appointment booked.\n"); 
    fclose(file); 
    pauseProg();
}

void viewLabReports()
{
    FILE *file = fopen("labreports.txt", "r"); 
    if (!file) { 
        printf("\nNo lab reports found.\n"); 
        pauseProg(); 
        return; 
    }
    int pid; 
    printf("\nEnter your Patient ID: "); 
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        fclose(file); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    int found = 0; 
    printf("\n====== Your Lab Reports ======\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char report[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok); 
        if (id != pid) continue;
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(report, tok, sizeof(report) - 1); 
        trim(report);
        printf("Report: %s\n-----------------------------\n", report); 
        found = 1;
    }
    if (!found) printf("No lab report available.\n"); 
    fclose(file); 
    pauseProg();
}

void viewPrescriptions()
{
    FILE *file = fopen("prescriptions.txt", "r"); 
    if (!file) { 
        printf("\nNo prescriptions found.\n"); 
        pauseProg(); 
        return; 
    }
    int pid; 
    printf("\nEnter your Patient ID: "); 
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        fclose(file); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    int found = 0; 
    printf("\n====== Your Prescriptions ======\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char doctorIdStr[20] = {0}, pres[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok); 
        if (id != pid) continue;
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(pres, tok, sizeof(pres) - 1); 
        trim(pres);
        int doctorId = atoi(doctorIdStr); 
        char* doctorName = getDoctorNameById(doctorId);
        printf("Doctor: %s (ID: %d)\nPrescription: %s\n-----------------------------\n", 
               doctorName, doctorId, pres); 
        found = 1;
    }
    if (!found) printf("No prescription available.\n"); 
    fclose(file); 
    pauseProg();
}

void patientPortal()
{
    int choice;
    while (1) {
        clearScreen();
        printf("\n===============\033[1;32m PATIENT PORTAL \033[0m===============\n\n");
        printf("1. View My Details\n"); 
        printf("2. View Appointments\n"); 
        printf("3. View Lab Reports\n"); 
        printf("4. View Prescriptions\n\n"); 
        printf("5. Back to Main Menu\n");
        printf("\nEnter your choice: "); 
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
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
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Patient ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        pauseProg(); 
        return; 
    }
    printf("Enter Doctor ID: "); 
    if (scanf("%d", &did) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Doctor ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isDoctorIdExists(did)) { 
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", did); 
        pauseProg(); 
        return; 
    }
    printf("Enter Prescription Details: "); 
    read_line(prescription, sizeof(prescription)); 
    trim(prescription);
    for (char *q = prescription; *q; ++q) if (*q == '|') *q = ' ';
    FILE *file = fopen("prescriptions.txt", "a"); 
    if (!file) { 
        perror("Error opening prescriptions.txt"); 
        pauseProg(); 
        return; 
    }
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
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Patient ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        pauseProg(); 
        return; 
    }
    printf("Enter Doctor ID: "); 
    if (scanf("%d", &did) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Doctor ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isDoctorIdExists(did)) { 
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", did); 
        pauseProg(); 
        return; 
    }
    printf("Enter Medical Record Details: "); 
    read_line(record, sizeof(record)); 
    trim(record);
    for (char *q = record; *q; ++q) if (*q == '|') *q = ' ';
    FILE *file = fopen("medicalrecords.txt", "a"); 
    if (!file) { 
        perror("Error opening medicalrecords.txt"); 
        pauseProg(); 
        return; 
    }
    fprintf(file, "%d|%d|%s\n", pid, did, record); 
    fclose(file);
    printf("\nMedical record updated successfully!\n"); 
    pauseProg();
}

void doctorPortal()
{
    int choice;
    while (1) {
        clearScreen();
        printf("\n========== \033[1;32m DOCTOR PORTAL \033[0m ==========\n\n");
        printf("1. View Patient List\n"); 
        printf("2. Write Prescription\n"); 
        printf("3. Update Medical Record\n");
        printf("4. View All Appointments\n"); 
        printf("5. View Medical Records\n\n"); 
        printf("6. Back to Main Menu\n");
        printf("\nEnter your choice: "); 
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
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
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Patient ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        pauseProg(); 
        return; 
    }
    printf("Enter Lab Report Details: "); 
    read_line(report, sizeof(report)); 
    trim(report);
    for (char *q = report; *q; ++q) if (*q == '|') *q = ' ';
    FILE *file = fopen("labreports.txt", "a"); 
    if (!file) { 
        perror("Error opening labreports.txt"); 
        pauseProg(); 
        return; 
    }
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
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid Patient ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        pauseProg(); 
        return; 
    }
    sprintf(filename, "patient_%d_report.txt", pid);
    srcFile = fopen("labreports.txt", "r"); 
    if (!srcFile) { 
        printf("\nNo lab reports found.\n"); 
        pauseProg(); 
        return; 
    }
    destFile = fopen(filename, "w"); 
    if (!destFile) { 
        perror("Error creating report file"); 
        fclose(srcFile); 
        pauseProg(); 
        return; 
    }
    fprintf(destFile, "LAB REPORT FOR PATIENT ID: %d\n=====================================\n\n", pid);
    while (fgets(line, sizeof(line), srcFile)) {
        char *tok; 
        int id; 
        char report[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok); 
        if (id != pid) continue;
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(report, tok, sizeof(report) - 1); 
        trim(report);
        fprintf(destFile, "Report: %s\n-------------------------------------\n", report); 
        found = 1;
    }
    fclose(srcFile); 
    fclose(destFile);
    if (found) {
        printf("\nLab report printed to file: %s\n", filename);
        printf("\n====== Lab Report Preview ======\n"); 
        srcFile = fopen(filename, "r");
        if (srcFile) { 
            while (fgets(line, sizeof(line), srcFile)) { 
                printf("%s", line); 
            } 
            fclose(srcFile); 
        }
    } else { 
        printf("\nNo lab report found for this patient.\n"); 
        remove(filename); 
    }
    pauseProg();
}

void labPortal()
{
    int choice;
    while (1) {
        clearScreen();
        printf("\n==========\033[1;32m LAB PORTAL\033[0m ==========\n\n");
        printf("1. Enter Test Report\n"); 
        printf("2. Print Report\n"); 
        printf("3. View All Lab Reports\n\n"); 
        printf("4. Back to Main Menu\n");
        printf("\nEnter your choice: "); 
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
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
    if (scanf("%d", &s.id) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (isStaffIdExists(s.id)) { 
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
    trim(s.name); trim(s.position); trim(s.department);
    FILE *file = fopen("staff.txt", "a"); 
    if (!file) { 
        perror("Error opening staff.txt"); 
        pauseProg(); 
        return; 
    }
    for (char *q = s.name; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = s.position; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = s.department; *q; ++q) if (*q == '|') *q = ' ';
    fprintf(file, "%d|%s|%s|%s\n", s.id, s.name, s.position, s.department); 
    fclose(file);
    printf("\nStaff member added successfully!\n"); 
    pauseProg();
}

void removeStaff()
{
    int id; 
    printf("\nEnter Staff ID to remove: "); 
    if (scanf("%d", &id) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isStaffIdExists(id)) { 
        printf("\n\033[1;31mError: Staff ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
        return; 
    }
    printf("\nAre you sure you want to delete this staff member? (y/n): ");
    char confirm; 
    scanf(" %c", &confirm); 
    while (getchar() != '\n') {}
    if (confirm == 'y' || confirm == 'Y') 
        modifyRecordInFile("staff.txt", id, ACTION_DELETE, NULL, 4);
    else { 
        printf("\nDeletion cancelled.\n"); 
        pauseProg(); 
    }
}

void addDoctor()
{
    struct Doctor d;
    printf("\nEnter Doctor ID: "); 
    if (scanf("%d", &d.id) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid ID.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (isDoctorIdExists(d.id)) { 
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
    trim(d.name); trim(d.specialization); trim(d.department);
    FILE *file = fopen("doctors.txt", "a"); 
    if (!file) { 
        perror("Error opening doctors.txt"); 
        pauseProg(); 
        return; 
    }
    for (char *q = d.name; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = d.specialization; *q; ++q) if (*q == '|') *q = ' ';
    for (char *q = d.department; *q; ++q) if (*q == '|') *q = ' ';
    fprintf(file, "%d|%s|%s|%s\n", d.id, d.name, d.specialization, d.department); 
    fclose(file);
    printf("\nDoctor added successfully!\n"); 
    pauseProg();
}

void removeDoctor()
{
    int id; 
    printf("\nEnter Doctor ID to remove: "); 
    if (scanf("%d", &id) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isDoctorIdExists(id)) { 
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
        return; 
    }
    printf("\nAre you sure you want to delete this doctor? (y/n): ");
    char confirm; 
    scanf(" %c", &confirm); 
    while (getchar() != '\n') {}
    if (confirm == 'y' || confirm == 'Y') 
        modifyRecordInFile("doctors.txt", id, ACTION_DELETE, NULL, 4);
    else { 
        printf("\nDeletion cancelled.\n"); 
        pauseProg(); 
    }
}

void viewAllDoctors()
{
    FILE *file = fopen("doctors.txt", "r"); 
    if (!file) { 
        printf("\nNo doctor records found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[512]; 
    printf("\n==== Doctor List ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(specialization, tok, sizeof(specialization) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(department, tok, sizeof(department) - 1);
        trim(name); trim(specialization); trim(department);
        printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n--------------\n", 
               id, name, specialization, department);
    }
    fclose(file); 
    pauseProg();
}

void viewAllStaff()
{
    FILE *file = fopen("staff.txt", "r"); 
    if (!file) { 
        printf("\nNo staff records found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[512]; 
    printf("\n==== Staff List ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char name[100] = {0}, position[50] = {0}, department[100] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(position, tok, sizeof(position) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(department, tok, sizeof(department) - 1);
        trim(name); trim(position); trim(department);
        printf("ID: %d\nName: %s\nPosition: %s\nDepartment: %s\n--------------\n", 
               id, name, position, department);
    }
    fclose(file); 
    pauseProg();
}

void viewAllAppointments()
{
    FILE *file = fopen("appointments.txt", "r"); 
    if (!file) { 
        printf("\nNo appointments found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[512]; 
    printf("\n==== All Appointments ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int pid, did; 
        char date[64] = {0}, time[64] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        pid = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) did = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(date, tok, sizeof(date) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(time, tok, sizeof(time) - 1);
        trim(date); trim(time); 
        char* doctorName = getDoctorNameById(did);
        printf("Patient ID: %d\nDoctor: %s (ID: %d)\nDate: %s\nTime: %s\n--------------\n", 
               pid, doctorName, did, date, time);
    }
    fclose(file); 
    pauseProg();
}

void viewAllLabReports()
{
    FILE *file = fopen("labreports.txt", "r"); 
    if (!file) { 
        printf("\nNo lab reports found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    printf("\n==== All Lab Reports ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int pid; 
        char report[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        pid = atoi(tok);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(report, tok, sizeof(report) - 1); 
        trim(report);
        printf("Patient ID: %d\nReport: %s\n--------------\n", pid, report);
    }
    fclose(file); 
    pauseProg();
}

void viewAllPrescriptions()
{
    FILE *file = fopen("prescriptions.txt", "r"); 
    if (!file) { 
        printf("\nNo prescriptions found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    printf("\n==== All Prescriptions ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int pid; 
        char doctorIdStr[20] = {0}, pres[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        pid = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(pres, tok, sizeof(pres) - 1); 
        trim(pres);
        int doctorId = atoi(doctorIdStr); 
        char* doctorName = getDoctorNameById(doctorId);
        printf("Patient ID: %d\nDoctor: %s (ID: %d)\nPrescription: %s\n--------------\n", 
               pid, doctorName, doctorId, pres);
    }
    fclose(file); 
    pauseProg();
}

void viewMedicalRecords()
{
    FILE *file = fopen("medicalrecords.txt", "r"); 
    if (!file) { 
        printf("\nNo medical records found.\n"); 
        pauseProg(); 
        return; 
    }
    int pid; 
    printf("\nEnter Patient ID: "); 
    if (scanf("%d", &pid) != 1) { 
        while (getchar() != '\n') {} 
        printf("Invalid input.\n"); 
        fclose(file); 
        pauseProg(); 
        return; 
    } 
    while (getchar() != '\n') {}
    if (!isPatientIdExists(pid)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", pid); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    int found = 0; 
    printf("\n====== Medical Records for Patient ID: %d ======\n", pid);
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char doctorIdStr[20] = {0}, record[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok); 
        if (id != pid) continue;
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(doctorIdStr, tok, sizeof(doctorIdStr) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(record, tok, sizeof(record) - 1); 
        trim(record);
        int doctorId = atoi(doctorIdStr); 
        char* doctorName = getDoctorNameById(doctorId);
        printf("Doctor: %s (ID: %d)\nRecord: %s\n-----------------------------\n", 
               doctorName, doctorId, record); 
        found = 1;
    }
    if (!found) printf("No medical records available for this patient.\n"); 
    fclose(file); 
    pauseProg();
}

// ===================================================================
// == REFACTORED EDIT/DELETE FUNCTIONS START HERE ==
// ===================================================================

void editPatientLogic(char** tokens, int numFields) {
    printf("\nCurrent Details:\nID: %s, Name: %s, Age: %s, Gender: %s, Blood: %s, Mobile: %s\n", 
           tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
    printf("\nEnter new Name (leave blank to keep current): "); 
    char newName[100]; 
    read_line(newName, sizeof(newName)); 
    if (strlen(newName) > 0) tokens[1] = strdup(newName);
    printf("Enter new Age (0 to keep current): "); 
    int newAge; 
    if (scanf("%d", &newAge) == 1 && newAge > 0) { 
        char buf[20]; 
        sprintf(buf, "%d", newAge); 
        tokens[2] = strdup(buf); 
    } 
    while (getchar() != '\n') {}
    printf("Enter new Gender (leave blank to keep current): "); 
    char newGender[20]; 
    read_line(newGender, sizeof(newGender)); 
    if (strlen(newGender) > 0) tokens[3] = strdup(newGender);
    printf("Enter new Blood Group (leave blank to keep current): "); 
    char newBg[5]; 
    read_line(newBg, sizeof(newBg)); 
    if (strlen(newBg) > 0) tokens[4] = strdup(newBg);
    printf("Enter new Mobile (leave blank to keep current): "); 
    char newMobile[15]; 
    read_line(newMobile, sizeof(newMobile)); 
    if (strlen(newMobile) > 0) tokens[5] = strdup(newMobile);
}

void editDoctorLogic(char** tokens, int numFields) {
    printf("\nCurrent Details:\nID: %s, Name: %s, Spec: %s, Dept: %s\n", 
           tokens[0], tokens[1], tokens[2], tokens[3]);
    printf("\nEnter new Name (leave blank to keep current): "); 
    char newName[100]; 
    read_line(newName, sizeof(newName)); 
    if (strlen(newName) > 0) tokens[1] = strdup(newName);
    printf("Enter new Specialization (leave blank to keep current): "); 
    char newSpec[100]; 
    read_line(newSpec, sizeof(newSpec)); 
    if (strlen(newSpec) > 0) tokens[2] = strdup(newSpec);
    printf("Enter new Department (leave blank to keep current): "); 
    char newDept[100]; 
    read_line(newDept, sizeof(newDept)); 
    if (strlen(newDept) > 0) tokens[3] = strdup(newDept);
}

void editStaffLogic(char** tokens, int numFields) {
    printf("\nCurrent Details:\nID: %s, Name: %s, Position: %s, Dept: %s\n", 
           tokens[0], tokens[1], tokens[2], tokens[3]);
    printf("\nEnter new Name (leave blank to keep current): "); 
    char newName[100]; 
    read_line(newName, sizeof(newName)); 
    if (strlen(newName) > 0) tokens[1] = strdup(newName);
    printf("Enter new Position (leave blank to keep current): "); 
    char newPos[50]; 
    read_line(newPos, sizeof(newPos)); 
    if (strlen(newPos) > 0) tokens[2] = strdup(newPos);
    printf("Enter new Department (leave blank to keep current): "); 
    char newDept[100]; 
    read_line(newDept, sizeof(newDept)); 
    if (strlen(newDept) > 0) tokens[3] = strdup(newDept);
}

void editAppointmentLogic(char** tokens, int numFields) {
    char* doctorName = getDoctorNameById(atoi(tokens[1]));
    printf("\nCurrent Appointment Details:\nPatient ID: %s, Doctor: %s (ID: %s), Date: %s, Time: %s\n", 
           tokens[0], doctorName, tokens[1], tokens[2], tokens[3]);
    printf("\nEnter new Doctor ID (0 to keep current): "); 
    int newDid; 
    if (scanf("%d", &newDid) == 1 && newDid > 0 && isDoctorIdExists(newDid)) { 
        char buf[20]; 
        sprintf(buf, "%d", newDid); 
        tokens[1] = strdup(buf); 
    } 
    else if(newDid > 0) printf("\nWarning: Doctor ID %d does not exist. Keeping current.\n", newDid); 
    while (getchar() != '\n') {}
    printf("Enter new Date (leave blank to keep current): "); 
    char newDate[64]; 
    read_line(newDate, sizeof(newDate)); 
    if (strlen(newDate) > 0) tokens[2] = strdup(newDate);
    printf("Enter new Time (leave blank to keep current): "); 
    char newTime[64]; 
    read_line(newTime, sizeof(newTime)); 
    if (strlen(newTime) > 0) tokens[3] = strdup(newTime);
}

void editLabReportLogic(char** tokens, int numFields) {
    printf("\nCurrent Lab Report:\nPatient ID: %s, Report: %s\n", tokens[0], tokens[1]);
    printf("\nEnter new Report Details (leave blank to keep current): "); 
    char newReport[900]; 
    read_line(newReport, sizeof(newReport)); 
    if (strlen(newReport) > 0) tokens[1] = strdup(newReport);
}

void editPrescriptionLogic(char** tokens, int numFields) {
    char* doctorName = getDoctorNameById(atoi(tokens[1]));
    printf("\nCurrent Prescription:\nPatient ID: %s, Doctor: %s (ID: %s), Prescription: %s\n", 
           tokens[0], doctorName, tokens[1], tokens[2]);
    printf("\nEnter new Doctor ID (0 to keep current): "); 
    int newDid; 
    if (scanf("%d", &newDid) == 1 && newDid > 0 && isDoctorIdExists(newDid)) { 
        char buf[20]; 
        sprintf(buf, "%d", newDid); 
        tokens[1] = strdup(buf); 
    } 
    else if(newDid > 0) printf("\nWarning: Doctor ID %d does not exist. Keeping current.\n", newDid); 
    while (getchar() != '\n') {}
    printf("Enter new Prescription Details (leave blank to keep current): "); 
    char newPres[900]; 
    read_line(newPres, sizeof(newPres)); 
    if (strlen(newPres) > 0) tokens[2] = strdup(newPres);
}

void modifyRecordInFile(const char* filename, int targetId, ModifyAction action, void (*editFunc)(char**, int), int numFields) {
    FILE *file = fopen(filename, "r"); 
    if (!file) { 
        printf("\nNo records found in %s.\n", filename); 
        pauseProg(); 
        return; 
    }
    FILE *temp = fopen("temp.txt", "w"); 
    if (!temp) { 
        perror("Error creating temporary file"); 
        fclose(file); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    int found = 0;
    while (fgets(line, sizeof(line), file)) {
        char* tokens[20]; 
        char* line_copy = strdup(line); 
        int i = 0; 
        char* token = strtok(line_copy, "|\n");
        while (token != NULL && i < numFields) { 
            tokens[i++] = token; 
            token = strtok(NULL, "|\n"); 
        }
        int currentId = atoi(tokens[0]);
        if (currentId == targetId) {
            found = 1;
            if (action == ACTION_DELETE) { 
                free(line_copy); 
                continue; 
            }
            if (action == ACTION_EDIT && editFunc) { 
                editFunc(tokens, numFields); 
            }
        }
        for (int j = 0; j < i; j++) { 
            fprintf(temp, "%s%s", tokens[j], (j < i - 1) ? "|" : "\n"); 
        }
        free(line_copy);
    }
    fclose(file); 
    fclose(temp);
    if (found) { 
        remove(filename); 
        rename("temp.txt", filename); 
        printf("\nOperation completed successfully!\n"); 
    }
    else { 
        remove("temp.txt"); 
        printf("\nRecord with ID %d not found!\n", targetId); 
    }
    pauseProg();
}

void editPatientRecord() { 
    int id; 
    printf("\nEnter Patient ID to edit: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("patients.txt", id, ACTION_EDIT, editPatientLogic, 6); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void deletePatientRecord() { 
    int id; 
    printf("\nEnter Patient ID to delete: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("patients.txt", id, ACTION_DELETE, NULL, 6); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void editDoctorRecord() { 
    int id; 
    printf("\nEnter Doctor ID to edit: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isDoctorIdExists(id)) 
        modifyRecordInFile("doctors.txt", id, ACTION_EDIT, editDoctorLogic, 4); 
    else { 
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void deleteDoctorRecord() { 
    int id; 
    printf("\nEnter Doctor ID to delete: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isDoctorIdExists(id)) 
        modifyRecordInFile("doctors.txt", id, ACTION_DELETE, NULL, 4); 
    else { 
        printf("\n\033[1;31mError: Doctor ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void editStaffRecord() { 
    int id; 
    printf("\nEnter Staff ID to edit: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isStaffIdExists(id)) 
        modifyRecordInFile("staff.txt", id, ACTION_EDIT, editStaffLogic, 4); 
    else { 
        printf("\n\033[1;31mError: Staff ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void deleteStaffRecord() { 
    int id; 
    printf("\nEnter Staff ID to delete: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isStaffIdExists(id)) 
        modifyRecordInFile("staff.txt", id, ACTION_DELETE, NULL, 4); 
    else { 
        printf("\n\033[1;31mError: Staff ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void editAppointment() { 
    int id; 
    printf("\nEnter Patient ID for the appointment to edit: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("appointments.txt", id, ACTION_EDIT, editAppointmentLogic, 4); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void deleteAppointment() { 
    int id; 
    printf("\nEnter Patient ID for the appointment to delete: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("appointments.txt", id, ACTION_DELETE, NULL, 4); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void editLabReport() { 
    int id; 
    printf("\nEnter Patient ID for the lab report to edit: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("labreports.txt", id, ACTION_EDIT, editLabReportLogic, 2); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void deleteLabReport() { 
    int id; 
    printf("\nEnter Patient ID for the lab report to delete: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("labreports.txt", id, ACTION_DELETE, NULL, 2); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void editPrescription() { 
    int id; 
    printf("\nEnter Patient ID for the prescription to edit: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("prescriptions.txt", id, ACTION_EDIT, editPrescriptionLogic, 3); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

void deletePrescription() { 
    int id; 
    printf("\nEnter Patient ID for the prescription to delete: "); 
    if (scanf("%d", &id) != 1) { 
        while(getchar()!='\n'){} 
    } 
    while(getchar()!='\n'){} 
    if(isPatientIdExists(id)) 
        modifyRecordInFile("prescriptions.txt", id, ACTION_DELETE, NULL, 3); 
    else { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
    } 
}

// ===================================================================
// == REFACTORED EDIT/DELETE FUNCTIONS END HERE ==
// ===================================================================

void generateSystemReport()
{
    FILE *patients = fopen("patients.txt", "r"), 
         *doctors = fopen("doctors.txt", "r"), 
         *staff = fopen("staff.txt", "r"), 
         *appointments = fopen("appointments.txt", "r"), 
         *labreports = fopen("labreports.txt", "r"), 
         *prescriptions = fopen("prescriptions.txt", "r");
    int patientCount = 0, doctorCount = 0, staffCount = 0, appointmentCount = 0, labReportCount = 0, prescriptionCount = 0; 
    char line[1024];
    if (patients) { 
        while (fgets(line, sizeof(line), patients)) patientCount++; 
        fclose(patients); 
    }
    if (doctors) { 
        while (fgets(line, sizeof(line), doctors)) doctorCount++; 
        fclose(doctors); 
    }
    if (staff) { 
        while (fgets(line, sizeof(line), staff)) staffCount++; 
        fclose(staff); 
    }
    if (appointments) { 
        while (fgets(line, sizeof(line), appointments)) appointmentCount++; 
        fclose(appointments); 
    }
    if (labreports) { 
        while (fgets(line, sizeof(line), labreports)) labReportCount++; 
        fclose(labreports); 
    }
    if (prescriptions) { 
        while (fgets(line, sizeof(line), prescriptions)) prescriptionCount++; 
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
    if (!file) { 
        printf("\nNo feedback records found.\n"); 
        pauseProg(); 
        return; 
    }
    char line[1024]; 
    int found = 0, feedbackCount = 0; 
    printf("\n==== All Feedback ====\n\n");
    rewind(file); 
    while (fgets(line, sizeof(line), file)) { 
        feedbackCount++; 
    }
    printf("Total Feedback Entries: %d\n\n", feedbackCount);
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char feedback[900] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(feedback, tok, sizeof(feedback) - 1); 
        trim(feedback);
        printf("Feedback Entry:\nFrom: %s\nFeedback: %s\n-----------------------------\n", 
               (id == 0) ? "Anonymous User" : (isPatientIdExists(id) ? "Patient ID" : "Patient ID (No longer in system)"), 
               feedback); 
        found = 1;
    }
    if (!found) printf("No feedback available.\n"); 
    fclose(file); 
    pauseProg();
}

void editDatabase()
{
    int choice;
    while (1) {
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
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
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
    if (!authenticateAdmin()) { 
        printf("\nAuthentication failed. Access denied.\n"); 
        pauseProg(); 
        return; 
    }
    int choice;
    while (1) {
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
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
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
    if (!file) { 
        printf("\nNo doctor records found.\n"); 
        pauseProg(); 
        return; 
    }
    char searchName[100]; 
    printf("\nEnter Doctor Name to search: "); 
    read_line(searchName, sizeof(searchName)); 
    trim(searchName);
    for (char *p = searchName; *p; ++p) *p = tolower(*p);
    char line[512]; 
    int found = 0; 
    printf("\n==== Search Results ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(specialization, tok, sizeof(specialization) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(department, tok, sizeof(department) - 1);
        trim(name); trim(specialization); trim(department);
        char lowerName[100]; 
        strcpy(lowerName, name); 
        for (char *p = lowerName; *p; ++p) *p = tolower(*p);
        if (strstr(lowerName, searchName) != NULL) { 
            printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n--------------\n", 
                   id, name, specialization, department); 
            found = 1; 
        }
    }
    if (!found) printf("No doctors found with name containing '%s'.\n", searchName); 
    fclose(file); 
    pauseProg();
}

void searchDoctorBySpecialization()
{
    FILE *file = fopen("doctors.txt", "r"); 
    if (!file) { 
        printf("\nNo doctor records found.\n"); 
        pauseProg(); 
        return; 
    }
    char searchSpec[100]; 
    printf("\nEnter Specialization to search: "); 
    read_line(searchSpec, sizeof(searchSpec)); 
    trim(searchSpec);
    for (char *p = searchSpec; *p; ++p) *p = tolower(*p);
    char line[512]; 
    int found = 0; 
    printf("\n==== Search Results ====\n\n");
    while (fgets(line, sizeof(line), file)) {
        char *tok; 
        int id; 
        char name[100] = {0}, specialization[100] = {0}, department[100] = {0};
        tok = strtok(line, "|"); 
        if (!tok) continue; 
        id = atoi(tok);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(name, tok, sizeof(name) - 1);
        tok = strtok(NULL, "|"); 
        if (tok) strncpy(specialization, tok, sizeof(specialization) - 1);
        tok = strtok(NULL, "|\n"); 
        if (tok) strncpy(department, tok, sizeof(department) - 1);
        trim(name); trim(specialization); trim(department);
        char lowerSpec[100]; 
        strcpy(lowerSpec, specialization); 
        for (char *p = lowerSpec; *p; ++p) *p = tolower(*p);
        if (strstr(lowerSpec, searchSpec) != NULL) { 
            printf("ID: %d\nName: %s\nSpecialization: %s\nDepartment: %s\n--------------\n", 
                   id, name, specialization, department); 
            found = 1; 
        }
    }
    if (!found) printf("No doctors found with specialization containing '%s'.\n", searchSpec); 
    fclose(file); 
    pauseProg();
}

void findDoctorPortal()
{
    int choice;
    while (1) {
        clearScreen(); 
        printf("\n========== \033[1;32m FIND DOCTOR PORTAL \033[0m ==========\n\n");
        printf("1. Search Doctor by Name\n"); 
        printf("2. Search Doctor by Specialization\n"); 
        printf("3. View All Doctors\n"); 
        printf("4. Back to Main Menu\n");
        printf("\nEnter your choice: "); 
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) { 
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
    while (1) {
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


       
        pauseProg();
        return ;
        
        
    }
}

void feedback()
{
    clearScreen(); 
    int id; 
    char fb[1000];
    printf("Enter your Patient ID (or 0 to remain anonymous): "); 
    if (scanf("%d", &id) != 1) { 
        while (getchar() != '\n') {} 
        id = 0; 
    } 
    while (getchar() != '\n') {}
    if (id != 0 && !isPatientIdExists(id)) { 
        printf("\n\033[1;31mError: Patient ID %d does not exist!\033[0m\n", id); 
        pauseProg(); 
        return; 
    }
    printf("Enter your feedback (single line):\n"); 
    read_line(fb, sizeof(fb)); 
    trim(fb);
    FILE *f = fopen("feedback.txt", "a"); 
    if (!f) { 
        perror("Unable to open feedback.txt"); 
        pauseProg(); 
        return; 
    }
    fprintf(f, "%d|%s\n", id, fb); 
    fclose(f);
    printf("\nThank you for your feedback!\n"); 
    pauseProg();
}

int main()
{
    int choice;
    while (1) {
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
        if (scanf("%d", &choice) != 1) { 
            while (getchar() != '\n') {} 
            choice = 0; 
        } 
        while (getchar() != '\n') {}
        switch (choice) {
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
        }
    }
    return 0;
}
