#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


//yes I am aware that I should have made a function that verifies the input,but i made that realization too late in the development of the app
//the first limitation of this program is the orderitems string length, but, if set to 1000 it should allow up to an approximate 200 items per order.

struct order{   //structure for an order.
    int orderid;
    char ordertype[5];
    char orderitems[800];
    int price;
};
struct order StringtoOrder(char* temp){
    struct order ord;
    ord.orderid = atoi( strtok(temp,","));
    strcpy(ord.ordertype, strtok(NULL, ","));
    strcpy(ord.orderitems, strtok(NULL,","));
    ord.price = atoi(strtok(NULL,","));
    return ord;
};

struct product
{
    int id;
    char name[200];
    int price;
    int stock;
};

struct product StringToProduct(char *temp)
{
    struct product prod;
    if (temp[0] == '\0' || temp[0] == '\n') {
        prod.id = -1;
        return prod;
    }
    prod.id = atoi(strtok(temp, ","));
    strcpy(prod.name, strtok(NULL, ","));
    prod.price = atoi(strtok(NULL, ","));
    prod.stock = atoi(strtok(NULL, ","));
    return prod;
};

void Printer(int n)  //Prints any long text
{
    switch(n)
    {
        case 0:
            printf(
                    "\nPlease choose from one of the following actions:\n"
                    "1.Login/Register.\n"
                    "2.Search products.\n"
                    "3.View catalogue.\n"
                    "4.Help\n"
                    "0.Exit store.\n");
            break;
        case 1:
            break;
        case 2:
            printf("\nPlease choose the search mode you'd like:\n"
                   "1.Based on a key word/s\n"
                   "2.Based on price,where you provide a lower and an upper limit\n"
                   "3.Both where the result has to contain the key word/s and be its price has to be within the given limits\n"
                   "0.Return to home screen\n");
            break;
        case 3:
            break;
        case 4:
            printf("\nThe Login/Register function:\n"
                   "Accessed by typing 1, will either allow you to login into your already existing account or, in absence of one will create a new one.\n\n"
                   "The Place order function(only accessible by logged in members):\n"
                   "Accessed by typing 1 after logging in,will allow you to order products in selected quantities\n\n"
                   "The Sell products to store function(only accessible by logged in members):\n"
                   "Accessed by typing 2 after logging in,will allow you to sell items to the store.\n\n"
                   "The View placed orders function(only accessible by logged in members):\n"
                   "Accessed by typing 3 after logging in,will allow you to see alll your orders.\n\n"
                   "The search products function:\n"
                   "Accessed by typing 5,will allow you to see all the items in the catalogue that have a specific term in their name\n\n"
                   "The view catalogue function:\n"
                   "Accessed by typing 6,will allow you to see the list of all products available for purchase from the store.\n\n");
            break;
    }

};

void OrderPlacement(FILE *user, FILE* catalogue) {
    int productId;
    int quantity;
    int found = 0;
    // Prompt user for product ID
    printf("Please enter the ID of the product you want to buy: ");
    scanf("%d", &productId);

    // Check if the product ID exists in the catalogue
    rewind(catalogue);
    char temp[1000];
    while (fgets(temp, sizeof(temp), catalogue) != NULL) {
        struct product prod = StringToProduct(temp);
        if (prod.id == productId) {
            found = 1;
            printf("Product found: %s\n", prod.name);
            printf("Available stock: %d\n", prod.stock);
            printf("Please enter the quantity you want to buy: ");
            scanf("%d", &quantity);

            // Check if the requested quantity is available
            if (quantity <= prod.stock) {
                int totalPrice = quantity * prod.price;
                printf("Total price for %d %s(s): %d\n", quantity, prod.name, totalPrice);

                // Confirm purchase
                char confirm;
                printf("Confirm purchase? (Y/N): ");
                scanf(" %c", &confirm);

                if (confirm == 'Y' || confirm == 'y') {
                    // Update stock in the catalogue
                    FILE *tempFile = fopen("temp.csv", "w");
                    rewind(catalogue); // Move file cursor to the beginning of the file
                    while (fgets(temp, sizeof(temp), catalogue) != NULL) {
                        struct product tempProd = StringToProduct(temp);
                        if (tempProd.id == productId) {
                            tempProd.stock -= quantity; // Reduce stock
                            fprintf(tempFile, "%d,%s,%d,%d\n", tempProd.id, tempProd.name, tempProd.price, tempProd.stock);
                        } else {
                            fprintf(tempFile, "%d,%s,%d,%d\n", tempProd.id, tempProd.name, tempProd.price, tempProd.stock);
                        }
                    }
                    fclose(catalogue);
                    fclose(tempFile);
                    remove("catalogue.csv");
                    rename("temp.csv", "catalogue.csv");

                    // Write order to user file
                    fprintf(user, "%d,BUY,%s.%d,%d", productId, prod.name, quantity, totalPrice);
                    printf("Order placed successfully!\n");
                } else {
                    printf("Order canceled.\n");
                }
            } else {
                printf("Sorry, the requested quantity is not available.\n");
            }
            break; // Exit loop since product found
        }
    }

    // If product not found
    if (!found) {
        printf("Product not found in the catalogue.\n");
    }
    return;
}

