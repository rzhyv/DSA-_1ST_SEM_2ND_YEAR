#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 5

typedef struct list{
  int data[MAX];
  int last;
}LIST;

LIST l;

void makenull();
void insert(int x);
void delete(int x);
void display();
int locate(int x);
int isFull();
int isEmpty();
int menu();

int main(){
  int x;
  makenull();
  while(1){
    switch(menu()){
      case 1:
        printf("Enter Data: ");
        scanf("%d", &x);

        insert(x);
        break;

      case 2:
        printf("Enter data to delete: ");
        scanf("%d", &x);

        delete(x);
        break;

      case 3:
        display();
        break;

      case 4:
        exit(0);
        break;

      default:
        printf("Invalid Input.");
    }
  }
  return 0;
}


void makenull(){
  l.last = -1;
}

void insert(int x){
  if(isFull()){
    printf("Record is full.\n");
  }
  else{
    l.last++;
    l.data[l.last] = x;
  }
}

void delete(int x){
  if(isEmpty()){
    printf("Record is empty.\n");
  }
  else{
    int i, p;
    p = locate(x);

    if(p < 0){
      printf("Record not found");
    }
    else{
      int i;
      for(i = p; i < l.last; i++){
        l.data[i] = l.data[i+1];
      }
      l.last--;
    }
  }
}

void display(){
  system("cls");
  int i;
  for(i = 0; i < 10; i++) printf("=");
  printf("\n%3s %5s\n", "NO.", "DATA");
  for(i = 0; i < 10; i++) printf("=");

  for(i = 0; i <= l.last; i++){
    printf("\n%2d %4d", i+1, l.data[i]);
  }
  printf("\n");
  system("pause");
}

int locate(int x){
  int i;
  for(i = 0; i <= l.last; i++){
    if(l.data[i] == x){
      return i;
    }
  }
  return (-1);
}

int isFull(){
  return (l.last == MAX-1);
}

int isEmpty(){
  return (l.last == -1);
}

int menu(){
  int i, ch;
  system("cls");

  for(i = 0; i < 25; i++) printf("=");
  printf("\n\t  MENU\n");
  for(i = 0; i < 25; i++) printf("=");
  printf("\n[1] ADD RECORD\n");
  printf("[2] DELETE RECORD\n");
  printf("[3] DISPLAY ALL RECORD\n");
  printf("[4] EXIT\n");
  printf("Select(1-4): ");
  scanf("%d", &ch);

  return (ch);
}