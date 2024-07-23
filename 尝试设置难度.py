import pygame
import time
import sys
import numpy as np
import copy
import subprocess
import os

#黑白棋，启动!
pygame.init()
global not_move
global pvp

#引导语1
print('欢迎来到REVERSI黑白棋，还想请教您了，您是执黑还是执白，还是打算双人对战呢？')
print('1: 执黑  2：执白  3：双人对战  4: 斗蛐蛐')
for i in range(100000):
    ans = input()
    if (ans == "1"):
        not_move = False
        pvp = False
        ququ = False
        human = 1
        break
    elif(ans == "2"):
        not_move = True
        pvp = False
        human = -1
        ququ = False
        break
    elif (ans == "3"):
        pvp = True
        not_move = False
        ququ=False
        break
    elif (ans == "4"):
        for i in range (100):
            print('斗电子蛐蛐，当赛博王爷')
            print('请设置蛐蛐水平:')
            print('1: EASY 2: DIFFICULT')
            ququ_diffi = int(input())
            if(ququ_diffi == 1 or ququ_diffi == 2):
                ququ = True
                not_move = True
                pvp = False
                ans1 = 0
                break
            else:
                print('Invalid input. Please try again!')
        break
    else:
        print('Invalid input. Please Try again!')
for i in range(10000):
    if ans == "1" or ans == "2":
        print('请选择难度:')
        print('1: EASY 2:DIFFICULT')
        ans1 = input()
        if ans1 == "1" or ans1 == "2":
            break
        else:
            print('Invalid input.Please try again!')
    else:
        pass

print('玩法说明：')
print('按下S键：设置存档')
print('按下Q键：重新开始')
print('按下R键：读取存档')
print('按下U键：悔棋')
print('按下O键：退出游戏')
print('按下H键：帮助菜单')
print('没记住没关系，任何时间按下H键即可弹出帮助菜单')
print('准备好的话就开始吧！')

time.sleep(1.5)

#设置界面
windows = pygame.display.set_mode((800, 800))
pygame.display.set_caption('Reversi Game By H.Ayanokoji and NATTIN')


#放音乐
music_path = "C:\\Users\\10744\\Desktop\\ReversiGame\\musics.mp3"
pygame.mixer.init()
pygame.mixer.music.load(music_path)
pygame.mixer.music.set_volume(0.3)
pygame.mixer.music.play(loops=-1)

#加载图片（棋盘、小黑子和小白子）
board1 = pygame.image.load(
    "C:\\Users\\10744\\Desktop\\ReversiGame\\board1.png")
white1 = pygame.image.load("C:\\Users\\10744\Desktop\\ReversiGame\\white1.png")
black1 = pygame.image.load("C:\\Users\\10744\Desktop\\ReversiGame\\black1.png")
empty = pygame.image.load("C:\\Users\\10744\Desktop\\ReversiGame\\empty.png")
kun = pygame.image.load("C:\\Users\\10744\Desktop\\ReversiGame\\kun.png")
chessboard = "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\chessboard.txt"
wst = "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\wst.txt"

#画线，放起始棋子
windows.blit(board1, (0, 0))
windows.blit(white1, (302, 302))
windows.blit(black1, (402, 302))
windows.blit(white1, (402, 402))
windows.blit(black1, (302, 402))
for i in range(8):
    pygame.draw.line(windows, (0, 0, 0), (i*100, 0), (i*100, 800), 2)
for i in range(8):
    pygame.draw.line(windows, (0, 0, 0), (0, i*100), (800, i*100), 2)
pygame.display.flip()

#建一个二维数组表示落子情况以明确翻棋逻辑
rows, cols = 8, 8
matrixs = np.zeros((rows, cols))
matrixs[3][3] = matrixs[4][4] = -1
matrixs[3][4] = matrixs[4][3] = 1

#现在的玩家是谁？
global current_player
current_player = 1

#判断落子是否合法
#(判断每一个格子能)