void ProductSelling(FILE *user, FILE *catalogue) {
    char temp[200];
    int lastid = 0; // Initialize lastid to avoid uninitialized variable usage
    // Find the last ID in the catalogue
    while (fgets(temp, sizeof(temp), catalogue) != NULL) {
        struct product prod = StringToProduct(temp);
        if (prod.id > lastid) {
            lastid = prod.id;
        }
    }
    rewind(catalogue); // Reset file pointer to the beginning of the file

    printf("Please enter the number of product types you'd like to sell:\n");
    int types;
    scanf("%d", &types);
    getchar(); // Consume newline character

    printf("Please enter the products you'd like to be sold to the store in the following format: [product name] [product price] [product quantity].\n"
           "If the items are already in the store's catalogue, please make sure they have the same name.\n");

    // Loop through each product type to be sold
    for (int i = 0; i < types; i++) {
        lastid++; // Increment last ID for the new product
        struct product tempprod;
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = '\0'; // Remove newline character

        // Parse product information
        tempprod.id = lastid;
        strcpy(tempprod.name, strtok(temp, " "));
        tempprod.price = atoi(strtok(NULL, " "));
        tempprod.stock = atoi(strtok(NULL, " "));

        fprintf(catalogue, "%d,%s,%d,%d\n", tempprod.id, tempprod.name, tempprod.price, tempprod.stock);
        fprintf(user, "%d,SELL,%s.%d,%d\n", tempprod.id, tempprod.name, tempprod.stock, tempprod.stock * tempprod.price);
    }

    // Close the files
    fclose(catalogue);
    fclose(user);
}


void OrderView(FILE* user) {
    char temp[1000];


    // Read the first line from the file
    if (fgets(temp, sizeof(temp), user) == NULL) {
        printf("You have not placed any orders yet.\n");
        return;
    }

    // Loop through the file and process each order
    do {
        struct order ord = StringtoOrder(temp);
        printf("Order Type: %s  ", ord.ordertype);
        printf("Order Id: %d    ", ord.orderid);
        printf("Order price: %d   Ordered products:\n", ord.price);

        // Process ordered products
        char *token = strtok(ord.orderitems, "_");
        while (token != NULL) {
            struct product prod;
            strcpy(prod.name, strtok(token, "."));
            prod.stock = atoi(strtok(NULL, "."));
            printf("%s %d\n", prod.name, prod.stock);
            token = strtok(NULL, "_");
        }

        // Read the next line
    } while (fgets(temp, sizeof(temp), user) != NULL);
    return;
}

