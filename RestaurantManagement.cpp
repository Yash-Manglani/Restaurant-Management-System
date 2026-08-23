#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<windows.h>

#define MAX_ITEMS 200
#define MAX_STAFF 200
#define MAX_TABLES 50
#define TAX_PERCENT 5.0
#define DELIVERY_CHARGE 50.0
#define DISCOUNT_THRESHOLD 1000.0
#define DISCOUNT_PERCENT 5.0


typedef struct {
    int code;
    char name[64];
    float price;
    int qty; 
} Item;

typedef struct {
    int id;
    char name[64];
    char role[32]; 
    float salary;
    char contact[32];
    int available; 
} Staff;

typedef struct {
    int id;
    int reserved; 
    char by[64]; 
} Table;


Item items[MAX_ITEMS];
int itemCount = 0;

Staff staffs[MAX_STAFF];
int staffCount = 0;

Table tables[MAX_TABLES];
int tableCount = 0;


void setColor(int color){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void resetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}


void clear_input_buffer() {
    
    int c;
    while ((c=getchar()) != '\n' && c != EOF) {}
}

void strip_newline(char *s) {
    
    size_t n = strlen(s);
    if (n > 0 && s[n - 1] == '\n') s[n - 1] = '\0';
}

void pause_and_continue() {
    printf("\nPress ENTER to continue...");
    clear_input_buffer();
    system("cls");
}


void loadMenuFromFile() {
    FILE *fp = fopen("menu.txt", "r");
    itemCount = 0;
    if (!fp) return;
    while (!feof(fp) && itemCount < MAX_ITEMS) {
        int code, qty;
        float price;
        char name[64];
        if (fscanf(fp, "%d,%63[^,],%f,%d\n", &code, name, &price, &qty) == 4) {
            items[itemCount].code = code;
            strncpy(items[itemCount].name, name, 63);
            items[itemCount].price = price;
            items[itemCount].qty = qty;
            itemCount++;
        } else break;
    }
    fclose(fp);
}

void saveMenuToFile() {
    FILE *fp = fopen("menu.txt", "w");
    if (!fp) {
        printf("Error: cannot write menu.txt\n");
        return;
    }
    for (int i = 0; i < itemCount; i++) {
        fprintf(fp, "%d,%s,%.2f,%d\n", items[i].code, items[i].name, items[i].price, items[i].qty);
    }
    fclose(fp);
}

void loadStaffFromFile() {
    FILE *fp = fopen("staff.txt", "r");
    staffCount = 0;
    if (!fp) return;
    while (!feof(fp) && staffCount < MAX_STAFF) {
        int id, avail;
        float sal;
        char name[64], role[32], contact[32];
        if (fscanf(fp, "%d,%63[^,],%31[^,],%f,%31[^,],%d\n", &id, name, role, &sal, contact, &avail) == 6) {
            staffs[staffCount].id = id;
            strncpy(staffs[staffCount].name, name, 63);
            strncpy(staffs[staffCount].role, role, 31);
            staffs[staffCount].salary = sal;
            strncpy(staffs[staffCount].contact, contact, 31);
            staffs[staffCount].available = avail;
            staffCount++;
        } else break;
    }
    fclose(fp);
}

void saveStaffToFile() {
    FILE *fp = fopen("staff.txt", "w");
    if (!fp) {
        printf("Error: cannot write staff.txt\n");
        return;
    }
    for (int i = 0; i < staffCount; i++) {
        fprintf(fp, "%d,%s,%s,%.2f,%s,%d\n",
                staffs[i].id, staffs[i].name, staffs[i].role, staffs[i].salary, staffs[i].contact, staffs[i].available);
    }
    fclose(fp);
}

void loadTablesFromFile() {
    FILE *fp = fopen("tables.txt", "r");
    tableCount = 0;
    if (!fp) return;
    while (!feof(fp) && tableCount < MAX_TABLES) {
        int id, reserved;
        char by[64];
        if (fscanf(fp, "%d,%d,%63[^\n]\n", &id, &reserved, by) >= 2) {
            tables[tableCount].id = id;
            tables[tableCount].reserved = reserved;
            if (reserved == 1) strncpy(tables[tableCount].by, by, 63);
            else tables[tableCount].by[0] = '\0';
            tableCount++;
        } else break;
    }
    fclose(fp);
}

void saveTablesToFile() {
    FILE *fp = fopen("tables.txt", "w");
    if (!fp) {
        printf("Error: cannot write tables.txt\n");
        return;
    }
    for (int i = 0; i < tableCount; i++) {
        fprintf(fp, "%d,%d,%s\n", tables[i].id, tables[i].reserved, (tables[i].reserved ? tables[i].by : ""));
    }
    fclose(fp);
}

