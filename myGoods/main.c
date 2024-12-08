#include <windows.h>
#include <locale.h>
#include "goods.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DATA_FILE "goods.txt"  //数据文件路径
#define MAX_INPUT 256         //最大输入长度
#define _CRT_SECURE_NO_WARNINGS

//清空输入缓冲区
//功能：清除输入缓冲区中的剩余字符，防止影响下次输入
void clearInputBuffer() 
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//获取用户确认
//功能：获取用户的是/否确认输入
//参数：message - 提示信息
//返回：用户确认返回1，否则返回0
int getConfirmation(const char* message) 
{
    char input[MAX_INPUT];
    while (1) 
    {
        printf("%s (y/n): ", message);
        fflush(stdout);
        
        if (fgets(input, sizeof(input), stdin) == NULL) 
        {
            clearInputBuffer();
            continue;
        }
        
        //去除换行符
        input[strcspn(input, "\n")] = 0;
        
        //检查输入长度
        if (strlen(input) != 1) 
        {
            printf("Please enter a single character (y/n).\n");
            continue;
        }
        
        //检查输入字符
        char response = tolower(input[0]);
        if (response == 'y' || response == 'n') 
        {
            return response == 'y';
        }
        
        printf("Please enter 'y' or 'n'.\n");
    }
}

//显示主菜单
//功能：显示系统的主菜单选项
void displayMenu() 
{
    printf("\n=== Stationery Store Inventory Management System ===\n");
    printf("1. Batch Import Initial Products\n");
    printf("2. View All Products\n");
    printf("3. Search Products\n");
    printf("4. Add New Product\n");
    printf("5. Delete Product\n");
    printf("6. Update Product\n");
    printf("7. Count Products by Category\n");
    printf("8. Sort Products by Price\n");
    printf("9. Calculate Total Inventory Value\n");
    printf("0. Exit\n");
    printf("Please select an option (0-9): ");
}

//显示查询子菜单
//功能：显示商品查询的子菜单选项
void displaySearchMenu() 
{
    printf("\n=== Search Products ===\n");
    printf("1. Search by ID\n");
    printf("2. Search by Name\n");
    printf("3. Search by Brand\n");
    printf("4. Search by Category\n");
    printf("0. Return to Main Menu\n");
    printf("Please select search type (0-4): ");
}

//输入商品信息
//功能：获取用户输入的商品信息
//返回：包含用户输入的商品信息的Goods结构体
Goods inputGoodsInfo() 
{
    Goods goods;
    char category_str[MAX_INPUT];
    
    printf("Please enter product information:\n");
    
    printf("Product ID: ");
    scanf_s("%19s", goods.id, (unsigned)sizeof(goods.id));
    clearInputBuffer();
    
    printf("Product Name: ");
    scanf_s("%49s", goods.name, (unsigned)sizeof(goods.name));
    clearInputBuffer();
    
    printf("Category (Pen/Notebook/Paint/Other): ");
    scanf_s("%19s", category_str, (unsigned)sizeof(category_str));
    clearInputBuffer();
    goods.category = stringToCategory(category_str);
    
    printf("Brand: ");
    scanf_s("%49s", goods.brand, (unsigned)sizeof(goods.brand));
    clearInputBuffer();
    
    printf("Price: ");
    scanf_s("%f", &goods.price);
    clearInputBuffer();
    
    printf("Stock Quantity: ");
    scanf_s("%d", &goods.stock);
    clearInputBuffer();
    
    return goods;
}

