#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<time.h>

struct foodMenu{
    char name[1024];
    int qty;
    double price;
};

void getOrder(char name[], struct foodMenu order[], int size, int foodNameError, char foodName[], int qtyError);
void receipt(char name[], struct foodMenu order[], int size);
char mainMenu(int error);
double cost(struct foodMenu order[], int size);
void displayOrder(struct foodMenu order[], int size);
int equalsIgnoreCase(char s1[], char s2[]);
int paymentMenu(struct foodMenu order[], int size, int notEnough);
void record(struct foodMenu order[], int size);

int s = 0;  //counter of food available in menu
char payment[15];   //holds the payment

int main(){
    for(int i = 0 ; i < sizeof(payment) ; i++)  //initialize payment character array to NULL
        payment[i] = 0;

    //count lines in file to determine size alloted for array of structs foodmenu
    FILE* fileCounter = fopen("menu.csv","r");
    //catcher if menu.csv is not found
    if(fileCounter==NULL){ 
        printf("Menu is out of order.");
        fclose(fileCounter);
        return 0;
    }
    //start of counting
    char lineCounter[1024];
    while(fgets(lineCounter, 1024, fileCounter))
        s++;
    fclose(fileCounter);
    s--;        //to disregard the labels counted in the csv file
    if(s == 0){       //catcher if menu.csv is empty
        printf("Menu is out of order.");
        return 0;
    }
    
    struct foodMenu order[s];     //declare array of struct foodmenu

    //transfer data from file to foodmenu_class array
    FILE* fileTransfer = fopen("menu.csv","r");
    char lineTransfer[1024];
    fgets(lineTransfer, 1024, fileTransfer); //reads first line (labels in csv file)
    for(int i=0; i < s ; i++){
        fgets(lineTransfer, 1024, fileTransfer);
        const char *data = strtok(lineTransfer, ",");
        strcpy(order[i].name, data);
        int trash = equalsIgnoreCase("", order[i].name);  //make uppercase
        order[i].price = atof(strtok(NULL, ","));
        order[i].qty = 0;
    }
    fclose(fileTransfer);

    system("clear");    //clear console
    char name[50];
    printf("Enter name: ");
    scanf(" %s",name);       //get username
    
    //start of ordertaker ____________________________
    int cont = 1;
    int choiceError = 0;
    while(cont){
        system("clear");    //clear console
        int paid = 0;
        char choice = mainMenu(choiceError);    //main menu
        choiceError = 0;
        switch (choice){
            case '1':
                getOrder(name, order, s, 0, "", 0);
                printf("TEST\n");
                cont = 1;
                break;
            case '2':
                paid = paymentMenu(order, s, 0);
                break;
            case '3':
                cont = 0;
                system("clear");    //clear console
                printf("Thank you for using, have a nice day!\n");
                break;
            default:
                choiceError = 1;
        }
        if(paid){
            system("clear");    //clear console
            receipt(name, order, s);     //display receipt
            record(order, s);
            cont = 0;   //end program if paid enough
        }
    }
    
    return 0;
}

//general functions
char mainMenu(int error){
    printf("MAIN MENU:\n");
    printf("1 - Get Order\n");
    printf("2 - Proceed to Payment\n");
    printf("3 - Exit\n");
    if(error)
        printf("Invalid choice, try again");
    printf("\nEnter choice: ");
    char choice[20];
    scanf(" %s",choice);
    if(choice[1]!=0)
        return '0';
    return choice[0];
}

int paymentMenu(struct foodMenu order[], int size, int notEnough){
    system("clear");    //clear console
    int notEmpty = 0;
    for(int i = 0 ; i < size ; i++){    //check if list of order is empty
        if(order[i].qty>0)
            notEmpty = 1;
    }
    if(!notEmpty){      //if list of order is empty, return to main menu
        printf("No orders added\nPress Enter to continue...");
        getchar();
        getchar();
        return 0;
    }

    displayOrder(order, size);      //display list of order
    printf("Total cost: %.2f\n", cost(order, size));    //display total cost
    printf("Type anything to go back to Main Menu.\n");

    if(notEnough)   //display error if payment not enough
        printf("Not Enough");
    printf("\nEnter payment: ");  //get payment as string
    scanf(" %s", payment);
    for(int i = 0 ; i < sizeof(payment) ; i++)  //check if input is a valid double value
        if((payment[i]<'0' || payment[i]>'9') && payment[i]!=0 && payment[i] != '.' && payment[i] != '-')
            return 0;
    if(atof(payment) < cost(order, size)){    //recurse if payment is not enough
        return paymentMenu(order, size, 1);
    }
    printf("Your change is: %.2f\n", atof(payment) - cost(order, size));
    printf("Press enter to proceed...");
    getchar();
    getchar();
    return 1;
}