void ensureFilesExist() {
    FILE *fp;
    
    fp = fopen("menu.txt", "a");
    if (fp) fclose(fp);
   
    fp = fopen("staff.txt", "a");
    if (fp) fclose(fp);
    
    fp = fopen("tables.txt", "a");
    if (fp) fclose(fp);
   
    fp = fopen("sales.txt", "a");
    if (fp) fclose(fp);
    
    fp = fopen("delivery.txt", "a");
    if (fp) fclose(fp);
}


void displayMenuItems() {
    loadMenuFromFile();
    if (itemCount == 0) {
        printf("Menu is empty. Admin should add items.\n");
        return;
    }
    printf("\nCode\tName\t\t\tPrice\tStock\n");
    printf("----------------------------------------------------\n");
    for (int i = 0; i < itemCount; i++) {
        printf("%d\t%-20s\t%.2f\t%d\n", items[i].code, items[i].name, items[i].price, items[i].qty);
    }
}
void displayMenuItemsCustomer() {
    loadMenuFromFile();
    if (itemCount == 0) {
        printf("Menu is empty. Admin should add items.\n");
        return;
    }
    printf("\nCode\tName\t\t\tPrice\n");
    printf("---------------------------------------------\n");
    for (int i = 0; i < itemCount; i++) {
        printf("%d\t%-20s\t%.2f\n", items[i].code, items[i].name, items[i].price);
    }
}


void adminAddItem() {
    loadMenuFromFile();
    Item it;
    printf("\nEnter item code (int): ");
    scanf("%d", &it.code);
    clear_input_buffer();
    for (int i = 0; i < itemCount; i++) {
        if (items[i].code == it.code) {
            printf("Item code exists! Update the existing item instead.\n");
            return;
        }
    }
    printf("Enter item name: ");
    fgets(it.name, sizeof(it.name), stdin); strip_newline(it.name);
    printf("Enter price: ");
    scanf("%f", &it.price);
    printf("Enter stock quantity: ");
    scanf("%d", &it.qty);

    
    

    items[itemCount++] = it;
    saveMenuToFile();
    printf("Item added.\n");
}

void adminEditItem() {
    loadMenuFromFile();
    displayMenuItems();
    if (itemCount == 0) { printf("No items to edit."); return;}
    int code; printf("Enter item code to edit: "); scanf("%d", &code);
    for (int i = 0; i < itemCount; i++) {
        if (items[i].code == code) {
            printf("Editing %s (code %d)\n", items[i].name, items[i].code);
            printf("Enter new name (enter '.' to keep): ");
            clear_input_buffer();
            char buff[64]; fgets(buff, sizeof(buff), stdin); strip_newline(buff);
            if (strcmp(buff, ".") != 0) strncpy(items[i].name, buff, 63);
            printf("Enter new price (0 to keep it same): ");
            float p; scanf("%f", &p); if (p > 0) items[i].price = p;
            printf("Enter change in stock (positive to add, negative to subtract, 0 none): ");
            int s; scanf("%d", &s); if (s != 0) items[i].qty += s;
            saveMenuToFile();
            printf("Item updated.");
            return;
        }
    }
    printf("Item code not found");
   

    
    
    
}

void adminDeleteItem() {
    loadMenuFromFile();
    displayMenuItems();
    if (itemCount == 0) { printf("No items.\n"); return; }
    int code; printf("Enter item code to delete: "); scanf("%d", &code);clear_input_buffer();
    for (int i = 0; i < itemCount; i++) {
        if (items[i].code == code) {
            for (int j = i; j < itemCount - 1; j++) items[j] = items[j + 1];
            itemCount--;
            saveMenuToFile();
            printf("Item deleted.");
            return;
        }
    }
    printf("Item not found.");
    return;
    
}


void adminAddStaff() {
    loadStaffFromFile();
    if (staffCount >= MAX_STAFF) {
        printf("Staff database full! Cannot add more staff.\n");
        return;
    }
    
    Staff s;
    printf("\n=== ADD NEW STAFF MEMBER ===\n");
    printf("Enter staff ID: ");
    scanf("%d", &s.id);
    clear_input_buffer();
    
    
    
    for (int i = 0; i < staffCount; i++) {
        if (staffs[i].id == s.id){
            printf("Error: Staff ID %d already exists!\n", s.id);
            fflush;
            return;
        }
    }
    
    printf("Enter staff name: ");
    
    fgets(s.name, sizeof(s.name), stdin);
    strip_newline(s.name);
    
    printf("Enter role (Chef/TableStaff/Delivery/Manager): ");
    fgets(s.role, sizeof(s.role), stdin);
    strip_newline(s.role);
    
    printf("Enter monthly salary: ");
    scanf("%f", &s.salary);
    clear_input_buffer();
    
    printf("Enter contact number: ");
    fgets(s.contact, sizeof(s.contact), stdin);
    strip_newline(s.contact);
    
    s.available = 1;
    
    staffs[staffCount++] = s;
    saveStaffToFile();
    printf("\nStaff member added successfully!\n");
    printf("Name: %s\n", s.name);
    printf("Role: %s\n", s.role);
    printf("Salary: %.2f", s.salary);
    fflush;
    
}