def reverse_ok(x, y, matrixs):
    global current_player
    if current_player == 1:
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0),
                      (1, 1), (1, -1), (-1, 1), (-1, -1)]
        for dr, dc in directions:
            r, c = x + dr, y + dc
            if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == -1:
                to_flip = []
                while 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == -1:
                    to_flip.append((r, c))
                    r, c = r + dr, c + dc
                if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == 1:
                    return True
        return False
    elif current_player == -1:
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0),
                      (1, 1), (1, -1), (-1, 1), (-1, -1)]
        for dr, dc in directions:
            r, c = x + dr, y + dc
            if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == 1:
                to_flip = []
                while 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == 1:
                    to_flip.append((r, c))
                    r, c = r + dr, c + dc
                if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == -1:
                    return True
        return False

#翻棋函数


def reverse(x, y, matrixs):
    global current_player
    if current_player == 1:
        matrixs[x][y] = 1
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0),
                      (1, 1), (1, -1), (-1, 1), (-1, -1)]
        for dr, dc in directions:
            r, c = x + dr, y + dc
            if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == -1:
                to_flip = []
                while 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == -1:
                    to_flip.append((r, c))
                    r, c = r + dr, c + dc
                if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == 1:
                    for flip_row, flip_col in to_flip:
                        matrixs[flip_row][flip_col] = 1
        current_player = -current_player
        if (change_needed(matrixs)):
            pass
        else:
            current_player = -current_player
            print('白棋无处可下，PASS!', sep='\n')
            print(' ', sep='\n')
    elif current_player == -1:
        matrixs[x][y] = -1
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0),
                      (1, 1), (1, -1), (-1, 1), (-1, -1)]
        for dr, dc in directions:
            r, c = x + dr, y + dc
            if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == 1:
                to_flip = []
                while 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == 1:
                    to_flip.append((r, c))
                    r, c = r + dr, c + dc
                if 0 <= r < 8 and 0 <= c < 8 and matrixs[r][c] == -1:
                    for flip_row, flip_col in to_flip:
                        matrixs[flip_row][flip_col] = -1
        current_player = -current_player
        if (change_needed(matrixs)):
            pass
        else:
            current_player = -current_player
            print('黑棋无处可下，PASS', sep='\n')
            print(' ', sep='\n')


#记录总手数
global moves
moves = 0

#实际应用count 表示手数更方便


def count(matrixs):
    counts = 0
    for i in range(8):
        for j in range(8):
            if (matrixs[i][j] == 1 or matrixs[i][j] == -1):
                counts += 1
    return counts

#判断是否需要交换落子


def change_needed(matrixs):
    counting = 0
    for i in range(8):
        for j in range(8):
            if (matrixs[i][j] == 0):
                if (reverse_ok(i, j, matrixs)):
                    counting += 1
    return counting

#加载坤坤，即可落子位置


def available_show(matrixs):
    for i in range(8):
        for j in range(8):
            if (matrixs[i][j] == 0 and reverse_ok(i, j, matrixs)):
                windows.blit(kun, (i*100+2, j*100+2))
            elif (matrixs[i][j] == 0 and not reverse_ok(i, j, matrixs)):
                windows.blit(empty, (i*100+2, j*100+2))

    pygame.display.flip()

#存档函数


def save_to_file(matrixs, filename):
    transposed_matrix = np.transpose(matrixs)
    with open(filename, 'w') as file:
        for row in transposed_matrix:
            # 将每个浮点数转换为整数
            int_row = [int(element) for element in row]
            file.write(' '.join(map(str, int_row)) + '\n')


#存档个数，最多为3
files = 0

#记录存档时玩家


def save_player(filename):
    with open(filename, 'w') as file:
        file.write(str(current_player) + '\n')


#读档函数
matrix1 = []
matrix2 = []


def load_from_file(filename):
    global matrix2
    with open(filename, 'r') as file:
        for line in file:
            row = [float(val) if val !=
                   '[]' else 0 for val in line.strip().split()]
            matrix1.append(row)
            matrix2 = np.transpose(matrix1)

#读取存档时玩家


def load_player(filename):
    with open(filename, 'r') as file:
        current_player = int(file.read())

#告诉c++程序棋盘状况


