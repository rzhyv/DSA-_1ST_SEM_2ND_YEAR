#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX 5

//typedef lets you create own data types

typedef struct Student{
  char name[MAX][50];
  int q1[MAX], q2[MAX], q3[MAX];
  int last; //pointer
}STUDENT;

STUDENT st;

void makenull();
void insert(char n[], int Q1, int Q2, int Q3);
void delete(char n[]);
void display();
int locate(char n[]);
int isFull();
int isEmpty();
int menu();


int main(){
  char name[50];
  int q1, q2, q3;
  makenull();
  while(1){
    switch(menu()){
      case 1:
        printf("Enter name: ");
        scanf("%s", name);
        
        printf("Enter quiz 1 score: ");
        scanf("%d", &q1);
        
        printf("Enter quiz 2 score: ");
        scanf("%d", &q2);
        
        printf("Enter quiz 3 score: ");
        scanf("%d", &q3);
        
        insert(name, q1, q2, q3);
        break;
        
      case 2:
        printf("Enter name: ");
        scanf("%s", name);
        
        delete(name);
        break;

      case 3:
        display();
        break;

      case 4:
        exit(0);
        break;
      
      default:
      printf("Invalid input");
    }
  }
  return 0;
}


void makenull(){
  st.last = -1;
}

void insert(char n[], int Q1, int Q2, int Q3){
  if(isFull()){
    printf("Record is full.\n");
  }
  else{
    st.last++;
    strcpy(st.name[st.last], n);
    st.q1[st.last] = Q1;
    st.q2[st.last] = Q2;
    st.q3[st.last] = Q3;
  }
}

int isFull(){
  return (st.last == MAX-1);
}

void delete(char n[]){
  if(isEmpty()){
    printf("Record is empty.\n");
  }
  else{
    int i, p;
    p = locate(n);

    if(p == -1){
      printf("Record not found.\n");
    }
    else{
      for(i = p; i < st.last; i++){
        strcpy(st.name[i], st.name[i+1]);
        st.q1[i] = st.q1[i+1];
        st.q2[i] = st.q2[i+1];
        st.q3[i] = st.q3[i+1];
      }
      st.last--;
      printf("Record successfully deleted.\n");
      system("pause");
    }
  }
}

int isEmpty(){
  return (st.last == -1);
}

int locate(char n[]){
  int i;
  for(i = 0; i <= st.last; i++){
    if(strcmp(st.name[i], n)==0){
      return i;
    }
  }
  return -1;
}

void display(){
  int i;
  float ave;
  system("cls");
  printf("%-4s %-15s %-8s %-8s %-8s %-10s %-10s\n", "NO.", "NAME", "QUIZ 1", "QUIZ 2", "QUIZ 3", "AVERAGE", "REMARKS");
  
  for(i = 0; i <= st.last; i++){
    ave = ((float)st.q1[i] + (float)st.q2[i] + (float)st.q3[i]) / 3.0;
    printf("%-4d %-15s %-8d %-8d %-8d %-10.2f %-10s\n", i+1, st.name[i], st.q1[i], st.q2[i], st.q3[i], ave, (ave >= 75) ? "PASSED":"FAILED");
  }
  system("pause");
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

  return ch;
}