void adminEditStaff() {
    loadStaffFromFile();
    if (staffCount == 0) {
        printf("No staff members to edit.\n");
        return;
    }
    
    int id;
    printf("\n=== EDIT STAFF MEMBER ===\n");
    printf("Enter staff ID to edit: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    for (int i = 0; i < staffCount; i++) {
        if (staffs[i].id == id) {
            printf("\nEditing Staff ID: %d\n", id);
            printf("Current Name: %s\n", staffs[i].name);
            printf("Enter new name (or press Enter to keep current): ");
            char newName[64];
            fgets(newName, sizeof(newName), stdin);
            strip_newline(newName);
            if (strlen(newName) > 0) {
                strncpy(staffs[i].name, newName, 63);
            }
            
            printf("Current Role: %s\n", staffs[i].role);
            printf("Enter new role (or press Enter to keep current): ");
            char newRole[32];
            fgets(newRole, sizeof(newRole), stdin);
            strip_newline(newRole);
            if (strlen(newRole) > 0) {
                strncpy(staffs[i].role, newRole, 31);
            }
            
            printf("Current Salary: %.2f\n", staffs[i].salary);
            printf("Enter new salary (0 to keep current): ");
            float newSalary;
            scanf("%f", &newSalary);
            if (newSalary > 0) {
                staffs[i].salary = newSalary;
            }
            clear_input_buffer();
            
            printf("Current Contact: %s\n", staffs[i].contact);
            printf("Enter new contact (or press Enter to keep current): ");
            char newContact[32];
            fgets(newContact, sizeof(newContact), stdin);
            strip_newline(newContact);
            if (strlen(newContact) > 0) {
                strncpy(staffs[i].contact, newContact, 31);
            }
            
            saveStaffToFile();
            printf("\nStaff details updated successfully!");
            return;
        }
    }
    printf("Staff ID %d not found!\n", id);
}

void adminDeleteStaff() {
    loadStaffFromFile();
    if (staffCount == 0) {
        printf("No staff members to delete.\n");
        return;
    }
    
    int id;
    printf("\n=== DELETE STAFF MEMBER ===\n");
    printf("Enter staff ID to delete: ");
    scanf("%d", &id);
    clear_input_buffer();
    
    for (int i = 0; i < staffCount; i++) {
        if (staffs[i].id == id) {
            printf("\nAre you sure you want to delete?\n");
            printf("ID: %d\n", staffs[i].id);
            printf("Name: %s\n", staffs[i].name);
            printf("Role: %s\n", staffs[i].role);
            printf("Enter 'YES' to confirm deletion: ");
            
            char confirmation[10];
            fgets(confirmation, sizeof(confirmation), stdin);
            strip_newline(confirmation);
            
            if (strcmp(confirmation, "YES") == 0) {
                for (int j = i; j < staffCount - 1; j++) {
                    staffs[j] = staffs[j + 1];
                }
                staffCount--;
                saveStaffToFile();
                printf("Staff member deleted successfully!\n");
            } else {
                printf("Deletion cancelled.\n");
            }
            return;
        }
    }
    printf("Staff ID %d not found!\n", id);
}

void adminViewStaff() {
    loadStaffFromFile();
    if (staffCount == 0) { printf("No staff records.\n"); return; }
    printf("\nID\tName\t\tRole\t\tSalary\t\tContact\t\tAvailable\n");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < staffCount; i++) {
        printf("\n%d\t%-12s\t%-10s\t%.2f\t%s\t%s",
               staffs[i].id, staffs[i].name, staffs[i].role, staffs[i].salary, staffs[i].contact, staffs[i].available ? "Yes" : "No");
               fflush;
               
    }
    fflush;
    
}

