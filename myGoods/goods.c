#pragma warning(disable:4819)  // 禁用代码页警告
#define _CRT_SECURE_NO_WARNINGS
#include "goods.h"
#include <stdlib.h>

//初始化商品管理系统
//功能：分配并初始化一个新的商品管理系统结构体
//返回：成功返回管理器指针，失败返回NULL
GoodsManager* initGoodsManager() 
{
    //动态分配内存
    GoodsManager* manager = (GoodsManager*)malloc(sizeof(GoodsManager));
    if (manager == NULL) 
    {
        return NULL;  //内存分配失败
    }
    //初始化成员
    manager->head = NULL;  //链表初始为空
    manager->count = 0;    //初始商品数量为0
    return manager;
}

//释放商品管理系统内存
//功能：释放链表中所有节点和管理器本身的内存
//参数：manager - 要释放的管理器指针
void freeGoodsManager(GoodsManager* manager) 
{
    if (manager == NULL) 
    {
        return;
    }
    
    // 遍历释放所有节点
    GoodsNode* current = manager->head;
    while (current != NULL) 
    {
        GoodsNode* next = current->next;  //保存下一个节点
        free(current);                    //释放当前节点
        current = next;
    }
    
    free(manager);  //释放管理器本身
}

//商品类别转换为字符串
//功能：将枚举类型的商品类别转换为可读的字符串
const char* categoryToString(GoodsCategory category) 
{
    switch (category) 
    {
        case PEN: return "Pen";
        case NOTEBOOK: return "Notebook";
        case PAINT: return "Paint";
        case OTHER: return "Other";
        default: return "Unknown";
    }
}

//字符串转换为商品类别
//功能：将字符串转换为对应的枚举类型类别
GoodsCategory stringToCategory(const char* str) 
{
    if (strcmp(str, "Pen") == 0) return PEN;
    if (strcmp(str, "Notebook") == 0) return NOTEBOOK;
    if (strcmp(str, "Paint") == 0) return PAINT;
    return OTHER;   //默认返回其他类别
}

//验证商品信息有效性
//功能：检查商品信息是否符合要求
//返回：有效返回1，无效返回0
int isValidGoods(Goods goods) 
{
    //检查各字段是否符合要求
    if (strlen(goods.id) == 0 || strlen(goods.id) >= 19) return 0;    //最大18个字符
    if (strlen(goods.name) == 0 || strlen(goods.name) >= 49) return 0; //最大48个字符
    if (strlen(goods.brand) == 0 || strlen(goods.brand) >= 49) return 0; //最大48个字符
    if (goods.price <= 0) return 0;  //价格必须为正
    if (goods.stock < 0) return 0;   //库存不能为负
    return 1;
}

