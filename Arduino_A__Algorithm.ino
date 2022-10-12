#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>


//Parameters for MCUFriend (Calibrates touchscreen display)
int XP = 9, XM = A3, YP = A2, YM = 8;
int TS_LEFT=904, TS_RT=145, TS_TOP= 957, TS_BOT = 74;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 200);
TSPoint tp;
#define MINPRESSURE 0
#define MAXPRESSURE 1000
//Size of each square
int16_t BOXSIZE; 

//The following parameters are the xy coordinates of the 
//starting node (green) and the ending node (red)
uint16_t greenX = 9999;
uint16_t greenY = 9999;

uint16_t redX = 9999;
uint16_t redY = 9999;

uint16_t color;

//Boolean to check if random maze box is selected
bool generateRandomMaze = false;

struct Node {
  int x, y, parentX, parentY;
  double hValue ;
  int gValue;
  double fValue;
};
int mode = 0;
int randomNum;

int randomGreenX = random(0,6);
int randomGreenY = random(0,9);
int randomRedX = random(0,6);
int randomRedY = random(0,9);







uint16_t ID, oldcolor, currentcolor;

   static int a[10][8] = {
                 {0,0,0,0,0,0,0,0}, //first row is out of bounds
                 {0, 1,1,1,1,1,1,0},
                 {0, 1,1,1,1,1,1,0},
                 {0, 1,1,1,1,1,1,0},
                 {0, 1,1,1,1,1,1,0},
                 {0, 1,1,1,1,1,1,0},
                 {0, 1,1,1,1,1,1,0},
                 {0,1,1,1,1,1,1,0},
                 {0,1,1,1,1,1,1,0},
                 {0,0,0,0,0,0,0,0}};
#define BLACK 0x0000
#define WHITE   0xFFFF
#define PURPLE 0xF81F
#define GREEN 0x07E0
#define RED 0xf800
#define YELLOW  0xFFE0
static Node cell[10][8] ;
Node temp;


void push(Node list[], Node node, int size) {
 int n = 0;
  for(int i = 0; i<80; i++) {
  if(list[n].x == 9999 && list[n].y == 9999) {
    list[n] = node;
    break;
  }
  n++;
  
  }
}
Node pop(Node list[]) {
  int minIndex = 0;
  double min = list[0].fValue;
  for (int n = 0; n<80; n++) {
    if (list[n].fValue<min && list[n].hValue != -1) {
      min = list[n].fValue;
      minIndex = n;
      
    }
    
  }
  
  Node result = list[minIndex];
 /* Serial.println(result.x);
  Serial.println("+");
  Serial.println(result.y); */
  //Serial.println(result.hValue);
  list[minIndex] = temp;
  return result;
}
bool IsEmpty(Node list[80]) {
 for (int i = 0; i<80; i++) {
  if (list[i].hValue != 9999) {
    return false;
  }
 }
 return true;
}

bool Contains(Node list[80], Node node) {
  for (int i =0; i<80; i++) {
   if (list[i].x == node.x && list[i].y == node.y) {
    return true; 
   }
  }
  return false;
}