void adminSearchStaff() {
    loadStaffFromFile();
    if (staffCount == 0) {
        printf("No staff members in database.\n");
        return;
    }
    
    int choice;
    printf("\n=== SEARCH STAFF ===\n");
    printf("1. Search by ID\n");
    printf("2. Search by Name\n");
    printf("3. Search by Role\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    clear_input_buffer();
    
    char searchTerm[64];
    int searchId;
    int found = 0;
    
    switch (choice) {
        case 1:
            printf("Enter staff ID to search: ");
            scanf("%d", &searchId);
            clear_input_buffer();
            printf("\nSearch Results:\n");
            printf("\nID\tName\t\tRole\t\tSalary\t\tContact\t\tAvailable\n");
            printf("--------------------------------------------------------------------------------\n");
            for (int i = 0; i < staffCount; i++) {
                if (staffs[i].id == searchId) {
                    printf("%d\t%-12s\t%-10s\t%.2f\t%s\t%s\n",
                           staffs[i].id, staffs[i].name, staffs[i].role, 
                           staffs[i].salary, staffs[i].contact, 
                           staffs[i].available ? "Yes" : "No");
                    found = 1;
                    break;
                }
                
            }
            break;
            
        case 2:
            printf("Enter staff name to search: ");
            fgets(searchTerm, sizeof(searchTerm), stdin);
            strip_newline(searchTerm);
            printf("\nSearch Results:\n");
            printf("\nID\tName\t\tRole\t\tSalary\t\tContact\t\tAvailable\n");
            printf("--------------------------------------------------------------------------------\n");
            for (int i = 0; i < staffCount; i++) {
                if (strstr(staffs[i].name, searchTerm) != NULL) {
                    printf("%d\t%-12s\t%-10s\t%.2f\t%s\t%s\n",
                           staffs[i].id, staffs[i].name, staffs[i].role, 
                           staffs[i].salary, staffs[i].contact, 
                           staffs[i].available ? "Yes" : "No");
                    found = 1;
                    
                }
            }
            break;
            
        case 3:
            printf("Enter role to search (Chef/TableStaff/Delivery/Manager): ");
            fgets(searchTerm, sizeof(searchTerm), stdin);
            strip_newline(searchTerm);
            printf("\nSearch Results:\n");
            printf("\nID\tName\t\tRole\t\tSalary\t\tContact\t\tAvailable\n");
            printf("--------------------------------------------------------------------------------\n");
            for (int i = 0; i < staffCount; i++) {
                if (strstr(staffs[i].role, searchTerm) != NULL) {
                    printf("%d\t%-12s\t%-10s\t%.2f\t%s\t%s\n",
                           staffs[i].id, staffs[i].name, staffs[i].role, 
                           staffs[i].salary, staffs[i].contact, 
                           staffs[i].available ? "Yes" : "No");
                    found = 1;
                }
            }
            break;
            
        default:
            printf("Invalid choice!\n");
            return;
    }
    
    if (!found) {
        printf("No staff members found matching your search.\n");
    }
}

void adminViewStaffByRole() {
    loadStaffFromFile();
    if (staffCount == 0) {
        printf("No staff members in database.\n");
        return;
    }
    
    printf("\n=== STAFF BY ROLE ===\n");
    
    int chefs = 0, tableStaff = 0, delivery = 0, managers = 0, others = 0;
    
    for (int i = 0; i < staffCount; i++) {
        if (strstr(staffs[i].role, "Chef") != NULL) chefs++;
        else if (strstr(staffs[i].role, "TableStaff") != NULL) tableStaff++;
        else if (strstr(staffs[i].role, "Delivery") != NULL) delivery++;
        else if (strstr(staffs[i].role, "Manager") != NULL) managers++;
        else others++;
    }
    
    printf("Chefs: %d\n", chefs);
    printf("Table Staff: %d\n", tableStaff);
    printf("Delivery Staff: %d\n", delivery);
    printf("Managers: %d\n", managers);
    printf("Others: %d\n", others);
    printf("Total Staff: %d\n", staffCount);
}

void adminToggleStaffAvailability() {
    loadStaffFromFile();
    if (staffCount == 0) { printf("No staff.\n"); return; }
    int id; printf("Enter staff id to toggle availability: "); scanf("%d", &id);
    for (int i = 0; i < staffCount; i++) {
        if (staffs[i].id == id) {
            staffs[i].available = !staffs[i].available;
            saveStaffToFile();
            printf("Staff %d availability set to %s\n", id, staffs[i].available ? "Available" : "Busy");
            return;
        }
    }
    printf("Staff not found.\n");
}

void calculateTotalSalaryExpense() {
    loadStaffFromFile();
    if (staffCount == 0) {
        printf("No staff members in database.\n");
        return;
    }
    int sid;
    float totalMonthly = 0.0;
    float totalAnnual = 0.0;
    printf("Enter Staff ID (0 for all): ");
    scanf("%d", &sid);
    clear_input_buffer();
    if(sid != 0){ 
        for(int i=0;i<staffCount;i++){
            if(staffs[i].id == sid){
                printf("\n=== SALARY EXPENSE REPORT ===\n");
                printf("\nID\tName\t\tRole\t\tSalary\t\tContact\t\tAvailable\n");
                printf("--------------------------------------------------------------------------------\n");
                printf("%d\t%-12s\t%-10s\t%.2f\n",
                    staffs[i].id, staffs[i].name, staffs[i].role, staffs[i].salary);
                totalMonthly += staffs[i].salary;
                totalAnnual = totalMonthly * 12;
                printf("-----------------------------------------------------------------\n");
                printf("Total Monthly Salary Expense: %.2f\n", totalMonthly);
                printf("Total Annual Salary Expense: %.2f\n", totalAnnual);
                continue;
            }
            printf("\nStaff ID does not exist\n");
        }
    }

    printf("\n=== SALARY EXPENSE REPORT ===\n");
    printf("\nID\tName\t\tRole\t\tSalary\t\tContact\t\tAvailable\n");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < staffCount; i++) {
        printf("%d\t%-12s\t%-10s\t%.2f\n",
               staffs[i].id, staffs[i].name, staffs[i].role, staffs[i].salary);
        totalMonthly += staffs[i].salary;
    }
    
    totalAnnual = totalMonthly * 12;
    
    printf("------------------------------------------------------------------------\n");
    printf("   Total Monthly Salary Expense: %.2f\n", totalMonthly);
    printf("   Total Annual Salary Expense: %.2f\n", totalAnnual);
    printf("   Total Staff Members: %d\n", staffCount);
    printf("   Average Salary per Staff: %.2f\n", totalMonthly / staffCount);
}