def tell_cpp(matrixs, filename):
    transposed_matrix = np.transpose(matrixs)
    with open(filename, 'w') as file:
        for row in transposed_matrix:
            # 将每个浮点数转换为整数
            int_row = [int(element) for element in row]
            file.write(' '.join(map(str, int_row)) + '\n')

#告诉c++程序下一步谁下


def tell_cpp2(filename):
    with open(filename, 'w') as file:
        file.write(str(current_player) + '\n')


#存放60步的棋盘状况
rows, cols = 8, 8
empty_arrays = [[[] for _ in range(cols)] for _ in range(rows)]
num_arrays = 6000
list_of_arrays = [copy.deepcopy(empty_arrays) for _ in range(num_arrays)]

moves = 0

#假ai

ffflagend = False
specialend = False


def fakeAI(matrixs, human):
    flag = False
    ffflag = True
    for i in range(8):
        for j in range(8):
            if reverse_ok(j, i, matrixs) and matrixs[j][i] == 0:
                a = j
                b = i
                flag = True
                ffflag = False
                break
        else:
            continue
        break
    global current_player
    blackcount = whitecount = unsignedcount = 0
    if not flag and unsignedcount != 0:
        print('AI无处可下，PASS！')
        current_player = -current_player
        return 0

    try:
        print('AI思考中......')
        print(' ')
        time.sleep(0.3)
        print("AI落子：", a+1, b+1)
        matrixs[a][b] = -human
        global moves
        moves += 1
        reverse(a, b, matrixs)
        list_of_arrays[moves] = copy.deepcopy(matrixs)
        for i in range(8):
            for j in range(8):
                if (matrixs[i][j] == 1):
                    windows.blit(black1, (i*100+2, j*100+2))
                    blackcount += 1
                elif (matrixs[i][j] == -1):
                    windows.blit(white1, (i*100+2, j*100+2))
                    whitecount += 1
                elif(matrixs[i][j] == 0):
                    unsignedcount += 1
        pygame.display.flip()
        ans = (count(matrixs)) - 4
        print('现在是第', ans, '手', sep='')
        if (current_player == 1):
            print('下一手黑棋落子（B）')
        elif(current_player == -1):
            print('下一手白棋落子（W）')
        print('下一手可落子点位数：', change_needed(matrixs))
        print('黑棋数：', blackcount)
        print('白棋数：', whitecount)
        print('未定区域：', unsignedcount)
        print(' ', sep='\n')
        available_show(matrixs)
        tell_cpp(matrixs, chessboard)
        tell_cpp2(wst)
        if (unsignedcount == 0):
            if(blackcount > whitecount):
                print("游戏结束，黑方获胜: B+", blackcount-whitecount)
            elif (whitecount > blackcount):
                print("游戏结束，白方获胜: W+", -blackcount+whitecount)
            else:
                print('游戏结束，平局！')
    except:
        global ffflagend
        ffflagend = True

#特判情况


def special(matrixs):
    if(current_player == -1 and not change_needed(matrixs) and moves != 60):
        current_player == 1
        global specialend
        if not change_needed(matrixs):
            print('游戏结束！')
            specialend = True
            if(blackcount > whitecount):
                print('黑棋获胜：B+', blackcount-whitecount)
            elif (whitecount > blackcount):
                print('白棋获胜：W+', whitecount-blackcount)
            else:
                print('平局')

        else:
            pass
    if(current_player == 1 and not change_needed(matrixs) and moves != 60 and moves!= 0):
        current_player == -1
        if not change_needed(matrixs):
            print('游戏结束！')
            specialend = True
            if(blackcount > whitecount):
                print('黑棋获胜：B+', blackcount-whitecount)
            elif (whitecount > blackcount):
                print('白棋获胜：W+', whitecount-blackcount)
            else:
                print('平局')
        else:
            pass