void Authentication(FILE* catalogue) //Allows the user to authenticate and use the order,see orders and sell products commands
{
    char *name = malloc(50*sizeof(char));
    char *surname = malloc(50*sizeof(char));
    printf("Please enter login information(format:Name Surname):");
    scanf("%s %s",name,surname);
    for(int i = 0; i<strlen(name); i++)
        name[i]=tolower(name[i]);
    for(int i = 0; i<strlen(surname); i++)
        surname[i]=tolower(surname[i]);     // making all the characters lowercase so the user doesn't have to worry about it
    char *filename = malloc(100*sizeof(char));
    if (filename != NULL) {
        filename[0] = '\0';}
    filename = strcat(filename,name); filename = strcat(filename,"_"); filename = strcat(filename,surname); filename = strcat(filename,".csv"); // create filename
    FILE* user = fopen(filename,"r+");
    if (!user) {
        printf("You do not have an account,would you like to create a new one?\n"
               "Y/N\n");
        char inp;
        scanf("%s1",&inp);
        if (inp == 'Y' || inp == 'y') //a csv file with the users details is created in order to register his orders.
        {
            user = fopen(filename,"w+");
            user = fopen(filename,"a+");
            if(user != NULL) // verifies if the file was created and opens it in read and append mode
                printf("Account and account file were created succesfully\n");
            else {
                printf("Failed to create account and account file exiting to main page\n");
                return;
            }
        }else
            return; // exits login screen.
    }
    char inp;
    int zeropressed = 0;
    while (zeropressed == 0) {
        printf("Welcome %s %s what would you like to do?\n"
               "1.Place order.\n"
               "2.Sell products to store.\n"
               "3.View placed orders.\n"
               "0.Exit to home screen\n",name,surname);
        scanf(" %s1",&inp);
        switch (inp - 48) {
            case 1:
                user = fopen(filename, "r+");
                OrderPlacement(user,catalogue);
                fclose(catalogue);
                break;
            case 2:
                user = fopen(filename, "a+");
                catalogue = fopen("catalogue.csv","r+");
                ProductSelling(user, catalogue);
                fclose(catalogue);
                break;
            case 3:
                user = fopen(filename, "r+");
                OrderView(user);
                fclose(catalogue);
                fclose(user);
                break;
            case 0:
                zeropressed =1;
                return;
            default:
                printf("Please enter a correct command\n");
                break;
        }
    }



};

void ProductSearch(FILE *catalogue)    //Allows the user to search for a product either in a price range or with an implied word,or both
{
    Printer(2);
    int UpperLimit,LowerLimit;
    char criteria[100];
    char temp[1000];
    int inp = 6 ;
    scanf("%d",&inp);
    switch (inp) {
        case 1:
            printf("Please input the key word/s you'd like to make your search for:\n");
            scanf("%s",&criteria); //storing given criteria
            catalogue = fopen("catalogue.csv","r");
            while(fgets(temp,1000,catalogue)!=NULL)
            {
                struct product prod = StringToProduct(temp);
                if (strstr(prod.name,criteria) != 0) //verifies each item if its name contains the given criteria
                {
                    printf("%d %s %d\n",prod.id,prod.name,prod.price);
                }

            }
            fclose(catalogue);
            break;
        case 2:
            printf("Please input the upper limit and the lower limit in the following format [Lower Limit] [Upper Limit]:\n");
            scanf("%d %d",&LowerLimit,&UpperLimit);
            catalogue = fopen("catalogue.csv","r");
            while(feof(catalogue)==0)
            {
                fgets(temp,1000,catalogue);
                struct product prod = StringToProduct(temp);
                if (prod.price>=LowerLimit && prod.price<=UpperLimit) //verifies each item if its data contains the given criteria
                {

                    printf("%d %s %d\n",prod.id,prod.name,prod.price);
                }

            }
            fclose(catalogue);
            break;
        case 3:
            printf("Please input the key word/s you'd like to make your search for:\n");
            scanf("%s",&criteria); //storing given criteria
            printf("Please input the upper limit and the lower limit in the following format [Lower Limit] [Upper Limit]:\n");
            scanf("%d %d",&LowerLimit,&UpperLimit);
            catalogue = fopen("catalogue.csv","r");
            while(feof(catalogue)==0)
            {
                fgets(temp,1000,catalogue);
                struct product prod = StringToProduct(temp);
                if (prod.price>=LowerLimit && prod.price<=UpperLimit && strstr(prod.name,criteria) != 0) //verifies each item if its data contains the given criteria
                {

                    printf("%d %s %d\n",prod.id,prod.name,prod.price);
                }

            }
            fclose(catalogue);

            break;
        case 0:
            printf("Returning to home screen.\n");
            break;
        default:
            printf("Please introduce a correct input:\n");
            scanf("%d", &inp);
            break;
    }

};