void adminViewSalaries(){
    loadStaffFromFile();
    if (staffCount == 0){ printf("No staff.\n"); return; }
    printf("\nStaff Salaries and Contacts:\n");
    for (int i = 0; i < staffCount; i++){
        printf("%d - %s (%s): Salary %.2f, Contact %s\n", staffs[i].id, staffs[i].name, staffs[i].role, staffs[i].salary, staffs[i].contact);
    }
}

void ensureDefaultTables(int n) {
    loadTablesFromFile();
    if (tableCount >= n) return;
    // create tables if file empty
    for (int i = tableCount; i < n && i < MAX_TABLES; i++) {
        tables[i].id = i + 1;
        tables[i].reserved = 0;
        tables[i].by[0] = '\0';
    }
    tableCount = n;
    saveTablesToFile();
}

void viewTables() {
    loadTablesFromFile();
    if (tableCount == 0) { printf("No tables defined. Admin can define tables.\n"); return; }
    printf("\nTableID\tStatus\tBy\n-------------------------\n");
    for (int i = 0; i < tableCount; i++) {
        printf("%d\t%s\t%s\n", tables[i].id, tables[i].reserved ? "Reserved" : "Free", tables[i].reserved ? tables[i].by : "-");
    }
}

void reserveTableByManagerOrCustomer(char *custName) {
    loadTablesFromFile();
    if (tableCount == 0) { printf("No tables available.\n"); return; }
    int tid;
    viewTables();
    printf("Enter table id to reserve: "); scanf("%d", &tid); clear_input_buffer();
    for (int i = 0; i < tableCount; i++) {
        if (tables[i].id == tid) {
            if (tables[i].reserved) { printf("Table already reserved by %s\n", tables[i].by); return; }
            tables[i].reserved = 1;
            if (custName && strlen(custName) > 0) strncpy(tables[i].by, custName, 63);
            else {
                printf("Enter customer name for reservation: ");
                fgets(tables[i].by, sizeof(tables[i].by), stdin); strip_newline(tables[i].by);
            }
            saveTablesToFile();
            printf("Reserved table %d for %s\n", tid, tables[i].by);
            return;
        }
    }
    printf("Invalid table id\n");
    pause_and_continue();
}

void unreserveTable() {
    loadTablesFromFile();
    viewTables();
    if (tableCount == 0) { printf("No tables.\n"); return; }
    int tid; printf("Enter table id to unreserve: "); 
    scanf("%d", &tid); 
    clear_input_buffer();
    for (int i = 0; i < tableCount; i++) {
        if (tables[i].id == tid) {
            if (!tables[i].reserved) { printf("Table is already free.\n"); return; }
            tables[i].reserved = 0; tables[i].by[0] = '\0';
            saveTablesToFile();
            printf("Table %d is now unreserved.\n", tid);
            return;
        }
    }
    printf("Invalid id.\n");
}

int getNextOrderId() {
    FILE *fp = fopen("sales.txt", "r");
    int maxid = 0;
    if (!fp) return 1;
    while (!feof(fp)) {
        int oid; if (fscanf(fp, "ORDER_ID:%d,", &oid) == 1) if (oid > maxid) maxid = oid;
        // read next line
        char tmp[256]; if (!fgets(tmp, sizeof(tmp), fp)) break;
    }
    fclose(fp);
    return maxid + 1;
}

