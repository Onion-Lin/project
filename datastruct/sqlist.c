#include <stdio.h>
#include <errno.h>
typedef int ElemType;       //解耦
/*顺序表的存储结构*/

//顺序表定义
#define MAXSIZE 100
typedef struct{
    ElemType* elem;     //基地址
    int length;         //长度
} Sqlist;

//错误处理函数，被调用的时候检查 errno 信息
void die(const char* message){
    if(errno){  //在存在报错信息的时候打印报错的信息
        perror(message);
    } else {    //没有就打印传入的字符串
        printf("ERROE:%s\n",message);
    }
    exit(1);    //并调用exit终止程序并返回错误值1
}


//初始化
Sqlist init_list(){
    Sqlist l;
    l.elem = malloc(MAXSIZE * sizeof( ElemType ));
    if(!l.elem)    perror("Malloc failed");
    l.length = 0;
    return l;
}

//查找
 ElemType get_elem(Sqlist l,int i){
    if(i<1 || i> MAXSIZE)   die("wrong i");
    return l.elem[i-1];
 }

 //值查找
  ElemType locate_elem(Sqlist l, ElemType e){
    for(int i=0;i<MAXSIZE;i++){
        if(l.elem[i] == e)  return i+1;
    }
    return 0;   //查找失败
  }