void ViewCatalogue(FILE *catalogue) //Allows the user to see all the products offered by the store
{
    printf("\nProducts will be shown in the following format [productid] [productname] [price/piece]\n");
    char temp[1000];
    struct product prod;
    fclose(catalogue);
    catalogue = fopen("catalogue.csv","r");
    while(feof(catalogue) == 0)
    {

        if(fgets(temp,1000,catalogue) == NULL)
        {
            printf("The catalogue is empty.");
            return;
        }
        prod = StringToProduct(temp);
        printf("%d %s %d %d \n",prod.id,prod.name,prod.price,prod.stock);

    }
    printf("\n");

};




int main()
{
    printf("Welcome to Le Mag-for all your vehicular needs,\n");
    FILE* catalogue = fopen("catalogue.csv", "r");
    if (!catalogue)
    {
        printf("The catalogue file does not exist,an empty one can be created or you can import one(it needs to be named 'catalogue.csv'\n"
               "Would you like to create one?  Y/N\n");
        char inp;
        scanf("%s1", &inp);
        if (inp == 'y' || inp == 'Y') {
            catalogue = fopen("catalogue.csv", "w");
            catalogue = fopen("catalogue.csv", "r");
            if (catalogue == NULL) {
                printf("Failed to create a file,please create one manually. The program will exit.\n");
                return 0;
            } else // I am aware I could have put the common code,but I am human as well and copy pasting it seems to work fine.
            {
                printf("The file was created successfully\n");
                fclose(catalogue);
                Printer(0);
                char *input = malloc(10 * sizeof(char)); //stores user-given input
                int zeropressed = 0; //variable used to store if the user chose to exit the store
                while (zeropressed ==
                       0) {  //while used in order to let the client do actions until he chooses to press 0 in order to close the program
                    scanf("%s", input); //read user input
                    if (strchr("01234567", input[0]) != NULL &&
                        strlen(input) == 1) //verify if the input given by the operator is correct
                    {
                        int switchinp = *input - '0';
                        switch (switchinp) {
                            case 1:
                                Authentication(catalogue);
                                fclose("catalogue.csv");
                                Printer(0);
                                break;
                            case 2:
                                ProductSearch(catalogue);
                                fclose("catalogue.csv");
                                Printer(0);
                                break;
                            case 3:
                                ViewCatalogue(catalogue);
                                fclose("catalogue.csv");
                                Printer(0);
                                break;
                            case 4:
                                Printer(4);
                                Printer(0);
                                break;
                            case 0:
                                printf("Exiting Store");
                                zeropressed = 1;
                                break;
                        }
                    } else {
                        printf("Please provide a correct input:");
                    }
                }
                free(input);
            }
        } else {
            printf("Exiting program\n");
            return 0;
        }
    }else
    {
        Printer(0);
        char *input = malloc(10 * sizeof(char)); //stores user-given input
        int zeropressed = 0; //variable used to store if the user chose to exit the store
        while (zeropressed ==
               0) {  //while used in order to let the client do actions until he chooses to press 0 in order to close the program
            scanf(" %s", input); //read user input
            if (strchr("01234567", input[0]) != NULL &&
                strlen(input) == 1) //verify if the input given by the operator is correct
            {
                int switchinp = *input - '0';
                switch (switchinp) {
                    case 1:
                        Authentication(catalogue);
                        Printer(0);
                        break;
                    case 2:
                        ProductSearch(catalogue);
                        Printer(0);
                        break;
                    case 3:
                        catalogue = fopen("catalogue.csv","r");
                        ViewCatalogue(catalogue);
                        Printer(0);
                        break;
                    case 4:
                        Printer(4);
                        Printer(0);
                        break;
                    case 0:
                        printf("Exiting Store");
                        zeropressed = 1;
                        break;
                }
            } else {
                printf("Please provide a correct input:");
            }
        }
        free(input);
    }

    return 0;

}