int assignDeliveryStaff() {
    loadStaffFromFile();
    for (int i = 0; i < staffCount; i++) {
        // choose staff with role "Delivery" and available
        if (strstr(staffs[i].role, "Delivery") != NULL || strstr(staffs[i].role, "delivery") != NULL) {
            if (staffs[i].available) {
                staffs[i].available = 0; // mark busy
                saveStaffToFile();
                return staffs[i].id;
            }
        }
    }

    return 0; // none available
}

void recordSaleToFile(int orderId, const char *type, float total) {
    FILE *fp = fopen("sales.txt", "a");
    if (!fp) return;
    // Add a line with ORDER_ID:...,TYPE,total,datetime
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timestr[64]; strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
    fprintf(fp, "ORDER_ID:%d,%s,%.2f,%s\n", orderId, type, total, timestr);
    fclose(fp);
}

void recordDeliveryToFile(int orderId, char *cust, char *addr, int staffId, float total) {
    FILE *fp = fopen("delivery.txt", "a");
    if (!fp) return;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timestr[64]; strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
    fprintf(fp, "ORDER_ID:%d,CUSTOMER:%s,ADDR:%s,DELIVERED_BY:%d,TOTAL:%.2f,%s\n", orderId, cust, addr, staffId, total, timestr);
    fclose(fp);
}


void takeOrderFlow(int byManager) {
   
    loadMenuFromFile();
    if (itemCount == 0) { printf("No menu items available.\n"); return; }
    displayMenuItemsCustomer();
    int orderId = getNextOrderId();
    printf("\n--- Taking Order (Order ID #%d) ---\n", orderId);

    
    char custName[64] = "Guest";
    char address[128] = "";
    int isDelivery = 0;
    int chooseDelivery = 0;

    if (!byManager) {
        
        printf("Do you want delivery? (1 = yes, 0 = dine-in): ");
        scanf("%d", &chooseDelivery); clear_input_buffer();
        if (chooseDelivery == 1) {
            isDelivery = 1;
            printf("Enter your name: "); fgets(custName, sizeof(custName), stdin); strip_newline(custName);
            printf("Enter delivery address: "); fgets(address, sizeof(address), stdin); strip_newline(address);
        } else {
            
            printf("Do you want to reserve a table? (1 = yes, 0 = no): ");
            int r; scanf("%d", &r); clear_input_buffer();
            if (r == 1) {
                printf("Enter your name for reservation: "); fgets(custName, sizeof(custName), stdin); strip_newline(custName);
                reserveTableByManagerOrCustomer(custName);
            } else {
                printf("Enter name for order (or press ENTER to stay Guest): ");
                fgets(custName, sizeof(custName), stdin); strip_newline(custName);
                if(strcmp(custName, "Guest") == 0) printf("Bruh! Could have just pressed enter.\n");
                if (strlen(custName) == 0) strncpy(custName, "Guest", sizeof(custName));
            }
        }
    } else {
        
        printf("Enter customer name: "); fgets(custName, sizeof(custName), stdin); strip_newline(custName);
        printf("Delivery? (1 = yes, 0 = dine-in): "); scanf("%d", &chooseDelivery); clear_input_buffer();
        if (chooseDelivery == 1) { isDelivery = 1; printf("Enter address: "); fgets(address, sizeof(address), stdin); strip_newline(address); }
        else isDelivery = 0;
    }

    // order loop
    float subtotal = 0.0;
    char type[100];
    int more = 1;
    while (more) {

        displayMenuItemsCustomer();
        printf("\nEnter item code to add (or 0 to finish): ");
        int code; scanf("%d", &code); clear_input_buffer();
        if (code == 0) break;
        int found = 0;
        for (int i = 0; i < itemCount; i++) {
            if (items[i].code == code) {
                found = 1;
                printf("Enter quantity: "); int q; scanf("%d", &q); clear_input_buffer();
                if (q <= 0) { printf("Invalid quantity\n"); break; }
                if (items[i].qty < q) { printf("Only %d in stock. Cannot add.\n", items[i].qty); break; }
                items[i].qty = items[i].qty - q;
                saveMenuToFile();
                float line = items[i].price * q;
                subtotal += line;
                printf("Added %s x %d = %.2f\n", items[i].name, q, line);
                strcpy(type, items[i].name);
                FILE *fp = fopen("sales.txt", "a");
                if (fp) {
                    fprintf(fp, "ORDER_ID:%d,QTY:%d,AMOUNT:%.2f,%s\n", orderId, q, line, type);
                    fclose(fp);
                }
                break;
            }
            
        }
        if (!found) printf("Item code not found.\n");
        

        printf("Add more items? (1 = yes, 0 = no): ");
        scanf("%d", &more); clear_input_buffer();
        
    }

    // compute discounts & tax
    float discount = 0.0;
    if (subtotal >= DISCOUNT_THRESHOLD) discount = subtotal * (DISCOUNT_PERCENT / 100.0);
    float taxed = (subtotal - discount) * (TAX_PERCENT / 100.0);
    float total = subtotal - discount + taxed;
    if (isDelivery) {
        total += DELIVERY_CHARGE;
    }

    // write back updated menu stocks
    saveMenuToFile();

    // Attempt to assign delivery staff if needed
    int assignedStaffId = 0;
    if (isDelivery) {
        assignedStaffId = assignDeliveryStaff();
        if (assignedStaffId == 0) {
            printf("Warning: No delivery staff available right now. Order will be pending assignment.\n");
        } else {
            printf("Delivery assigned to staff ID %d\n", assignedStaffId);
        }
    }

    // finalize order records
    recordSaleToFile(orderId, isDelivery ? "DELIVERY" : "DINE_IN", total);
    if (isDelivery) recordDeliveryToFile(orderId, custName, address, assignedStaffId, total);

    // Add a final summary to the sales.txt for readability
    FILE *fp = fopen("sales.txt", "a");
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char timestr[64]; strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
    if (fp) {
        fprintf(fp, "ORDER_ID:%d,SUMMARY,Subtotal:%.2f,Discount:%.2f,Tax:%.2f,DeliveryCharge:%.2f,Total:%.2f,%s,%s\n",
                orderId, subtotal, discount, taxed, isDelivery?DELIVERY_CHARGE:0.0, total, timestr, type);
        fclose(fp);
    }

    printf("\n--- Receipt ---\n");

    printf("Order ID: %d\nCustomer: %s\n", orderId, custName);
    if (isDelivery) printf("Delivery Address: %s\n", address);
    printf("Subtotal: %.2f\nDiscount: %.2f\nTax: %.2f\n", subtotal, discount, taxed);
    if (isDelivery) printf("Delivery Charge: %.2f\n", DELIVERY_CHARGE);
    printf("Total: %.2f\n", total);
    printf("Time: %s\n", timestr);
    printf("Items: %s\n", type);

    pause_and_continue();
}

