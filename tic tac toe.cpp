#define _WIN32_WINNT 0x0500

#include <cstdlib>
#include <iostream>
#include <process.h>
#include <windows.h>
#include <ctime>
#include <cstdio>
#include <math.h>

using namespace std;

void ProceedClick (int x, int y);
int GetCell(int x, int y);
void InitializeGame();
void PrintMatrix();
void CheckIfWon();

int matrix[3][3];
int moveCounter;
bool gameWon;
int winner;

class MouseInputHandler
{
      //mouse input handler callback function pointer
      typedef void (*MIHCB)(int,int,void*);

      public:
      MouseInputHandler(){}
      ~MouseInputHandler(){}
      void init();

      inline void start_polling()
      {
             _beginthread(polling_function,0,this);
             polling=true;
      }

      inline void stop_polling()
      {
             input.quit=true;
             while (polling);
      }

      int getx();
      int gety();
      void getxy(int & x, int & y);

      //setting the callbacks
      inline void set_lbdcb(MIHCB f){lbdcb=f;}
      inline void set_lbucb(MIHCB f){lbucb=f;}
      inline void set_lbccb(MIHCB f){lbccb=f;}
      inline void set_rbdcb(MIHCB f){}
      inline void set_rbucb(MIHCB f){}
      inline void set_rbccb(MIHCB f){}

      //setting the params for the callbacks
      inline void set_lbdcbp(void*p){lbdcbp=p;}
      inline void set_lbucbp(void*p){lbucbp=p;}
      inline void set_lbccbp(void*p){lbccbp=p;}
      inline void set_rbdcbp(void*p){}
      inline void set_rbucbp(void*p){}
      inline void set_rbccbp(void*p){}

      private:
      struct MouseInput
      {
             //current and previous mouse button status
             bool cleft, pleft;
             bool cright, pright;

             bool quit;
             int x,y;

      } input;

      bool polling;

      static void polling_function(void*);
      static inline bool is_pressed(int key)
      {
             return (GetAsyncKeyState(key)>>15);
      }

      MIHCB lbdcb;
      MIHCB lbucb;
      MIHCB lbccb;
      MIHCB rbdcb;
      MIHCB rbucb;
      MIHCB rbccb;

      void* lbdcbp;
      void* lbucbp;
      void* lbccbp;
      void* rbdcbp;
      void* rbucbp;
      void* rbccbp;

};

void MouseInputHandler::polling_function(void*p)
{
     MouseInputHandler * m=(MouseInputHandler*)p;

     m->input.quit=false;

     HWND window=GetConsoleWindow();
     RECT wpos;
     POINT cpos;


     m->input.cleft=m->input.pleft=m->input.cright=m->input.pright=false;
     int x,y;
     int tl, tr,t;


     while (!m->input.quit)
     {
           GetWindowRect(window,&wpos);
           GetCursorPos(&cpos);

           cpos.x-=wpos.left;
           cpos.y-=wpos.top;

           //transform screen to console coords
           x=(cpos.x-10);
           y=(cpos.y-31);

           m->input.cleft=is_pressed(VK_LBUTTON);
           m->input.cright=is_pressed(VK_RBUTTON);

           //mouse down event: it was up and now is down
           if (m->input.cleft && !m->input.pleft)
           {
              tl=(clock()*1000)/CLOCKS_PER_SEC;
              m->lbdcb(x,y,m->lbdcbp);
           }

           //mouse up event: it was down and now is up
           if (!m->input.cleft && m->input.pleft)
           {
              t=(clock()*1000)/CLOCKS_PER_SEC;
              m->lbucb(x,y,m->lbucbp);

              //mouse click event:
              //down->up in less than 100 ms
              if (t-tl<=100)
              m->lbccb(x,y,m->lbccbp);
           }

           //...more stuff here

           m->input.pleft=m->input.cleft;
           //m->input.pright=m->input.cright;

           Sleep(25);
     }
     m->polling=false;
}

////////////////////////////////////////////////////////////////////////////////

void left_down(int x, int y, void*p){}

void left_up(int x, int y, void*p)
{
    ProceedClick(x, y);									//Game happens
}

void left_click(int x, int y, void*p){}

