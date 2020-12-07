#include <graphics.h>
#include <stdio.h>

int main() {
   int gd = DETECT,gm,i=0,j=0;
   initgraph(&gd,&gm,NULL);
   outtextxy(25,240,"Press any key to view the moving car");
   getch();

   int leftpadding=20, carwidth=150;
   int disance=450;
   int speed=5; // how fast will the car go.
   int framedelay=20; // lower framedelay == smoother and faster car

   for(i = 0 ; i <= disance ; i+=speed, j++)
   {
      rectangle(leftpadding+i,370,leftpadding+carwidth+i,400);
      rectangle(leftpadding+25+i,345,leftpadding+(carwidth*2)/3+i,370);
      line(leftpadding+i,370,leftpadding+25+i,345);
      line(leftpadding+carwidth+i,370,leftpadding+(carwidth*2)/3+i,345);
      int cx=leftpadding+25+i, cy=410;
      int sparam=(i/speed)%10; // stroke parameter
      circle(cx,cy,10); // left wheel
      line(cx+sparam, cy-10+sparam, cx-sparam, cy+10-sparam); // left wheel stroke 1
      line(cx+10-sparam, cy+sparam, cx-10+sparam, cy-sparam); // left wheel stroke 2
      cx+=(carwidth*2)/3;
      circle(cx,cy,10); // right wheel
      line(cx+sparam, cy-10+sparam, cx-sparam, cy+10-sparam); // right wheel stroke 1
      line(cx+10-sparam, cy+sparam, cx-10+sparam, cy-sparam); // right wheel stroke 2
      line(0,420,650,420);
      delay(framedelay);

      if(i+speed >= disance)
         break;
   
      cleardevice(); // clear screen
   }

   getch();
   closegraph();
   return 0;
}