void setup() {
  temp.x = 9999;
  temp.y = 9999;
  temp.fValue = 9999;

 

  tft.reset();
  ID = tft.readID();
  tft.begin(ID);
  
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  Serial.begin(9600);
     

  BOXSIZE = tft.width() / 6; //BOXSIZE is 53 for 3.5 tft screen
  BOXSIZE = tft.width() / 6;
  
  tft.fillScreen(PURPLE);
  tft.println("Welcome to A* Pathfinding Algorithm");
  tft.println("");
  tft.println("Press");
  tft.println("Anywhere to");
  tft.println("Begin");
  
  //defs for boxes
  tft.fillRect(0,tft.height()/2,BOXSIZE/2,BOXSIZE/2,WHITE);
  tft.setTextSize(2);
  tft.setCursor(0,tft.height()/2+5);
  tft.println("   Wall (Not Passable)");
  
  tft.fillRect(0,tft.height()/2+30,BOXSIZE/2,BOXSIZE/2,BLACK);
  tft.setCursor(0,tft.height()/2+35);
  tft.println("   Floor (Passable)");


  tft.fillRect(0,tft.height()/2+60,BOXSIZE/2,BOXSIZE/2,GREEN);
  tft.setCursor(0,tft.height()/2+65);
  tft.println("   Starting Point");

  tft.fillRect(0,tft.height()/2+90,BOXSIZE/2,BOXSIZE/2,RED);
  tft.setCursor(0,tft.height()/2+95);
  tft.println("   Ending Point");
  
  tft.fillRect(0,tft.height()/2+150,BOXSIZE*6,BOXSIZE*2,WHITE);
  tft.fillRect(0,tft.height()/2+150,BOXSIZE*6,3,BLACK);
  tft.setCursor(0,tft.height()/2+160);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);
  tft.println(" GENERATE RANDOM");
  tft.setCursor(0,tft.height()/2+200);
  tft.println("      MAZE");
  while(tp.z == 0) { //Press anywhere to begin
     tp = ts.getPoint(); 
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
      randomNum = random(0,4);
      randomGreenX = random(1,6);
      randomGreenY = random(1,9);

      randomRedX = random(1,6);
      randomRedY = random(1,9);
  } 

  if(tp.y > BOXSIZE*15) {
    generateRandomMaze = true;
  }
  
  tft.fillScreen(WHITE);
  
  tft.fillRect(0,0,BOXSIZE,BOXSIZE,BLACK); //Color Boxes on top of screen
  tft.fillRect(BOXSIZE*2,0,BOXSIZE,BOXSIZE,GREEN);
  tft.fillRect(BOXSIZE*3,0,BOXSIZE,BOXSIZE,RED);
  
  tft.fillRect(0,BOXSIZE-3,tft.width()-2,3,PURPLE);
   
  tft.fillRect(BOXSIZE*4,0,BOXSIZE*2,BOXSIZE,YELLOW); //START Rectangle
  tft.setCursor(BOXSIZE*4.2,BOXSIZE/3);
  tft.setTextSize(3);
  tft.setTextColor(BLACK);
  
  tft.println("START");

 
  if (generateRandomMaze == true) {
    GenerateMaze(randomNum);
  }
  
 
  

}
void GenerateMaze(int randomNum) {
  for(int i = 1; i<10; i++) {
    for (int j = 0; j<8; j++) {
      //int randomNum = random(0,4);
      if(randomNum == 0) {
      a[i][j] = 0;
      tft.fillRect(BOXSIZE*(j-1), BOXSIZE*i, BOXSIZE, BOXSIZE, BLACK);
      
      }
      randomNum = random(0,4);
    }
  }
  check();
  a[randomGreenY][randomGreenX] = 2;
  tft.fillRect(BOXSIZE*(randomGreenX-1), BOXSIZE*randomGreenY, BOXSIZE, BOXSIZE, GREEN);
  a[randomRedY][randomRedX] = 3;
  tft.fillRect(BOXSIZE*(randomRedX-1), BOXSIZE*randomRedY, BOXSIZE, BOXSIZE, RED);

  redX = randomRedX-1;
  redY = randomRedY;
  greenX = randomGreenX-1;
  greenY = randomGreenY;

}
void check() { 
  //Makes sure green and red square don't randomly 
  //spawn in the same location
  if (randomGreenX == randomRedX && randomGreenY == randomRedY) {
    randomRedX = random(1,6);
    randomRedY = random(1,9);
    check(); 
  }
}
void loop() {
  //int i = 0;
  uint16_t xpos, ypos;
  tp = ts.getPoint();
  
 // a point object holds x y and z coordinates
 
   pinMode(XM, OUTPUT);
   pinMode(YP, OUTPUT);
  
  xpos = map(tp.x, TS_RT, TS_LEFT, 0, tft.width());
  ypos = map(tp.y, TS_BOT, TS_TOP, 0, tft.height());
   for (int i = 0; i<10; i++) {
      for (int j = 0; j<8; j++) {
        if (i == 0 || j == 0 || i ==9 || j ==7) {
          a[i][j] = 0;
          
          
        }
      }
    }
  tft.fillRect(53*6, 0, 550, 550, BLACK);
  tft.fillRect(0, 53*9, 550, 550, BLACK);
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
 if (tp.z > MINPRESSURE) {
  if(ypos>BOXSIZE) {

   tft.fillRect((xpos/BOXSIZE)*BOXSIZE, (ypos/BOXSIZE)*BOXSIZE, BOXSIZE, BOXSIZE, color); 
   a[((ypos/BOXSIZE)*BOXSIZE)/53][1+((xpos/BOXSIZE)*BOXSIZE)/53] = mode;
   if (color == GREEN && (((ypos/BOXSIZE)*BOXSIZE)/53 != greenY || ((xpos/BOXSIZE)*BOXSIZE)/53 != greenX)) {
    a[greenY][greenX+1] = 1;
    tft.fillRect(53*greenX, 53*greenY, BOXSIZE, BOXSIZE, WHITE);
    
    greenX = ((xpos/BOXSIZE)*BOXSIZE)/53;
    greenY = ((ypos/BOXSIZE)*BOXSIZE)/53;
   }

   if (color == RED && (((ypos/BOXSIZE)*BOXSIZE)/53 != redY || ((xpos/BOXSIZE)*BOXSIZE)/53 != redX)) {
    a[redY][redX+1] = 1;
    tft.fillRect(53*redX, 53*redY, BOXSIZE, BOXSIZE, WHITE);
    
    redX = ((xpos/BOXSIZE)*BOXSIZE)/53;
    redY = ((ypos/BOXSIZE)*BOXSIZE)/53;
   }
   
   PrintArray(a);
   
   
  }
   else {
    if (xpos<BOXSIZE) {
     color = BLACK ;
     mode = 0; 
    }
    else if (xpos<2*BOXSIZE) {
     color = WHITE;
     mode = 1;
    }
    else if (xpos<3*BOXSIZE) {
     color = GREEN;
     mode = 2;
     
    }
    else if (xpos<4*BOXSIZE) {
     color = RED;
     mode = 3;
     
    }
    else {
      PrepAlgorithm(a);
    }
   }
    
  }
  
 
 
 }

 void PrepAlgorithm(int a[10][8]) {
  
  Serial.println("Starting algorithm");
  //delay(1000);
  int startingX = 9999;
  int startingY = 9999;
  int endingX = 9999;
  int endingY = 9999;
  
  
  for(int i=0;i<10;i++){
    for(int j=0;j<8;j++) {
       if (a[i][j] == 2) {
        startingX = j;
        startingY = i;
    
       }
       if (a[i][j] == 3) {
        endingX = j;
        endingY = i;
   
       }
       cell[i][j].x = j;
       cell[i][j].y = i;
       
      
       
    }
    
 }
  
 if (startingX != 9999 && endingX != 9999) {
  
  GenerateHValues(endingX,endingY);
  AStar(a,startingX,startingY,endingX,endingY);
 }
 else {
  
 }


 }

 void GenerateHValues(int endingX, int endingY) { //I don't think you need startingX/startingY
  //Based off of Manhattan Distance
  
  for(int i = 0; i<10; i++) { 
    for(int j = 0; j<8; j++) {
      if (a[i][j] == 0) {
        cell[i][j].hValue = -1; //Wall
      }
      else {
        cell[i][j].hValue = abs(i-endingY) +abs(j-endingX);
      }
      cell[i][j].parentX = 0;
      cell[i][j].parentY = 0;
    
  }
  }
  
 }
