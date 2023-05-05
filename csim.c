/*
 *csim.c-使用C编写一个Cache模拟器，它可以处理来自Valgrind的跟踪和输出统计
 *息，如命中、未命中和逐出的次数。更换政策是LRU。
 * 设计和假设:
 *  1. 每个加载/存储最多可导致一个缓存未命中。（最大请求是8个字节。）
 *  2. 忽略指令负载（I），因为我们有兴趣评估trace.c内容中数据存储性能。
 *  3. 数据修改（M）被视为加载，然后存储到同一地址。因此，M操作可能导致两次缓存命中，或者一次未命中和一次命中，外加一次可能的逐出。
 * 使用函数printSummary() 打印输出，输出hits, misses and evictions 的数，这对结果评估很重要
*/
#include "cachelab.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <getopt.h>
#include <math.h>

int s,S,E,b,B;
int v,h;
char* t;
int hit_count,miss_count, eviction_count;

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

typedef struct
{
   int valid_bit;   // 有效标志位
   int tag_bits;    // 标记位
   int timestamp;   // 时间戳
}cache_line_t, *cache_set_t, **cache_t;

cache_t cache = NULL; //高速缓存
//                    请在此处添加代码  
//****************************Begin*********************
void printUsage()
{
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n"
            "Options:\n"
            "  -h         Print this help message.\n"
            "  -v         Optional verbose flag.\n"
            "  -s <num>   Number of set index bits.\n"
            "  -E <num>   Number of lines per set.\n"
            "  -b <num>   Number of block offset bits.\n"
            "  -t <file>  Trace file.\n"

            "Examples:\n"
            "  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
            "  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

void initCache()
{
    cache = (cache_t)malloc(sizeof(cache_set_t) * S);
    for (int i = 0; i < S; i++) {
        cache[i] = (cache_set_t)malloc(sizeof(cache_line_t) * E);
        for (int j = 0; j < E; j++) {
            cache[i][j].valid_bit = 0;
            cache[i][j].tag_bits = -1;
            cache[i][j].timestamp = -1;
        }
    }
}

int hitMissEviction(unsigned int address)
{
    unsigned int setIndex = address >> b & (S -1);
    int tag = address >> (s+b); //标记
    cache_set_t cacheSet = cache[setIndex];
    int maxTimeStamp = -1;
    int maxTimeStampIndex = -1;

    // hint
    for(int i = 0;i < E;i++)
    {
        if(cacheSet[i].tag_bits == tag)
        {
            cacheSet[i].timestamp = 0; //命中将时间置为0
            hit_count++;
            return 0;
        }
    }

    // miss
    for(int i = 0;i < E;i++)
    {
        if(!cacheSet[i].valid_bit)
        {
            cacheSet[i].valid_bit = 1;
            cacheSet[i].tag_bits = tag;
            cacheSet[i].timestamp = 0;
            miss_count++;
            return 1;
        }
    }


    // eviction
    for(int i = 0;i < E;i++)
    {
        if(cacheSet[i].timestamp > maxTimeStamp)
        {
            maxTimeStamp = cacheSet[i].timestamp;
            maxTimeStampIndex = i;
        }
    }


    //将最长时间停留的内存进行替换
    cacheSet[maxTimeStampIndex].tag_bits = tag;
    cacheSet[maxTimeStampIndex].timestamp = 0;
    eviction_count++;
    miss_count++;
    return 2;
}

void update_timestamp()
{
    for (int i = 0; i < S; i++)
        for (int j = 0; j< E; j++)
            if (cache[i][j].valid_bit)
                cache[i][j].timestamp++;
}


void readFile()
{
    FILE * pFile;
    pFile = fopen(t,"r");
    if(pFile == NULL)
    {
        return;
    }
    char operation; // 进行操作 L,M,S
    unsigned int address; // 地址参数
    int size; //大小
    //int ret;
    // 例如读取行 " M 20,1" or "L 19,3"
    while(fscanf(pFile," %c %x,%d", &operation, &address, &size)>0)
    {
    // 相关操作
        if (v)
        {
            printf("%c %x,%d hit\n", operation, address, size);
        }
        switch(operation)
        {
        case 'L':
            hitMissEviction(address);
            break;
        case 'M':
            hitMissEviction(address);
        case 'S':
            hitMissEviction(address);
            break;
        }
        update_timestamp();
    }
    fclose(pFile); //记得完成后close文件
}
void freeCache()
{
    for (int i = 0; i < S; i++)
        free(cache[i]);
    free(cache);
}

int main(int argc, char** argv)
{   
    int opt;
    /* 循环参数 */
    while(-1 != (opt = getopt(argc, argv, "hvs:E:b:t:"))){
        /* 确定正在处理的参数 */
        switch(opt) {
            case 'h':
                printUsage(argv);
                break;
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                S = pow(2,s);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                B = pow(2,b);
                break;
            case 't':
                t = optarg;
                break;
            default:
                printf("wrong argument\n");
                break;
        } 
    }
    if (s == 0 || E == 0 || b == 0 || t == NULL)
    {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }


    /* Initialize cache */
    initCache();
    readFile();

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);//输出hit、miss和evictions数量统计 
    return 0;
}
//****************************End**********************#