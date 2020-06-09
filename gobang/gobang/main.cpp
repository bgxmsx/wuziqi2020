#include<graphics.h>
#include<algorithm>
#include<conio.h>
#include"init.h"
#include"data.h"
#include"evaluate.h"
Point q[226];

extern int board[16][16];
extern int dir[4][2];
int count;//��ǰ����
int lastx, lasty;
bool final5[16][16];//�����������γ����������ӣ����ڽ��ʱ������ʾ
bool reverse, can;

MOUSEMSG mouse;

void startGame();
Point dfs(int depth, int maxdepth, int color, int maxpre);
int checkwin();
bool toofar(int x, int y);
void putlatest(int x, int y, int color);
void putpiece(int x, int y, int color);
void selectiffirst();


int main()
{
	startGame();
	return 0;
}

void startGame()
{
	selectiffirst();
	initboard();
	while (1)
	{
		count++;
		if (count == 1)//����������ߣ���һ����������
		{
			putlatest(8, 8, Black);
			lastx = 8; lasty = 8;
		}
		else if (count & 1)//������
		{
			Point nextPoint;
			for (int maxdepth = 2; maxdepth <= 4; maxdepth += 2)//����
			{

				nextPoint = dfs(1, maxdepth, Black, -0x7fffffff);
				if (nextPoint.score > 20000000) break;
			}
			putpiece(lastx, lasty, White);
			putlatest(nextPoint.x, nextPoint.y, Black);
			lastx = nextPoint.x; lasty = nextPoint.y;
		}
		else//����
		{
			while (1)
			{
				mouse = GetMouseMsg();
				if (mouse.mkLButton)
				{
					int x = mouse.x - 55, y = mouse.y - 55;
					if (x < 0 || x>35 * 14 || y < 0 || y>35 * 14) continue;
					int posx = (x + 17) / 35 + 1;
					int posy = (y + 17) / 35 + 1;
					if (board[posx][posy]) continue;
					if(lastx) putpiece(lastx, lasty, Black);
					putlatest(posx, posy, White);
					lastx = posx; lasty = posy;
					break;
				}
			}
		}
		int whoWin = checkwin();//�ж���Ӯ������ʾ
		if (whoWin)
		{
			settextcolor(WHITE);
			settextstyle(50, 0, _T("����"));
			RECT r = { 35 * 0, 35 * -1.5, 35 * 14, 35 * 5 };
			if (whoWin == White)
			{
				drawtext(_T("��Ӯ�ˣ�"), &r, DT_CENTER);
			}
			else drawtext(_T("�����ˡ�"), &r, DT_CENTER);
			for (int i = 1; i <= 15; i++)
				for (int j = 1; j <= 15; j++)
					if (final5[i][j])
						putlatest(i, j, board[i][j]);
			while (1)
			{
				mouse = GetMouseMsg();
				if (mouse.mkLButton) break;
			}
			break;
		}
	}
}

Point dfs(int depth,int maxdepth,int color,int maxpre)//����
{
	int cnt = 0;
	for (int i = 1; i <= 15; i++)
		for (int j = 1; j <= 15; j++)
		{
			if (board[i][j] || toofar(i, j)) continue;//��λ���Ѿ��������˲����»�����������̫Զ�˲�����
			Point p=evaluatePoint(i, j);//���㵱ǰλ�õ����ȼ�
			if (p.score < 2) continue;
			q[++cnt] = p;
		}
	std::sort(q + 1, q + cnt + 1);//�������ȶȶԵ�ǰҪ�µ�λ�ý���������������֦��Ч��
	Point maxPoint;//ʹ�����÷���ߵĵ�
	maxPoint.score = -0x7fffffff;
	for (int i = 1; i <= cnt; i++)
	{
		if (depth == 1) can = true;
		Point nowp = q[i];
		board[nowp.x][nowp.y] = color;
		Point selPoint;
		if (depth == maxdepth) selPoint = evaluateWhole(nowp.x, nowp.y);//������������������㵱ǰ����ķ���
		else selPoint = dfs(depth + 1, maxdepth, 3 - color, maxPoint.score);

		if (maxdepth == 4 && depth == 2 && selPoint.score > 20000000)
		{
			Point judgePoint = evaluateWhole(nowp.x, nowp.y);
			if (judgePoint.score < -20000000) can = false;
		}//�����Ĳ�֮�󼺷���������������ʱ�Է����Ѿ����������(��Ϊ���������������ȶԷ������������ߣ������ܻ�ʤʱֱ�Ӳ�ȡ�ܻ�ʤ�ķ�ʽ)

		board[nowp.x][nowp.y] = Empty;//����ʱ��Ҫ���µ���ɾȥ
		selPoint.score *= -1;//�ò����ܽ���С����ֵ����ͳһΪ����ֵ����
		if (depth == 1 && can == false) selPoint.score = -0x7fffffff + 1;
		if (selPoint.score > maxPoint.score)
		{
			maxPoint.score = selPoint.score;
			maxPoint.x = nowp.x; maxPoint.y = nowp.y;
		}
		if (maxPoint.score * -1 <= maxpre) break;//��-�¼�֦
	}
	return maxPoint;
}