void AStar(int a[10][8], int startingX, int startingY, int endingX, int endingY) {
  int trials = 0;
  int pathListSize = 0;
  int closedListSize = 0;
  int openListSize = 0;
  Node pathList[80];
  Node closedList[80];
  Node openList[80];
  for (int i = 0; i<80; i++) {
    openList[i] = temp;
    closedList[i] = temp;
  }
  
    
  
 
  push(openList,cell[startingY][startingX], openListSize);
  openListSize++;
  
  while (true) {
    trials++;
    if(trials >300) {
      Serial.println("No path found");
      tft.fillRect(BOXSIZE*4,0,BOXSIZE*2,BOXSIZE,RED); //START Rectangle
      tft.setCursor(BOXSIZE*4+15,BOXSIZE/8);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
  
      tft.println("NO PATH");
      tft.setCursor(BOXSIZE*4+25,BOXSIZE/2);
      tft.println("FOUND");
      delay(2000);

      tft.fillRect(BOXSIZE*4,0,BOXSIZE*2,BOXSIZE,YELLOW); //START Rectangle
      tft.setCursor(BOXSIZE*4.2,BOXSIZE/3);
      tft.setTextSize(3);
      tft.setTextColor(BLACK);
  
      tft.println("START");
      
      break;
    }
   
    if (IsEmpty(openList)) {
      Serial.println("empty");
      break;
    }
 
    Node node = pop(openList);
    openListSize--;
    if (node.x == cell[endingY][endingX].x && node.y == cell[endingY][endingX].y) {

      Serial.println("path found");
      tft.fillRect(BOXSIZE*4,0,BOXSIZE*2,BOXSIZE,GREEN); //START Rectangle
      tft.setCursor(BOXSIZE*4+30,BOXSIZE/6);
      tft.setTextSize(2);
      tft.setTextColor(BLACK);
  
      tft.println("PATH");
      tft.setCursor(BOXSIZE*4+25,BOXSIZE/2);
      tft.println("FOUND");
     

    
      
      push(closedList, node, closedListSize);
      closedListSize++;
      Serial.println("found it");
      PinkSquares(closedList, closedListSize, endingX, endingY);
      tft.fillRect((startingX-1)*BOXSIZE, (startingY)*BOXSIZE, BOXSIZE, BOXSIZE, GREEN);
      tft.fillRect((endingX-1)*BOXSIZE, (endingY)*BOXSIZE, BOXSIZE, BOXSIZE, RED);
      tft.fillRect(BOXSIZE*4,0,BOXSIZE*2,BOXSIZE,YELLOW); //START Rectangle
      tft.setCursor(BOXSIZE*4.2,BOXSIZE/3);
      tft.setTextSize(3);
      tft.setTextColor(BLACK);
  
      tft.println("START");
      break;
    }
    if(!Contains(closedList,node)){
  push(closedList, node, closedListSize);
  closedListSize++;
  }




  //left
  
  if(node.x-1>=-1) {
    
    if (cell[node.y][node.x-1].hValue != -1 && !Contains(openList, cell[node.y][node.x-1]) && !Contains(closedList, cell[node.y][node.x-1])) {
     double tCost = node.fValue +1;
     cell[node.y][node.x-1].gValue= 1;
     double cost = cell[node.y][node.x-1].hValue+tCost;
     if(cell[node.y][node.x-1].fValue > cost || !Contains(openList, cell[node.y][node.x-1])) {
      cell[node.y][node.x-1].fValue = cost;
     
     } 
     if (!Contains(openList, cell[node.y][node.x-1])){
     cell[node.y][node.x-1].parentX = node.x;
     cell[node.y][node.x-1].parentY = node.y;
     push(openList, cell[node.y][node.x-1], openListSize);
     openListSize++;
     
     }
     
    }
    
     
  }
  //right
  if(node.x<6) {
    
    if (cell[node.y][node.x+1].hValue != -1 && !Contains(openList, cell[node.y][node.x+1]) && !Contains(closedList, cell[node.y][node.x+1])) {
     double tCost = node.fValue +1;
     cell[node.y][node.x+1].gValue= 1;
     double cost = cell[node.y][node.x+1].hValue+tCost;
     if(cell[node.y][node.x+1].fValue > cost || !Contains(openList, cell[node.y][node.x+1])) {
      cell[node.y][node.x+1].fValue = cost;
      //Serial.println("right");
     } 
     if(!Contains(openList, cell[node.y][node.x+1])) {
     cell[node.y][node.x+1].parentX = node.x;
     cell[node.y][node.x+1].parentY = node.y;
     push(openList, cell[node.y][node.x+1], openListSize);
     openListSize++;
    
     }
    
     
    }
    
  } 

  //down
  if(node.y<10) {
    
    if (cell[node.y+1][node.x].hValue != -1 && !Contains(openList, cell[node.y+1][node.x]) && !Contains(closedList, cell[node.y+1][node.x])) {
     double tCost = node.fValue +1;
     cell[node.y+1][node.x].gValue= 1;
     double cost = cell[node.y+1][node.x].hValue+tCost;
     if(cell[node.y+1][node.x].fValue > cost || !Contains(openList, cell[node.y+1][node.x])) {
      cell[node.y+1][node.x].fValue = cost;
      //Serial.println("up");
     } 
     if(!Contains(openList, cell[node.y+1][node.x])){
     cell[node.y+1][node.x].parentX = node.x;
     cell[node.y+1][node.x].parentY = node.y;
     push(openList, cell[node.y+1][node.x], openListSize);
     openListSize++;
   
     }
 
     //Serial.println(cell[node.y+1][node.x].parentX);
     
    }
     
    
  } 
  //up
  if(node.y>-2) {
    
    if (cell[node.y-1][node.x].hValue != -1 && !Contains(openList, cell[node.y-1][node.x]) && !Contains(closedList, cell[node.y-1][node.x])) {
     double tCost = node.fValue +1;
     cell[node.y-1][node.x].gValue = 1;
     double cost = cell[node.y-1][node.x].hValue+tCost;
     if(cell[node.y-1][node.x].fValue > cost || !Contains(openList, cell[node.y-1][node.x])) {
      cell[node.y-1][node.x].fValue = cost;
     
     } 
     if(!Contains(openList, cell[node.y-1][node.x])){
     cell[node.y-1][node.x].parentX = node.x;
     cell[node.y-1][node.x].parentY = node.y;
     push(openList, cell[node.y-1][node.x], openListSize);
     openListSize++;

     }
     
    }
   
  
  } 
  

  }
  
  
  
  
 
  
  


  
 }

