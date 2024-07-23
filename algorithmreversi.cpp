
#include <cstring>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
using namespace std;


const int ROUND=4;
const int ROUND_TWO = 7;
const int ROUND_three=8;
/**/
int chessboard[10][10];             // 棋盘, 0 is empty; 1 is black; -1 is white
int alternative[10][10];            // 可下的子
double weight[10][10];              // 权重
int colornow;                       // 目前颜色
int rd=0;                     		// 目前搜索深度 
int strategy;                       // 搜索方式

/**/
//10000 empty chessboard for search
struct cbforsearch{
                                    // 棋盘的一些参数，有的是搜索会用到 
    int cb[10][10];                 // 棋盘
    int wst;                        // who's turn
    int bestx,besty;                // 此步最好的点位
    int an[10][10]={};              // 可选下的表格
    int child[30]={0};			    // 子棋盘序列
    int numofchild=0;               // 子棋盘个数
	int father=0;					// 父棋盘
	int round=0;					// 计算复杂度 
	int alpha=-99;					// 黑减白最大值 
	int beta=99; 					// 白减黑最大值 
}emboard[60000];
enum direction{                     
    NN,NW,WW,SW,SS,SE,EE,NE
}dir;

// eight next blocks(NOT IMPORTANT)
int neighbors[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};

bool isInBoundary(int x,int y){                 // 判断是否出界
    if(x<1||x>8||y<1||y>8)return false;
    return true;
}

int k,m_searchx,m_searchy;                      // 中间变量，后续可以再用
extern int round_game;
bool thiswayIsAvaliable(int dir,int x,int y){   // （x，y）往dir方向是可以反棋ornot的
    if(chessboard[x][y]!=0)return false;
    if(chessboard[x+neighbors[dir][0]][y+neighbors[dir][1]]==-colornow){
        k=2;
        while(true){
            m_searchx=x+k*neighbors[dir][0];
            m_searchy=y+k*neighbors[dir][1];
            if(isInBoundary(m_searchx,m_searchy)==false)break;
            if(chessboard[m_searchx][m_searchy]==0){break;}
            else if(chessboard[m_searchx][m_searchy]==colornow){
                return true;
            }else{
                k++;
                continue;
            }
            
        }        
    }
    return false;
}

bool isValidMove(int x,int y){                  // （x，y）落子合法
    if(!isInBoundary(x,y))return false;
    bool b=false;
    for(int l = 0;l < 8;l++){
        b = b||thiswayIsAvaliable(l,x,y);
    }
    return b;
}
    
void renewAlternative(){                        // 更新可下列表
    for(int i = 1;i <= 8;i++){
        for(int j = 1;j <=8;j++){
            
            if(isValidMove(i,j)==true){
                alternative[i][j]=1;
            }else{
                alternative[i][j]=0;
            }
        }
    }
}

class funcforsearch{
public:                                                                        // 这个是为了搜索重载的函数
    bool thiswayIsAvaliable(int loc,int dir,int x,int y){   // （x，y）往dir方向是可以反棋ornot的
        if(emboard[loc].cb[x][y]!=0)return false;
        if(emboard[loc].cb[x+neighbors[dir][0]][y+neighbors[dir][1]]==-emboard[loc].wst){
            k=2;
            while(true){
                m_searchx=x+k*neighbors[dir][0];
                m_searchy=y+k*neighbors[dir][1];
                if(isInBoundary(m_searchx,m_searchy)==false)break;
                if(emboard[loc].cb[m_searchx][m_searchy]==0){break;}
                else if(emboard[loc].cb[m_searchx][m_searchy]==emboard[loc].wst){
                    return true;
                }else{
                    k++;
                    continue;
                }
                
            }        
        }
        return false;
    }
    bool isValidMove(int loc,int x,int y){                  // （x，y）落子合法
        if(!isInBoundary(x,y))return false;
        bool b=false;
        for(int l = 0;l < 8;l++){
            b = b||thiswayIsAvaliable(loc,l,x,y);
        }
        return b;
    }
    void renewAlternative(int loc){                        // 更新可下列表
        for(int i = 1;i <= 8;i++){
            for(int j = 1;j <=8;j++){
                if(isValidMove(loc,i,j)){
                    emboard[loc].an[i][j]=1;
                }else{
                    emboard[loc].an[i][j]=0;
                }
            }
        }
    }
    bool canBeMove(int loc){                               //能否落子，用来判断是否跳过回合
 	   renewAlternative(loc);
 	   for(int i = 1;i <=8 ;i++){
 	       for(int j = 1;j <= 8;j++){
 	           if(emboard[loc].an[i][j]==1){
 	               return true;
 	           }
 	       }
 	   }
 	   return false;
	}
    void reverseChess(int loc,int x,int y){                 //翻棋函数
    	int zan = emboard[loc].wst;							//氧化 
    	emboard[loc].wst=emboard[loc].cb[x][y];
        emboard[loc].cb[x][y]=0;
        for(int dir = 0;dir <8;dir++){
            if(thiswayIsAvaliable(loc,dir,x,y)){
                k = 1;
                while(true){
                    m_searchx=x+k*neighbors[dir][0];
                    m_searchy=y+k*neighbors[dir][1];
                    if(emboard[loc].cb[m_searchx][m_searchy]!=emboard[loc].wst){
                        emboard[loc].cb[m_searchx][m_searchy]*=-1;
                        k++;
                    }else{
                        break;
                    }
                }
            }
        }
        emboard[loc].cb[x][y]=emboard[loc].wst;				//还原
		emboard[loc].wst=zan;
        
    }
};
	