int checkwin()//����ÿ����֮����ã�����Ƿ���һ���Ѿ���ʤ�����ѻ�ʤ�����ǵ��»�ʤ�������壬������ʤ����
{
	int winner;
	for (int i = 1; i <= 15; i++)
		for (int j = 1; j <= 15; j++)
		{
			if (board[i][j] == Empty) continue;
			for (int k = 0; k < 4; k++)
			{
				if (i + 4 * dir[k][0] > 15 || i + 4 * dir[k][0] < 1 || j + 4 * dir[k][1]>15) continue;
				int nowx = i, nowy = j;
				bool FLAG = true;
				for (int K = 1; K <= 4; K++)
					if (board[i + K * dir[k][0]][j + K * dir[k][1]] != board[i + (K - 1) * dir[k][0]][j + (K - 1) * dir[k][1]])
					{
						FLAG = false;
						break;
					}
				if (FLAG)
				{
					for (int K = 0; K <= 4; K++)
						final5[i + K * dir[k][0]][j + K * dir[k][1]] = true;
					winner = board[i][j];
					return winner;
				}
			}
		}
	return 0;
}


bool toofar(int x, int y)//�жϸ�λ���Ƿ������е�����̫Զ������ȷ����ǰ���µ�λ�õļ���
{
	for (int i = -2; i <= 2; i++)
		for (int j = -2; j <= 2; j++)
		{
			if (outboard(x + i, y + j)) continue;
			if (i == 0 && j == 0) continue;
			if (board[x + i][y + j]) return false;
		}
	return true;
}


void putlatest(int x, int y, int color)//�����µ��ǿ���(��ΧΪ��ɫ����ʾ�û�)
{
	board[x][y] = color;
	if (reverse) color = 3 - color;
	setfillcolor(color == White ? WHITE : BLACK);
	setlinecolor(YELLOW);
	fillcircle((x - 1) * 35, (y - 1) * 35, 12);
}
void putpiece(int x, int y, int color)//����һ����ΧΪ��ɫ�����ӱ�Ϊ������ɫ
{
	if (reverse) color = 3 - color;
	setfillcolor(color == White ? WHITE : BLACK);
	setlinecolor(color == White ? WHITE : BLACK);
	fillcircle((x - 1) * 35, (y - 1) * 35, 12);
}


void selectiffirst()//ѡ�����»��Ǻ���
{
	initboard();
	setfillcolor(WHITE);
	setlinecolor(BLACK);
	fillrectangle(35 * 5.5, 35 * 4, 35 * 8.5, 35 * 5.25);
	fillrectangle(35 * 5.5, 35 * 5.75, 35 * 8.5, 35 * 7);
	setbkmode(TRANSPARENT);
	settextcolor(BLACK);
	settextstyle(35, 0, _T("����"));
	RECT r1 = { 35 * 5.5, 35 * 4, 35 * 8.5, 35 * 5.5 };
	RECT r2 = { 35 * 5.5, 35 * 5.75, 35 * 8.5, 35 * 7 };
	drawtext(_T("����"), &r1, DT_CENTER);
	drawtext(_T("����"), &r2, DT_CENTER);
	while (1)
	{
		mouse = GetMouseMsg();
		if (mouse.mkLButton)
		{
			int x = mouse.x - 55, y = mouse.y - 55;
			if (x < 35 * 5.5 || x>35 * 8.5) continue;
			if (y >= 35 * 4 && y <= 35 * 5.25)
			{
				count++;
				reverse = true;
				break;
			}
			if (y >= 35 * 5.75 && y <= 35 * 7) break;
		}
	}
}