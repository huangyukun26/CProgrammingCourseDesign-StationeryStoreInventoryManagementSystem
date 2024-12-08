#ifndef GOODS_H
#define GOODS_H

#include <stdio.h>
#include <string.h>

//商品类别枚举
//用于定义商品的基本分类：笔类、本类、颜料类和其他类
typedef enum {
    PEN,        //笔类商品
    NOTEBOOK,   //本类商品
    PAINT,      //颜料类商品
    OTHER       //其他类商品
} GoodsCategory;

//商品基本信息结构体
//包含商品的所有基本属性：编号、名称、类别、品牌、单价和库存
typedef struct {
    char id[20];           //商品编号，最大18字符+'\0'
    char name[50];         //商品名称，最大48字符+'\0'
    GoodsCategory category;//商品类别
    char brand[50];        //商品品牌，最大48字符+'\0'
    float price;           //商品单价(>0)
    int stock;            //库存数量(>=0)
} Goods;

//商品链表节点结构体
//用于构建商品信息的链表，包含商品数据和指向下一个节点的指针
typedef struct GoodsNode {
    Goods data;            //商品数据
    struct GoodsNode* next;//指向下一个节点的指针
} GoodsNode;

//商品管理系统结构体
//用于管理整个商品链表，包含头节点指针和商品总数
typedef struct {
    GoodsNode* head;      //链表头节点指针
    int count;            //商品总数计数器
} GoodsManager;

//基础功能函数声明
GoodsManager* initGoodsManager();    //初始化商品管理系统
void freeGoodsManager(GoodsManager* manager);  //释放商品管理系统内存
int loadFromFile(GoodsManager* manager, const char* filename);  //从文件加载数据
int saveToFile(GoodsManager* manager, const char* filename);    //保存数据到文件

//基本操作函数声明
int addGoods(GoodsManager* manager, Goods goods);  //添加商品
int deleteGoods(GoodsManager* manager, const char* id);  //删除商品
int updateGoods(GoodsManager* manager, const char* id, Goods newData);  //更新商品信息
GoodsNode* findGoodsById(GoodsManager* manager, const char* id);  //按ID查找商品
void displayAllGoods(GoodsManager* manager);  //显示所有商品

//辅助函数声明
const char* categoryToString(GoodsCategory category);  //将商品类别转换为字符串
GoodsCategory stringToCategory(const char* str);  //将字符串转换为商品类别
int isValidGoods(Goods goods);  //验证商品信息是否有效

//高级功能函数声明
//统计和查询功能
int countGoodsByCategory(GoodsManager* manager, GoodsCategory category);  //按类别统计商品数量
void displayGoodsByCategory(GoodsManager* manager, GoodsCategory category);  //显示指定类别的商品
void sortGoodsByPrice(GoodsManager* manager, int ascending);  //按价格排序
float calculateTotalValue(GoodsManager* manager);  //计算总库存价值

//搜索功能
GoodsNode* findGoodsByName(GoodsManager* manager, const char* name);  //按名称搜索
GoodsNode* findGoodsByBrand(GoodsManager* manager, const char* brand);  //按品牌搜索
void displaySearchResults(GoodsNode* results);  //显示搜索结果

#endif 