
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
int chessboard[10][10];             // ����, 0 is empty; 1 is black; -1 is white
int alternative[10][10];            // ���µ���
double weight[10][10];              // Ȩ��
int colornow;                       // Ŀǰ��ɫ
int rd=0;                     		// Ŀǰ������� 
int strategy;                       // ������ʽ

/**/
//10000 empty chessboard for search
struct cbforsearch{
                                    // ���̵�һЩ�������е����������õ� 
    int cb[10][10];                 // ����
    int wst;                        // who's turn
    int bestx,besty;                // �˲���õĵ�λ
    int an[10][10]={};              // ��ѡ�µı��
    int child[30]={0};			    // ����������
    int numofchild=0;               // �����̸���
	int father=0;					// ������
	int round=0;					// ���㸴�Ӷ� 
	int alpha=-99;					// �ڼ������ֵ 
	int beta=99; 					// �׼������ֵ 
}emboard[60000];
enum direction{                     
    NN,NW,WW,SW,SS,SE,EE,NE
}dir;

// eight next blocks(NOT IMPORTANT)
int neighbors[8][2]={{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1}};

bool isInBoundary(int x,int y){                 // �ж��Ƿ����
    if(x<1||x>8||y<1||y>8)return false;
    return true;
}

int k,m_searchx,m_searchy;                      // �м������������������
extern int round_game;
bool thiswayIsAvaliable(int dir,int x,int y){   // ��x��y����dir�����ǿ��Է���ornot��
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

bool isValidMove(int x,int y){                  // ��x��y�����ӺϷ�
    if(!isInBoundary(x,y))return false;
    bool b=false;
    for(int l = 0;l < 8;l++){
        b = b||thiswayIsAvaliable(l,x,y);
    }
    return b;
}
    
void renewAlternative(){                        // ���¿����б�
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
public:                                                                        // �����Ϊ���������صĺ���
    bool thiswayIsAvaliable(int loc,int dir,int x,int y){   // ��x��y����dir�����ǿ��Է���ornot��
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
    bool isValidMove(int loc,int x,int y){                  // ��x��y�����ӺϷ�
        if(!isInBoundary(x,y))return false;
        bool b=false;
        for(int l = 0;l < 8;l++){
            b = b||thiswayIsAvaliable(loc,l,x,y);
        }
        return b;
    }
    void renewAlternative(int loc){                        // ���¿����б�
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
    bool canBeMove(int loc){                               //�ܷ����ӣ������ж��Ƿ������غ�
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
    void reverseChess(int loc,int x,int y){                 //���庯��
    	int zan = emboard[loc].wst;							//���� 
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
        emboard[loc].cb[x][y]=emboard[loc].wst;				//��ԭ
		emboard[loc].wst=zan;
        
    }
};
	
int importance[10][10]={                        // ����Ȩ�أ�ǰ������û�к���Ҫ����ǰ���õģ������ٱߣ���ռ�м䣩   
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

void renewWeight(){                             // ****����Ȩ�أ�������������ʱ��Ҫ�����������ݺ��㷨�����ĸĽ�****
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

void reverseChess(int x,int y){                 //���庯��
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

bool canBeMove(){                               //�ܷ����ӣ������ж��Ƿ������غ�
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
    // ���ļ�������ļ������ڽ��ᱻ����
    ofstream myfile("AIdrop.txt");
    // ����ļ��Ƿ�ɹ���
    if (myfile.is_open()) {
        // ��x��yд���ļ���֮���û��з��ָ�
        myfile << x << ' '<<y<< endl;
        // �ر��ļ�
        myfile.close();
        cout << "�ļ�д��ɹ���" << endl;
    } else {
        cout << "�ļ���ʧ�ܣ�" << endl;
    }
}
bool readChessboard() {
    ifstream file("chessboard.txt");
    if (!file.is_open()) {
        cerr << "�޷����ļ��� " <<"chessboard.txt"<< endl;
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
    
    // ����Ƿ��ȡ��8��
    return row == 9;
}
bool readwst(){
    ifstream file("wst.txt");
    if (!file.is_open()) {
        cerr << "�޷����ļ��� " << "wst.txt" << endl;
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

int currentTableIndex=1;									// ��Զ�ǿյ� 
void copyBoard(int loc){									
	for(int k = 1;k <=8;k++){						        // copy cb 
	    for(int l = 1;l <=8;l++){
	        emboard[currentTableIndex].cb[k][l]=emboard[loc].cb[k][l];
	    }
	}
	emboard[currentTableIndex].father=loc;
	emboard[loc].child[emboard[loc].numofchild]=currentTableIndex;  // ����loc������������
	emboard[loc].numofchild++;                                      // �����и�������
}

int judgeEnd = 0;								
int nw,nb;int zn;
int AIDrop(int loc){										
	// ����alpha-beta��֦�㷨 ,alpha�ں���ϱ�ʾ�Ժ����Ѻó̶ȣ�-beta�ں���ϱ�ʾ�԰����Ѻó̶�
	// ��emboard��loc������
    funcforsearch ffs;										// û���õ�Ϊ������class�����õı��� 
    ffs.renewAlternative(loc);                              // ��ֹ�����ڣ���֪����cb��an��wst��father��round��
                                                            // Ҫ��child��أ�alpha��beta
    if(emboard[loc].round>=ROUND&&strategy>=ROUND_three){   // ��ֵ�����̻��ܿն����Ѿ�����ROUND�֣����������ֵļ����汾
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
    }else if(emboard[loc].round >= strategy){          // ��ֵ���ѵ�ͷ��
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
        emboard[loc].alpha = nb-nw;						    // ���غ���-����������Ŀ 
        emboard[loc].beta = nb-nw;						    // ���غ���-������С����Ŀ
        return 0;
    }else if(!ffs.canBeMove(loc)){	                        // �����ƶ�
		if(judgeEnd==2){
            // �����������
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
            emboard[loc].alpha = nb-nw;						// ���غ���-����������Ŀ 
            emboard[loc].beta = nb-nw;						// ���غ���-������С����Ŀ
            return 0;
        }else{												// ���������֣���������ٴ�һ������ 
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
        // �����ƶ���Ӧ�ü�������
        judgeEnd=0;
        // �������п��ܵ�����
        for(int i = 1;i <=8;i++){
            for(int j = 1;j <=8;j++){
                if(emboard[loc].an[i][j]==1){                                       // �µ���һ�����̣�����װ���������
                    copyBoard(loc);
                    emboard[currentTableIndex].wst=(-1)*emboard[loc].wst;
					emboard[currentTableIndex].cb[i][j]=emboard[loc].wst;
                    emboard[currentTableIndex].round=emboard[loc].round+1;
                    ffs.reverseChess(currentTableIndex,i,j);
                    emboard[loc].child[emboard[loc].numofchild]=currentTableIndex;  // ����loc������������
                    currentTableIndex++;                                            // ������������
                    emboard[loc].numofchild++;                                      // �����и�������
                    if(currentTableIndex>=50000){
                    	cout<<"warning-�б����ķ���";
					}
                }
            }
        }
        //
        int mx = 0;// �м���
        int howshouldido;
        // ���ò�ͬ�㷨
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
			case 1:															// ʣ���岻�൫Ҳ���٣����á����⡱���㷨 
            if(emboard[loc].wst==1){                                         // �ú����ˣ������ѡ��alpha>0���·�
			    for(int i = 0;i < emboard[loc].numofchild;i++){				// ö����������� ,������alpha,betaֵ
				    AIDrop(emboard[loc].child[i]);
                    int zn = emboard[loc].child[i];
                    if (emboard[zn].alpha>0){                               // ��������
                        emboard[loc].alpha=emboard[zn].alpha;
                        emboard[loc].beta=emboard[zn].alpha;
                        
                        break;
                    }else{                                                  // �����������
                        emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                        emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
                    }
				}
			}else{
                for(int i = 0;i < emboard[loc].numofchild;i++){				// ö����������� ,������alpha,betaֵ
				    AIDrop(emboard[loc].child[i]);
                    int zn = emboard[loc].child[i];
                    if (emboard[zn].beta<0){                                // ��������
                        emboard[loc].alpha=emboard[zn].beta;
                        emboard[loc].beta=emboard[zn].beta;
                        break;
                    }else{                                                  // �����������
                        emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                        emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
                    }
				}
            }break;
            case 2:                                                         // ʣ������٣�7��һ�£���֧��ö��
            for(int i = 0;i < emboard[loc].numofchild;i++){	                // ������alpha-beta��֦
                AIDrop(emboard[loc].child[i]);
                int zn = emboard[loc].child[i];
                emboard[loc].alpha=max(emboard[loc].alpha,emboard[zn].alpha);
                emboard[loc].beta=min(emboard[loc].beta,emboard[zn].beta);
            }
            break;
            case 3:                                                         //���㷽ʽ��Ϊ������
            if(!ffs.canBeMove(loc)){
                emboard[loc].alpha=99*emboard[loc].wst;
                emboard[loc].beta=-99*emboard[loc].wst;
                cout<<"498bug!"<<endl;
                return 0;
            }
            for(int i = 0;i < emboard[loc].numofchild;i++){	                // ������alpha-beta��֦
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
    cout<<"  ���������Щ������Щ������Щ������Щ������Щ������Щ������Щ�������"<<endl;
    for (int i=1;i<=8;i++) {
        cout<<' '<<i<<"��";
        for(int j=1;j<=8;j++) {
            switch (emboard[loc].cb[i][j]) {
            case (0):
                cout<<"   ";
                break;
            case (-1):
                cout<<" �� ";
                break;
            case (1):
                cout<<" �� ";
                break;
            }
            cout<<"��";
        }
        if (i==8)
            cout<<endl<< "  ���������ة������ة������ة������ة������ة������ة������ة�������"<<endl;
        else
            cout<<endl<< "  ���������੤�����੤�����੤�����੤�����੤�����੤�����੤������"<<endl;
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
void AIDrop(){            // ����AI����˼·��ǰ30����֤û�л��У��м��20����ʼ�������֣��ص�Լ50�֣�50���Ҹ��ڵ�ʵ������
						//ʱ�䲻���ˣ���ʱ�����bot 
    int maxx,maxy;
    // ѡһ����Ȩ������ʼ��
    for(int i = 1;i <=8 ;i++){
        for(int j = 1;j <= 8;j++){
            if(alternative[i][j]==1){
                maxx=i;maxy=j;
            }
        }
    }
    //���¿�ѡ���λ������Ȩ��
    renewAlternative();
    renewWeight();
    //��Ȩ�����Ӳ���ת
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
	//���ص�ǰ����Ӯ����ಽ������¼����õĵ�λ
	 
} 
/**/