void PinkSquares(Node list[80], int size, int endingX, int endingY) {
  
 for(int i = 0; i<size; i++ ){
  /*Serial.println(list[i].x);
  Serial.println("+");
  Serial.println(list[i].y);*/
  tft.fillRect((list[i].x-1)*BOXSIZE, (list[i].y)*BOXSIZE, BOXSIZE, BOXSIZE, PURPLE);
  delay(100);
 }
 delay(1000);
 tft.fillRect((endingX-1)*BOXSIZE, (endingY)*BOXSIZE, BOXSIZE, BOXSIZE, GREEN);
 GreenSquares(list[size-1], list);
 delay(1000);
 for(int i = 0; i<size; i++ ){
  /*Serial.println(list[i].x);
  Serial.println("+");
  Serial.println(list[i].y); */
  tft.fillRect((list[i].x-1)*BOXSIZE, (list[i].y)*BOXSIZE, BOXSIZE, BOXSIZE, WHITE);
  delay(25);
 }
}

void GreenSquares(Node node, Node list[80]) {
  Serial.println(node.parentX);
  Serial.println(node.parentY);
  for (int i = 0 ; i<80; i++) {
    if (list[i].x == node.parentX && list[i].y == node.parentY) {
    delay(100);
    tft.fillRect((node.parentX-1)*BOXSIZE, (node.parentY)*BOXSIZE, BOXSIZE, BOXSIZE, GREEN); 
    Serial.println("FoundParent");
    GreenSquares(list[i],list);
    
    
      
    }
  } 
  
    
    
    
  }

Node FindParents(Node node, Node closedList[80]) {
  
  return temp;
}
 void PrintArray(int a[10][8]) {
  for(int ii=0;ii<10;ii++){
    for(int jj=0;jj<8;jj++) {
       // a[ii][jj] = 0;
        Serial.print(a[ii][jj]);
        //Serial.println("twst");
    }
    Serial.println("");
      
}
Serial.println(""); 
 }
 //Serial.println(tp.z);  
 
   //Serial.println("not"+tp.z);

  //delay(100);
  