#游戏界面
magic = False
while True:
    if ffflagend:
        break

    special(matrixs)

    if specialend:
        break
    if not pvp and not ququ:
        if current_player == human:
            not_move = False
        else:
            not_move = True

    if ququ:
        if ququ_diffi == 1:
            while moves<60:
                fakeAI(matrixs,-1)
                fakeAI(matrixs,1)
        if ququ_diffi == 2:
            while moves <60:
                human = 1
                cpp_executable_path = "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\algorithmreversi.exe"
                # 使用 subprocess.run 执行程序并捕获输出
                result = subprocess.run(
                    [cpp_executable_path], stdout=subprocess.PIPE)
                try:
                    output_str = result.stdout.decode('utf-8')
                    x_loc = int(output_str[0])
                    y_loc = int(output_str[1])
                except:
                    x_loc = 1
                    y_loc = 1

                if (human == 1):  # 人类黑棋
                    matrixs[y_loc-1][x_loc-1] = -1
                    if not reverse_ok(y_loc-1, x_loc-1, matrixs):
                        current_player = - current_player
                    if reverse_ok(y_loc-1, x_loc-1, matrixs):
                            moves += 1
                            reverse(y_loc-1, x_loc-1, matrixs)
                            list_of_arrays[moves] = copy.deepcopy(matrixs)
                            blackcount = whitecount = unsignedcount = 0
                            for i in range(8):
                                for j in range(8):
                                    if (matrixs[i][j] == 1):
                                        windows.blit(black1, (i*100+2, j*100+2))
                                        blackcount += 1
                                    elif (matrixs[i][j] == -1):
                                        windows.blit(white1, (i*100+2, j*100+2))
                                        whitecount += 1
                                    elif(matrixs[i][j] == 0):
                                        unsignedcount += 1
                            pygame.display.flip()
                            ans = (count(matrixs)) - 4
                            print('现在是第', ans, '手', sep='')
                            if (current_player == 1):
                                print('下一手黑棋落子（B）')
                            elif(current_player == -1):
                                print('下一手白棋落子（W）')
                            print('下一手可落子点位数：', change_needed(matrixs))
                            print('黑棋数：', blackcount)
                            print('白棋数：', whitecount)
                            print('未定区域：', unsignedcount)
                            print(' ', sep='\n')
                            available_show(matrixs)
                            tell_cpp(matrixs, chessboard)
                            tell_cpp2(wst)
                            if (unsignedcount == 0):
                                if(blackcount > whitecount):
                                    print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                                elif (whitecount > blackcount):
                                    print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                                else:
                                    print('游戏结束，平局！')
                    else:
                            pass
                elif (human == -1):
                    matrixs[y_loc-1][x_loc-1] = 1
                    if not reverse_ok(y_loc-1, x_loc-1, matrixs):
                        current_player = - current_player
                        if current_player == human:
                            not_move = False
                        else:
                            not_move = True
                    if reverse_ok(y_loc-1, x_loc-1, matrixs):
                            moves += 1
                            reverse(y_loc-1, x_loc-1, matrixs)
                            list_of_arrays[moves] = copy.deepcopy(matrixs)
                            blackcount = whitecount = unsignedcount = 0
                            for i in range(8):
                                for j in range(8):
                                        if (matrixs[i][j] == 1):
                                            windows.blit(black1, (i*100+2, j*100+2))
                                            blackcount += 1
                                        elif (matrixs[i][j] == -1):
                                            windows.blit(white1, (i*100+2, j*100+2))
                                            whitecount += 1
                                        elif(matrixs[i][j] == 0):
                                            unsignedcount += 1
                                pygame.display.flip()
                                ans = (count(matrixs)) - 4
                                print('现在是第', ans, '手', sep='')
                                if (current_player == 1):
                                    print('下一手黑棋落子（B）')
                                elif(current_player == -1):
                                    print('下一手白棋落子（W）')
                                print('下一手可落子点位数：', change_needed(matrixs))
                                print('黑棋数：', blackcount)
                                print('白棋数：', whitecount)
                                print('未定区域：', unsignedcount)
                                print(' ', sep='\n')
                                available_show(matrixs)
                                tell_cpp(matrixs, chessboard)
                                tell_cpp2(wst)
                                if (unsignedcount == 0):
                                    if(blackcount > whitecount):
                                        print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                                    elif (whitecount > blackcount):
                                        print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                                    else:
                                        print('游戏结束，平局！')
                human = -1
                if (human == 1):  # 人类黑棋
                    matrixs[y_loc-1][x_loc-1] = -1
                    if not reverse_ok(y_loc-1, x_loc-1, matrixs):
                        current_player = - current_player
                    if reverse_ok(y_loc-1, x_loc-1, matrixs):
                            moves += 1
                            reverse(y_loc-1, x_loc-1, matrixs)
                            list_of_arrays[moves] = copy.deepcopy(matrixs)
                            blackcount = whitecount = unsignedcount = 0
                            for i in range(8):
                                for j in range(8):
                                    if (matrixs[i][j] == 1):
                                        windows.blit(black1, (i*100+2, j*100+2))
                                        blackcount += 1
                                    elif (matrixs[i][j] == -1):
                                        windows.blit(white1, (i*100+2, j*100+2))
                                        whitecount += 1
                                    elif(matrixs[i][j] == 0):
                                        unsignedcount += 1
                            pygame.display.flip()
                            ans = (count(matrixs)) - 4
                            print('现在是第', ans, '手', sep='')
                            if (current_player == 1):
                                print('下一手黑棋落子（B）')
                            elif(current_player == -1):
                                print('下一手白棋落子（W）')
                            print('下一手可落子点位数：', change_needed(matrixs))
                            print('黑棋数：', blackcount)
                            print('白棋数：', whitecount)
                            print('未定区域：', unsignedcount)
                            print(' ', sep='\n')
                            available_show(matrixs)
                            tell_cpp(matrixs, chessboard)
                            tell_cpp2(wst)
                            if (unsignedcount == 0):
                                if(blackcount > whitecount):
                                    print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                                elif (whitecount > blackcount):
                                    print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                                else:
                                    print('游戏结束，平局！')
                    else:
                            pass
                elif (human == -1):
                    matrixs[y_loc-1][x_loc-1] = 1
                    if not reverse_ok(y_loc-1, x_loc-1, matrixs):
                        current_player = - current_player
                        if current_player == human:
                            not_move = False
                        else:
                            not_move = True
                    if reverse_ok(y_loc-1, x_loc-1, matrixs):
                            moves += 1
                            reverse(y_loc-1, x_loc-1, matrixs)
                            list_of_arrays[moves] = copy.deepcopy(matrixs)
                            blackcount = whitecount = unsignedcount = 0
                            for i in range(8):
                                for j in range(8):
                                    if (matrixs[i][j] == 1):
                                            windows.blit(black1, (i*100+2, j*100+2))
                                            blackcount += 1
                                    elif (matrixs[i][j] == -1):
                                            windows.blit(white1, (i*100+2, j*100+2))
                                            whitecount += 1
                                    elif(matrixs[i][j] == 0):
                                            unsignedcount += 1
                            pygame.display.flip()
                            ans = (count(matrixs)) - 4
                            print('现在是第', ans, '手', sep='')
                            if (current_player == 1):
                                print('下一手黑棋落子（B）')
                            elif(current_player == -1):
                                print('下一手白棋落子（W）')
                            print('下一手可落子点位数：', change_needed(matrixs))
                            print('黑棋数：', blackcount)
                            print('白棋数：', whitecount)
                            print('未定区域：', unsignedcount)
                            print(' ', sep='\n')
                            available_show(matrixs)
                            tell_cpp(matrixs, chessboard)
                            tell_cpp2(wst)
                            if (unsignedcount == 0):
                                if(blackcount > whitecount):
                                    print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                                elif (whitecount > blackcount):
                                    print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                                else:
                                    print('游戏结束，平局！')


    #AI落子的情况
    if not_move and not pvp and ans1 == "1" and not ququ:
        fakeAI(matrixs,human)
    if not_move and not pvp and ans1 == "2" and not ququ:
        tell_cpp(matrixs, chessboard)
        tell_cpp2(wst)
        print('AI思考中...')
        print('')
        time.sleep(0.3)
        magic = False
        if not (magic):
            cpp_executable_path = "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\algorithmreversi.exe"
            # 使用 subprocess.run 执行程序并捕获输出
            result = subprocess.run(
                [cpp_executable_path], stdout=subprocess.PIPE)
            output_str = result.stdout.decode('utf-8')
            x_loc = int(output_str[0])
            y_loc = int(output_str[1])

        if (human == 1):  # 人类黑棋
            matrixs[y_loc-1][x_loc-1] = -1
            if not reverse_ok(y_loc-1, x_loc-1, matrixs):
                current_player = - current_player
            if reverse_ok(y_loc-1, x_loc-1, matrixs):
                    moves += 1
                    reverse(y_loc-1, x_loc-1, matrixs)
                    list_of_arrays[moves] = copy.deepcopy(matrixs)
                    blackcount = whitecount = unsignedcount = 0
                    for i in range(8):
                        for j in range(8):
                            if (matrixs[i][j] == 1):
                                windows.blit(black1, (i*100+2, j*100+2))
                                blackcount += 1
                            elif (matrixs[i][j] == -1):
                                windows.blit(white1, (i*100+2, j*100+2))
                                whitecount += 1
                            elif(matrixs[i][j] == 0):
                                unsignedcount += 1
                    pygame.display.flip()
                    ans = (count(matrixs)) - 4
                    print('现在是第', ans, '手', sep='')
                    if (current_player == 1):
                        print('下一手黑棋落子（B）')
                    elif(current_player == -1):
                        print('下一手白棋落子（W）')
                    print('下一手可落子点位数：', change_needed(matrixs))
                    print('黑棋数：', blackcount)
                    print('白棋数：', whitecount)
                    print('未定区域：', unsignedcount)
                    print(' ', sep='\n')
                    available_show(matrixs)
                    tell_cpp(matrixs, chessboard)
                    tell_cpp2(wst)
                    if (unsignedcount == 0):
                        if(blackcount > whitecount):
                            print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                        elif (whitecount > blackcount):
                            print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                        else:
                            print('游戏结束，平局！')
            else:
                        pass
        elif (human == -1):
            matrixs[y_loc-1][x_loc-1] = 1
            if not reverse_ok(y_loc-1, x_loc-1, matrixs):
                current_player = - current_player
                if current_player == human:
                    not_move = False
                else:
                    not_move = True
            if reverse_ok(y_loc-1, x_loc-1, matrixs):
                    moves += 1
                    reverse(y_loc-1, x_loc-1, matrixs)
                    list_of_arrays[moves] = copy.deepcopy(matrixs)
                    blackcount = whitecount = unsignedcount = 0
                    for i in range(8):
                        for j in range(8):
                                if (matrixs[i][j] == 1):
                                    windows.blit(black1, (i*100+2, j*100+2))
                                    blackcount += 1
                                elif (matrixs[i][j] == -1):
                                    windows.blit(white1, (i*100+2, j*100+2))
                                    whitecount += 1
                                elif(matrixs[i][j] == 0):
                                    unsignedcount += 1
                        pygame.display.flip()
                        ans = (count(matrixs)) - 4
                        print('现在是第', ans, '手', sep='')
                        if (current_player == 1):
                            print('下一手黑棋落子（B）')
                        elif(current_player == -1):
                            print('下一手白棋落子（W）')
                        print('下一手可落子点位数：', change_needed(matrixs))
                        print('黑棋数：', blackcount)
                        print('白棋数：', whitecount)
                        print('未定区域：', unsignedcount)
                        print(' ', sep='\n')
                        available_show(matrixs)
                        tell_cpp(matrixs, chessboard)
                        tell_cpp2(wst)
                        if (unsignedcount == 0):
                            if(blackcount > whitecount):
                                print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                            elif (whitecount > blackcount):
                                print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                            else:
                                print('游戏结束，平局！')

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            exit()
        if event.type == pygame.MOUSEBUTTONDOWN:
            if not_move:
                pass
            else:
                mouse_x, mouse_y = event.pos
                c = int(mouse_x/100)
                d = int(mouse_y/100)
                if matrixs[c][d] == 1 or matrixs[c][d] == -1:
                    pass
                elif matrixs[c][d] == 0:
                    if reverse_ok(c, d, matrixs):
                        moves += 1
                        reverse(c, d, matrixs)
                        list_of_arrays[moves] = copy.deepcopy(matrixs)
                        blackcount = whitecount = unsignedcount = 0
                        for i in range(8):
                            for j in range(8):
                                if (matrixs[i][j] == 1):
                                    windows.blit(black1, (i*100+2, j*100+2))
                                    blackcount += 1
                                elif (matrixs[i][j] == -1):
                                    windows.blit(white1, (i*100+2, j*100+2))
                                    whitecount += 1
                                elif(matrixs[i][j] == 0):
                                    unsignedcount += 1
                        pygame.display.flip()
                        ans = (count(matrixs)) - 4
                        print('现在是第', ans, '手', sep='')
                        if (current_player == 1):
                            print('下一手黑棋落子（B）')
                        elif(current_player == -1):
                            print('下一手白棋落子（W）')
                        print('下一手可落子点位数：', change_needed(matrixs))
                        print('黑棋数：', blackcount)
                        print('白棋数：', whitecount)
                        print('未定区域：', unsignedcount)
                        print(' ', sep='\n')
                        available_show(matrixs)
                        tell_cpp(matrixs, chessboard)
                        tell_cpp2(wst)
                        if (unsignedcount == 0):
                            if(blackcount > whitecount):
                                print("游戏结束，黑方获胜: B+", blackcount-whitecount)
                            elif (whitecount > blackcount):
                                print("游戏结束，白方获胜: W+", -blackcount+whitecount)
                            else:
                                print('游戏结束，平局！')
                            break
                    else:
                        pass
                else:
                    pass
        elif event.type == pygame.KEYDOWN:    # 悔棋
            if event.key == pygame.K_u:
                if not_move:
                    break
                if moves <= 2:
                    break
                print('悔棋成功！')
                print(' ', sep='\n')
                moves -= 2
                matrixs = copy.deepcopy(list_of_arrays[moves])
                blackcount = whitecount = unsignedcount = 0
                for i in range(8):
                    for j in range(8):
                        if (matrixs[i][j] == 1):
                            windows.blit(black1, (i*100+2, j*100+2))
                            blackcount += 1
                        elif (matrixs[i][j] == -1):
                            windows.blit(white1, (i*100+2, j*100+2))
                            whitecount += 1
                        elif(matrixs[i][j] == 0):
                            windows.blit(empty, (i*100+2, j*100+2))
                            unsignedcount += 1
                pygame.display.flip()
                ans = count(matrixs)-4
                print('现在是第', ans, '手', sep='')
                if (current_player == 1):
                    print('下一手黑棋落子（B）')
                elif(current_player == -1):
                    print('下一手白棋落子（W）')
                print('下一手可落子点位数：', change_needed(matrixs))
                print('黑棋数：', blackcount)
                print('白棋数：', whitecount)
                print('未定区域：', unsignedcount)
                print(' ', sep='\n')
                available_show(matrixs)
                tell_cpp(matrixs, chessboard)
                tell_cpp2(wst)
            if event.key == pygame.K_s:
                if not_move:
                    break
                if files == 0:
                    print("存档成功，已存为存档1，您随时可以退出游戏，再次进入游戏时按R键即可读取目前进度")
                    tell_cpp(
                        matrixs, "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves.txt")
                    save_player(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves1.txt")
                    files += 1
                    hasfile1 = True
                elif files == 1:
                    print("存档成功，已记为存档2，您随时可以退出游戏，再次进入游戏时按R键即可读取目前进度")
                    tell_cpp(
                        matrixs, "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Savess.txt")
                    save_player(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves2.txt")
                    files += 1
                    hasfile2 = True
                elif files == 2:
                    print("存档成功，已记为存档3，您随时可以退出游戏，再次进入游戏时按R键即可读取目前进度")
                    tell_cpp(
                        matrixs, "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Savesss.txt")
                    save_player(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves3.txt")
                    files += 1
                    hasfile3 = True
                else:
                    print('可用的存档个数用完了呢......')
            elif event.key == pygame.K_r:
                if not_move:
                    break
                print('想读取哪个存档呢？')
                print(' ')
                n = input()
                if n == "1":
                    load_from_file(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves.txt")
                    load_player(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves1.txt")
                    try:
                        for i in range(8):
                            for j in range(8):
                                matrixs[i][j] = matrix2[i][j]
                        moves = count(matrixs)-4
                    except:
                        print('啊呀，存档里什么都没有呢...')
                        break
                elif n == "2":
                    load_from_file(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Savess.txt")
                    load_player(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves2.txt")
                    try:
                        for i in range(8):
                            for j in range(8):
                                matrixs[i][j] = matrix2[i][j]
                        moves = count(matrixs)-4

                    except:
                        print('啊呀，存档里什么都没有呢...')
                        break
                elif n == "3":
                    load_from_file(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Savesss.txt")
                    load_player(
                        "C:\\Users\\10744\\Desktop\\ReversiGame\\backup2-reversiQLC\\Saves3.txt")
                    try:
                        for i in range(8):
                            for j in range(8):
                                matrixs[i][j] = matrix2[i][j]
                        moves = count(matrixs)-4

                    except:
                        print('啊呀，存档里什么都没有呢...')
                        break
                else:
                    print('没有这个存档呢...')
            try:
                for i in range(8):
                    for j in range(8):
                        matrixs[i][j] = matrix2[i][j]
                blackcount = whitecount = unsignedcount = 0
                for i in range(8):
                    for j in range(8):
                        if (matrixs[i][j] == 1):
                            windows.blit(black1, (i*100+2, j*100+2))
                            blackcount += 1
                        elif (matrixs[i][j] == -1):
                            windows.blit(white1, (i*100+2, j*100+2))
                            whitecount += 1
                        elif(matrixs[i][j] == 0):
                            windows.blit(empty, (i*100+2, j*100+2))
                            unsignedcount += 1
                ans = count(matrixs) - 4
                if (current_player == 1):
                    print('下一手黑棋落子（B）')
                elif(current_player == -1):
                    print('下一手白棋落子（W）')
                print('现在是第', ans, '手', sep='')
                print('下一手可落子点位数：', change_needed(matrixs))
                print('黑棋数：', blackcount)
                print('白棋数：', whitecount)
                print('未定区域：', unsignedcount)
                print(' ', sep='\n')
                available_show(matrixs)
                tell_cpp(matrixs, chessboard)
                tell_cpp2(wst)
                pygame.display.flip()
            except:
                pass
            if event.key == pygame.K_o:
                print('真的要退出吗？')
                print('1: 退出  2: 我再想想')
                for i in range(1000):
                    ans4 = input()
                    if ans4 == "1":
                        print('退出咯')
                        pygame.quit()
                        sys.exit()
                    elif ans4 == "2":
                        break
                    else:
                        print('Invalid input.Please try again!')
            if event.key == pygame.K_q:
                print('确定要重新开始对局吗？当前进度可能丢失')
                print('1：确定    2：我再想想')
                for i in range (80):
                    ans = input()
                    if ans == "2":
                        break
                    elif ans == "1":
                        moves = 0
                        print('OK!')
                        for i in range (8):
                            for j in range (8):
                                matrixs[i][j] = 0
                        matrixs[3][3] = matrixs[4][4] = -1
                        matrixs[3][4] = matrixs[4][3] = 1
                        current_player = 1
                        blackcount = whitecount = unsingedcount = 0
                        for i in range(8):
                            for j in range(8):
                                if (matrixs[i][j] == 1):
                                    windows.blit(black1, (i*100+2, j*100+2))
                                    blackcount += 1
                                elif (matrixs[i][j] == -1):
                                    windows.blit(white1, (i*100+2, j*100+2))
                                    whitecount += 1
                                elif(matrixs[i][j] == 0):
                                    windows.blit(empty, (i*100+2, j*100+2))
                                    unsignedcount += 1
                        pygame.display.flip()
                        break
            if event.key == pygame.K_h:
                print(' ')
                print('玩法说明：')
                print('按下S键：设置存档')
                print('按下Q键：重新开始')
                print('按下R键：读取存档')
                print('按下U键：悔棋')
                print('按下O键：退出游戏')
                print('按下H键：帮助菜单')
                print('没记住没关系，任何时间按下H键即可弹出帮助菜单')
                print('准备好的话就开始吧！')
                print('')




pygame.time.Clock().tick(30)