int importance[10][10]={                        // 基本权重（前期棋形没有很重要，给前期用的，尽量少边，多占中间）   
    {-10000,-10000,-10000,-10000,-10000,-10000,-10000,-10000,-10000,-10000},
    {-10000,200,-10,0,0,0,0,-10,200,-10000},
    {-10000,-10,-10,1,1,1,1,-10,-10,-10000},
    {-10000,0,1,2,10000,10000,2,1,0,-10000},
    {-10000,0,1,10000,3,3,10000,1,0,-1},
    {-10000,0,1,10000,3,3,10000,1,0,-1},
    {-10000,0,1,2,10000,10000,2,1,0,-10000},
    {-10000,-10,-10,2,1,1,2,-10,-10,-10000},
    {-10000,200,-10,0,0,0,0,-10,200,-10000},
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
};

void renewWeight(){                             // ****计算权重，这个后期如果有时间要往里面填数据和算法带来的改进****
    for(int i = 1;i <=8 ;i++){
        for(int j = 1;j <= 8;j++){
            weight[i][j]=-0.1;
            if(alternative[i][j]==1){
                weight[i][j]+=0.1*importance[i][j];
            }
            
        }
    }
    if(true){
        memset(emboard[0].cb,0,sizeof(emboard[0].cb));
        memset(emboard[0].an,0,sizeof(emboard[0].an));
        emboard[0].wst = colornow;
        for(int i = 0;i <=9 ;i++){
            for(int j = 0;j <=9 ;j++){
                emboard[0].cb[i][j]=chessboard[i][j];
            }
        }
        emboard[0].wst = colornow;
        //searchForLastStep(emboard[0]);
    }
    weight[emboard[0].bestx][emboard[0].besty]=10000;
}

void reverseChess(int x,int y){                 //翻棋函数
    chessboard[x][y]=0;
    for(int dir = 0;dir <8;dir++){
        if(thiswayIsAvaliable(dir,x,y)){
            k = 1;
            while(true){
                m_searchx=x+k*neighbors[dir][0];
                m_searchy=y+k*neighbors[dir][1];
                if(chessboard[m_searchx][m_searchy]!=colornow){
                    chessboard[m_searchx][m_searchy]*=-1;
                    k++;
                }else{
                    break;
                }
            }
        }
    }
    chessboard[x][y]=colornow;
}

bool canBeMove(){                               //能否落子，用来判断是否跳过回合
    renewAlternative();
    for(int i = 1;i <=8 ;i++){
        for(int j = 1;j <= 8;j++){
            if(alternative[i][j]==1){
                return true;
            }
        }
    }
    return false;
}