//处理批量导入
//功能：从文件导入商品数据或手动输入多条商品信息
//参数：manager - 商品管理器指针
void handleBatchInput(GoodsManager* manager) 
{
    printf("\n=== Batch Import Products ===\n");
    
    // 检查是否已有数据
    if (manager->count > 0) {
        if (getConfirmation("Existing data will be cleared. Continue?")) {
            // 清空现有数据
            freeGoodsManager(manager);
            manager = initGoodsManager();
            if (manager == NULL) {
                printf("System initialization failed!\n");
                return;
            }
        } else {
            return;
        }
    }
    
    if (loadFromFile(manager, DATA_FILE)) 
    {
        printf("Successfully loaded products from file!\n");
    } 
    else 
    {
        printf("Failed to load file or file does not exist.\n");
        printf("Please manually enter at least 5 products.\n");
        
        int count = 0;
        while (count < 5) 
        {
            printf("\nEntering product %d of 5\n", count + 1);
            Goods goods = inputGoodsInfo();
            
            if (isValidGoods(goods))
            {
                if (addGoods(manager, goods)) 
                {
                    count++;
                    printf("Product added successfully!\n");
                } 
                else 
                {
                    printf("Product ID already exists, please try again.\n");
                }
            } 
            else 
            {
                printf("Invalid product information, please try again.\n");
            }
        }
        
        if (saveToFile(manager, DATA_FILE))
        {
            printf("Products saved to file.\n");
        } 
        else 
        {
            printf("Failed to save file!\n");
        }
    }
}

//处理商品查询
//功能：处理各种商品查询操作
//参数：manager - 商品管理器指针
void handleSearch(GoodsManager* manager) 
{
    int choice;
    char searchTerm[MAX_INPUT];
    GoodsNode* result;
    
    do {
        displaySearchMenu();
        if (scanf_s("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input. Please enter a number between 0 and 4.\n");
            continue;
        }
        clearInputBuffer();
        
        if (choice < 0 || choice > 4) {
            printf("Invalid choice. Please enter a number between 0 and 4.\n");
            continue;
        }
        
        switch (choice) 
        {
            case 0:
                return;
                
            case 1:  //按ID查询
                printf("Enter Product ID: ");
                scanf_s("%s", searchTerm, (unsigned)sizeof(searchTerm));
                clearInputBuffer();
                result = findGoodsById(manager, searchTerm);
                displaySearchResults(result);
                break;
                
            case 2:  //按名称查询
                printf("Enter Product Name: ");
                scanf_s("%s", searchTerm, (unsigned)sizeof(searchTerm));
                clearInputBuffer();
                result = findGoodsByName(manager, searchTerm);
                displaySearchResults(result);
                break;
                
            case 3:  //按品牌查询
                printf("Enter Brand: ");
                scanf_s("%s", searchTerm, (unsigned)sizeof(searchTerm));
                clearInputBuffer();
                result = findGoodsByBrand(manager, searchTerm);
                displaySearchResults(result);
                break;
                
            case 4:  //按类别查询
                printf("Enter Category (Pen/Notebook/Paint/Other): ");
                scanf_s("%s", searchTerm, (unsigned)sizeof(searchTerm));
                clearInputBuffer();
                displayGoodsByCategory(manager, stringToCategory(searchTerm));
                break;
                
            default:
                printf("Invalid choice, please try again.\n");
        }
    } while (1);
}

//处理商品更新
//功能：更新现有商品的信息
//参数：manager - 商品管理器指针
void handleUpdate(GoodsManager* manager) 
{
    char id[MAX_INPUT];
    printf("\n=== Update Product ===\n");
    printf("Enter Product ID to update: ");
    scanf_s("%s", id, (unsigned)sizeof(id));
    clearInputBuffer();
    
    GoodsNode* node = findGoodsById(manager, id);
    if (node == NULL) 
    {
        printf("Product not found!\n");
        return;
    }
    
    printf("\nCurrent product information:\n");
    displaySearchResults(node);
    
    if (getConfirmation("Confirm update this product?")) 
    {
        Goods newData = inputGoodsInfo();
        if (updateGoods(manager, id, newData))
        {
            printf("Update successful!\n");
            if (saveToFile(manager, DATA_FILE)) 
            {
                printf("Changes saved to file.\n");
            } 
            else 
            {
                printf("Failed to save file!\n");
            }
        } 
        else 
        {
            printf("Update failed!\n");
        }
    }
}

//处理商品删除
//功能：删除指定的商品
//参数：manager - 商品管理器指针
void handleDelete(GoodsManager* manager) 
{
    char id[MAX_INPUT];
    printf("\n=== Delete Product ===\n");
    printf("Enter Product ID to delete: ");
    scanf_s("%s", id, (unsigned)sizeof(id));
    clearInputBuffer();
    
    GoodsNode* node = findGoodsById(manager, id);
    if (node == NULL) 
    {
        printf("Product not found!\n");
        return;
    }
    
    printf("\nProduct to delete:\n");
    displaySearchResults(node);
    
    if (getConfirmation("Confirm delete this product?")) 
    {
        if (deleteGoods(manager, id)) 
        {
            printf("Delete successful!\n");
            if (saveToFile(manager, DATA_FILE))
            {
                printf("Changes saved to file.\n");
            } 
            else 
            {
                printf("Failed to save file!\n");
            }
        } 
        else 
        {
            printf("Delete failed!\n");
        }
    }
}

