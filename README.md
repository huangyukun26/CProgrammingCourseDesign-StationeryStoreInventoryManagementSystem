# Stationery Store Inventory Management System

## Introduction

This is a C language based inventory management system designed for stationery stores. The system provides comprehensive functionality for managing stationery products inventory, including basic CRUD operations, data persistence, searching, sorting and statistical analysis.

The system uses linked list data structure to store product information in memory and provides file-based data persistence. It features a command-line interface with input validation and formatted data display.

Key Features:

* Product CRUD operations (Create, Read, Update, Delete)
* Batch import/export of product data
* Multiple search methods (by ID, name, brand, category)
* Product categorization (Pen, Notebook, Paint, Other)
* Price-based sorting (ascending/descending)
* Category-based statistics
* Total inventory value calculation
* Data persistence using text files
* Input validation and error handling
* Formatted table display with truncation

## Project Structure 

```bash
├── myGoods/ # Source code directory
│ ├── main.c # Main program entry and UI logic
│ ├── goods.h # Header file with data structures and function declarations
│ ├── goods.c # Implementation of core business logic
│ └── goods.txt # Data persistence file
├── .gitignore # Git ignore rules
└── README.md # Project documentation
```

## Tech Stack

* Language: C
* Build System: Visual Studio 2022
* Data Structure: Linked List
* Data Storage: Text File
* Interface: Command Line

## Features

### Product Management
* Add new products with validation
* Update existing product information
* Delete products with confirmation
* View all products in formatted table
* Batch import products from file

### Search Functions
* Search by product ID
* Search by product name
* Search by brand name
* Filter by product category

### Analysis Features
* Sort products by price
* Count products by category
* Calculate total inventory value
* Display statistics

## Installation

### Requirements
* Visual Studio 2022 or other C compiler
* Windows environment (for Visual Studio)

### Setup Steps

1. Clone the repository

```bash
git clone [repository-url]
cd stationery-store-system
```

2. Open the project in Visual Studio 2022
3. Build and run the project

## Usage Guide

1. Main Menu Options:
   * 1 - Batch Import Initial Products
   * 2 - View All Products
   * 3 - Search Products
   * 4 - Add New Product
   * 5 - Delete Product
   * 6 - Update Product
   * 7 - Count Products by Category
   * 8 - Sort Products by Price
   * 9 - Calculate Total Inventory Value
   * 0 - Exit

2. Data Format:
   * Product ID: Max 18 characters
   * Product Name: Max 48 characters
   * Brand: Max 48 characters
   * Category: Pen/Notebook/Paint/Other
   * Price: Positive number
   * Stock: Non-negative integer

## Development Guide

### Code Standards
* Use clear and meaningful variable names
* Add comments for functions and complex logic
* Validate all user inputs
* Handle error cases appropriately
* Format output for readability

### Data Validation Rules
* All string fields have maximum length limits
* Price must be positive
* Stock quantity must be non-negative
* Product ID must be unique
* Category must be one of the predefined types

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

MIT License