void writeAIDrop(int x,int y){ 
    // 打开文件，如果文件不存在将会被创建
    ofstream myfile("AIdrop.txt");
    // 检查文件是否成功打开
    if (myfile.is_open()) {
        // 将x和y写入文件，之间用换行符分隔
        myfile << x << ' '<<y<< endl;
        // 关闭文件
        myfile.close();
        cout << "文件写入成功！" << endl;
    } else {
        cout << "文件打开失败！" << endl;
    }
}
bool readChessboard() {
    ifstream file("chessboard.txt");
    if (!file.is_open()) {
        cerr << "无法打开文件： " <<"chessboard.txt"<< endl;
        return false;
    }
    string line;
    int row = 1;
    int k;
    while (getline(file, line)) {
        istringstream iss(line);
        for(int i = 1;i <= 8;i++){
        	iss>>k;
            emboard[0].cb[row][i]=k;
        }
        row++;
    }
    file.close();
    
    // 检查是否读取了8行
    return row == 9;
}
bool readwst(){
    ifstream file("wst.txt");
    if (!file.is_open()) {
        cerr << "无法打开文件： " << "wst.txt" << endl;
        return false;
    }
    string line;
    int row = 1;
    getline(file,line);
    switch(line[0]){
    	case '-':
    		emboard[0].wst=-1;
    		break;
    	case '0':
    		emboard[0].wst=0;
    		break;
    	case '1':
    		emboard[0].wst=1;
    		break;
	}
    
    file.close();
}

int currentTableIndex=1;									// 永远是空的 
void copyBoard(int loc){									
	for(int k = 1;k <=8;k++){						        // copy cb 
	    for(int l = 1;l <=8;l++){
	        emboard[currentTableIndex].cb[k][l]=emboard[loc].cb[k][l];
	    }
	}
	emboard[currentTableIndex].father=loc;
	emboard[loc].child[emboard[loc].numofchild]=currentTableIndex;  // 加入loc的子棋盘序列
	emboard[loc].numofchild++;                                      // 子数列个数增加
}