//从文件加载商品数据
//功能：从指定文件读取商品信息并构建链表
//返回：成功返回1，失败返回0
int loadFromFile(GoodsManager* manager, const char* filename) 
{
    if (manager == NULL) {
        return 0;
    }

    FILE* file = NULL;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == NULL) 
    {
        return 0;  //文件打开失败
    }

    char line[256];
    char id[20], name[50], brand[50], category_str[20];
    float price;
    int stock;
    int success_count = 0;
    int invalid_count = 0;
    int duplicate_count = 0;

    //逐行读取文件内容
    int line_number = 0;  //行号计数
    while (fgets(line, sizeof(line), file)) 
    {
        line_number++;
        //解析每行数据
        if (sscanf_s(line, "%19s %49s %19s %49s %f %d",
                   id, (unsigned)sizeof(id),
                   name, (unsigned)sizeof(name),
                   category_str, (unsigned)sizeof(category_str),
                   brand, (unsigned)sizeof(brand),
                   &price, &stock) == 6) {
            
            //预检查数据长度 -检查条件为严格限制
            if (strlen(id) >= 19) 
            {  //ID最多18个字符
                printf("Warning: Line %d - ID '%s' exceeds length limit (max 18 chars), skipping...\n", 
                       line_number, id);
                invalid_count++;
                continue;
            }
            if (strlen(name) >= 49) 
            {  //名称最多48个字符
                printf("Warning: Line %d - Name '%s' exceeds length limit (max 48 chars), skipping...\n", 
                       line_number, name);
                invalid_count++;
                continue;
            }
            if (strlen(brand) >= 49) 
            {  //品牌最多48个字符
                printf("Warning: Line %d - Brand '%s' exceeds length limit (max 48 chars), skipping...\n", 
                       line_number, brand);
                invalid_count++;
                continue;
            }

            //预检查数值有效性
            if (price <= 0) 
            {
                printf("Warning: Line %d - Invalid price value (must be > 0), skipping...\n", line_number);
                invalid_count++;
                continue;
            }
            if (stock < 0) 
            {
                printf("Warning: Line %d - Invalid stock value (must be >= 0), skipping...\n", line_number);
                invalid_count++;
                continue;
            }

            //检查类别是否有效
            GoodsCategory category = stringToCategory(category_str);
            if (category == OTHER && strcmp(category_str, "Other") != 0) 
            {
                printf("Warning: Line %d - Invalid category '%s', skipping...\n", 
                       line_number, category_str);
                invalid_count++;
                continue;
            }

            //所有检查通过后才构造商品对象
            Goods goods;
            strncpy_s(goods.id, sizeof(goods.id), id, _TRUNCATE);
            strncpy_s(goods.name, sizeof(goods.name), name, _TRUNCATE);
            strncpy_s(goods.brand, sizeof(goods.brand), brand, _TRUNCATE);
            goods.category = category;
            goods.price = price;
            goods.stock = stock;

            //添加商品
            if (findGoodsById(manager, goods.id) != NULL) 
            {
                printf("Warning: Line %d - Duplicate product ID '%s', skipping...\n", 
                       line_number, goods.id);
                duplicate_count++;
                continue;
            }
            if (addGoods(manager, goods)) 
            {
                success_count++;
            }
        } 
        else
        {
            printf("Warning: Line %d - Invalid format, skipping...\n", line_number);
            invalid_count++;
        }
    }

    fclose(file);
    
    //显示导入结果
    printf("\nImport summary:\n");
    printf("Successfully imported: %d products\n", success_count);
    if (duplicate_count > 0) 
    {
        printf("Skipped %d duplicate products\n", duplicate_count);
    }
    if (invalid_count > 0) 
    {
        printf("Skipped %d invalid records\n", invalid_count);
    }
    
    return success_count > 0;
}

//保存商品数据到文件
//功能：将当前链表中的所有商品信息写入文件
int saveToFile(GoodsManager* manager, const char* filename) 
{
    if (manager == NULL) 
    {
        return 0;
    }

    FILE* file = NULL;
    errno_t err = fopen_s(&file, filename, "w");
    if (err != 0 || file == NULL) 
    {
        return 0;
    }

    // 遍历链表写入数据
    GoodsNode* current = manager->head;
    while (current != NULL) 
    {
        fprintf(file, "%s %s %s %s %.2f %d\n",
                current->data.id,
                current->data.name,
                categoryToString(current->data.category),
                current->data.brand,
                current->data.price,
                current->data.stock);
        current = current->next;
    }

    fclose(file);
    return 1;
}

