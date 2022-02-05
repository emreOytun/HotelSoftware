#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define ROOM_NUM 100
const char *PRICE_FORMAT_OUT = "CURRENT PRICE : %lf";
const char *PRICE_FORMAT_IN = "CURRENT PRICE : %lf";
// degisken adlarini duzenle.
// scanf islemlerini kontorllu hale getir.
// dosya acma kapama islemlerini kontrollu hale getir.
typedef struct{
	char fname[20];
	char lname[20];
}customerInfo;

typedef struct{
	int day;
	int month;
	int year;
}dateInfo;

typedef struct{
	int peopleNum;
	int roomState;
	int roomNum;
	double roomPrice;
	customerInfo customer;
	dateInfo checkInDate;
	dateInfo checkOutDate;
}roomInfo;

typedef enum{
	unknown=-1,customer=0,personnel=1
}accessLevels;

int isLeapYear(int year){
	if( (year%4==0 && year%100!=0) || (year%400==0) ) return 1;
	else return 0;
}

int dayDifference(dateInfo date1, dateInfo date2){	// Find how many days are there from 00/00/0000 to date1 and date2. Then find difference.
	int days1=0,days2=0;							// Formula : Days within this year + Days till this year ( year * 365 + leap year number)
	int monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	int i;
	
	// Find days till date1 : 
	days1 += date1.year * 365;
	days1 += (date1.year-1)/4 - (date1.year-1)/100 + (date1.year-1)/400; // add leap year number
	
	if(isLeapYear(date2.year)) monthDays[1] = 29;
	for(i=1;i<date1.month;i++){
		days1 += monthDays[i-1];
	}
	days1 += date1.day;
	monthDays[1] = 28;
	
	//Find days till date2 : 
	days2 += date2.year * 365;
	days2 += (date2.year-1)/4 - (date2.year-1)/100 + (date2.year-1)/400;
	
	if(isLeapYear(date2.year)) monthDays[1] = 29;
	for(i=1;i<date2.month;i++){
		days2 += monthDays[i-1];
	}
	days2 += date2.day;
	
	//Find difference between days: 
	return days2 - days1;
}

double getPrice(){
	double PRICE_PER_DAY;
	FILE *file = fopen("CurrentPrice.txt","r");
	if(file==NULL){
		return 0;
	}
	fscanf(file, PRICE_FORMAT_IN , &PRICE_PER_DAY);
	fclose(file);
	return PRICE_PER_DAY;
}

double calcPrice(roomInfo room){
	char ch;
	int dayNum = dayDifference(room.checkInDate, room.checkOutDate);
	double PRICE_PER_DAY; 
	
	if((PRICE_PER_DAY = getPrice()) == 0){
		return 0;
	}
	return PRICE_PER_DAY * room.peopleNum * dayNum;
}