/* ------------ View Sales Report ------------ */
void viewSalesReport() {
    FILE *fp = fopen("sales.txt", "r");
    if (!fp) { printf("No sales yet.\n"); return; }
    printf("\n--- Sales Log ---\n");
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
}

/* ------------ Login with retry logic ------------ */
int loginWithRetry(const char *roleExpected) {
	system("cls");
    // roleExpected: "admin" or "manager"
    char uname[64], pass[64];
    int retryChoice = 0;
    while (1) {
        printf("Enter username: "); fgets(uname, sizeof(uname), stdin); strip_newline(uname);
        printf("Enter password: "); fgets(pass, sizeof(pass), stdin); strip_newline(pass);
        if (strcmp(roleExpected, "admin") == 0) {
            if (strcmp(uname, "yassu") == 0 && strcmp(pass, "yassu123") == 0) return 1;
        } else if (strcmp(roleExpected, "manager") == 0) {
            if (strcmp(uname, "papu") == 0 && strcmp(pass, "papu123") == 0) return 1;
        }
        printf("\nWrong username or password!\nPress any number to try again or '1' to exit: ");
        int c; if (scanf("%d", &c) != 1) { clear_input_buffer(); c = 1; }
        clear_input_buffer();
        system("cls");
        if (c == 1) return 0;
    }
}