//按ID查找商品
//功能：在链表中查找指定ID的商品
//参数：manager - 管理器指针，id - 要查找的商品ID
//返回：找到返回节点指针，未找到返回NULL
GoodsNode* findGoodsById(GoodsManager* manager, const char* id) 
{
    if (manager == NULL || id == NULL) 
    {
        return NULL;
    }

    //遍历链表查找匹配的ID
    GoodsNode* current = manager->head;
    while (current != NULL) 
    {
        if (strcmp(current->data.id, id) == 0) 
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

//添加商品
//功能：向链表头部添加新商品
//参数：manager - 管理器指针，goods - 要添加的商品信息
//返回：成功返回1，失败返回0
int addGoods(GoodsManager* manager, Goods goods) 
{
    if (manager == NULL || !isValidGoods(goods)) 
    {
        return 0;
    }

    //检查是否存在重复ID
    if (findGoodsById(manager, goods.id) != NULL) 
    {
        return 0;
    }

    //创建新节点
    GoodsNode* newNode = (GoodsNode*)malloc(sizeof(GoodsNode));
    if (newNode == NULL) 
    {
        return 0;  //内存分配失败
    }

    //初始化新节点并插入到链表头部
    newNode->data = goods;
    newNode->next = manager->head;
    manager->head = newNode;
    manager->count++;

    return 1;
}

//删除商品
//功能：从链表中���除指定ID的商品
//参数：manager - 管理器指针，id - 要删除的商品ID
//返回：成功返回1，失败返回0
int deleteGoods(GoodsManager* manager, const char* id) 
{
    if (manager == NULL || id == NULL || manager->head == NULL) 
    {
        return 0;
    }

    GoodsNode* current = manager->head;
    GoodsNode* prev = NULL;

    //遍历查找删除的节点
    while (current != NULL) 
    {
        if (strcmp(current->data.id, id) == 0) 
        {
            //处理删除节点的链表连接
            if (prev == NULL) 
            {
                manager->head = current->next;  //删除头节点
            } 
            else 
            {
                prev->next = current->next;     //删除中间节点
            }
            free(current);  //释放节点内存
            manager->count--;
            return 1;
        }
        prev = current;
        current = current->next;
    }

    return 0;  //未找到指定ID的商品
}

//更新商品信息
//功能：更新指定ID商品的信息
//参数：manager - 管理器指针，id - 要更新的商品ID，newData - 新的商品信息
//返回：成功返回1，失败返回0
int updateGoods(GoodsManager* manager, const char* id, Goods newData) 
{
    if (manager == NULL || id == NULL || !isValidGoods(newData)) 
    {
        return 0;
    }

    //查找要更新的节点
    GoodsNode* node = findGoodsById(manager, id);
    if (node == NULL) {
        return 0;  //未找到指定ID的商品
    }

    //保持原ID不变，更新其他信息
    strcpy_s(newData.id, sizeof(newData.id), id);
    node->data = newData;
    return 1;
}

//添加字符串截断辅助函数
void truncateString(char* dest, const char* src, size_t maxLen) 
{
    if (strlen(src) > maxLen - 3) 
    {  //为"..."预留空间
        strncpy(dest, src, maxLen - 3);
        dest[maxLen - 3] = '\0';  //确保字符串结束
        strcat(dest, "...");
    } 
    else 
    {
        strcpy(dest, src);
    }
}

//显示函数
void displayAllGoods(GoodsManager* manager) 
{
    if (manager == NULL) 
    {
        printf("Manager not initialized!\n");
        return;
    }

    if (manager->head == NULL) 
    {
        printf("No products found.\n");
        return;
    }

    //打印表头
    printf("\n");
    printf("%-16s  ", "ID");              //ID列，左对齐，16字符
    printf("%-25s  ", "Name");            //名称列，左对齐，25字符
    printf("%-12s  ", "Category");        //类别列，左对齐，12字符
    printf("%-20s  ", "Brand");           //品牌列，左对齐，20字符
    printf("%10s  ", "Price");            //价格列，右对齐，10字符
    printf("%8s\n", "Stock");             //库存列，右对齐，8字符

    //打印分隔线
    printf("----------------  ");          //16字符
    printf("-------------------------  "); //25字符
    printf("------------  ");             //12字符
    printf("--------------------  ");     //20字符
    printf("----------  ");               //10字符
    printf("--------\n");                 //8字符

    //遍历打印每个商品的信息
    GoodsNode* current = manager->head;
    while (current != NULL) 
    {
        char truncated_id[17] = {0};
        char truncated_name[26] = {0};
        char truncated_brand[21] = {0};

        truncateString(truncated_id, current->data.id, 16);
        truncateString(truncated_name, current->data.name, 25);
        truncateString(truncated_brand, current->data.brand, 20);

        printf("%-16s  ", truncated_id);
        printf("%-25s  ", truncated_name);
        printf("%-12s  ", categoryToString(current->data.category));
        printf("%-20s  ", truncated_brand);
        printf("%10.2f  ", current->data.price);
        printf("%8d\n", current->data.stock);
        current = current->next;
    }

    //打印底部分隔线
    printf("----------------  ");
    printf("-------------------------  ");
    printf("------------  ");
    printf("--------------------  ");
    printf("----------  ");
    printf("--------\n");
}

//类别统计商品数量
//功能：统计指定类别的商品数量
//参数：manager - 管理器指针，category - 要统计的商品类别
//返回：该类别的商品数量
int countGoodsByCategory(GoodsManager* manager, GoodsCategory category)
{
    if (manager == NULL) 
    {
        return 0;
    }

    //遍历链表统计指定类别的商品数量
    int count = 0;
    GoodsNode* current = manager->head;
    while (current != NULL) 
    {
        if (current->data.category == category) 
        {
            count++;
        }
        current = current->next;
    }
    return count;
}

//显示指定类别的商品
//功能：以表格形式显示指定类别的所有商品信息
//参数：manager - 管理器指针，category - 要显示的商品类别
void displayGoodsByCategory(GoodsManager* manager, GoodsCategory category) 
{
    if (manager == NULL)
    {
        printf("Manager not initialized!\n");
        return;
    }

    //打印表头
    printf("\nProducts in category [%s]:\n", categoryToString(category));
    printf("%-16s  ", "ID");              //ID列，左对齐，16字符
    printf("%-25s  ", "Name");            //名称列，左对齐，25字符
    printf("%-20s  ", "Brand");           //品牌列，左对齐，20字符
    printf("%10s  ", "Price");            //价格列，右对齐，10字符
    printf("%8s\n", "Stock");             //库存列，右对齐，8字符
    
    //打印分隔线
    printf("----------------  ");          //16字符
    printf("-------------------------  "); //25字符
    printf("--------------------  ");     //20字符
    printf("----------  ");               //10字符
    printf("--------\n");                 //8字符

    //遍历打印符合类别的商品
    int count = 0;
    GoodsNode* current = manager->head;
    while (current != NULL)
    {
        if (current->data.category == category) 
        {
            char truncated_id[17] = {0};
            char truncated_name[26] = {0};
            char truncated_brand[21] = {0};

            truncateString(truncated_id, current->data.id, 16);
            truncateString(truncated_name, current->data.name, 25);
            truncateString(truncated_brand, current->data.brand, 20);

            printf("%-16s  ", truncated_id);
            printf("%-25s  ", truncated_name);
            printf("%-20s  ", truncated_brand);
            printf("%10.2f  ", current->data.price);
            printf("%8d\n", current->data.stock);
            count++;
        }
        current = current->next;
    }

    //打印底部分隔线
    printf("----------------  ");
    printf("-------------------------  ");
    printf("--------------------  ");
    printf("----------  ");
    printf("--------\n");

    //显示统计结果
    if (count == 0)
    {
        printf("No products found in this category.\n");
    } 
    else 
    {
        printf("\nFound %d products.\n", count);
    }
}

//链表排序辅助函数：合并两个有序链表
//功能：将两个有序链表合并为一个新的有序链表
//参数：a,b - 要合并的两个链表头指针，ascending - 是否升序
//返回：合并后的链表头指针
GoodsNode* mergeSortedLists(GoodsNode* a, GoodsNode* b, int ascending) 
{
    if (a == NULL) return b;  //如果a为空，直接返回b
    if (b == NULL) return a;  //如果b为空，直接返回a

    GoodsNode* result = NULL;

    //根据排序方式选择较规则
    if (ascending) 
    {
        //升序排序：价格小的在前，价格相同时按ID升序
        if (a->data.price < b->data.price || 
            (a->data.price == b->data.price && strcmp(a->data.id, b->data.id) < 0)) 
        {
            result = a;
            result->next = mergeSortedLists(a->next, b, ascending);
        } 
        else 
        {
            result = b;
            result->next = mergeSortedLists(a, b->next, ascending);
        }
    } 
    else 
    {
        //降序排序：价格大的在前，价格相同时按ID升序
        if (a->data.price > b->data.price || 
            (a->data.price == b->data.price && strcmp(a->data.id, b->data.id) < 0)) 
        {
            result = a;
            result->next = mergeSortedLists(a->next, b, ascending);
        } 
        else 
        {
            result = b;
            result->next = mergeSortedLists(a, b->next, ascending);
        }
    }

    return result;
}

//链表排序辅助函数：分割链表
//功能：使用快慢指针法将链表分割为两个大致相等的部分
//参数：source - 源链表，frontRef,backRef - 用于存储分割结果的指针
void splitList(GoodsNode* source, GoodsNode** frontRef, GoodsNode** backRef) 
{
    //处理特殊情况
    if (source == NULL || source->next == NULL) {
        *frontRef = source;
        *backRef = NULL;
        return;
    }

    //使用快慢指针法找到中点
    GoodsNode* slow = source;
    GoodsNode* fast = source->next;

    while (fast != NULL) 
    {
        fast = fast->next;
        if (fast != NULL) 
        {
            fast = fast->next;
            slow = slow->next;
        }
    }

    //在中点分割链表
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

//链表归并排序
//功能：对链表进行归并排序
//参数：headRef - 链表头指针的指针，ascending - 是否升序
void mergeSort(GoodsNode** headRef, int ascending) 
{
    GoodsNode* head = *headRef;
    //处理基本情况
    if (head == NULL || head->next == NULL) 
    {
        return;
    }

    //分割链表
    GoodsNode* a;
    GoodsNode* b;
    splitList(head, &a, &b);

    //递归排序两个子链表
    mergeSort(&a, ascending);
    mergeSort(&b, ascending);

    //合并排序后的子链表
    *headRef = mergeSortedLists(a, b, ascending);
}

//按单价对商品排序
//功能：对商品链表按价格进行排序
//参数：manager - 管理器指针，ascending - 是否升序
void sortGoodsByPrice(GoodsManager* manager, int ascending) 
{
    if (manager == NULL || manager->head == NULL) 
    {
        return;
    }

    //使用归并排序对链表进行排序
    mergeSort(&(manager->head), ascending);
}

//计算当前库存商品的总价值
//功能：计算所有商品的库存总价值
//参数：manager - 管理器指针
//返回：库存总价值
float calculateTotalValue(GoodsManager* manager)
{
    if (manager == NULL) 
    {
        return 0.0f;
    }

    //遍历计算总价值
    float totalValue = 0.0f;
    GoodsNode* current = manager->head;
    while (current != NULL)
    {
        totalValue += current->data.price * current->data.stock;
        current = current->next;
    }
    return totalValue;
}

//按名称查找商品
//功能：查找名称中包含指定字符串的商品
//参数：manager - 管理器指针，name - 要查找的商品名称
//返回：找到的第一个匹配商品节点，未找到返回NULL
GoodsNode* findGoodsByName(GoodsManager* manager, const char* name) 
{
    if (manager == NULL || name == NULL) 
    {
        return NULL;
    }

    //遍历查找匹配的商品名称
    GoodsNode* current = manager->head;
    while (current != NULL)
    {
        if (strstr(current->data.name, name) != NULL) 
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

//按品牌查找商品
//功能：查找指定品牌的商品
//参数：manager - 管理器指针，brand - 要查找的品牌名称
//返回：找到的第一个匹配商品节点，未找到返回NULL
GoodsNode* findGoodsByBrand(GoodsManager* manager, const char* brand) 
{
    if (manager == NULL || brand == NULL) 
    {
        return NULL;
    }

    // 遍历查找匹配的品牌
    GoodsNode* current = manager->head;
    while (current != NULL) 
    {
        if (strstr(current->data.brand, brand) != NULL) 
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

//显示查询结果
//功能：以表格形式显示查询到的商品信息
//参数：results - 查询结果节点指针
void displaySearchResults(GoodsNode* results) 
{
    if (results == NULL) 
    {
        printf("No matching products found.\n");
        return;
    }

    //打印表头
    printf("\nSearch Results:\n");
    printf("%-16s  ", "ID");              //ID列，左对齐，16字符
    printf("%-25s  ", "Name");            //名称列，左对齐，25字符
    printf("%-12s  ", "Category");        //类别列，左对齐，12字符
    printf("%-20s  ", "Brand");           //品牌列，左对齐，20字符
    printf("%10s  ", "Price");            //价格列，右对齐，10字符
    printf("%8s\n", "Stock");             //库存列，右对齐，8字符
    
    //打印分隔线
    printf("----------------  ");          //16字符
    printf("-------------------------  "); //25字符
    printf("------------  ");             //12字符
    printf("--------------------  ");     //20字符
    printf("----------  ");               //10字符
    printf("--------\n");                 //8字符

    //打印结果
    char truncated_id[17] = {0};
    char truncated_name[26] = {0};
    char truncated_brand[21] = {0};

    truncateString(truncated_id, results->data.id, 16);
    truncateString(truncated_name, results->data.name, 25);
    truncateString(truncated_brand, results->data.brand, 20);

    printf("%-16s  ", truncated_id);
    printf("%-25s  ", truncated_name);
    printf("%-12s  ", categoryToString(results->data.category));
    printf("%-20s  ", truncated_brand);
    printf("%10.2f  ", results->data.price);
    printf("%8d\n", results->data.stock);

    //打印底部分隔线
    printf("----------------  ");
    printf("-------------------------  ");
    printf("------------  ");
    printf("--------------------  ");
    printf("----------  ");
    printf("--------\n");
}