int judgeEnd = 0;								
int nw,nb;int zn;
int AIDrop(int loc){										
	// 采用alpha-beta剪枝算法 ,alpha在宏观上表示对黑棋友好程度，-beta在宏观上表示对白棋友好程度
	// 对emboard【loc】讨论
    funcforsearch ffs;										// 没卵用的为了引用class库设置的变量 
    ffs.renewAlternative(loc);                              // 截止到现在，已知的有cb，an，wst，father，round，
                                                            // 要求child相关，alpha，beta
    if(emboard[loc].round>=ROUND&&strategy>=ROUND_three){   // 估值，棋盘还很空而且已经到了ROUND轮，返回凝聚手的技术版本
        zn = 0;
        if(emboard[loc].wst==1){
            for(int j = 1;j <=8;j++){
                for(int k = 1;k <= 8;k++){
                    if(emboard[loc].an[j][k]==1){
                        zn++;
                    }
                }
            }
            emboard[loc].alpha = zn;
            emboard[loc].beta = (-1)*(strategy-emboard[loc].round-zn);
        }else{
            for(int j = 1;j <=8;j++){
                for(int k = 1;k <= 8;k++){
                    if(emboard[loc].an[j][k]==1){
                        zn++;
                    }
                }
            }
            emboard[loc].beta = -zn;
            emboard[loc].alpha = (strategy-emboard[loc].round-zn);
        }
        return 0;
    }else if(emboard[loc].round >= strategy){          // 估值，搜到头了
        int nw,nb;
        for(int i = 1;i <=8;i++){
            for(int j = 1;j <=8;j++){
                if(emboard[loc].cb[i][j]==1){
                    nb++;
                }else if(emboard[loc].cb[i][j]==-1){
                    nw++;
                }else{
                    continue;
                }
            }
        }
        emboard[loc].alpha = nb-nw;						    // 返回黑棋-白棋最大的数目 
        emboard[loc].beta = nb-nw;						    // 返回黑棋-白棋最小的数目
        return 0;
    }else if(!ffs.canBeMove(loc)){	                        // 不能移动
		if(judgeEnd==2){
            // 进入计数环节
            for(int i = 1;i <=8;i++){
                for(int j = 1;j <=8;j++){
                    if(emboard[loc].cb[i][j]==1){
                        nb++;
                    }else if(emboard[loc].cb[i][j]==-1){
                        nw++;
                    }else{
                        continue;
                    }
                }
            }
            emboard[loc].alpha = nb-nw;						// 返回黑棋-白棋最大的数目 
            emboard[loc].beta = nb-nw;						// 返回黑棋-白棋最小的数目
            return 0;
        }else{												// 控了凝聚手，保险起见再创一个棋盘 
        	judgeEnd++;
        	copyBoard(loc);
        	emboard[currentTableIndex].wst=(-1)*emboard[loc].wst;
            emboard[currentTableIndex].round=emboard[loc].round;
            currentTableIndex++;
            int s_zn = currentTableIndex-1;
            zn = 0;
            if(emboard[loc].wst==1){
                for(int j = 1;j <=8;j++){
                    for(int k = 1;k <= 8;k++){
                        if(emboard[loc].an[j][k]==1){
                            zn++;
                        }
                    }
                }
                emboard[loc].alpha = zn;
                emboard[loc].beta = (-1)*(strategy-emboard[loc].round-zn);
            }else{
                for(int j = 1;j <=8;j++){
                    for(int k = 1;k <= 8;k++){
                        if(emboard[loc].an[j][k]==1){
                            zn++;
                        }
                    }
                }
                emboard[loc].beta = -zn;
                emboard[loc].alpha = (strategy-emboard[loc].round-zn);
            }

            AIDrop(s_zn);
        }return 0;
	}else{
        // 可以移动，应该继续搜索
        judgeEnd=0;
        // 生成所有可能的子列
        for(int i = 1;i <=8;i++){
            for(int j = 1;j <=8;j++){
                if(emboard[loc].an[i][j]==1){                                       // 新调用一个棋盘，用来装这个子棋盘
                    copyBoard(loc);
                    emboard[currentTableIndex].wst=(-1)*emboard[loc].wst;
					emboard[currentTableIndex].cb[i][j]=emboard[loc].wst;
                    emboard[currentTableIndex].round=emboard[loc].round+1;
                    ffs.reverseChess(currentTableIndex,i,j);
                    emboard[loc].child[emboard[loc].numofchild]=currentTableIndex;  // 加入loc的子棋盘序列
                    currentTableIndex++;                                            // 棋盘索引增加
                    emboard[loc].numofchild++;                                      // 子数列个数增加
                    if(currentTableIndex>=50000){
                    	cout<<"warning-有爆掉的风险";
					}
                }
            }
        }
        //
        int mx = 0;// 中间量
        int howshouldido;
        // 设置不同算法
        if(strategy>=ROUND_three){
            howshouldido = 3;
        }else{
            if(strategy-emboard[loc].round<ROUND_TWO){
                howshouldido = 2;
            }else if(strategy-emboard[loc].round>=ROUND_three){
                cout<<"406 is bug!";
            }else{
                howshouldido = 1;
            }
        }
        
		switch(howshouldido){
			case 1:															// 剩的棋不多但也不少，采用“弱解”的算法 
            if(emboard[loc].wst==1){                                         // 该黑棋了，黑棋会选择alpha>0的下法
			    for(int i = 0;i < emboard[loc].numofchild;i++){				// 枚举子棋盘情况 ,并计算alpha,beta值
				    AIDrop(emboard[loc].child[i]);
                    int zn = emboard[loc].child[i];
                    if (emboard[zn].alpha>0){                               // 存在弱解
                        emboard[loc].alpha=emboard[zn].alpha;
                        emboard[loc].beta=emboard[zn].alpha;
                        
                        break;
                    }else{                                                  // 这个不是弱解
                        emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                        emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
                    }
				}
			}else{
                for(int i = 0;i < emboard[loc].numofchild;i++){				// 枚举子棋盘情况 ,并计算alpha,beta值
				    AIDrop(emboard[loc].child[i]);
                    int zn = emboard[loc].child[i];
                    if (emboard[zn].beta<0){                                // 存在弱解
                        emboard[loc].alpha=emboard[zn].beta;
                        emboard[loc].beta=emboard[zn].beta;
                        break;
                    }else{                                                  // 这个不是弱解
                        emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                        emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
                    }
				}
            }break;
            case 2:                                                         // 剩的棋很少（7个一下），支持枚举
            for(int i = 0;i < emboard[loc].numofchild;i++){	                // 正常的alpha-beta剪枝
                AIDrop(emboard[loc].child[i]);
                int zn = emboard[loc].child[i];
                emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
            }
            break;
            case 3:                                                         //计算方式变为凝聚手
            if(!ffs.canBeMove(loc)){
                emboard[loc].alpha=99*emboard[loc].wst;
                emboard[loc].beta=-99*emboard[loc].wst;
                cout<<"498bug!"<<endl;
                return 0;
            }
            for(int i = 0;i < emboard[loc].numofchild;i++){	                // 正常的alpha-beta剪枝
                AIDrop(emboard[loc].child[i]);
                int zn = emboard[loc].child[i];
                emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
            }
            break;
            default:
            cout<<"508 bug!";
            break;
		} 
        
    }
    
}
void printChessBoard(int loc){
    system("cls");
    cout<<"    1   2   3   4   5   6   7   8"<<endl;
    cout<<"  ┌───┬───┬───┬───┬───┬───┬───┬───┐"<<endl;
    for (int i=1;i<=8;i++) {
        cout<<' '<<i<<"│";
        for(int j=1;j<=8;j++) {
            switch (emboard[loc].cb[i][j]) {
            case (0):
                cout<<"   ";
                break;
            case (-1):
                cout<<" ○ ";
                break;
            case (1):
                cout<<" ● ";
                break;
            }
            cout<<"│";
        }
        if (i==8)
            cout<<endl<< "  └───┴───┴───┴───┴───┴───┴───┴───┘"<<endl;
        else
            cout<<endl<< "  ├───┼───┼───┼───┼───┼───┼───┼───┤"<<endl;
    }
}
int main(){
    // for debug
    int n;
    string line = "wst.txt" ;
    readChessboard();readwst();
    funcforsearch ffs;
    ffs.renewAlternative(0);
    int nw,nb;
    for(int i = 1;i <=8;i++){
        for(int j = 1;j <=8;j++){
            if(emboard[0].cb[i][j]==1){
                nb++;
            }else if(emboard[0].cb[i][j]==-1){
                nw++;
            }else{
                continue;
            }
        }
    }
    strategy=64-nb-nw;
    emboard[0].round = 0;
    for(int i = 1;i < 9;i++){
    	for(int j = 1;j < 9;j++){
    		if(emboard[0].an[i][j]==1){
    			emboard[0].bestx=i;
    			emboard[0].besty=j;
			}
		}
	} 
    //printChessBoard(0);
    AIDrop(0);
    for(int i = 0;i < emboard[0].numofchild;i++){
        if(emboard[0].wst==1){
            int zn = emboard[0].child[i];
            if(emboard[0].alpha==emboard[zn].alpha){
                for(int k = 1;k <= 8;k++){
                    for(int j = 1;j <= 8;j++){
                        if((emboard[0].cb[k][j]==0&&emboard[zn].cb[k][j]!=0)||(emboard[0].cb[k][j]!=0&&emboard[zn].cb[k][j]==0)){
                            emboard[0].bestx = k; 
                            emboard[0].besty = j; 
                        }
                    }
                }
            }
        }else{
            int zn = emboard[0].child[i];
            if(emboard[0].beta==emboard[zn].beta){
                for(int k = 1;k <= 8;k++){
                    for(int j = 1;j <= 8;j++){
                        if(emboard[0].cb[k][j]!=emboard[zn].cb[k][j]){
                            emboard[0].bestx = k; 
                            emboard[0].besty = j; 
                        }
                    }
                }
            }
        }
    }
    cout<<emboard[0].bestx<<emboard[0].besty;
    cin>>n;
}
/**/
/**
void AIDrop(){            // 理想AI落子思路：前30步保证没有昏招，中间从20步开始控凝聚手，控到约50手，50后找个节点实现搜索
						//时间不够了，有时间更新bot 
    int maxx,maxy;
    // 选一个加权计算起始点
    for(int i = 1;i <=8 ;i++){
        for(int j = 1;j <= 8;j++){
            if(alternative[i][j]==1){
                maxx=i;maxy=j;
            }
        }
    }
    //更新可选择点位，计算权重
    renewAlternative();
    renewWeight();
    //按权重落子并反转
    for(int i = 1;i <=8 ;i++){
        for(int j = 1;j <= 8;j++){
            if(weight[i][j]>weight[maxx][maxy]){
                maxx=i;
                maxy=j;
            }
        }
    }
    chessboard[maxx][maxy]=colornow;
    reverseChess(maxx,maxy);
}
int AIDrop(int loc){
	//返回当前人能赢得最多步数，并录入最好的点位
	 
} 
/**/