void adminPanel() {
	system("cls");
    printf("Welcome Admin\n");
    int choice;
    int choice2;
    do {
        printf("\n==== ADMIN PANEL ======\n");
        printf("1. Staff Management\n2. Menu Management\n3. System Management\n4. Log Out\nEnter Choice: ");
        scanf("%d", &choice2);
        clear_input_buffer();
        
        if(choice2 == 1){
            system("cls");
            printf("=== STAFF MANAGEMENT ===\n");
            printf("1. Add Staff Member\n2. Edit Staff Member\n3. Delete Staff Member\n4. View All Staff\n5. Search Staff\n6. View Staff by Role\n7. Toggle Staff Availability\n8. Calculate Salary Expense\n9. View Staff Salaries & Contacts\n0. Return to Menu\n");
        }
        else if(choice2 == 2){
            system("cls");
            printf("\n=== MENU MANAGEMENT ===\n");
            printf("10. Add Menu Item\n11. Edit Menu Item\n12. Delete Menu Item\n13. View Menu\n0. Return to Menu");
        }
        else if(choice2 == 3){
            system("cls");
            printf("\n=== SYSTEM MANAGEMENT ===\n14. View Tables\n15. Ensure Default Tables\n16. View Sales Report\n0. Return to Menu\n17. Logout\n");
        
        }
        else if(choice2 == 4){
            printf("Logging out.\n");
            return;
        }
        else{
            printf("\nWrong username or password!\nPress 0 to try again or 1 to exit: ");
            int c; if (scanf("%d", &c) != 1) { clear_input_buffer(); c = 1; }
            clear_input_buffer();
            system("cls");
            continue;
        }
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clear_input_buffer();
        
        switch (choice) {
            //staff
            case 0: system("cls");continue;
            case 1: adminAddStaff(); break;
            case 2: adminEditStaff(); break;
            case 3: adminDeleteStaff(); break;
            case 4: adminViewStaff();break;
            case 5: adminSearchStaff(); break;
            case 6: adminViewStaffByRole(); break;
            case 7: adminToggleStaffAvailability(); break;
            case 8: calculateTotalSalaryExpense(); break;
            case 9: adminViewSalaries(); break;
            
            //menu
            case 10: adminAddItem(); break;
            case 11: adminEditItem();break;
            case 12: adminDeleteItem(); break;
            case 13: displayMenuItems(); break;
            
            //system        
            case 14: viewTables(); break;
            case 15: ensureDefaultTables(10); printf("Created/ensured 10 tables.\n"); break;
            case 16: viewSalesReport(); break;
            case 17: printf("Logging out Admin.\n"); break;
            
            default: printf("Invalid choice! Please try again.\n"); break;
        }
        
        if (choice != 17){
            clear_input_buffer();
            pause_and_continue();
        }
    } while (choice != 17);
}

void managerPanel() {
	system("cls");
    int choice = 0;
    do {
        printf("\n===== MANAGER PANEL =====\n");
        printf("1. View Menu\n2. Take Order\n3. Reserve Table\n4. Unreserve Table\n5. View Tables\n6. View Sales Report\n7. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice); 
        clear_input_buffer();
        switch (choice) {
            case 1: displayMenuItems(); break;
            case 2: takeOrderFlow(1); break;
            case 3: reserveTableByManagerOrCustomer(NULL); break;
            case 4: unreserveTable(); break;
            case 5: viewTables(); break;
            case 6: viewSalesReport(); break;
            case 7: printf("Logging out Manager.\n"); break;
            default: printf("Invalid\n"); break;
        }
    } while (choice != 7);
}

void customerPanel() {
	system("cls");
    int choice = 0;
    do {
        printf("\n===== CUSTOMER MENU =====\n");
        printf("1. View Menu\n2. Place Order\n3. Reserve Table\n4. Exit to Main\n");
        printf("Enter choice: ");
        scanf("%d", &choice); clear_input_buffer();
        switch (choice) {
            case 1: displayMenuItemsCustomer(); pause_and_continue();break;
            case 2: takeOrderFlow(0); break;
            case 3: reserveTableByManagerOrCustomer(NULL); break;
            case 4: printf("Thanks for visiting!\n"); break;
            default: printf("Invalid\n"); break;
        }
    } while (choice != 4);
}


int main() {
	system("cls");
    ensureFilesExist();
    loadMenuFromFile();
    loadStaffFromFile();
    loadTablesFromFile();
    ensureDefaultTables(10);

    int mainChoice = 0;
    while (1){
        system("cls");
        setColor(11);
        printf("\t\t\t\t\t=======================================\n");
        printf("\t\t\t\t\t YASSU's RESTAURANT MANAGEMENT SYSTEM\n");
        printf("\t\t\t\t\t=======================================\n");
        resetColor();
        setColor(3);
        printf("\n\t\t\t\t\t\t\tMain Menu:\n\n\t\t\t\t\t\t1. Admin Login\n\t\t\t\t\t\t2. Manager Login\n\t\t\t\t\t\t3. Customer\n\t\t\t\t\t\t4. Exit\n");
        resetColor();
        printf("\t\t\t\t\t\tEnter choice: ");
        if (scanf("%d", &mainChoice) != 1)
        { 
            clear_input_buffer(); 
            mainChoice = 4; 
        }
        clear_input_buffer();
        if (mainChoice == 1){
            if (loginWithRetry("admin")) adminPanel();
            else { printf("Back to main menu.\n"); }
        } else if (mainChoice == 2){
            if (loginWithRetry("manager")) managerPanel();
            else { printf("Back to main menu.\n"); }
        } else if (mainChoice == 3){
            customerPanel();
        } else if (mainChoice == 4){
            printf("Exiting program. Goodbye!\n");
            break;
        } else {
            printf("Invalid choice\n");
        }
    }

    return 0;
}