int main()
{
    MouseInputHandler mouse;
    mouse.set_lbdcb(left_down);
    mouse.set_lbucb(left_up);
    mouse.set_lbccb(left_click);

    mouse.start_polling();

	InitializeGame();

    while (!(GetAsyncKeyState(VK_ESCAPE)>>15));

    mouse.stop_polling();

    system("pause");
    return 0;
}

void ProceedClick (int x, int y)
{

	int position = GetCell(x,y);
	if(position == 9)//restart button
	{
		InitializeGame();
	}
	else if(position >= 0 && !gameWon)
	{
		int a = position / 3;
		int b = position % 3;
		if(matrix[a][b] == ' ')
		{
			if(moveCounter % 2)matrix[a][b] = 'X';
			else matrix[a][b] = 'O';
			moveCounter++;
			CheckIfWon();
		}
	}

	PrintMatrix();

}

int GetCell(int x, int y)
{
	if( x > 13 && x < 28 && y > 10 && y < 31) return 0;
	if( x > 28 && x < 44 && y > 10 && y < 31) return 1;
	if( x > 44 && x < 56 && y > 10 && y < 31) return 2;
	if( x > 13 && x < 28 && y > 31 && y < 52) return 3;
	if( x > 28 && x < 44 && y > 31 && y < 52) return 4;
	if( x > 44 && x < 56 && y > 31 && y < 52) return 5;
	if( x > 13 && x < 28 && y > 52 && y < 74) return 6;
	if( x > 28 && x < 44 && y > 52 && y < 74) return 7;
	if( x > 44 && x < 56 && y > 52 && y < 74) return 8;

	if( x > 136 && x < 192 && y > 11 && y < 24) return 9;

	return -1;
}

void InitializeGame()
{
	moveCounter = 1;
	gameWon = false;
	winner = 0;

	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)matrix[i][j] = ' ';

	PrintMatrix();
}
void PrintMatrix()
{
	system("cls");
	cout << endl;
    cout << "  " << (char)matrix[0][0] << "|" << (char)matrix[0][1] << "|" << (char)matrix[0][2] <<"          RESTART" << endl;
    cout << "  -+-+-" << endl;
    cout << "  " << (char)matrix[1][0] << "|" << (char)matrix[1][1] << "|" << (char)matrix[1][2] << endl;
    cout << "  -+-+-" << endl;
    cout << "  " << (char)matrix[2][0] << "|" << (char)matrix[2][1] << "|" << (char)matrix[2][2] << endl;
    if(gameWon)
	{
		if(winner == -1)cout << endl << "IT'S A DRAW!" << endl;
		else cout << endl << "PLAYER " << winner + 1 << " WINS!" << endl;
	}
}

void CheckIfWon()
{
	if( matrix[0][0] != ' ' && matrix[0][0] == matrix [0][1] && matrix [0][0] == matrix[0][2]){ gameWon = true; winner = moveCounter %2; return;}//row 1
	if( matrix[1][0] != ' ' && matrix[1][0] == matrix [1][1] && matrix [1][0] == matrix[1][2]){ gameWon = true; winner = moveCounter %2; return;}//row 2
	if( matrix[2][0] != ' ' && matrix[2][0] == matrix [2][1] && matrix [2][0] == matrix[2][2]){ gameWon = true; winner = moveCounter %2; return;}//row 3
	if( matrix[0][0] != ' ' && matrix[0][0] == matrix [1][0] && matrix [0][0] == matrix[2][0]){ gameWon = true; winner = moveCounter %2; return;}//col 1
	if( matrix[0][1] != ' ' && matrix[0][1] == matrix [1][1] && matrix [0][1] == matrix[2][1]){ gameWon = true; winner = moveCounter %2; return;}//col 2
	if( matrix[0][2] != ' ' && matrix[0][2] == matrix [1][2] && matrix [0][2] == matrix[2][2]){ gameWon = true; winner = moveCounter %2; return;}//col 3
	if( matrix[0][0] != ' ' && matrix[0][0] == matrix [1][1] && matrix [0][0] == matrix[2][2]){ gameWon = true; winner = moveCounter %2; return;}//main diagonal
	if( matrix[2][0] != ' ' && matrix[2][0] == matrix [1][1] && matrix [2][0] == matrix[0][2]){ gameWon = true; winner = moveCounter %2; return;}//revese diagonal

	if(moveCounter == 10){gameWon = true; winner = -1;}
}