void checkIn(){
	int roomNum, isFound = 0;
	char ch;
	roomInfo room;
	system("cls");
	printf("\tCHECK-IN : \n");
	printf("*****************************\n");
	
	printf("Please enter room number: ");
	scanf("%d",&roomNum);
	
	FILE *file = fopen("HotelRooms.txt","rb+");
	if(file==NULL){
		printf("Sorry...There is a problem with opening database file.\n");
		printf("Going back to main menu...\n");
		sleep(4);
		return;
	}
	while(fread(&room,sizeof(roomInfo),1,file)){
		if(room.roomNum==roomNum && room.roomState==1){
			printf("Unfortunately, the room you want to check in is booked until the date %d/%d/%d.\n",room.checkOutDate.day,room.checkOutDate.month,room.checkOutDate.year);
			printf("\nPress any key to go back to menu...\n");
			fflush(stdin);
			ch = getchar();
			fclose(file);
			return;
		}
		else if(room.roomNum==roomNum && room.roomState==0){
			isFound = 1;
			break;
		}
	}
	if(isFound){
		long int structSize = sizeof(roomInfo);
		fseek(file,(-structSize),SEEK_CUR);
	}
	else{
		room.roomNum = roomNum;
//		fseek(file,-1,SEEK_CUR); //************ Herhangi bir okuma yapmadikca cursor hareket etmez, fread son karaktere kadar okur daha sonrasini okuyamaz orda bekler, yani EOF karakterine getirmez cursor'i.
	}
	printf("Please enter customer name and surname : ");	
	scanf("%s %s",&room.customer.fname,&room.customer.lname);	
	printf("Please enter how many people will stay in the room: ");
	scanf("%d",&room.peopleNum);
	printf("Please enter check-in date(DD/MM/YY): ");
	scanf("%d/%d/%d",&room.checkInDate.day,&room.checkInDate.month,&room.checkInDate.year);
	printf("Please enter check-out date(DD/MM/YY): ");
	scanf("%d/%d/%d",&room.checkOutDate.day,&room.checkOutDate.month,&room.checkOutDate.year);
	if((room.roomPrice = calcPrice(room))==0){
		printf("Sorry...There is a problem with opening database.\nPlease check files and try again later.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
		fclose(file);
		return;
	}
	printf("Total price: %.2lf$\n",room.roomPrice);
	do{
		printf("Do you approve? (Y/N): ");		
		fflush(stdin);
		ch = getchar();
	}while(ch!='Y' && ch!='y' && ch!='N' && ch!='n');	
	if(ch=='N' || ch=='n'){
		printf("Reservation cancelletion is successful.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
		fclose(file);
		return;
	}
	room.roomState = 1;
	fwrite(&room,sizeof(room),1,file);
	if(fwrite==0){
		printf("There is a problem with updating the room. Please try again later...\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
		fclose(file);
		return;
	}
	printf("Reservation is completed successfully.\n");
	printf("\nPress any key to go back to menu...\n");
	fflush(stdin);
	ch = getchar();
	fclose(file);
}

void listBookedRooms(){
	system("cls");
	char ch;
	roomInfo room;
	FILE *file = fopen("HotelRooms.txt","r");
	if(file==NULL){
		printf("Sorry...There is a problem with opening database file. Please try again later.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
		return;
	}
	
	printf("\tBOOKED ROOMS: \n");
	printf("*****************************\n");
	while(fread(&room,sizeof(roomInfo),1,file)){
		if(room.roomState==1){
			printf("---------------------------");
			printf("Room Number: %d\n",room.roomNum);
			printf("Room State: Booked\n");
			printf("Customer name: %s %s\n",room.customer.fname,room.customer.lname);
			printf("Check-out date: %d/%d/%d\n",room.checkOutDate.day,room.checkOutDate.month,room.checkOutDate.year);
			printf("Total price: %.2lf$\n",room.roomPrice);
		}
	}
	printf("\nPress any key to go back to menu...\n");
	fflush(stdin);
	ch = getchar();
	fclose(file);
}

void checkOut(){
	system("cls");
	printf("\tCHECK-OUT : \n");
	printf("*****************************\n");
	char ch;
	FILE *file = fopen("HotelRooms.txt","rb+");
	if(file==NULL){
		printf("Sorry...There is a problem with opening hotel database. Please try again later.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
		return;
	}
	roomInfo room;
	int roomNum , isFound;
	printf("Please enter room number you want to check out: ");
	scanf("%d",&roomNum);
	
	while(fread(&room,sizeof(roomInfo),1,file)){
		if(roomNum == room.roomNum){
			isFound = 1;
			room.roomState = 0;
			room.checkOutDate.year = 0;
			room.checkOutDate.month = 0;
			room.checkOutDate.day = 0;
			long int structSize = sizeof(roomInfo);
			fseek(file,(-structSize),SEEK_CUR);
			fwrite(&room,sizeof(roomInfo),1,file);
			if(fwrite==0){
				printf("There is a problem with updating the room. Please try again later...\n");
				printf("\nPress any key to go back to menu...\n");
				fflush(stdin);
				ch = getchar();
			}
			else{
				printf("Check-out has been completed successfully.\n");
				printf("\nPress any key to go back to menu...\n");
				fflush(stdin);
				ch = getchar();
			} 
			break;
		}
	}
	if(!isFound){
		printf("There could not be found a registry for this room.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
	}
	fclose(file);
}

void checkRoomCus(){
	system("cls");
	printf("\tROOM INQUIRY : \n");
	printf("*****************************\n");
	char ch;
	roomInfo room;
	int roomNum , isFound = 0;
	do{
	printf("Please enter room number: ");
	scanf("%d",&roomNum);
	}while(!(1<=roomNum && roomNum<=ROOM_NUM));
	
	FILE *file = fopen("HotelRooms.txt","r");
	while(fread(&room,sizeof(roomInfo),1,file)){
		if(roomNum == room.roomNum){
			isFound = 1;
			if(room.roomState == 1){
				printf("Room %d has already been booked until the date %d/%d/%d\n",room.roomNum,room.checkOutDate.day,room.checkOutDate.month,room.checkOutDate.year);
				printf("\nPress any key to go back to menu...\n");
				fflush(stdin);
				ch = getchar();
			} 
			else{
				printf("This room is available.\n");
				printf("\nPress any key to go back to menu...\n");
				fflush(stdin);
				ch = getchar();
			} 
			break;
		}
	}
	if(isFound == 0){
		printf("This room is available.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
	}
	fclose(file);
}

void roomInformation(){
	system("cls");
	printf("\tROOM INFORMATION: \n");
	printf("*****************************\n");
	roomInfo room;
	int roomNum, isFound = 0;
	char ch;
	do{
		printf("Please enter room number: ");
		scanf("%d",&roomNum);
	}while(!(1<=roomNum && roomNum<=ROOM_NUM));
	
	FILE *file = fopen("HotelRooms.txt","rb+");
	while(fread(&room,sizeof(roomInfo),1,file)){
		if(room.roomNum == roomNum && room.roomState == 1){
			isFound = 1;
			printf("---------------------------");
			printf("Room Number: %d\n",roomNum);
			printf("Room state: Booked.\n");
			printf("Customer name: %s %s\n",room.customer.fname,room.customer.lname);
			printf("Check-out date: %d/%d/%d\n",room.checkOutDate.day,room.checkOutDate.month,room.checkOutDate.year);
			printf("Total price: %.2lf$\n",room.roomPrice);
			printf("\nPress any key to go back to menu...\n");
			fflush(stdin);
			ch = getchar();
			break;
		}
	}
	if(!isFound){
		printf("---------------------------");
		printf("Room Number: %d\n",roomNum);
		printf("Room State : Available.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
	}
	fclose(file);
}

void updatePrice(){
	system("cls");
	printf("\tPRICE UPDATING : \n");
	printf("*****************************\n");
	double newPrice;
	char ch;
	printf("Please enter new price : ");
	scanf("%lf",&newPrice);
	do{
		printf("New price will be updated as %.2lf$. Do you approve ? (Y-N) : ",newPrice);
		fflush(stdin);
		ch = getchar();
		if(ch=='Y' || ch=='y'){
			FILE *file = fopen("CurrentPrice.txt","r+");
			if(file==NULL){
				printf("Sorry...There is a problem with opening database.\nPlease check files and try again later.\n");
				printf("\nPress any key to go back to menu...\n");
				fflush(stdin);
				ch = getchar();
				return;
			}
			fprintf(file, PRICE_FORMAT_OUT , newPrice);
			printf("Price has been updated successfully.\n");
			printf("\nPress any key to go back to menu...\n");
			fflush(stdin);
			ch = getchar();
			fclose(file);
			return;
		}
		else if(ch=='N' || ch=='n'){
			printf("The process of updating price has been terminated.\n");
			printf("\nPress any key to go back to menu...\n");
			fflush(stdin);
			ch = getchar();
			return;
		}
		else printf("You entered a wrong character. Enter 'Y' or 'N' \n");
	}while(1);
}

void showPrice(){
	char ch;
	double PRICE_PER_DAY;
	
	system("cls");
	printf("\tPRICE INFORMATION : \n");
	printf("*****************************\n");
	if((PRICE_PER_DAY = getPrice())==0){
		printf("Sorry...There is a problem with opening database.\nPlease check files and try again later.\n");
		printf("\nPress any key to go back to menu...\n");
		fflush(stdin);
		ch = getchar();
		return;
	} 
	printf("Current price is : %.2lf$\n",PRICE_PER_DAY);
	printf("\nPress any key to go back to menu...\n");
	fflush(stdin);
	ch = getchar();
}

int main(){
	
	accessLevels accessLevel=unknown;
	int pswrd = 517864;
	char ch;
	
	do{
		do{
			system("cls");
			printf("\t\t\t~~~~~~~ WELCOME TO HOTEL BOOKING SYSTEM ~~~~~~~\n\n");
			printf("Please enter 'c' if you are customer, 'p' if you are personnel, 'q' to close program: ");
			fflush(stdin);
			scanf("%c",&ch);
			if(ch=='c' || ch=='C'){
				accessLevel = customer;
				ch = 'Y';	
			}	 
			else if(ch=='p' || ch=='P'){
				int input;
				printf("Please enter personnel password: ");
				scanf("%d",&input);
				if(pswrd==input){
					accessLevel = personnel;
					ch = 'Y';
				}
				else{
					printf("You entered a wrong password.\n");
					ch = 'N';
				}
			}
			else if(ch=='q' || ch=='Q'){
				printf("Program is closing...\n");
				sleep(1);
				accessLevel = unknown;
				ch = 'Q';
			}
			else{
				printf("You entered a wrong character. Please try again.\n");
				ch = 'N';
			}
		}while(ch!='Y' && ch!='Q');
	
		if(accessLevel==customer){
			char input;
			do{
				system("cls");
				printf("\tCUSTOMER MENU: \n");
				printf("*****************************\n");
				printf("(A) View Room Information\n");
				printf("(B) Show price information\n");
				printf("(Q) Quit menu\n");
				printf("Please enter the process you want: ");
				fflush(stdin);
				scanf("%c",&input);
				
				if(input=='A' || input=='a') checkRoomCus();
				else if(input=='B' || input=='b') showPrice();
				else if(input=='Q' || input=='q'){}
				else printf("You entered a wrong character.\n");
			}while(input!='Q' && input!='q');
		}
		if(accessLevel==personnel){
			char input;
			do{
				system("cls");
				printf("\tPERSONNEL MENU: \n");
				printf("*****************************\n");
				printf("(A) Check-In\n");
				printf("(B) Check-Out\n");
				printf("(C) View Reservations\n");
				printf("(D) View Room Information\n");
				printf("(E) Update price\n");
				printf("(Q) Quit Menu\n");
				printf("Please enter the process you want: ");
				fflush(stdin);
				scanf("%c",&input);
				
				if(input=='A' || input=='a') checkIn();
				else if(input=='B' || input=='b') checkOut();
				else if(input=='C' || input=='c') listBookedRooms();
				else if(input=='D' || input=='d') roomInformation();
				else if(input=='E' || input=='e') updatePrice();
				else if(input=='Q' || input=='q'){}
				else printf("You entered a wrong character.\n");
			}while(input!='Q' && input!='q');
		}
	}while(ch!='Q');
	printf("Program is closed.Goodbye!\n");
	
	return 0;
}