void receipt(char name[], struct foodMenu order[], int size){
    printf("---------RECEIPT---------\n\n");
    printf("Customer Name: %s\n", name);
    printf("\nOrder List:\n");
    for(int i=0 ; i < size ; i++)
        if(order[i].qty>0)
            printf("%d %s - %.2f\n", order[i].qty, order[i].name, order[i].price * order[i].qty);

    printf("\nTotal cost: \t%.2f\n", cost(order, size));
    printf("Payment: \t%.2f\n", atof(payment));
    printf("\t\t-----\n");
    printf("Change: \t%.2f\n", atof(payment) - cost(order, size));
    time_t t;
    time(&t);
    printf("\nDate: %s", ctime(&t));
    printf("\n-------------------------\n");
}

void getOrder(char name[50], struct foodMenu order[s], int size, int foodNameError, char foodName[20], int qtyError){
    system("clear"); //clear console
    printf("Hi %s, ",name); 
    displayOrder(order, size);  //display orderlist
    printf("Total cost: %.2f\n\n", cost(order, size));  //display cost
    printf("What would you like to order?\n\n");
    printf("Menu:\n");
    for(int i=0 ; i < size ; i++)   //display menu
            printf("%s - P%.2f\n", order[i].name, order[i].price);
    printf("Type \"..\" to go back.\n\n");

    if(foodNameError)   //display foodnameerror
        printf("Error: \"%s\" is not in the menu!", foodName);
    else if(qtyError)
        printf("Error: Quantity Invalid!");

    //enter food name
    char* foodInput = (char*) malloc(20 * sizeof(char));
    printf("\nEnter food name: ");
    scanf(" %[^\n]", foodInput);
    if(strcmp(foodInput, "..") == 0){ //exit when .. is inputted
        return;
    }

    for(int i = 0 ; i < size ; i++){    //find input if it exists
        if(equalsIgnoreCase(order[i].name, foodInput)==0){
            foodNameError = 0;
            break;
        }else
            foodNameError = 1;
    }
    if(foodNameError){
        getOrder(name, order, size, foodNameError, foodInput, 0);
        return;
    }

    //enter quantity
    char qty[10];
    for(int i = 0 ; i < sizeof(qty) ; i++)  //initialize all element in qty to null
        qty[i] = 0;
    printf("Enter quantity: ");     //get qty
    scanf(" %s", qty);
    //check if quantity is valid
    for(int i = 0 ; i < sizeof(qty) ; i++){
        if((qty[i]<'0' || qty[i]>'9') && qty[i]!=0 && qty[i]!='-'){
            getOrder(name, order, size, 0, "", 1);
            return;
        }
    }

    //add qty
    for(int i = 0 ; i < size ; i++){
        if(equalsIgnoreCase(order[i].name, foodInput)==0)   //add qty to corresponding foodinput
            order[i].qty += atoi(qty);
        if(order[i].qty<0)      //reset to zero if qty goes negative
            order[i].qty = 0;
    }
    getOrder(name, order, size, 0, "", 0);
}

void record(struct foodMenu order[], int size){
    //open file to append
    FILE* fileRecord = fopen("records.csv", "a");
    fprintf(fileRecord, "\n");  //print newline
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    //print date and profit
    fprintf(fileRecord, "%d-%d-%d,%.2f,", tm.tm_mday, tm.tm_mon+1, tm.tm_year+1900, cost(order, size));
    //print list of orders
    for(int i = 0 ; i < size ; i++)
        if(order[i].qty>0)
            fprintf(fileRecord, " %d %s /", order[i].qty, order[i].name);
    //close file
    fclose(fileRecord);
}

//supporting functions
double cost(struct foodMenu order[], int size){
    double total = 0;
    for(int i=0 ; i < size ; i++)
        total += order[i].qty * order[i].price;
    return total;
}

void displayOrder(struct foodMenu order[], int size){
    int morethanone = 0;
    printf("your current orders are: ( ");
    for(int i=0 ; i < size ; i++){
        if(order[i].qty<1)
            continue;
        if(morethanone)
            printf(" : ");
        morethanone = 1;
        printf("%d %s",order[i].qty, order[i].name);
    }
    printf(" )\n");
}

int equalsIgnoreCase(char *s1, char *s2){
    char string[1024];
    strcpy(string, s2);
    for(int i=0 ; i < sizeof(string) ; i++){
        s2[i] = toupper(s2[i]);
    }
    return strcmp(s1,s2);
}