//输入验证函数
int getIntegerInput(int min, int max) {
    int value;
    char line[256];
    while (1) 
    {
        if (fgets(line, sizeof(line), stdin)) 
        {
            if (sscanf_s(line, "%d", &value) == 1) 
            {
                if (value >= min && value <= max) 
                {
                    return value;
                }
            }
        }
        printf("Invalid input. Please enter a number between %d and %d: ", min, max);
        clearInputBuffer();
    }
}

//主函数
//功能：程序的入口点，实现主要的交互逻辑
int main() {
    //初始化商品管理器
    GoodsManager* manager = initGoodsManager();
    if (manager == NULL)
    {
        printf("System initialization failed!\n");
        return 1;
    }

    //主循环
    int choice;
    do {
        displayMenu();
        if (scanf_s("%d", &choice) != 1) {
            clearInputBuffer();
            printf("Invalid input. Please enter a number between 0 and 9.\n");
            continue;
        }
        clearInputBuffer();
        
        if (choice < 0 || choice > 9) {
            printf("Invalid choice. Please enter a number between 0 and 9.\n");
            continue;
        }
        
        //处理用户选择
        switch (choice) {
            case 0:  //退出系统
                if (getConfirmation("Confirm exit?")) 
                {
                    freeGoodsManager(manager);
                    printf("Thank you for using. Goodbye!\n");
                    return 0;
                }
                break;

            case 1:  //批量导入
                handleBatchInput(manager);
                break;

            case 2:  //显示所有商品
                displayAllGoods(manager);
                break;

            case 3:  //查询商品
                handleSearch(manager);
                break;

            case 4:  //添加新商品
                printf("\n=== Add New Product ===\n");
                Goods newGoods = inputGoodsInfo();
                if (isValidGoods(newGoods)) 
                {
                    if (addGoods(manager, newGoods)) 
                    {
                        printf("Add successful!\n");
                        if (saveToFile(manager, DATA_FILE)) 
                        {
                            printf("Changes saved to file.\n");
                        } 
                        else 
                        {
                            printf("Failed to save file!\n");
                        }
                    } 
                    else 
                    {
                        printf("Add failed, Product ID may already exist!\n");
                    }
                } 
                else 
                {
                    printf("Invalid product information!\n");
                }
                break;

            case 5:  //删除商品
                handleDelete(manager);
                break;

            case 6:  //更新商品
                handleUpdate(manager);
                break;

            case 7:  //按类别统计
                printf("\n=== Product Count by Category ===\n");
                printf("Pen products: %d\n", countGoodsByCategory(manager, PEN));
                printf("Notebook products: %d\n", countGoodsByCategory(manager, NOTEBOOK));
                printf("Paint products: %d\n", countGoodsByCategory(manager, PAINT));
                printf("Other products: %d\n", countGoodsByCategory(manager, OTHER));
                break;

            case 8:  //按价格排序
                printf("\n=== Sort by Price ===\n");
                printf("1. Ascending\n2. Descending\nSelect sort order (1-2): ");
                int sortChoice = 1;
                if (scanf_s("%d", &sortChoice) != 1 || (sortChoice != 1 && sortChoice != 2)) {
                    clearInputBuffer();
                    printf("Invalid input, using default ascending order.\n");
                    sortChoice = 1;
                } else {
                    clearInputBuffer();
                }
                sortGoodsByPrice(manager, sortChoice == 1);
                displayAllGoods(manager);
                break;

            case 9:  //计算总价值
                printf("\n=== Total Inventory Value ===\n");
                float totalValue = calculateTotalValue(manager);
                printf("Current total inventory value: %.2f\n", totalValue);
                break;

            default:
                printf("Invalid choice, please try again.\n");
        }
    } while (1);

    return 0;
} 